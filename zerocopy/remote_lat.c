#include "../include/zmq.h"
#include "../include/zmq_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>

unsigned GetNumberOfDigits (unsigned i)
{
    return i > 0 ? (int) log10 ((double) i) + 1 : 1;
}

int main (int argc, char *argv [])
{
    const char *connect_to;
    int roundtrip_count;
    size_t message_size;
    void *ctx;
    void *s;
    int rc;
    int i;
    zmq_msg_t msg;
    void *watch = NULL;
    unsigned long elapsed;
    double latency;
    
    if (argc != 4) {
        printf ("usage: remote_lat <connect-to> <message-size> "
                "<roundtrip-count>\n");
        return 1;
    }
    connect_to = argv [1];
    message_size = atoi (argv [2]);
    roundtrip_count = atoi (argv [3]);
    
    char check_dropped_packets = 1;
    if(message_size < GetNumberOfDigits(roundtrip_count)) {
        printf("CAUTION: Message size too small to check for dropped packets\r\n");
        check_dropped_packets = 0;
    }
    
    ctx = zmq_init (1);
    if (!ctx) {
        printf ("error in zmq_init: %s\n", zmq_strerror (errno));
        return -1;
    }
    
    s = zmq_socket (ctx, ZMQ_SUB);
    if (!s) {
        printf ("error in zmq_socket: %s\n", zmq_strerror (errno));
        return -1;
    }
    
    // set infinity HWM
    int hwm = 0;
    rc = zmq_setsockopt(s, ZMQ_RCVHWM, &hwm , sizeof(hwm));
    if (rc != 0) {
        printf ("error in zmq_setsockopt (ZMQ_RCVHWM): %s\n", zmq_strerror (errno));
        return -1;
    }
    
    // get HWM
    size_t hwmlen = sizeof(hwm);
    rc = zmq_getsockopt(s, ZMQ_RCVHWM, &hwm , &hwmlen);
    if (rc != 0) {
        printf ("error in zmq_getsockopt (ZMQ_RCVHWM): %s\n", zmq_strerror (errno));
        return -1;
    }
    printf("HWM=%d\r\n", hwm);
    
    rc = zmq_connect (s, connect_to);
    if (rc != 0) {
        printf ("error in zmq_connect: %s\n", zmq_strerror (errno));
        return -1;
    }
    
    rc = zmq_setsockopt(s, ZMQ_SUBSCRIBE, "TEST", 4);
    if (rc != 0) {
        printf ("error in zmq_setsockopt: %s\n", zmq_strerror (errno));
        return -1;
    }
    
    rc = zmq_msg_init(&msg);
    if (rc != 0) {
        printf ("error in zmq_msg_init: %s\n", zmq_strerror (errno));
        return -1;
    }
    
     
    for (i = 0; i != roundtrip_count; i++) {
 

        //printf("RECEIVING %d\r\n", i);
        rc = zmq_msg_recv(&msg, s, 0);
        if (rc < 0) {
            printf ("error in zmq_msg_recv: %s\n", zmq_strerror (errno));
            return -1;
        }

        // Start timer after the first message (we don't want to take the delay between client and server)
	if(watch == NULL) {
           watch = zmq_stopwatch_start ();
	}

        //printf("TOPIC SIZE %zd\r\n", zmq_msg_size(&msg));
        if (zmq_msg_size (&msg) != 5) {
            printf ("message of incorrect size received (*1*)\n");
            return -1;
        }
        
        /* Determine if more message parts are to follow */
        int more;
        size_t more_size = sizeof(more);
        rc = zmq_getsockopt (s, ZMQ_RCVMORE, &more, &more_size);
        if (rc != 0) {
            printf ("error in zmq_getsockopt: %s\n", zmq_strerror (errno));
            return -1;
        }
        assert(more!=0);
        
        rc = zmq_msg_recv(&msg, s, 0);
        if (rc < 0) {
            printf ("error in zmq_msg_recv: %s\n", zmq_strerror (errno));
            return -1;
        }
        if (zmq_msg_size (&msg) != message_size) {
            printf ("message of incorrect size received (*2*)\n");
            return -1;
        }
        //printf("DATA SIZE %zd\r\n", zmq_msg_size(&msg));
        if(check_dropped_packets) {
            int n = atoi(zmq_msg_data(&msg));
            if(n!=i) {
                printf("EXPECTED %d - RECEIVED %d \r\n", i, n);
		i = n;
                //exit(-1);
            }
        }
        
        /* Determine if more message parts are to follow */
        rc = zmq_getsockopt (s, ZMQ_RCVMORE, &more, &more_size);
        if (rc != 0) {
            printf ("error in zmq_getsockopt: %s\n", zmq_strerror (errno));
            return -1;
        }
        assert(more==0);
    }
    
    elapsed = zmq_stopwatch_stop (watch);
    
    rc = zmq_msg_close (&msg);
    if (rc != 0) {
        printf ("error in zmq_msg_close: %s\n", zmq_strerror (errno));
        return -1;
    }
    
    latency = (double) elapsed / (roundtrip_count-1); // discard first message
    
    printf ("message size: %d [B]\n", (int) message_size);
    printf ("roundtrip count: %d\n", (int) roundtrip_count);
    printf ("average latency: %.3f [us]\n", (double) latency);
    
    rc = zmq_close (s);
    if (rc != 0) {
        printf ("error in zmq_close: %s\n", zmq_strerror (errno));
        return -1;
    }
    
    rc = zmq_term (ctx);
    if (rc != 0) {
        printf ("error in zmq_term: %s\n", zmq_strerror (errno));
        return -1;
    }
    
    return 0;
}
