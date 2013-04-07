#include <czmq.h>

static void client(void* args, zctx_t *context, void* pipe) {
    assert(context!=0);

    void *client = zsocket_new (context, ZMQ_REQ);
    //zsocket_set_identity(client, "client"); 
    zsocket_connect (client, "inproc://example");
   
    int request_nbr=0; 
    while(!zctx_interrupted) {
	zstr_sendm(client, args);
	zstr_sendf(client, "%d", ++request_nbr);
        char* data = zstr_recv(client);
	printf("CLIENT OP: %s RESULT: %s\r\n", (char*)args, data);
        free(data);
    }
}

static void server(void* args, zctx_t* context, void* pipe) {
    assert(context!=0);

    void *sink = zsocket_new (context, ZMQ_REP);
    zsocket_bind (sink, "inproc://example");

    while(!zctx_interrupted) {
        char* func= zstr_recv(sink);
        char* data = zstr_recv(sink);
	printf("SERVER FUNC: %s\r\n", func);
	int i = atoi(data);
	int result;
	if(strcmp(func,"SUMA")==0) {
            result = i+1;
        } else if(strcmp(func,"MULT")==0) {
            result = i*2;
	} else {
	  assert(0 && "FUNC NOT KNOWN!");
	}
	zstr_sendf(sink, "%d", result);
        free(func);
	free(data);
    }
}

int main (void)
{
    zctx_t* context = zctx_new();
    assert(context!=0);
    
    zthread_fork (context, server, NULL);
    sleep(1);
    zthread_fork (context, client, "SUMA");
    zthread_fork (context, client, "MULT");

    while(!zctx_interrupted) {
       sleep(1);
    }

    printf("DESTROYING CTX\r\n");
    zmq_ctx_destroy (&context);
    printf("CTX DESTROYED\r\n");
    return 0;
}
