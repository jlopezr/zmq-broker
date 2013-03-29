//  Pathological publisher
//  Sends out 1,000 topics and then one random update per second

#include "czmq.h"

int main (int argc, char *argv [])
{
    zctx_t *context = zctx_new ();
    void *publisher = zsocket_new (context, ZMQ_PUB);
     if (argc < 3) {
       printf("usage: %s <subscription> [--broker|--local|<zmq port>]\r\n",argv[0]);
       return 1;
    }
    if (strcmp(argv[2], "--broker") == 0) {
	printf("Connecting to ipc://frontend (BROKER)");
	zsocket_connect (publisher, "ipc://frontend");

    } else if(strcmp(argv[2], "--local") == 0) {
	printf("Listening on ipc://test (LOCAL)");
	zsocket_bind (publisher, "ipc://test");
    } else {
	printf("Listening on %s\r\n", argv[2]);
	int rc = zsocket_bind (publisher, argv[2]);
    	assert(rc==0);
    }

    //  Ensure subscriber connection has time to complete
    sleep (1);

    //  Send one random update per second
    srandom ((unsigned) time (NULL));
    int i = 0;
    while (!zctx_interrupted && i<10) {
        sleep (1);
        zstr_sendm (publisher, argv[1]);
        int n = randof(1000);
	printf("Sending %03d\r\n", n);
	zstr_send (publisher, "A1 VALUE = %03d", n);
    	i++;
    }

    zsocket_destroy (context, publisher);
    printf("SOCKET DESTRUIDO\r\n");
    sleep(100);

    zctx_destroy (&context);
    return 0;
}
