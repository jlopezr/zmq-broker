#include "czmq.h"

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

    zctx_t *context = zctx_new ();
    void *publisher = zsocket_new (context, ZMQ_XPUB);
    printf("Connecting to %s\r\n", bind_to);
    zsocket_bind (publisher, bind_to);
    zctx_set_linger(context, 1000);
  
    zsocket_set_sndhwm(publisher, 0);

    int hwm = zsocket_sndhwm(publisher);
    printf("HMW=%d\r\n", hwm);

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
        
        zmsg_t* msg = zmsg_new();
	zmsg_addstr(msg, "TEST", 4);	
        zmsg_addmem(msg, data, message_size);

	//zmsg_dump(msg);
	zmsg_send (&msg, publisher);
	i++;
	free(data);
	zclock_sleep(1);
    }

    zctx_destroy (&context);
    return 0;
}
