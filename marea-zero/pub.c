//  Pathological publisher
//  Sends out 1,000 topics and then one random update per second
 
#include "czmq.h"
#include <string.h>

#include "emitter.c"

int main (int argc, char *argv [])
{
    zctx_t *context = zctx_new ();
    void *publisher = zsocket_new (context, ZMQ_PUB);
    int port_nbr = 0;

    if (argc < 2) {
       printf("usage: %s <subscription> --ipc|--tcp\r\n", argv[0]);
       return 1;
    }
    if (strcmp(argv[2], "--tcp") == 0) {
        port_nbr = zsocket_bind (publisher, "tcp://*:*");
	printf("Connecting to tcp://*:%d\r\n", port_nbr);
    } else if(strcmp(argv[2], "--ipc") == 0) {
	char* url; 
	asprintf(&url, "ipc://%s", argv[1]);
	printf("Listening on %s\r\n", url);
	zsocket_bind (publisher, url);
	free(url);
    } else {
        printf("option %s not understood\r\n", argv[2]);
	return -1;
    }

    //  Ensure subscriber connection has time to complete
    sleep (1);

    publish_service(context, argv[1], port_nbr);

    //  Send one update per second
    int i = 0;
    while (!zctx_interrupted) {
        sleep (1);
	printf("Sending %d\r\n", i);
        zstr_sendm (publisher, argv[1]);
	zstr_send (publisher, "%d", i);
    	i++;
    }

    unpublish_service(context, argv[1]);

    zsocket_destroy (context, publisher);
    zctx_destroy (&context);
    return 0;
}
