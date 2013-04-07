#include <czmq.h>

static void client(void* args, zctx_t *context, void* pipe) {
    assert(context!=0);

    void *client = zsocket_new (context, ZMQ_REQ);
    //zsocket_set_identity(client, "client"); 
    zsocket_connect (client, "inproc://example");
   
    int request_nbr=0; 
    while(!zctx_interrupted) {
	zstr_send(client, args);
        char* data = zstr_recv(client);
	assert(strcmp(args,data)==0);
	printf("CLIENT RESULT: %s\r\n",data);	
    }
}

static void server(void* args, zctx_t* context, void* pipe) {
    assert(context!=0);

    void *sink = zsocket_new (context, ZMQ_REP);
    zsocket_bind (sink, "inproc://example");

    while(!zctx_interrupted) {
        char* data = zstr_recv(sink);
        if(data==NULL) {
	     printf("NO MESSAGE RECEIVED\r\n");
	     sleep(1);
             continue;
	}
	zstr_send(sink, data);
    }
}

int main (void)
{
    zctx_t* context = zctx_new();
    assert(context!=0);
    
    zthread_fork (context, server, NULL);
    sleep(1);
    zthread_fork (context, client, "A");
    zthread_fork (context, client, "B");

    while(!zctx_interrupted) {
       sleep(1);
    }

    printf("DESTROYING CTX\r\n");
    zmq_ctx_destroy (&context);
    printf("CTX DESTROYED\r\n");
    return 0;
}
