#include <czmq.h>

int main(int argc, char** argv) {

    //  Basic test: create a service and announce it
    zctx_t *ctx = zctx_new ();

    //  Create a service socket and bind to an ephemeral port
    void *service = zsocket_new (ctx, ZMQ_PUB);
    int port_nbr = zsocket_bind (service, "tcp://*:*");

    char* service_name;
    if(argc>1) {
        service_name = argv[1];
    } else {
        service_name = "TEST";
    } 

    printf("SERVICE NAME: %s \r\n", service_name);
    
    //  Create beacon to broadcast our service
    byte announcement [2] = { (port_nbr >> 8) & 0xFF, port_nbr & 0xFF };

    int len = strlen(service_name)+3;
    byte* frame = malloc(len);
    frame[0] = announcement[0];
    frame[1] = announcement[1];
    strcpy((char*)&(frame[2]),service_name);

    zbeacon_t *service_beacon = zbeacon_new (9999);
    zbeacon_set_interval (service_beacon, 100);
    zbeacon_publish (service_beacon, frame, len);

    while (!zctx_interrupted) {
        sleep (1);
    }

    zbeacon_destroy (&service_beacon);
    zctx_destroy (&ctx);

    return 0;
}
