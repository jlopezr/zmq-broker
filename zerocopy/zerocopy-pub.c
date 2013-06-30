//  Pathological publisher
//  Sends out 1,000 topics and then one random update per second

#include "czmq.h"

void free_frame(void *data, void *arg) {
  free(data);
}

void free_static_frame(void *data, void *arg) {
  //DO NOTHING 
}
	
int main (int argc, char *argv [])
{
    zctx_t *context = zctx_new ();
    void *publisher = zsocket_new (context, ZMQ_PUB);
    printf("Connecting to tcp://localhost:5556\r\n");
    zsocket_connect (publisher, "tcp://localhost:5556");
    
    int total_count = 1000000; 
    int message_size = 1024;
    void* data;
    data = malloc(message_size);
    bzero(data, message_size);
    int* pointer= (int*)data;

    int i = 0;
    while (i<total_count && !zctx_interrupted) {
        *pointer = i;
	//printf("Sending...\r\n");
        zmsg_t* msg = zmsg_new();
        zframe_t* topic_frame = zframe_new_zero_copy("TEST", 4, free_static_frame, NULL); 
        zframe_t* data_frame = zframe_new_zero_copy(data, 1024, free_static_frame, NULL); //DO NOT FREE BUFFER ALWAYS USING THE SAME
	zmsg_add (msg, topic_frame);
	zmsg_add (msg, data_frame);
	zmsg_send (&msg, publisher);
	i++;
    }
    
    zclock_sleep(1000);
    zctx_destroy (&context);
    return 0;
}
