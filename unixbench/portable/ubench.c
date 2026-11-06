/*******************************************************************************
 *  UnixBench portable harness - runner
 *
 *  Plays the role of the Perl "Run" script for the tests ported so far:
 *  runs each test for a number of passes, drops the worst third, takes the
 *  geometric mean of the rest, and indexes the result against the
 *  SPARCstation 20-61 baseline in ../pgms/index.base.
 *
 *  usage: ubench [-v] [-q] [-i passes] [test ...]
 ******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "ubench.h"
#include "plat.h"

#define UB_VERSION      "6.0.1-portable"
#define UB_LONG_PASSES  10      /* Run's $longIterCount */
#define UB_SHORT_PASSES 3       /* Run's $shortIterCount */
#define UB_MAX_PASSES   100

volatile int ub_stop;
int ub_verbose;

static double ub_t0;

typedef struct {
    const char *name;       /* as in Run and index.base */
    const char *desc;       /* Run's "logmsg" */
    const char *unit;
    int         seconds;    /* Run's "options" duration */
    int         long_run;   /* Run's "repeat" => 'long' */
    double      baseline;   /* index.base score; 0 if not in the index */
    void      (*run)(int seconds, ub_sample *s);
} ub_test;

/* The full UnixBench system index has 12 tests; only these two are ported. */
#define UB_INDEX_TESTS 12

static const ub_test ub_tests[] = {
    { "dhry2reg", "Dhrystone 2 using register variables", "lps",
      10, 1, 116700.0, ub_dhry2reg },
    { "whetstone-double", "Double-Precision Whetstone", "MWIPS",
      10, 1, 55.0, ub_whetstone },
    { "arithoh", "Arithoh", "lps", 10, 0, 0.0, ub_arithoh },
    { "short", "Arithmetic Test (short)", "lps", 10, 0, 0.0, ub_arith_short },
    { "int", "Arithmetic Test (int)", "lps", 10, 0, 0.0, ub_arith_int },
    { "long", "Arithmetic Test (long)", "lps", 10, 0, 0.0, ub_arith_long },
    { "float", "Arithmetic Test (float)", "lps", 10, 0, 0.0, ub_arith_float },
    { "double", "Arithmetic Test (double)", "lps",
      10, 0, 0.0, ub_arith_double },
    { "hanoi", "Recursion Test -- Tower of Hanoi", "lps",
      20, 0, 0.0, ub_hanoi }
};
#define UB_NTESTS ((int)(sizeof(ub_tests) / sizeof(ub_tests[0])))

typedef struct {
    double score;
    int    kept;            /* passes that went into the score */
    int    failed;
} ub_result;

void ub_timed_begin(int seconds)
{
    ub_stop = 0;
    plat_alarm_start(seconds, &ub_stop);
    ub_t0 = plat_seconds();
}

void ub_timed_end(ub_sample *s, double count)
{
    s->elapsed = plat_seconds() - ub_t0;
    plat_alarm_end();
    s->count = count;
    s->is_rate = 0;
}

/* Per-pass score: a rate as-is, otherwise iterations per second. */
static double ub_pass_score(const ub_sample *s)
{
    if (s->is_rate || s->elapsed <= 0.0)
        return s->count;
    return s->count / s->elapsed;
}

static int ub_cmp_double(const void *a, const void *b)
{
    double x = *(const double *)a, y = *(const double *)b;
    return (x > y) - (x < y);
}

/* Same reduction as Run's combinePassResults(): sort, drop the lowest
   third, geometric mean of the rest. */
static void ub_run_test(const ub_test *t, int passes, ub_result *r)
{
    double scores[UB_MAX_PASSES];
    double logsum;
    ub_sample s;
    int i, ndump;

    r->score = 0.0;
    r->kept = 0;
    r->failed = 0;

    printf("%-40s", t->desc);
    fflush(stdout);

    for (i = 0; i < passes; ++i) {
        memset(&s, 0, sizeof(s));
        t->run(t->seconds, &s);
        if (s.error) {
            r->failed = 1;
            printf(" wrong answer\n");
            return;
        }
        scores[i] = ub_pass_score(&s);
        if (ub_verbose)
            printf("\n    pass %2d: %14.1f %s in %.3f s",
                   i + 1, scores[i], t->unit, s.elapsed);
        else
            printf(" %d", i + 1);
        fflush(stdout);
    }
    printf("\n");

    qsort(scores, passes, sizeof(scores[0]), ub_cmp_double);
    ndump = passes / 3;
    logsum = 0.0;
    for (i = ndump; i < passes; ++i) {
        if (scores[i] > 0.0)
            logsum += log(scores[i]);
        ++r->kept;
    }
    r->score = exp(logsum / r->kept);
}

static const ub_test *ub_find(const char *name)
{
    int i;

    for (i = 0; i < UB_NTESTS; ++i)
        if (strcmp(ub_tests[i].name, name) == 0)
            return &ub_tests[i];
    return NULL;
}

static void ub_usage(void)
{
    int i;

    printf("usage: ubench [-v] [-q] [-f] [-i passes] [test ...]\n\n");
    printf("  -v         show each pass and the tests' own output\n");
    printf("  -f         run even if this CPU is older than the build "
           "targets\n");
    printf("  -q         quick check: 1 second per test, 1 pass\n");
    printf("  -i passes  passes for the long tests (default %d)\n\n",
           UB_LONG_PASSES);
    printf("tests (default: all):\n");
    for (i = 0; i < UB_NTESTS; ++i)
        printf("  %-18s %s\n", ub_tests[i].name, ub_tests[i].desc);
}

int main(int argc, char **argv)
{
    const ub_test *sel[UB_NTESTS];
    ub_result res[UB_NTESTS];
    int nsel = 0, long_passes = UB_LONG_PASSES, quick = 0, force = 0;
    int i, passes, nindexed, target_ok;
    double logsum;
    char info[256], target[128];

    plat_init(&argc, &argv);

    for (i = 1; i < argc; ++i) {
        if (strcmp(argv[i], "-v") == 0) {
            ub_verbose = 1;
        } else if (strcmp(argv[i], "-q") == 0) {
            quick = 1;
        } else if (strcmp(argv[i], "-f") == 0) {
            force = 1;
        } else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            long_passes = atoi(argv[++i]);
            if (long_passes < 1 || long_passes > UB_MAX_PASSES) {
                printf("passes must be 1-%d\n", UB_MAX_PASSES);
                return 1;
            }
        } else if (argv[i][0] == '-') {
            ub_usage();
            return 1;
        } else {
            const ub_test *t = ub_find(argv[i]);

            if (t == NULL) {
                printf("unknown test \"%s\"\n\n", argv[i]);
                ub_usage();
                return 1;
            }
            if (nsel < UB_NTESTS)
                sel[nsel++] = t;
        }
    }
    if (nsel == 0)
        for (nsel = 0; nsel < UB_NTESTS; ++nsel)
            sel[nsel] = &ub_tests[nsel];

    plat_sysinfo(info, sizeof(info));
    target_ok = plat_check_target(target, sizeof(target));
    printf("UnixBench %s\n", UB_VERSION);
    printf("System: %s\n", info);
    printf("Build:  %s\n\n", target);
    if (!target_ok && !force) {
        printf("This build needs a newer CPU.  Run the generic ubench.exe, "
               "or use -f to\nrun it anyway (it may crash, or give "
               "meaningless results if the OS\nemulates the missing "
               "instructions).\n");
        return 1;
    }

    for (i = 0; i < nsel; ++i) {
        ub_test t = *sel[i];

        passes = t.long_run ? long_passes : UB_SHORT_PASSES;
        if (quick) {
            t.seconds = 1;
            passes = 1;
        }
        ub_run_test(&t, passes, &res[i]);
    }

    printf("\n%-40s %14s %5s\n", "Benchmark Run", "Score", "Passes");
    for (i = 0; i < nsel; ++i) {
        if (res[i].failed)
            printf("%-40s %14s\n", sel[i]->desc, "FAILED");
        else
            printf("%-40s %14.1f %-5s (%d)\n", sel[i]->desc, res[i].score,
                   sel[i]->unit, res[i].kept);
    }

    nindexed = 0;
    logsum = 0.0;
    for (i = 0; i < nsel; ++i)
        if (sel[i]->baseline > 0.0 && !res[i].failed)
            ++nindexed;
    if (nindexed > 0) {
        printf("\n%-40s %10s %10s %8s\n", "System Benchmarks Index Values",
               "BASELINE", "RESULT", "INDEX");
        for (i = 0; i < nsel; ++i) {
            double ratio;

            if (sel[i]->baseline <= 0.0 || res[i].failed)
                continue;
            ratio = res[i].score / sel[i]->baseline;
            if (ratio > 0.000001)
                logsum += log(ratio);
            printf("%-40s %10.1f %10.1f %8.1f\n", sel[i]->desc,
                   sel[i]->baseline, res[i].score, ratio * 10.0);
        }
        printf("%-40s %10s %10s %8s\n", "", "", "", "========");
        sprintf(info, "Partial Index (%d of %d index tests)",
                nindexed, UB_INDEX_TESTS);
        printf("%-62s %8.1f\n", info, exp(logsum / nindexed) * 10.0);
    }

    return 0;
}
