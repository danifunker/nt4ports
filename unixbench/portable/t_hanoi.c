/*******************************************************************************
 *  UnixBench portable harness - Tower of Hanoi recursion test, from
 *  ../src/hanoi.c.  mov() is unchanged; main() became ub_hanoi() and polls
 *  ub_stop instead of being stopped by SIGALRM.
 ******************************************************************************/

#include "ubench.h"

#define other(i,j) (6-(i+j))

static void mov(int n, int f, int t);

static volatile unsigned long iter = 0;
static int num[4];

void ub_hanoi(int seconds, ub_sample *s)
{
	int disk=10; /* default number of disks */

	num[1] = disk;
	num[2] = 0;
	num[3] = 0;
	iter = 0;

	ub_timed_begin(seconds);

	while(!ub_stop) {
		mov(disk,1,3);
		iter++;
		}

	ub_timed_end(s, (double)iter);
}

static void mov(int n, int f, int t)
{
	int o;
	if(n == 1) {
		num[f]--;
		num[t]++;
		return;
	}
	o = other(f,t);
	mov(n-1,f,o);
	mov(1,f,t);
	mov(n-1,o,t);
}
