/*******************************************************************************
 *  UnixBench portable harness - POSIX platform layer
 *
 *  Not a deployment target; it exists so the harness can be run next to the
 *  regular UnixBench binaries on the same machine to check the scores agree.
 ******************************************************************************/

/* -std=c89 hides gettimeofday() and friends unless asked for. */
#define _DEFAULT_SOURCE
#define _DARWIN_C_SOURCE

#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/utsname.h>
#include "plat.h"

static volatile int *plat_alarm_flag;

void plat_init(int *argc, char ***argv)
{
    (void)argc;
    (void)argv;
}

double plat_seconds(void)
{
    struct timeval tv;

    gettimeofday(&tv, NULL);
    return (double)tv.tv_sec + (double)tv.tv_usec * 1.0e-6;
}

static void plat_on_alarm(int sig)
{
    (void)sig;
    *plat_alarm_flag = 1;
}

void plat_alarm_start(int seconds, volatile int *flag)
{
    plat_alarm_flag = flag;
    signal(SIGALRM, plat_on_alarm);
    alarm((unsigned)seconds);
}

void plat_alarm_end(void)
{
    alarm(0);
    signal(SIGALRM, SIG_DFL);
}

int plat_check_target(char *buf, int len)
{
    strncpy(buf, "generic", len - 1);
    buf[len - 1] = '\0';
    return 1;
}

void plat_sysinfo(char *buf, int len)
{
    struct utsname u;
#ifdef _SC_NPROCESSORS_ONLN
    long ncpu = sysconf(_SC_NPROCESSORS_ONLN);
#else
    long ncpu = 1;
#endif
    char tmp[sizeof(u.sysname) + sizeof(u.release) + sizeof(u.machine) + 32];

    if (uname(&u) != 0) {
        strncpy(buf, "unknown POSIX system", len - 1);
        buf[len - 1] = '\0';
        return;
    }
    sprintf(tmp, "%s %s; %s; %ld CPU%s", u.sysname, u.release, u.machine,
            ncpu, ncpu == 1 ? "" : "s");
    strncpy(buf, tmp, len - 1);
    buf[len - 1] = '\0';
}
