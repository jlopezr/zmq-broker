//  Pathological subscriber
//  Subscribes to one random topic and prints received messages

#include "czmq.h"
#include <stdlib.h>

int main (int argc, char *argv [])
{
    zctx_t *context = zctx_new ();
    void *subscriber = zsocket_new (context, ZMQ_SUB);

    if (argc < 3) {
       printf("usage: %s <subscription> [--broker|--local|<zmq port>]\r\n",argv[0]);
       return 1;
    }
    if (strcmp(argv[2], "--broker") == 0) {
	printf("Connecting to ipc://backend (BROKER)\r\n");
        zsocket_connect (subscriber, "ipc://backend");
    } else if (strcmp(argv[2], "--local") == 0) {
	printf("Connecting to ipc://test (LOCAL)\r\n");
        zsocket_connect (subscriber, "ipc://test");
    } else {
	printf("Connecting to %s\r\n", argv[2]);
        zsocket_connect (subscriber, argv[2]);
    }

    srandom ((unsigned) time (NULL));
    //char subscription [5];
    //sprintf (subscription, "%03d", randof (1000));
    //zsocket_set_subscribe (subscriber, subscription);
    zsocket_set_subscribe (subscriber, argv[1]);
    printf("Subscribed to %s\r\n",argv[1]);
    while (true) {
        char *topic = zstr_recv (subscriber);
        if (!topic)
            break;
        char *data = zstr_recv (subscriber);
        assert (streq (topic, argv[1]));
        puts (data);
        free (topic);
        free (data);
    }
    zctx_destroy (&context);
    return 0;
}
