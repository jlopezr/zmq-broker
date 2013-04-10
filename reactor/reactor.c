#include <czmq.h>

typedef int (*mz_reactor_fn)(char* key, char* data);

typedef struct mz_reactor_t {
    zhash_t* events;
} mz_reactor_t;

mz_reactor_t* mz_reactor_new() {
    mz_reactor_t* ret = malloc(sizeof(mz_reactor_t));
    ret->events = zhash_new();
    return ret;
}

void mz_reactor_destroy(mz_reactor_t** rct) {
    zhash_destroy(&((*rct)->events)); 
    free(*rct);
    rct = NULL;	
}

int key_A_fn(char* key, char* data) {
   printf("KEY A => %s => %s\r\n", key, data);
   return 0;
}

int key_B_fn(char* key, char* data) {
   printf("KEY B => %s => %s\r\n", key, data);
   return 0;
}

int mz_reactor_event(mz_reactor_t* rct, char* key, char* data) {
    mz_reactor_fn fn = (mz_reactor_fn)zhash_lookup(rct->events, key);
    printf("LOOKUP %p \r\n", fn);
    printf("FUNCT. %p \r\n", key_A_fn);
    if(fn!=NULL) {
	return fn(key, data);
    } else {
	assert(0 && "Key not found in events");
    }
    return -1;
}

int mz_reactor_run() {
    return -1;
}

void mz_reactor_add(mz_reactor_t* rct, char* key, mz_reactor_fn fn) {
    zhash_update(rct->events, key, fn);
}

void mz_reactor_remove(mz_reactor_t* rct, char* key) {
    zhash_delete(rct->events, key);
}

int main() {
  
   mz_reactor_t* rct = mz_reactor_new();
   mz_reactor_add(rct, "A", key_A_fn);
   mz_reactor_add(rct, "B", key_B_fn);
   mz_reactor_add(rct, "C", key_B_fn);

   mz_reactor_event(rct, "A", "QUE PASA NEN 1");
   mz_reactor_event(rct, "B", "QUE PASA NEN 2");
   mz_reactor_event(rct, "C", "QUE PASA NEN 3");
   
   mz_reactor_destroy(&rct);   
   
   return 0;
}
