#include "czmq.h"
#include "zmq_utils.h"
#include <stdlib.h>

unsigned GetNumberOfDigits (unsigned i)
{
    return i > 0 ? (int) log10 ((double) i) + 1 : 1;
}

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

    char check_dropped_packets = 1;
    if(message_size < GetNumberOfDigits(roundtrip_count)) {
        printf("CAUTION: Message size too small to check for dropped packets\r\n");
        check_dropped_packets = 0;
    }

    int count = 0;
    void* watch = NULL;
    while (count < roundtrip_count && !zctx_interrupted) {
        zmsg_t* msg = zmsg_recv(subscriber);
	//zmsg_dump(msg);

	if(watch == NULL) {
            watch = zmq_stopwatch_start ();
	}

	zframe_t* topic_frame = zmsg_first(msg);
        zframe_t* data_frame = zmsg_next(msg);
        assert(zframe_size(topic_frame)==4);
	assert(zframe_size(data_frame)==message_size);

	if(check_dropped_packets) {
	    int i = atoi(zframe_data(data_frame));
            if(count != i) {
                printf("EXPECTED %d - RECEIVED %d \r\n", i, count);
		i = count;
                //exit(-1);
            }
        }
	
	zmsg_destroy(&msg);
	count++;
    }
    unsigned long elapsed = zmq_stopwatch_stop (watch);
    double latency = (double) elapsed / (roundtrip_count-1);
    printf ("average latency: %.3f [us]\n", (double) latency);
    
    zctx_destroy (&context);
    return 0;
}
