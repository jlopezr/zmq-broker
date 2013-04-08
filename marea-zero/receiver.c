#include <czmq.h>
#include "constants.h"


//TODO this should be in mz_ctx
zhash_t* services_found;

typedef struct discover_service_t {
   long time;
   char* url;
} discover_service_t; 

char* discover(zctx_t* context, const char* key) {
    void *client = zsocket_new (context, ZMQ_REQ);
    zsocket_connect (client, "inproc://discovery");
    zstr_sendm(client, "GET");
    zstr_send(client, key);
    return zstr_recv(client);
}

void free_discover_service_fn(void *data) {
   discover_service_t* svc = (discover_service_t*)data;
   if(svc->url!=NULL) {
       free(svc->url);
   }
}

void update_service(char* service_name, discover_service_t* svc) {
   discover_service_t* current = zhash_lookup(services_found, service_name);

   if(current==NULL) {
        zhash_insert(services_found, service_name, svc);
        printf("Service %s has been discovered at %s\r\n", 
		    service_name, svc->url );
   } else {
	int lookup = strcmp(svc->url, current->url);
        if(lookup==0) {
            current->time = svc->time;	
	} else if(lookup<0) {
            zhash_update(services_found, service_name, svc);
            printf("Service %s has been discovered at %s (old was %s)\r\n", 
		    service_name, svc->url, current->url );
        } else { // lookup > 0
            printf("Service %s has been detected at %s, but %s is kept\r\n", 
		    service_name, svc->url, current->url );
	}
   }
}

int process_services(const char *key, void *item, void *argument) {
    //  si el tiempo_actual > tiempo_guardado+3*TIMEOUT
    //     lo elimino de la tabla
    //     aviso de que ya no esta detectado

    long* value = (long*)item;
    long* time = (long*)argument;

    //printf("TIME %ld \r\n", *time);
    //printf("KEY %s => %ld\r\n", key, *value);

    if(*time>*value+(BEACON_INTERVAL*BEACON_MISSED)) {
        printf("Service %s has dissapeared\r\n", key);
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
	    printf("SERVER FUNC: %s\r\n", func);
	    if(strcmp(func,"GET")==0) {
		discover_service_t* svc = zhash_lookup(services_found, data);
		if(svc!=NULL) {
		   result = svc->url;
		} else {
	           result = NULL;
		}   
            } else if(strcmp(func,"QUIT")==0) {
                result = "OK";
	    } else {
	        assert(0 && "FUNC NOT KNOWN!");
	    }
	    zstr_send(sink, result);
            free(func);
	    free(data);
        }

	//BEACON
        if(items[1].revents & ZMQ_POLLIN) {

            //recibo con timeout
            char *ipaddress = zstr_recv (zbeacon_pipe (client_beacon));

            //si hay trama la proceso, i.e
            if (ipaddress) {
                zframe_t *content = zframe_recv (zbeacon_pipe (client_beacon));
                int received_port = (zframe_data (content) [0] << 8)
                                    +  zframe_data (content) [1];
                char* service_name = ((char*)content)+2;
                //printf("IP   DETECTED %s\r\n", ipaddress);
                //printf("PORT DETECTED %d\r\n", received_port);
                //printf("SVC  DETECTED %s\r\n", service_name);

                //  si ya existe actualizo su tiempo
                //  sino la añado
                long* tmp = malloc(sizeof(long));
                *tmp = zclock_time();
                int num_items = zhash_size(services_found);

		discover_service_t* svc = malloc(sizeof(discover_service_t));
		svc->time = zclock_time();
		asprintf(&(svc->url),"tcp://%s:%d", ipaddress, received_port);

                update_service(service_name, svc);

                zframe_destroy (&content);
                free (ipaddress);
            }

        }

        //recorro todos los elementos del hash
        now = zclock_time();
        zhash_foreach (services_found, process_services, &now);
    }
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


