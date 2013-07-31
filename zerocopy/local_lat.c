#include "../include/zmq.h"
#include "../include/zmq_utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <math.h>

/// Initialize a zeromq message with a given null-terminated string
#define ZMQ_PREPARE_STRING(msg, data, size) \
zmq_msg_init(&msg) && printf("zmq_msg_init: %s\n", zmq_strerror(errno)); \
zmq_msg_init_size (&msg, size + 1) && printf("zmq_msg_init_size: %s\n",zmq_strerror(errno)); \
memcpy(zmq_msg_data(&msg), data, size + 1);

unsigned GetNumberOfDigits (unsigned i)
{
    return i > 0 ? (int) log10 ((double) i) + 1 : 1;
}

void my_free (void *data, void *hint)
{
    free (data);
}

int main (int argc, char *argv [])
{
    const char *bind_to;
    int roundtrip_count;
    size_t message_size;
    void *ctx;
    void *s;
    int rc;
    int i;
    zmq_msg_t msg;
    zmq_msg_t topic;
    
    if (argc != 4) {
        printf ("usage: %s <bind-to> <message-size> "
                "<roundtrip-count>\n", argv[0]);
        return 1;
    }
    bind_to = argv [1];
    message_size = atoi (argv [2]);
    roundtrip_count = atoi (argv [3]);
    
    ctx = zmq_init (1);
    if (!ctx) {
        printf ("error in zmq_init: %s\n", zmq_strerror (errno));
        return -1;
    }
    
    s = zmq_socket (ctx, ZMQ_XPUB);
    if (!s) {
        printf ("error in zmq_socket: %s\n", zmq_strerror (errno));
        return -1;
    }
    
    char check_dropped_packets = 1;
    if(message_size < GetNumberOfDigits(roundtrip_count)) {
        printf("CAUTION: Message size too small to check for dropped packets\r\n");
        check_dropped_packets = 0;
    }

    // set PUB_RELIABLE
    int pub_reliable = 1;
    rc = zmq_setsockopt(s, ZMQ_PUB_RELIABLE, &pub_reliable, sizeof(pub_reliable));
    if (rc != 0) {
        printf ("error in zmq_setsockopt (ZMQ_PUB_RELIABLE): %s\n", zmq_strerror (errno));
        return -1;
    }

    // set infinity HWM
    int hwm = 1000;
    rc = zmq_setsockopt(s, ZMQ_SNDHWM, &hwm , sizeof(hwm));
    if (rc != 0) {
        printf ("error in zmq_setsockopt (ZMQ_SNDHWM): %s\n", zmq_strerror (errno));
        return -1;
    }
    
    // get HWM
    size_t hwmlen = sizeof(hwm);
    rc = zmq_getsockopt(s, ZMQ_SNDHWM, &hwm , &hwmlen);
    if (rc != 0) {
        printf ("error in zmq_getsockopt (ZMQ_SNDHWM): %s\n", zmq_strerror (errno));
        return -1;
    }
    printf("HWM=%d\r\n", hwm);
    
    rc = zmq_bind (s, bind_to);
    if (rc != 0) {
        printf ("error in zmq_bind: %s\n", zmq_strerror (errno));
        return -1;
    }
    
    rc = zmq_msg_init (&msg);
    if (rc != 0) {
        printf ("error in zmq_msg_init: %s\n", zmq_strerror (errno));
        return -1;
    }
       
    printf("Waiting for subscriber.\r\n");
    rc = zmq_recvmsg (s, &msg, 0);
    if (rc < 0) {
        printf ("error in zmq_recvmsg: %s\n", zmq_strerror (errno));
        return -1;
    }
    printf("Subscriber detected!\n");
    
    rc = zmq_msg_close (&msg);
    if (rc != 0) {
        printf ("error in zmq_msg_close: %s\n", zmq_strerror (errno));
        return -1;
    }
    
    for (i = 0; i != roundtrip_count; i++) {
        //PREPARE MSGS
        ZMQ_PREPARE_STRING(topic, "TEST", 4);
       
        void* buffer = malloc(message_size);
        memset (buffer, 0, message_size);
        
	if(check_dropped_packets) {
            sprintf(buffer, "%d", i);
        }

        rc = zmq_msg_init_data (&msg, buffer, message_size, my_free, NULL);
        if (rc != 0) {
            printf ("error in zmq_msg_init_data: %s\n", zmq_strerror (errno));
            return -1;
        }
        
        rc = zmq_msg_send (&topic, s, ZMQ_SNDMORE);
        if (rc < 0) {
            printf ("error in zmq_sendmsg: %s\n", zmq_strerror (errno));
            return -1;
        }
        rc = zmq_msg_send (&msg, s, 0);
        if (rc < 0) {
            printf ("error in zmq_sendmsg: %s\n", zmq_strerror (errno));
            return -1;
        }
	    //printf("SENT %d\n",i);
    }
    
    rc = zmq_msg_close (&topic);
    if (rc != 0) {
        printf ("error in zmq_msg_close: %s\n", zmq_strerror (errno));
        return -1;
    }
    
    rc = zmq_msg_close (&msg);
    if (rc != 0) {
        printf ("error in zmq_msg_close: %s\n", zmq_strerror (errno));
        return -1;
    }
    
    zmq_sleep (1);
    
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
