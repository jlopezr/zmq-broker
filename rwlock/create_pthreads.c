#define _MULTI_THREADED
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "check.h"

void* func(void* arg) {
	printf("Wait 1000...\n");
	sleep(1000);
	printf("Finishing thread.\n");
        return NULL;
}

int main (int argc, char **argv)
{
  int                   rc=0;
  pthread_t*            thread;

  thread = malloc(sizeof(pthread_t));
  rc = pthread_create(thread, NULL, func, NULL);
  compResults("pthread_create\n", rc);

  printf("Main completed\n");
  return 0;
}
