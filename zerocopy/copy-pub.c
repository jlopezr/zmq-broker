#include "czmq.h"
#include "zmq.h"

unsigned GetNumberOfDigits (unsigned i)
{
    return i > 0 ? (int) log10 ((double) i) + 1 : 1;
}

int main (int argc, char *argv [])
{
   if (argc != 4) {
        printf ("usage: copy-pub <bind-to> <message-size> "
                "<roundtrip-count>\n");
        return 1;
    }
    char* bind_to = argv [1];
    long message_size = atoi (argv [2]);
    long roundtrip_count = atoi (argv [3]);

    char check_dropped_packets = 1;
    if(message_size < GetNumberOfDigits(roundtrip_count)) {
        printf("CAUTION: Message size too small to check for dropped packets\r\n");
        check_dropped_packets = 0;
    }

    zctx_t *context = zctx_new ();
    void *publisher = zsocket_new (context, ZMQ_XPUB);
    
    zctx_set_linger(context, 1000);
    zsocket_set_sndhwm(publisher, 1000);
    int hwm = zsocket_sndhwm(publisher);
    printf("HMW=%d\r\n", hwm);

    // set PUB_RELIABLE
    int pub_reliable = 1;
    int rc = zmq_setsockopt(publisher, ZMQ_PUB_RELIABLE, &pub_reliable, sizeof(pub_reliable));
    if (rc != 0) {
        printf ("error in zmq_setsockopt (ZMQ_PUB_RELIABLE): %s\n", zmq_strerror (errno));
        return -1;
    }

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
        if(check_dropped_packets) {
	    sprintf(data, "%d", i);
        }

        zmsg_t* msg = zmsg_new();
	zmsg_addstr(msg, "TEST", 4);	
        zmsg_addmem(msg, data, message_size);

	//zmsg_dump(msg);
	zmsg_send (&msg, publisher);
	i++;
	free(data);
    }

    zctx_destroy (&context);
    return 0;
}
