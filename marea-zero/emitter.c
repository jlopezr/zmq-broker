#include <czmq.h>
#include "constants.h"

//TODO Now only one publisher is allow BUT API is prepared for more

//TODO this should be in mz_ctx
zbeacon_t* service_beacon;

void publish_service(zctx_t* ctx, const char* service_name, int port_nbr) {

    printf("Publishing service named: %s \r\n", service_name);
    
    //  Create beacon to broadcast our service
    byte announcement [2] = { (port_nbr >> 8) & 0xFF, port_nbr & 0xFF };

    int len = strlen(service_name)+3;
    byte* frame = malloc(len);
    frame[0] = announcement[0];
    frame[1] = announcement[1];
    strcpy((char*)&(frame[2]),service_name);

    service_beacon = zbeacon_new (9999);
    zbeacon_set_interval (service_beacon, BEACON_INTERVAL);
    zbeacon_publish (service_beacon, frame, len);
}

const char* get_hostname(zctx_t* ctx) {
    return zbeacon_hostname(service_beacon);
}

void unpublish_service(zctx_t* ctx, const char* service_name) {
    printf("Unpublishing service named: %s \r\n", service_name);
    
    zbeacon_destroy (&service_beacon);
}
