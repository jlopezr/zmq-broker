//
//  Show inter-thread signalling using 0MQ sockets
//
#include "zhelpers.h"

static void *
child_thread (void *context)
{
    void *socket = zmq_socket (context, ZMQ_PAIR);
    assert (zmq_connect (socket, "inproc://sink") == 0);

    printf("CLIENT *1*\r\n");
    s_send (socket, "happy");
    printf("CLIENT *2*\r\n");
    s_send (socket, "sad");
    printf("CLIENT *3*\r\n");
    s_send (socket, "done");
    printf("CLIENT *4*\r\n");

    zmq_close (socket);
    return (NULL);
}

int main ()
{
    s_version ();
    //  Threads communicate via shared context
    void *context = zmq_init (1);

    //  Create sink socket, bind to inproc endpoint
    void *socket = zmq_socket (context, ZMQ_PAIR);
    assert (zmq_bind (socket, "inproc://sink") == 0);

    printf("SERVER *0*\r\n");

    //  Start child thread
    pthread_t thread;
    pthread_create (&thread, NULL, child_thread, context);

    //  Get messages from child thread
    while (1) {
    	printf("SERVER *1*\r\n");
        char *mood = s_recv (socket);
    	printf("SERVER *2*\r\n");
        printf ("You're %s\r\n", mood);
        if (strcmp (mood, "done") == 0)
            break;
        free (mood);
    }
    zmq_close (socket);
    zmq_term (context);
    return 0;
}
