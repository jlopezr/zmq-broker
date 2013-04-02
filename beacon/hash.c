#include<czmq.h>
#include<string.h>

int func(const char *key, void *item, void *argument) {
    printf("%s => %d\r\n",key,*((int*)item));
    return 0;
}

int func2(const char *key, void *item, void *argument) {
    int* tmp = (int*)item;
    	if( (*tmp%2) == 0) {
       zhash_delete(argument, key );
    }
    return 0;
}

int main(void) {
    zhash_t* h = zhash_new();

    void* d1 = malloc(sizeof(int));
    void* d2 = malloc(sizeof(int));
    void* d3 = malloc(sizeof(int));
    void* d4 = malloc(sizeof(int));
    void* d5 = malloc(sizeof(int));

    *((int*)d1) = 1;
    *((int*)d2) = 2;
    *((int*)d3) = 3;
    *((int*)d4) = 4;
    *((int*)d5) = 5;

    zhash_insert(h, "K1", d1);
    zhash_insert(h, "K2", d2);
    zhash_insert(h, "K3", d3);
    zhash_insert(h, "K4", d4);

    zhash_update(h, "K5", d5);



    zhash_foreach(h, func, NULL);
    zhash_foreach(h, func2, h);
    printf("-----------------\r\n");
    zhash_foreach(h, func, NULL);

    zhash_destroy(&h);
    return 0;
}
