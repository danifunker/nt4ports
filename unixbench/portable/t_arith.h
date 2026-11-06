/*******************************************************************************
 *  UnixBench portable harness - arithmetic test template
 *
 *  ../src/arith.c is built six times with different -Ddatum values.  A
 *  single executable can't do that, so t_arith.c includes this file once
 *  per type.  Before each include it defines:
 *
 *      ARITH_FN    name of the test entry point
 *      ARITH_K     name of this variant's dumb_stuff()
 *      datum       the arithmetic type (left undefined with arithoh)
 *      arithoh     only for the loop-overhead variant
 *
 *  The loop and dumb_stuff() bodies are unchanged from arith.c.
 ******************************************************************************/

static int ARITH_K(int i)
{
#ifndef arithoh
	datum	x, y, z;
		z = 0;
#endif
		/*
		 *     101
		 * sum       i*i/(i*i-1)
		 *     i=2
		 */
		/* notice that the i value is always reset by the loop */
		for (i=2; i<=101; i++)
			{
#ifndef arithoh
			x = i;
			y = x*x;
			z += y/(y-1);
			}
return(x+y+z);
#else
}
return(0);
#endif
}

void ARITH_FN(int seconds, ub_sample *s)
{
	int result = 0;

	iter = 0;	/* init iteration count */
	iter_hi = 0;
	ub_timed_begin(seconds);

	/* this loop runs until the alarm sets ub_stop */
	while (!ub_stop)
	{
	if (++iter == 0)
		++iter_hi;
	/* the loop calls a function to insure that something is done
	   the results of the function are fed back in (just so they
	   they won't be thrown away. A loop with
	   unused assignments may get optimized out of existence */
	result = ARITH_K(result);
	}

	ub_timed_end(s, (double)iter_hi * 4294967296.0 + (double)iter);
}
