#include <stdio.h>
#include <czmq.h>
#include <string.h>
#include <assert.h>

static int read_msg(void* s, zmq_event_t* event, char* ep)
{
    int rc ;
    zmq_msg_t msg1;  // binary part
    zmq_msg_init (&msg1);
    zmq_msg_t msg2;  //  address part
    zmq_msg_init (&msg2);
    rc = zmq_msg_recv (&msg1, s, 0);
    if (rc == -1 && zmq_errno() == ETERM)
        return 1 ;
    assert (rc != -1);
    assert (zmq_msg_more(&msg1) != 0);
    rc = zmq_msg_recv (&msg2, s, 0);
    if (rc == -1 && zmq_errno() == ETERM)
        return 1;
    assert (rc != -1);
    assert (zmq_msg_more(&msg2) == 0);
    // copy binary data to event struct
    const char* data = (char*)zmq_msg_data(&msg1);
    memcpy(&(event->event), data, sizeof(event->event));
    memcpy(&(event->value), data+sizeof(event->event), sizeof(event->value));
    // copy address part
    const size_t len = zmq_msg_size(&msg2) ;
    ep = memcpy(ep, zmq_msg_data(&msg2), len);
    *(ep + len) = 0 ;
    return 0 ;
}

// REP socket monitor thread
static void rep_socket_monitor (void *args, zctx_t *ctx, void *pipe)
{
    zmq_event_t event;
    static char addr[1025] ;
    int rc;

    printf("starting zmonitor...\n");

    void *s = zsocket_new(ctx, ZMQ_PAIR);
    assert (s);

    rc = zsocket_connect (s, "inproc://monitor.rep");
    assert (rc == 0);

    while (!read_msg(s, &event, addr)) {
        switch (event.event) {
        case ZMQ_EVENT_LISTENING:
            printf ("listening socket descriptor %d\n", event.value);
            printf ("listening socket address %s\n", addr);
            break;
        case ZMQ_EVENT_ACCEPTED:
            printf ("accepted socket descriptor %d\n", event.value);
            printf ("accepted socket address %s\n", addr);
            break;
        case ZMQ_EVENT_CLOSE_FAILED:
            printf ("socket close failure error code %d\n", event.value);
            printf ("socket address %s\n", addr);
            break;
        case ZMQ_EVENT_CLOSED:
            printf ("closed socket descriptor %d\n", event.value);
            printf ("closed socket address %s\n", addr);
            break;
        case ZMQ_EVENT_DISCONNECTED:
            printf ("disconnected socket descriptor %d\n", event.value);
            printf ("disconnected socket address %s\n", addr);
            break;
        }
    }
    //zsocket_destroy(ctx, &s);
    zsocket_disconnect(s, "inproc://monitor.rep");
}

int main()
{
    pthread_t thread;

    //  Create the infrastructure
    zctx_t* ctx = zctx_new();
    assert(ctx);
    
    // REP socket
    void* rep = zsocket_new(ctx, ZMQ_REP);
    assert (rep);

    // REP socket monitor, all events
    int rc = zmq_socket_monitor (rep, "inproc://monitor.rep", ZMQ_EVENT_ALL);
    assert (rc == 0);

    void *pipe = zthread_fork (ctx, rep_socket_monitor, NULL);
    assert (pipe);

    rc = zsocket_bind(rep, "tcp://127.0.0.1:6666" );
    assert (rc == 6666);

    // Allow some time for event detection
    zmq_sleep (10000);

    // Close the REP socket
    zctx_destroy(&ctx);

    return 0; 
}
