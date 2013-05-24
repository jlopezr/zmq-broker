#include<stdio.h>
#include<stdlib.h>
#include<sys/types.h>
 
#if defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0
#include<time.h>
#else
#include<sys/time.h>
#endif
 
int main(int argc, char *argv[]) {
    struct timespec ts;
#if defined(_POSIX_TIMERS) && _POSIX_TIMERS > 0
    clock_gettime(CLOCK_REALTIME, &ts);
#else
    struct timeval tv;
    gettimeofday(&tv, NULL);
    ts.tv_sec = tv.tv_sec;
    ts.tv_nsec = tv.tv_usec * 1000;
#endif
    printf(" sec: %u\n", (uint)ts.tv_sec);
    printf("nsec: %u\n", (uint)ts.tv_nsec);

    long long n = ((uint)ts.tv_sec)*1000000000LL;
    n = n + ((uint)ts.tv_nsec);

    printf("TOTAL %lu\n", n);

    return 0;
}
