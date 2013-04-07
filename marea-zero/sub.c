//  Pathological subscriber
//  Subscribes to one random topic and prints received messages

#include "czmq.h"
#include <stdlib.h>

#include "receiver.c"

int main (int argc, char *argv [])
{
    zctx_t *context = zctx_new ();
    void *subscriber = zsocket_new (context, ZMQ_SUB);

    if (argc < 2) {
       printf("usage: %s <subscription>\r\n",argv[0]);
       return -1;
    }

    start_discovery(context);
    sleep(5); 

    char* location = discover(context, argv[1]);
    assert(location!=NULL);
    
    printf("Connecting to %s\r\n", location);

    sleep(-1);

    zsocket_connect (subscriber, location);
    printf("Subscribed to %s\r\n",argv[1]);
    zsocket_set_subscribe (subscriber, argv[1]);
    
    while (!zctx_interrupted) {
        char *topic = zstr_recv (subscriber);
        if (!topic)
            break;
        char *data = zstr_recv (subscriber);
        puts (data);
        free (topic);
        free (data);
    }
    
    printf("DESTROYING CTX\r\n");
    stop_discovery(context);
    zctx_destroy (&context);
    printf("CTX DESTROYED\r\n");
    
    return 0;
}
