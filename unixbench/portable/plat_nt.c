/*******************************************************************************
 *  UnixBench portable harness - Windows NT platform layer
 *
 *  Uses only Win32 calls present in NT 3.51 and 4.0, so the same source
 *  builds with Visual C++ 4.x for PowerPC (and MIPS/Alpha/x86) and with
 *  current Visual Studio.  No __int64: 64-bit counters are split into
 *  HighPart/LowPart, since not every RISC compiler of the era had it.
 *
 *  The alarm is a high-priority thread that sleeps and then sets the flag.
 *  The thread touches no C runtime state, so it is safe with the
 *  single-threaded runtime (/ML) that VC++ 4.x links by default.
 ******************************************************************************/

#include <windows.h>
#include <stdio.h>
#include <string.h>
#include "plat.h"

#if defined(_MSC_VER) && _MSC_VER >= 1400
#pragma warning(disable: 4996)  /* GetVersionEx, sprintf deprecation */
#endif

#ifndef PROCESSOR_ARCHITECTURE_INTEL
#define PROCESSOR_ARCHITECTURE_INTEL 0
#endif
#ifndef PROCESSOR_ARCHITECTURE_MIPS
#define PROCESSOR_ARCHITECTURE_MIPS  1
#endif
#ifndef PROCESSOR_ARCHITECTURE_ALPHA
#define PROCESSOR_ARCHITECTURE_ALPHA 2
#endif
#ifndef PROCESSOR_ARCHITECTURE_PPC
#define PROCESSOR_ARCHITECTURE_PPC   3
#endif
#ifndef PROCESSOR_ARCHITECTURE_ARM
#define PROCESSOR_ARCHITECTURE_ARM   5
#endif
#ifndef PROCESSOR_ARCHITECTURE_AMD64
#define PROCESSOR_ARCHITECTURE_AMD64 9
#endif
#ifndef PROCESSOR_ARCHITECTURE_ARM64
#define PROCESSOR_ARCHITECTURE_ARM64 12
#endif

#ifndef PF_ALPHA_BYTE_INSTRUCTIONS
#define PF_ALPHA_BYTE_INSTRUCTIONS 5
#endif

/* Which CPU the kernels were compiled for; Makefile.nt passes
   /DUB_TARGET_<variant>.  Alpha CPUs report their part number (21064,
   21164, 21264) as the processor level, so that is the minimum. */
#if defined(UB_TARGET_ev4)
#define UB_TARGET_NAME  "Alpha EV4 (21064)"
#define UB_TARGET_LEVEL 21064
#define UB_TARGET_BWX   0
#elif defined(UB_TARGET_ev5)
#define UB_TARGET_NAME  "Alpha EV5 (21164)"
#define UB_TARGET_LEVEL 21164
#define UB_TARGET_BWX   0
#elif defined(UB_TARGET_ev56)
#define UB_TARGET_NAME  "Alpha EV56 (21164A, BWX)"
#define UB_TARGET_LEVEL 21164
#define UB_TARGET_BWX   1
#elif defined(UB_TARGET_ev6)
#define UB_TARGET_NAME  "Alpha EV6 (21264)"
#define UB_TARGET_LEVEL 21264
#define UB_TARGET_BWX   1
#else
#define UB_TARGET_NAME  "generic"
#define UB_TARGET_LEVEL 0
#define UB_TARGET_BWX   0
#endif

static double plat_qpc_scale;   /* seconds per counter tick; 0 = no QPC */
static HANDLE plat_alarm_thread;
static DWORD plat_alarm_ms;
static volatile int *plat_alarm_flag;

static double plat_li_to_double(LARGE_INTEGER li)
{
    return (double)li.HighPart * 4294967296.0 + (double)li.LowPart;
}

void plat_init(int *argc, char ***argv)
{
    LARGE_INTEGER freq;

    (void)argc;
    (void)argv;
    if (QueryPerformanceFrequency(&freq)) {
        double f = plat_li_to_double(freq);
        if (f > 0.0)
            plat_qpc_scale = 1.0 / f;
    }
}

double plat_seconds(void)
{
    LARGE_INTEGER now;

    if (plat_qpc_scale > 0.0 && QueryPerformanceCounter(&now))
        return plat_li_to_double(now) * plat_qpc_scale;
    /* Wraps after 49.7 days; a single pass never spans that. */
    return (double)GetTickCount() * 0.001;
}

static DWORD WINAPI plat_alarm_proc(LPVOID arg)
{
    (void)arg;
    Sleep(plat_alarm_ms);
    *plat_alarm_flag = 1;
    return 0;
}

void plat_alarm_start(int seconds, volatile int *flag)
{
    DWORD tid;

    plat_alarm_flag = flag;
    plat_alarm_ms = (DWORD)seconds * 1000;
    plat_alarm_thread = CreateThread(NULL, 0, plat_alarm_proc, NULL, 0, &tid);
    if (plat_alarm_thread == NULL) {
        /* Can't time the test; stop it at once rather than spin forever. */
        fprintf(stderr, "CreateThread failed (error %lu)\n",
                (unsigned long)GetLastError());
        *flag = 1;
        return;
    }
    /* The test spins at normal priority.  On a uniprocessor the alarm must
       preempt it as soon as Sleep() returns. */
    SetThreadPriority(plat_alarm_thread, THREAD_PRIORITY_TIME_CRITICAL);
}

void plat_alarm_end(void)
{
    if (plat_alarm_thread != NULL) {
        WaitForSingleObject(plat_alarm_thread, INFINITE);
        CloseHandle(plat_alarm_thread);
        plat_alarm_thread = NULL;
    }
}

static const char *plat_cpu_name(const SYSTEM_INFO *si)
{
    switch (si->wProcessorArchitecture) {
    case PROCESSOR_ARCHITECTURE_PPC:
        switch (si->wProcessorLevel) {
        case 1:  return "PowerPC 601";
        case 3:  return "PowerPC 603";
        case 4:  return "PowerPC 604";
        case 6:  return "PowerPC 603e";
        case 7:  return "PowerPC 603ev";
        case 9:  return "PowerPC 604e";
        case 20: return "PowerPC 620";
        default: return "PowerPC";
        }
    case PROCESSOR_ARCHITECTURE_INTEL: return "x86";
    case PROCESSOR_ARCHITECTURE_MIPS:  return "MIPS";
    case PROCESSOR_ARCHITECTURE_ALPHA: return "Alpha";
    case PROCESSOR_ARCHITECTURE_ARM:
        /* PINT reports the CPU part number as the level */
        switch (si->wProcessorLevel) {
        case 0xC07: return "ARM Cortex-A7";
        case 0xD03: return "ARM Cortex-A53";
        case 0xD08: return "ARM Cortex-A72";
        default:    return "ARM";
        }
    case PROCESSOR_ARCHITECTURE_AMD64: return "x64";
    case PROCESSOR_ARCHITECTURE_ARM64: return "ARM64";
    default:                           return "unknown CPU";
    }
}

/* IsProcessorFeaturePresent() is new in NT 4.0; look it up so the binary
   still loads on 3.51.  Without it, assume the feature is missing. */
static int plat_has_feature(DWORD feature)
{
    typedef BOOL (WINAPI *ub_ipfp)(DWORD);
    HMODULE k32 = GetModuleHandleA("KERNEL32.DLL");
    ub_ipfp ipfp;

    if (k32 == NULL)
        return 0;
    ipfp = (ub_ipfp)GetProcAddress(k32, "IsProcessorFeaturePresent");
    return ipfp != NULL && ipfp(feature);
}

int plat_check_target(char *buf, int len)
{
    SYSTEM_INFO si;
    char tmp[128];
    int ok = 1;

    strncpy(buf, UB_TARGET_NAME, len - 1);
    buf[len - 1] = '\0';
    if (UB_TARGET_LEVEL == 0)
        return 1;

    GetSystemInfo(&si);
    if (si.wProcessorArchitecture != PROCESSOR_ARCHITECTURE_ALPHA
        || si.wProcessorLevel < UB_TARGET_LEVEL)
        ok = 0;
    if (UB_TARGET_BWX && !plat_has_feature(PF_ALPHA_BYTE_INSTRUCTIONS))
        ok = 0;
    if (!ok) {
        sprintf(tmp, "%s - this %s %u does not support it", UB_TARGET_NAME,
                plat_cpu_name(&si), (unsigned)si.wProcessorLevel);
        strncpy(buf, tmp, len - 1);
        buf[len - 1] = '\0';
    }
    return ok;
}

void plat_sysinfo(char *buf, int len)
{
    OSVERSIONINFO ver;
    SYSTEM_INFO si;
    char tmp[256];

    memset(&ver, 0, sizeof(ver));
    ver.dwOSVersionInfoSize = sizeof(ver);
    if (!GetVersionEx(&ver)) {
        ver.dwMajorVersion = 0;
        ver.dwMinorVersion = 0;
        ver.dwBuildNumber = 0;
        ver.szCSDVersion[0] = '\0';
    }
    GetSystemInfo(&si);

    /* szCSDVersion is at most 127 chars, so tmp cannot overflow. */
    sprintf(tmp, "Windows NT %lu.%lu (Build %lu%s%s); %s, level %u rev %04x;"
            " %lu CPU%s",
            (unsigned long)ver.dwMajorVersion,
            (unsigned long)ver.dwMinorVersion,
            (unsigned long)(ver.dwBuildNumber & 0xFFFF),
            ver.szCSDVersion[0] ? ": " : "", ver.szCSDVersion,
            plat_cpu_name(&si), (unsigned)si.wProcessorLevel,
            (unsigned)si.wProcessorRevision,
            (unsigned long)si.dwNumberOfProcessors,
            si.dwNumberOfProcessors == 1 ? "" : "s");
    strncpy(buf, tmp, len - 1);
    buf[len - 1] = '\0';
}
