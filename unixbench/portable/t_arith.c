/*******************************************************************************
 *  UnixBench portable harness - arithmetic tests (arithoh, short, int, long,
 *  float, double), from ../src/arith.c.  See t_arith.h.
 ******************************************************************************/

#include "ubench.h"

/* long is 32 bits on Windows (even x64) and on classic Mac OS.  When a
   modern optimizer reduces the loop to just the counter, 10 seconds can
   pass 2^32, so iter_hi counts the wraps. */
static volatile unsigned long iter, iter_hi;

#define ARITH_FN ub_arithoh
#define ARITH_K  dumb_stuff_oh
#define arithoh
#include "t_arith.h"
#undef arithoh
#undef ARITH_K
#undef ARITH_FN

#define ARITH_FN ub_arith_short
#define ARITH_K  dumb_stuff_short
#define datum    short
#include "t_arith.h"
#undef datum
#undef ARITH_K
#undef ARITH_FN

#define ARITH_FN ub_arith_int
#define ARITH_K  dumb_stuff_int
#define datum    int
#include "t_arith.h"
#undef datum
#undef ARITH_K
#undef ARITH_FN

#define ARITH_FN ub_arith_long
#define ARITH_K  dumb_stuff_long
#define datum    long
#include "t_arith.h"
#undef datum
#undef ARITH_K
#undef ARITH_FN

#define ARITH_FN ub_arith_float
#define ARITH_K  dumb_stuff_float
#define datum    float
#include "t_arith.h"
#undef datum
#undef ARITH_K
#undef ARITH_FN

#define ARITH_FN ub_arith_double
#define ARITH_K  dumb_stuff_double
#define datum    double
#include "t_arith.h"
#undef datum
#undef ARITH_K
#undef ARITH_FN
