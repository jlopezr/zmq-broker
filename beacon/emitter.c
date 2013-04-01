#include <czmq.h>

int main(void) {

    //  Basic test: create a service and announce it
    zctx_t *ctx = zctx_new ();

    //  Create a service socket and bind to an ephemeral port
    void *service = zsocket_new (ctx, ZMQ_PUB);
    int port_nbr = zsocket_bind (service, "tcp://*:*");

    //  Create beacon to broadcast our service
    byte announcement [2] = { (port_nbr >> 8) & 0xFF, port_nbr & 0xFF };
    zbeacon_t *service_beacon = zbeacon_new (9999);
    zbeacon_set_interval (service_beacon, 100);
    zbeacon_publish (service_beacon, announcement, 2);

    //  Wait for at most 1/2 second if there's no broadcast networking
    zsocket_set_rcvtimeo (zbeacon_pipe (client_beacon), 500);

    while (!zctx_interrupted) {
        sleep (1);
    }

    zbeacon_destroy (&service_beacon);
    zctx_destroy (&ctx);

    return 0;
}
