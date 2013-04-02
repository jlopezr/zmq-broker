#include <czmq.h>
#include "constants.h"

int process_services(const char *key, void *item, void *argument) {
    //  si el tiempo_actual > tiempo_guardado+3*TIMEOUT
    //     lo elimino de la tabla
    //     aviso de que ya no esta detectado

    long* value = (long*)item;
    long* time = (long*)argument;

    if(*time>*value+(BEACON_INTERVAL*BEACON_MISSED)) {
        printf("TIMEOUT!!!!!!!!!!!!!!!!!!!!!!!\r\n");
    }

    printf("TIME %ld \r\n", *time); 
    printf("KEY %s => %ld\r\n", key, *value);

    return 0;
}

long now = 0;

int main(void) {

    //  Basic test: create a service and announce it
    zctx_t *ctx = zctx_new ();
    zhash_t* services_found = zhash_new(); 
    
    //  Create beacon to lookup service
    zbeacon_t *client_beacon = zbeacon_new (9999);
    zbeacon_subscribe (client_beacon, NULL, 0);

    //  Wait for at most 1/2 second if there's no broadcast networking
    zsocket_set_rcvtimeo (zbeacon_pipe (client_beacon), BEACON_TIMEOUT);

    while(!zctx_interrupted) {

        //recibo con timeout
	char *ipaddress = zstr_recv (zbeacon_pipe (client_beacon));

	//si hay trama la proceso, i.e
	if (ipaddress) {
            zframe_t *content = zframe_recv (zbeacon_pipe (client_beacon));
            int received_port = (zframe_data (content) [0] << 8)
                                +  zframe_data (content) [1];
	    char* service_name = ((char*)content)+2; 
            printf("IP   DETECTED %s\r\n", ipaddress);
            printf("PORT DETECTED %d\r\n", received_port);
            printf("SVC  DETECTED %s\r\n", service_name);
 
            //  si ya existe actualizo su tiempo
	    //  sino la añado
            long* tmp = malloc(sizeof(long));
	    *tmp = zclock_time();
	    
	    zhash_update(services_found, service_name, tmp); 
            
	    zframe_destroy (&content);
            free (ipaddress);
	}

	//recorro todos los elementos del hash
	now = zclock_time();
        zhash_foreach (services_found, process_services, &now);
    }
    
    zbeacon_destroy (&client_beacon);
    zctx_destroy (&ctx);

    return 0;
}
