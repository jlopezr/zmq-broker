//  Pathological publisher
//  Sends out 1,000 topics and then one random update per second

#include "czmq.h"

int main (int argc, char *argv [])
{
    zctx_t *context = zctx_new ();
    void *publisher = zsocket_new (context, ZMQ_XPUB);
    printf("Connecting to tcp://localhost:5556\r\n");
    zsocket_connect (publisher, "tcp://localhost:5556");
    
    int total_count = 100; 
    int message_size = 1024;
    void* data;
    data = malloc(message_size);
    bzero(data, message_size);
    int* pointer= (int*)data;

    //Wait for sub connection
    printf("Waiting for subscriber.\r\n");
    zmsg_t* connection = zmsg_recv(publisher);
    zmsg_destroy(&connection);
    zclock_sleep(1000);
    printf("Subscriber connected!\r\n");

    int i = 0;
    while (i<total_count && !zctx_interrupted) {
        *pointer = i;
        //printf("Sending...\r\n");
        zmsg_t* msg = zmsg_new();
	zframe_t* topic_frame = zframe_new("TEST", 4);
        zframe_t* data_frame = zframe_new(data, 1024);
	zmsg_add (msg, topic_frame);
	zmsg_add (msg, data_frame);
	zmsg_send (&msg, publisher);
	i++;
	printf("Count %d\r\n", i);
    }

    zctx_set_linger(context, 1000);
    zctx_destroy (&context);
    return 0;
}
