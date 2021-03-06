#define _MULTI_THREADED
#include <pthread.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include "check.h"

pthread_rwlock_t       rwlock = PTHREAD_RWLOCK_INITIALIZER;

void *wrlockThread(void *arg)
{
  int             rc;
  int             count=0;

  printf("%.8x: Entered thread, getting write lock\n",
         (unsigned int)pthread_self());
  Retry:
  rc = pthread_rwlock_trywrlock(&rwlock);
  if (rc == EBUSY) {
    if (count >= 10) {
      printf("%.8x: Retried too many times, failure!\n",
             (unsigned int)pthread_self());
      exit(EXIT_FAILURE);
    }

    ++count;
    printf("%.8x: Go off an do other work, then RETRY...\n",
           (unsigned int)pthread_self());
    sleep(1);
    goto Retry;
  }
  compResults("pthread_rwlock_trywrlock() 1\n", rc);
  printf("%.8x: Got the write lock\n", 
	  (unsigned int)pthread_self());

  sleep(2);

  printf("%.8x: Unlock the write lock\n",
         (unsigned int)pthread_self());
  rc = pthread_rwlock_unlock(&rwlock);
  compResults("pthread_rwlock_unlock()\n", rc);

  printf("%.8x: Secondary thread complete\n",
         (unsigned int)pthread_self());
  return NULL;
}

int main (int argc, char **argv)
{
  int rc=0;
  pthread_t thread, thread2;

  printf("Enter Testcase - %s\n", argv[0]);

  printf("Main, get the write lock\n");
  rc = pthread_rwlock_wrlock(&rwlock);
  compResults("pthread_rwlock_wrlock()\n", rc);

  printf("Main, create the timed write lock threads\n");
  rc = pthread_create(&thread, NULL, wrlockThread, NULL);
  compResults("pthread_create\n", rc);

  rc = pthread_create(&thread2, NULL, wrlockThread, NULL);
  compResults("pthread_create\n", rc);

  printf("Main, wait a bit holding this write lock\n");
  sleep(1);

  printf("Main, Now unlock the write lock\n");
  rc = pthread_rwlock_unlock(&rwlock);
  compResults("pthread_rwlock_unlock()\n", rc);

  printf("Main, wait for the threads to end\n");
  rc = pthread_join(thread, NULL);
  compResults("pthread_join\n", rc);

  rc = pthread_join(thread2, NULL);
  compResults("pthread_join\n", rc);

  rc = pthread_rwlock_destroy(&rwlock);
  compResults("pthread_rwlock_destroy()\n", rc);
  printf("Main completed\n");
  return 0;
}
