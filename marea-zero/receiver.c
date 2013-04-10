#include <czmq.h>
#include "constants.h"


//TODO this should be in mz_ctx
zhash_t* services_found;
void *svc_changes;

typedef struct discover_service_t {
   long time;
   char* url;
} discover_service_t; 

char* discover(zctx_t* context, const char* key) {
    void *client = zsocket_new (context, ZMQ_REQ);
    zsocket_connect (client, "inproc://discovery");
    zstr_sendm(client, "GET");
    zstr_send(client, key);
    char* result = zstr_recv(client);

    //TODO deberiamos destruir el socket
    //zsocket_destroy(context, &client);

    return result;
}

void decode_frame_tcp(char* ipaddress, zframe_t* content, char** service_name, char** url) {
   char* data = (char*)zframe_data(content);

   int received_port = (data[0] << 8) + data[1];
   *service_name = data + 2;
   asprintf(url,"tcp://%s:%d", ipaddress, received_port);
   //TODO esta la deberiamos liberar en algun sitio... la de decode_frame_url NO
}

void decode_frame_url(char* ipaddress, zframe_t* content, char** service_name, char** url) {
   char* data = (char*)zframe_data(content);
   *service_name = data+1;
   int len = strlen(*service_name);
   *url = data + 2 + len;
   //len = strlen(*url);
}

void decode_frame(char* ipaddress, zframe_t* content, char** service_name, char** url) {
}

void free_discover_service_fn(void *data) {
   discover_service_t* svc = (discover_service_t*)data;
   if(svc->url!=NULL) {
       free(svc->url);
       svc->url = NULL;
   }
   free(svc);
}

void update_service(char* service_name, discover_service_t** svc) {
   discover_service_t* current = zhash_lookup(services_found, service_name);

   if(current==NULL) {
        zhash_insert(services_found, service_name, *svc);
	zhash_freefn(services_found, service_name, free_discover_service_fn);
	
	printf("Service %s has been discovered at %s\r\n", 
		    service_name, (*svc)->url );
	zstr_sendm(svc_changes, service_name);
	zstr_send(svc_changes, (*svc)->url);
   } else {
	int lookup = strcmp((*svc)->url, current->url);
        if(lookup==0) {
            current->time = (*svc)->time;	
	} else if(lookup<0) {
            zhash_update(services_found, service_name, *svc);
	    //TODO No se si hace falta
	    zhash_freefn(services_found, service_name, free_discover_service_fn);
            printf("Service %s has been discovered at %s (old was %s)\r\n", 
		    service_name, (*svc)->url, current->url );
	    zstr_sendm(svc_changes, service_name);
	    zstr_send(svc_changes, (*svc)->url);
	} 
	/* 
	  else { // lookup > 0
            printf("Service %s has been detected at %s, but %s is kept\r\n", 
		    service_name, svc->url, current->url );
	} 
	*/
	free_discover_service_fn(*svc);
	*svc = NULL;
   }
}

int process_services(const char *key, void *item, void *argument) {
    //  si el tiempo_actual > tiempo_guardado+3*TIMEOUT
    //     lo elimino de la tabla
    //     aviso de que ya no esta detectado

    discover_service_t* value = (discover_service_t*)item;
    long* time = (long*)argument;

    //printf("TIME %ld\r\n", *time);
    //printf("KEY  %s\r\n", key);
    //printf("+URL %s\r\n", value->url);
    //printf("+CLK %ld\r\n", value->time);

    if( *time > (value->time + (BEACON_INTERVAL*BEACON_MISSED)) ) {
        printf("Service %s has dissapeared\r\n", key);
	zstr_sendm(svc_changes, key);
	zstr_send(svc_changes, "");
        zhash_delete(services_found, key);
    }

    return 0;
}

void discovery(void* args, zctx_t* ctx, void* pipe) {
    static long now = 0;

    assert(services_found != 0);

    //  Create beacon to lookup service
    zbeacon_t *client_beacon = zbeacon_new (9999);
    zbeacon_subscribe (client_beacon, NULL, 0);

    void *sink = zsocket_new (ctx, ZMQ_REP);
    zsocket_bind (sink, "inproc://discovery");

    svc_changes = zsocket_new (ctx, ZMQ_PUB);
    zsocket_bind (svc_changes, "inproc://discovery-changes");

    zmq_pollitem_t items[] = {
        { sink, 0, ZMQ_POLLIN, 0 },
        { zbeacon_pipe(client_beacon), 0, ZMQ_POLLIN, 0}
    };

    while(!zctx_interrupted) {
        // Wait for at most 1/2 second if there's no broadcast networking
        if(zmq_poll(items, 2, BEACON_TIMEOUT) < 0) {
            break; // interrupted (-1)
        }

	//PETICION DE CONSULTA
        if(items[0].revents & ZMQ_POLLIN) {
            char* func= zstr_recv(sink);
            char* data = zstr_recv(sink);
	    char* result;
	    //printf("SERVER FUNC: %s(%s)\r\n", func, data);
	    if(strcmp(func,"GET")==0) {
		discover_service_t* svc = zhash_lookup(services_found, data);
		if(svc!=NULL) {
		   result = svc->url;
		} else {
	           result = "";
		}   
            } else if(strcmp(func,"QUIT")==0) {
                result = "OK";
	    } else {
	        assert(0 && "FUNC NOT KNOWN!");
	    }
	    //printf("SERVER RES: [%s]\r\n", result);
	    zstr_send(sink, result);
            free(func);
	    free(data);
        }

	//BEACON
        if(items[1].revents & ZMQ_POLLIN) {

            //recibo con timeout
            char *ipaddress = zstr_recv (zbeacon_pipe (client_beacon));
            char* service_name;
	    char* url;

            //si hay trama la proceso, i.e
            if (ipaddress) {
                zframe_t *content = zframe_recv (zbeacon_pipe (client_beacon));

		decode_frame_url(ipaddress, content, &service_name, &url);

                //printf("IP   DETECTED %s\r\n", ipaddress);
                //printf("URL DETECTED %s\r\n", url);
                //printf("SVC  DETECTED %s\r\n", service_name);

                //  si ya existe actualizo su tiempo
                //  sino la añado
		discover_service_t* svc = malloc(sizeof(discover_service_t));
		svc->time = zclock_time();
		svc->url = strdup(url);
                
		update_service(service_name, &svc);

                zframe_destroy (&content);
                free (ipaddress);
            }
        }

        //recorro todos los elementos del hash
        now = zclock_time();
        zhash_foreach (services_found, process_services, &now);
    }

    //Deberiamos destruirlo tambien
    //zsocket_destroy(ctx, &sink);
    //zsocket_destroy(ctx, &svc_changes);
    zbeacon_destroy (&client_beacon);
    zhash_destroy(&services_found);
}

void start_discovery(zctx_t* ctx) {
    services_found = zhash_new();
    zthread_fork(ctx, discovery, NULL);
}

void stop_discovery(zctx_t* ctx) {
    	//TODO Should call zctx_destroy();
        //     or zbeacon_stop()
	//     or send a message to inproc://discovery to stop it
}


