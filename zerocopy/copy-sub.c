#include "czmq.h"
#include "zmq_utils.h"
#include <stdlib.h>

int main (int argc, char *argv [])
{

    if (argc != 4) {
        printf ("usage: copy-sub <connect-to> <message-size> "
                "<roundtrip-count>\n");
        return 1;
    }
    char* connect_to = argv [1];
    long message_size = atoi (argv [2]);
    long roundtrip_count = atoi (argv [3]);

    zctx_t *context = zctx_new ();
    void *subscriber = zsocket_new (context, ZMQ_SUB);
    
    zsocket_set_rcvhwm(subscriber , 0); 

    int hwm = zsocket_rcvhwm(subscriber);
    printf("HMW=%d\r\n", hwm);
    
    zsocket_connect (subscriber, connect_to);
    zsocket_set_subscribe (subscriber, "TEST");

    int total_count = 100000; 

    int count = 0;
    void* watch = zmq_stopwatch_start ();
    while (count < roundtrip_count && !zctx_interrupted) {
        zmsg_t* msg = zmsg_recv(subscriber);
	//zmsg_dump(msg);

	zframe_t* topic_frame = zmsg_first(msg);
        zframe_t* data_frame = zmsg_next(msg);
        assert(zframe_size(topic_frame)==4);
	assert(zframe_size(data_frame)==message_size);
	int i = atoi(zframe_data(data_frame));

	if(i != count) {
	    printf("EXPECTED %d - RECEIVED %d \r\n", count, i);
	    count = i;
	    //exit(-1);	      
	}
	
	zmsg_destroy(&msg);
	count++;
    }
    unsigned long elapsed = zmq_stopwatch_stop (watch);
    double latency = (double) elapsed / (roundtrip_count * 2);
    printf ("average latency: %.3f [us]\n", (double) latency);
    
    zctx_destroy (&context);
    return 0;
}
