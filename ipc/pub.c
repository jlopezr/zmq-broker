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
	printf("Connecting to %s\r\n", argv[2]);
	zsocket_connect (publisher, argv[2]);
    }

    //  Ensure subscriber connection has time to complete
    sleep (1);

    //  Send one random update per second
    srandom ((unsigned) time (NULL));
    while (!zctx_interrupted) {
        sleep (1);
        zstr_sendm (publisher, argv[1], ZMQ_SNDMORE); //TODO is ZMQ_SNDMORE needed?
        int n = randof(1000);
	printf("Sending %03d\r\n", n);
	zstr_send (publisher, "VALUE = %03d", n);
    }
    zctx_destroy (&context);
    return 0;
}
