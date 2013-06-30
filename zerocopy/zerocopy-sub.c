//  Pathological subscriber
//  Subscribes to one random topic and prints received messages

#include "czmq.h"
#include <stdlib.h>

int main (int argc, char *argv [])
{
    zctx_t *context = zctx_new ();
    void *subscriber = zsocket_new (context, ZMQ_SUB);
    printf("Listening on tcp://*:5556\r\n");
    zsocket_bind (subscriber, "tcp://*:5556");
    zsocket_set_subscribe (subscriber, "TEST");

    int total_count = 1000000; 
    int message_size = 1024;

    int count = 0;
    while (count < total_count && !zctx_interrupted) {
	//printf("Receiving...\r\n");
        zmsg_t* msg = zmsg_recv(subscriber);
        zframe_t* topic_frame = zmsg_first(msg);
        zframe_t* data_frame = zmsg_next(msg);
        assert(zframe_size(topic_frame)==4);
	assert(zframe_size(data_frame)==message_size);
	zmsg_destroy(&msg);
	count++;
    }
    zctx_destroy (&context);
    return 0;
}
