#include <czmq.h>

int main(void) {

    //  Basic test: create a service and announce it
    zctx_t *ctx = zctx_new ();

    //  Create beacon to lookup service
    zbeacon_t *client_beacon = zbeacon_new (9999);
    zbeacon_subscribe (client_beacon, NULL, 0);

    //  Wait for at most 1/2 second if there's no broadcast networking
    zsocket_set_rcvtimeo (zbeacon_pipe (client_beacon), 500);

    while(!zctx_interrupted) {

        char *ipaddress = zstr_recv (zbeacon_pipe (client_beacon));
        if (ipaddress) {
            zframe_t *content = zframe_recv (zbeacon_pipe (client_beacon));
            int received_port = (zframe_data (content) [0] << 8)
                                +  zframe_data (content) [1];
            printf("PORT DETECTED %d\r\n", received_port);
            zframe_destroy (&content);
            free (ipaddress);
        }

    }
    zbeacon_destroy (&client_beacon);
    zctx_destroy (&ctx);

    return 0;
}
