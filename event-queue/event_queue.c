#ifndef _EVENT_QUEUE_H
#define _EVENT_QUEUE_H

#include <czmq.h>
#include <stdio.h>
#include <assert.h>

typedef void (*event_queue_fn)(void* data);

typedef struct event_queue_t {
    zlist_t* events;
} event_queue_t;

typedef struct event_queue_item_t {
    long time;
    event_queue_fn function; 
    void* data;
} event_queue_item_t;

event_queue_t* event_queue_new();
void event_queue_destroy(event_queue_t** queue);
void event_queue_add(event_queue_t* queue, long ms_to_wait, event_queue_fn pointer, void* data);
int event_queue_run(event_queue_t* queue);

#endif

event_queue_t* event_queue_new() {
    event_queue_t* ret = malloc(sizeof(event_queue_t));
    ret->events = zlist_new();
    return ret;
}

void event_queue_destroy(event_queue_t** queue) {
    //TODO empty the queue
    zlist_destroy(&((*queue)->events)); 
    free(*queue);
    queue = NULL;	
}

bool event_queue_item_compare_fn(void *item1, void *item2) {
    event_queue_item_t* event1 = (event_queue_item_t*)item1;
    event_queue_item_t* event2 = (event_queue_item_t*)item2;
    return event1->time > event2->time;
}

void event_queue_add(event_queue_t* queue, long ms_to_wait, event_queue_fn function, void* data) {
    long now = zclock_time();
    event_queue_item_t* item = malloc(sizeof(event_queue_item_t));
    item->function = function;
    item->data = data;
    item->time = now + ms_to_wait;

    event_queue_item_t* tail = (event_queue_item_t*)zlist_tail(queue->events); 
    zlist_append(queue->events, item);
    if(tail!=NULL && tail->time > item->time) {
        // keep the list ordered
        printf("SORTING...\r\n");
	zlist_sort(queue->events, event_queue_item_compare_fn);
    }
}

// Do not destroy the data, the event_queue_fn should destroy it if it is needed.
int event_queue_run(event_queue_t* queue) {
   long now = zclock_time();
   int events_run = 0;

   // Get the first event
   event_queue_item_t* item = (event_queue_item_t*)zlist_first(queue->events);

   printf("event_queue_run():\r\n");

   while(item!=NULL) {
      printf("NOW %lu \r\n", now);
      printf("1ST %lu \r\n", item->time);
      //If the time has pass, run the event and increment the count
      if(item->time <= now) {
	  item = zlist_pop(queue->events);
          printf("RUN\r\n");
          item->function(item->data);
	  free(item); 
          events_run++;
          //Get the next event
          item = zlist_first(queue->events);
      } else {
	  item = NULL;    
      }
   }
   
   return events_run;
}

void func1(void *data) {
    char* str = (char*)data;
    printf("FUNC1 [%s]\r\n", str);
}

void func2(void *data) {
    char* str = (char*)data;
    printf("FUNC2 [%s]\r\n", str);
}

int main() {
   event_queue_t* queue = event_queue_new();
   
   printf("TEST 1\r\n");
   event_queue_add(queue, 1000, func1, "HOLA"); 
   assert(event_queue_run(queue)==0);
   zclock_sleep(1000);
   assert(event_queue_run(queue)==1);

   printf("TEST 2\r\n");
   event_queue_add(queue, 1000, func1, "HOLA");
   event_queue_add(queue, 2000, func2, "ADIOS");
   assert(event_queue_run(queue)==0);
   zclock_sleep(1000);
   assert(event_queue_run(queue)==1);
   zclock_sleep(1000);
   assert(event_queue_run(queue)==1);

   printf("TEST 3\r\n");
   event_queue_add(queue, 2000, func1, "ADIOS");
   event_queue_add(queue, 1000, func2, "HOLA");
   assert(event_queue_run(queue)==0);
   zclock_sleep(1000);
   assert(event_queue_run(queue)==1);
   zclock_sleep(1000);
   assert(event_queue_run(queue)==1);

   /*
   event_queue_add(queue, 1000, func1, "HOLA");
   event_queue_add(queue, 2000, func2, "ADIOS");
   assert(event_queue_run(queue)==0);
   zclock_sleep(2000);
   assert(event_queue_run(queue)==2);
   */
   event_queue_destroy(&queue);
   return 0;
}
