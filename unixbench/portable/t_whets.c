/*******************************************************************************
 *  UnixBench portable harness - Double-Precision Whetstone
 *
 *  ../src/whets.c is #included unchanged so the kernel (whetstones(), pa(),
 *  po(), p3(), pout()) is byte-for-byte the code the Unix build measures.
 *  Around it:
 *
 *    - DP and UNIXBENCH are defined, matching the Makefile's
 *      whetstone-double target.
 *    - None of whets.c's timer options is defined, so it declares dtime()
 *      but does not implement it; the definition below uses plat_seconds().
 *      WIN32 in particular is cleared, since its GetTickCount() timer only
 *      ticks every 10-16 ms.
 *    - Its main() is renamed out of the way (it ends in exit()), and
 *      ub_whetstone() repeats its UNIXBENCH calibrate-then-measure logic.
 *    - Its per-section table goes through ub_whets_printf(), which only
 *      prints with -v.
 *
 *  Whetstone is self-timed: it calibrates to run for about `seconds`, so it
 *  does not use the alarm.
 ******************************************************************************/

#include <math.h>
#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include "ubench.h"
#include "plat.h"

static int ub_whets_printf(const char *fmt, ...);

#undef Amiga
#undef UNIX
#undef UNIX_Old
#undef VMS
#undef BORLAND_C
#undef MSC
#undef MAC
#undef IPSC
#undef FORTRAN_SEC
#undef GTODay
#undef CTimer
#undef UXPM
#undef MAC_TMgr
#undef PARIX
#undef POSIX
#undef WIN32
#undef POSIX1

#define DP
#define UNIXBENCH
#define main   ub_whets_unused_main
#define printf ub_whets_printf
#include "../src/whets.c"
#undef printf
#undef main

SPDP dtime(void)
{
    return (SPDP)plat_seconds();
}

static int ub_whets_printf(const char *fmt, ...)
{
    va_list ap;
    int n;

    if (!ub_verbose)
        return 0;
    va_start(ap, fmt);
    n = vprintf(fmt, ap);
    va_end(ap);
    return n;
}

void ub_whetstone(int seconds, ub_sample *s)
{
    int count = 10, calibrate = 1;
    long xtra = 1;
    long x100 = 100;
    int duration = seconds;

    /* Calibrate: grow the pass count 5x until one run takes over 0.5 s. */
    do {
        TimeUsed = 0;
        whetstones(xtra, x100, calibrate);
        calibrate++;
        count--;
        if (TimeUsed > 0.5)
            count = 0;
        else
            xtra = xtra * 5;
    } while (count > 0);

    if (TimeUsed > 0) xtra = (long)((SPDP)(duration * xtra) / TimeUsed);
    if (xtra < 1) xtra = 1;

    calibrate = 0;
    TimeUsed = 0;
    whetstones(xtra, x100, calibrate);

    if (TimeUsed > 0)
        mwips = (float)(xtra) * (float)(x100) / (10 * TimeUsed);
    else
        mwips = 0;

    s->count = mwips;
    s->elapsed = TimeUsed;
    s->is_rate = 1;
    s->error = (Check == 0);
}
