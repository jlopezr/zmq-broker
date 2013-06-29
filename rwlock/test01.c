#define _MULTI_THREADED
#include <pthread.h>
#include <stdio.h>
#include <unistd.h>
#include <errno.h>
#include "check.h"

pthread_rwlock_t       rwlock = PTHREAD_RWLOCK_INITIALIZER;

void *rdlockThread(void *arg)
{
  int             rc;
  int             count=0;

  printf("Entered thread, getting read lock with mp wait\n");
  Retry:
  rc = pthread_rwlock_tryrdlock(&rwlock);
  if (rc == EBUSY) {
    if (count >= 10) {
      printf("Retried too many times, failure!\n");

      exit(EXIT_FAILURE);
    }
    ++count;
    printf("Could not get lock, do other work, then RETRY...\n");
    sleep(1);
    goto Retry;
  }
  compResults("pthread_rwlock_tryrdlock() 1\n", rc);

  sleep(2);

  printf("unlock the read lock\n");
  rc = pthread_rwlock_unlock(&rwlock);
  compResults("pthread_rwlock_unlock()\n", rc);

  printf("Secondary thread complete\n");
  return NULL;
}

int main (int argc, char **argv)
{
  int                   rc=0;
  pthread_t             thread;

  printf("Enter Testcase - %s\n", argv[0]);

  printf("Main, get the write lock\n");
  rc = pthread_rwlock_wrlock(&rwlock);
  compResults("pthread_rwlock_wrlock()\n", rc);

  printf("Main, create the try read lock thread\n");
  rc = pthread_create(&thread, NULL, rdlockThread, NULL);
  compResults("pthread_create\n", rc);

  printf("Main, wait a bit holding the write lock\n");
  sleep(5);

  printf("Main, Now unlock the write lock\n");
  rc = pthread_rwlock_unlock(&rwlock);
  compResults("pthread_rwlock_unlock()\n", rc);

  printf("Main, wait for the thread to end\n");
  rc = pthread_join(thread, NULL);
  compResults("pthread_join\n", rc);

  rc = pthread_rwlock_destroy(&rwlock);
  compResults("pthread_rwlock_destroy()\n", rc);
  printf("Main completed\n");
  return 0;
}
