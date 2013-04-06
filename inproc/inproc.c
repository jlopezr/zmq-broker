#include <czmq.h>

void* client(void* args) {
    void* context = args;

    void *client = zmq_socket (context, ZMQ_REQ);
    zmq_setsockopt (client, ZMQ_IDENTITY, "client", 6); //Optional
    zmq_connect (client, "inproc://example");
    
    while(!zctx_interrupted) {
        printf("c");
	sleep(1);
    }
    zmq_close(client);
    return NULL;
}

void* server(void* args) {
    void* context = args;
    void *sink = zmq_socket (context, ZMQ_ROUTER);
    zmq_bind (sink, "inproc://example");

    while(!zctx_interrupted) {
        printf("s");
	sleep(1);
    }
    zmq_close(sink);
    return NULL;
}

int main (void) 
{
    void *context = zmq_ctx_new ();

    zthread_new(client, context);
    zthread_new(server, context);

    sleep(5);
    printf("DESTROYING CTX\r\n");
    zmq_ctx_destroy (&context);
    printf("CTX DESTROYED\r\n");
    return 0;
}
