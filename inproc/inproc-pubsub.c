#include <czmq.h>

static void client2(void* args, zctx_t *context, void* pipe) {
    assert(context!=0);

    void *client = zsocket_new (context, ZMQ_SUB);
    zsocket_connect (client, "inproc://example");
    zsocket_set_subscribe (client, args);

    int request_nbr=0; 
    while(!zctx_interrupted) {
        char* topic = zstr_recv(client);
        char* data = zstr_recv(client);
	printf("*KEY: %s VALUE: %s\r\n", topic, data);
        free(topic);
	free(data);
    }
}

static void client(void* args, zctx_t *context, void* pipe) {
    assert(context!=0);

    void *client = zsocket_new (context, ZMQ_SUB);
    zsocket_connect (client, "inproc://example");
    zsocket_set_subscribe (client, args);

    int request_nbr=0; 
    while(!zctx_interrupted) {
        char* topic = zstr_recv(client);
        char* data = zstr_recv(client);
	printf("KEY: %s VALUE: %s\r\n", topic, data);
        free(topic);
	free(data);
    }
}

static void server(void* args, zctx_t* context, void* pipe) {
    assert(context!=0);

    void *sink = zsocket_new (context, ZMQ_PUB);
    zsocket_bind (sink, "inproc://example");

    int i=0;
    while(!zctx_interrupted) {
	zstr_sendm(sink, args);
	zstr_sendf(sink, "%d", ++i);
    }
}

int main (void)
{
    zctx_t* context = zctx_new();
    assert(context!=0);
    
    zthread_fork (context, server, "AA");
    sleep(1);
    zthread_fork (context, client, "AA");
    zthread_fork (context, client2, "XAA");

    while(!zctx_interrupted) {
       sleep(1);
    }

    printf("DESTROYING CTX\r\n");
    zmq_ctx_destroy (&context);
    printf("CTX DESTROYED\r\n");
    return 0;
}
