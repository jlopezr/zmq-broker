#include "czmq.h"
#include "zmq.h"

void my_free (void *data, void *hint)
{
    free (data);
}

int main (int argc, char *argv [])
{
   if (argc != 4) {
        printf ("usage: %s <bind-to> <message-size> "
                "<roundtrip-count>\n", argv[0]);
        return 1;
    }
    char* bind_to = argv [1];
    long message_size = atoi (argv [2]);
    long roundtrip_count = atoi (argv [3]);

    zctx_t *context = zctx_new ();
    void *publisher = zsocket_new (context, ZMQ_XPUB);
    
    zctx_set_linger(context, 1000);
    zsocket_set_sndhwm(publisher, 1000);
    int hwm = zsocket_sndhwm(publisher);
    printf("HMW=%d\r\n", hwm);

#ifdef ZMQ_PUB_RELIABLE
    // set PUB_RELIABLE
    int pub_reliable = 1;
    int rc = zmq_setsockopt(publisher, ZMQ_PUB_RELIABLE, &pub_reliable, sizeof(pub_reliable));
    if (rc != 0) {
        printf ("error in zmq_setsockopt (ZMQ_PUB_RELIABLE): %s\n", zmq_strerror (errno));
        return -1;
    }
#endif

    printf("Connecting to %s\r\n", bind_to);
    zsocket_bind (publisher, bind_to);

    //Wait for sub connection
    printf("Waiting for subscriber.\r\n");
    zmsg_t* connection = zmsg_recv(publisher);
    zmsg_destroy(&connection);
    printf("Subscriber connected!\r\n");

    int i = 0;
    while (i<roundtrip_count && !zctx_interrupted) {
        void* data = malloc(message_size);
        bzero(data, message_size);
	sprintf(data, "%d", i);

	//zframe_t * zframe_new_zero_copy (void *data, size_t size, zframe_free_fn *free_fn, void *arg);
        zframe_t* frame = zframe_new_zero_copy(data, message_size, my_free, NULL);

        zmsg_t* msg = zmsg_new();
	zmsg_addstr(msg, "TEST", 4);	
        //zmsg_addmem(msg, data, message_size);
	zmsg_add(msg, frame);

	//zmsg_dump(msg);
	zmsg_send (&msg, publisher);
	i++;
	//free(data);
    }

    zctx_destroy (&context);
    return 0;
}
