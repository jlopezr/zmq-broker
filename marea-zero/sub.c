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

    start_discovery(NULL);

    char* location = NULL;
    //TODO Get the location

    printf("Connecting to %s\r\n", location);
    zsocket_connect (subscriber, location);
    printf("Subscribed to %s\r\n",argv[1]);
    zsocket_set_subscribe (subscriber, argv[1]);
    
    while (true) {
        char *topic = zstr_recv (subscriber);
        if (!topic)
            break;
        char *data = zstr_recv (subscriber);
        //assert (streq (topic, argv[1]));
        puts (data);
        free (topic);
        free (data);
    }
    
    stop_discovery(NULL);
    zctx_destroy (&context);
    
    return 0;
}
