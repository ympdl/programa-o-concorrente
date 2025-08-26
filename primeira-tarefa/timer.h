/* timer.h */
#ifndef TIMER_H
#define TIMER_H

#include <sys/time.h>
#include <stdlib.h>

#define GET_TIME(now) { \
    struct timeval t;   \
    gettimeofday(&t, NULL); \
    now = t.tv_sec + t.tv_usec/1000000.0; \
}

#endif
