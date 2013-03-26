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
	printf("Connecting to ipc://frontend (BROKER)\r\n");
	zsocket_connect (publisher, "ipc://frontend");

    } else if(strcmp(argv[2], "--local") == 0) {
	printf("Connecting to epgm://239.192.1.1:5555 (LOCAL)\r\n");
	zsocket_connect (publisher, "epgm://239.192.1.1:5555");
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
        zstr_sendm (publisher, argv[1]); 
        int n = randof(1000);
	printf("Sending %03d\r\n", n);
	zstr_send (publisher, "VALUE = %03d", n);
    }
    zctx_destroy (&context);
    return 0;
}
