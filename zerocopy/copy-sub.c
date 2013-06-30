//  Pathological subscriber
//  Subscribes to one random topic and prints received messages

#include "czmq.h"
#include "zmq_utils.h"
#include <stdlib.h>

int main (int argc, char *argv [])
{
    zctx_t *context = zctx_new ();
    void *subscriber = zsocket_new (context, ZMQ_SUB);
    printf("Listening on tcp://*:5556\r\n");
    zsocket_bind (subscriber, "tcp://*:5556");
    zsocket_set_subscribe (subscriber, "TEST");

    int total_count = 100000; 
    int message_size = 1024;

    int count = 0;
    void* watch = zmq_stopwatch_start ();
    while (count < total_count && !zctx_interrupted) {
	//printf("Receiving...\r\n");
        zmsg_t* msg = zmsg_recv(subscriber);
        zframe_t* topic_frame = zmsg_first(msg);
        zframe_t* data_frame = zmsg_next(msg);
        assert(zframe_size(topic_frame)==4);
	assert(zframe_size(data_frame)==message_size);
	int* pointer = (int*)zframe_data(data_frame);
	int value = *pointer;
	printf("VALUE %d\r\n", value);
	assert(count == value);
	zmsg_destroy(&msg);
	count++;
        //printf("Count %d\r\n", count);
    }
    unsigned long elapsed = zmq_stopwatch_stop (watch);
    double latency = (double) elapsed / (total_count * 2);
    printf ("average latency: %.3f [us]\n", (double) latency);
    
    zctx_destroy (&context);
    return 0;
}
