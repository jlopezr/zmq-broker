//  Last value cache
//  Uses XPUB subscription messages to re-send data

#include "czmq.h"

int main (void)
{
    zctx_t *context = zctx_new ();
    void *frontend = zsocket_new (context, ZMQ_SUB);
    zsocket_bind (frontend, "tcp://*:5557");
    void *backend = zsocket_new (context, ZMQ_XPUB);
    zsocket_bind (backend, "tcp://*:5558");

    //  .split main poll loop
    //  We route topic updates from frontend to backend, and
    //  we handle subscriptions by sending whatever we cached,
    //  if anything:
    while (true) {
	printf("Polling...\r\n");
        zmq_pollitem_t items [] = {
            { frontend, 0, ZMQ_POLLIN, 0 },
            { backend,  0, ZMQ_POLLIN, 0 }
        };
        if (zmq_poll (items, 2, -1) <= 0)
            break;              //  Interrupted(-1) or no event signaled (0)

	printf("Something read...\r\n");

        //  Any new topic data we cache and then forward
        if (items [0].revents & ZMQ_POLLIN) {
	    printf("Received... ");
            char *topic = zstr_recv (frontend);
            char *current = zstr_recv (frontend);
            if (!topic)
                break;
	    printf("%s : %s \r\n", topic, current);

	    zstr_sendm (backend, topic);
            zstr_send (backend, current);
            free (topic);
        }
        //  .split handle subscriptions
        //  When we get a new subscription, we pull data from the cache:
        if (items [1].revents & ZMQ_POLLIN) {
	    printf("Received new subscription...\r\n");
            zframe_t *frame = zframe_recv (backend);
            if (!frame)
                break;
            //  Event is one byte 0=unsub or 1=sub, followed by topic
            byte *event = zframe_data (frame);
            char *topic = zmalloc (zframe_size (frame));
            memcpy (topic, event + 1, zframe_size (frame) - 1);
            printf ("Topic is %s\n", topic);
	    if (event [0] == 0) {
                printf ("Unsubscribing topic %s\r\n", topic);
		zsocket_set_unsubscribe (frontend, topic);
	    }
	    if (event [0] == 1) {
                printf ("Subscribing topic %s\r\n", topic);
		zsocket_set_subscribe (frontend, topic);
            }
	    free(topic);
            zframe_destroy (&frame);
        }
    } 
    zctx_destroy (&context);
    return 0;
}
