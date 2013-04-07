#include <czmq.h>

static void client(void* args, zctx_t *context, void* pipe) {
    assert(context!=0);

    void *client = zsocket_new (context, ZMQ_REQ);
    //zsocket_set_identity(client, "client"); 
    zsocket_connect (client, "inproc://example");
   
    int request_nbr=0; 
    while(!zctx_interrupted) {
        printf("CLIENT *0*\r\n");
	zstr_send(client, "HOLA");
        printf("CLIENT *1*\r\n");
        char* data = zstr_recv(client);
	printf("CLIENT RESULT: %s\r\n",data);	
    }
    //zsocket_destroy(context, client);
}

static void server(void* args, zctx_t* context, void* pipe) {
    assert(context!=0);

    void *sink = zmq_socket (context, ZMQ_REP);
    zmq_bind (sink, "inproc://example");

    while(!zctx_interrupted) {
        printf("SERVER *0*\r\n");
	char* data = zstr_recv(server);
        if(data==NULL) {
             continue;
	}
	printf("SERVER *1*\r\n");
	zstr_send(server, data);
        printf("SERVER *2*\r\n");
    }
    //zsocket_destroy(context, server);
}

int main (void)
{
    printf("STARTING\r\n");
    zctx_t* context = zctx_new();
    assert(context!=0);
    printf("STARTED\r\n");
    
    zthread_fork (context, server, NULL);
    sleep(1);
    zthread_fork (context, client, NULL);

    while(!zctx_interrupted) {
       sleep(1);
    }

    printf("DESTROYING CTX\r\n");
    zmq_ctx_destroy (&context);
    printf("CTX DESTROYED\r\n");
    return 0;
}
