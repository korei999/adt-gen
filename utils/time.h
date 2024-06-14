#pragma once
#include "ultratypes.h"

#include <time.h>

static inline f64
msTimeNow()
{
    struct timespec ts;
    clock_gettime(CLOCK_REALTIME, &ts);
    s64 micros = ts.tv_sec * 1000000000;
    micros += ts.tv_nsec;
    return micros / 1000000.0;
}
