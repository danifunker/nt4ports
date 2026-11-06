/*******************************************************************************
 *  UnixBench portable harness - shared declarations
 *
 *  The regular UnixBench runs each test as its own process and stops it with
 *  SIGALRM, whose handler prints the count and calls exit().  That model
 *  needs processes, signals and a shell, none of which exist on classic
 *  Mac OS, and NT 4.0 on PowerPC has no Perl to drive it.  Here every test
 *  is a function in one executable.  Time-limited tests loop until ub_stop
 *  is set by the platform alarm, then hand their count back.
 ******************************************************************************/

#ifndef UB_UBENCH_H
#define UB_UBENCH_H

/* Result of one pass of one test. */
typedef struct {
    double count;       /* iterations, or the rate itself if is_rate */
    double elapsed;     /* seconds the measured loop actually ran */
    int    is_rate;     /* count is already per-second (e.g. MWIPS) */
    int    error;       /* non-zero if the pass produced a wrong answer */
} ub_sample;

/* Set by the platform alarm; time-limited loops poll it. */
extern volatile int ub_stop;

/* Non-zero when the user asked for the tests' own diagnostic output. */
extern int ub_verbose;

/* Start a time-limited loop: clears ub_stop, arms the alarm and starts the
   clock.  Pair with ub_timed_end() once the loop has seen ub_stop. */
void ub_timed_begin(int seconds);
void ub_timed_end(ub_sample *s, double count);

/* Test entry points.  Each runs one pass and fills in *s. */
void ub_dhry2reg(int seconds, ub_sample *s);
void ub_whetstone(int seconds, ub_sample *s);
void ub_arithoh(int seconds, ub_sample *s);
void ub_arith_short(int seconds, ub_sample *s);
void ub_arith_int(int seconds, ub_sample *s);
void ub_arith_long(int seconds, ub_sample *s);
void ub_arith_float(int seconds, ub_sample *s);
void ub_arith_double(int seconds, ub_sample *s);
void ub_hanoi(int seconds, ub_sample *s);

#endif /* UB_UBENCH_H */
