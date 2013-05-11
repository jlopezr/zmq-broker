//  Pathological subscriber
//  Subscribes to one random topic and prints received messages

#include "czmq.h"
#include <stdlib.h>

int main (int argc, char *argv [])
{
    zctx_t *context = zctx_new ();
    void *socket = zsocket_new (context, ZMQ_REQ);
    
    zsocket_connect(socket, "tcp://localhost:6666");
    printf("Connected...\r\n");
    
    sleep(-1);

    zsocket_disconnect(socket, "tcp://localhost:6666");
    printf("Connected...\r\n");


    zctx_destroy (&context);
    return 0;
}
