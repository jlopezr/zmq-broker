//  Pathological subscriber
//  Subscribes to one random topic and prints received messages

#include "czmq.h"
#include <stdlib.h>

#include "receiver.c"

int main (int argc, char *argv [])
{
    if (argc < 2) {
        printf("usage: %s <subscription>\r\n",argv[0]);
        return -1;
    }
    
    zctx_t *context = zctx_new ();
    void *subscriber = zsocket_new (context, ZMQ_SUB);

    start_discovery(context);
    char* location = NULL;

    do {
        printf("Discovering %s ...\r\n",argv[1]);
        sleep(1);
        location = discover(context, argv[1]);
	if(*location!=0) {
	    break;
	}
	free(location);
	location = NULL;
    } while(!zctx_interrupted) ;

    if(zctx_interrupted) {
        goto cleanup;
    }

    printf("Connecting to %s\r\n", location);
    zsocket_connect (subscriber, location);
    printf("Subscribed to %s\r\n",argv[1]);
    zsocket_set_subscribe (subscriber, argv[1]);

    void* svc_changes = zsocket_new(context, ZMQ_SUB);
    zsocket_connect (svc_changes, "inproc://discovery-changes");
    zsocket_set_subscribe (svc_changes, argv[1]);

    zmq_pollitem_t items[] = {
        { subscriber, 0, ZMQ_POLLIN, 0 },
        { svc_changes, 0, ZMQ_POLLIN, 0}
    };

    int i=0;
    while (!zctx_interrupted && i < 10) {
        i++;
	
	if(zmq_poll(items, 2, -1) <= 0) {
            break; // interrupted (-1) or no events (0)
        }

        //NUEVOS DATOS
        if(items[0].revents & ZMQ_POLLIN) {
            char *topic = zstr_recv (subscriber);
            if (!topic)
                break;
            char *data = zstr_recv (subscriber);
            puts (data);
            free (topic);
            free (data);
        }

        //CAMBIO DE PUBLICADOR
        if(items[1].revents & ZMQ_POLLIN) {
	    char* topic = zstr_recv (svc_changes);
            char* url = zstr_recv (svc_changes);

            //printf("TOPIC    <%s>\r\n", topic);         
            //printf("URL      <%s>\r\n", url);         
            //printf("LOCATION <%s>\r\n", location);         

            if(strcmp(topic,argv[1])==0) {
		
		if(location!=NULL) {
		    int rc = zsocket_disconnect(subscriber, location);
                    assert(rc==0);
                    free(location);
		    location = NULL;
                }

                if(*url!=0) {
                    location = url;
                    url = NULL;
                    zsocket_connect(subscriber, location);
                }  
            }
            if(url!=NULL) {
                free(url);
            }
            free(topic);
        }
    }

    cleanup:

    if(location!=NULL) {
    	free(location);
    }
    printf("DESTROYING CTX\r\n");
    stop_discovery(context);
    zctx_destroy (&context);
    printf("CTX DESTROYED\r\n");

    return 0;
}
