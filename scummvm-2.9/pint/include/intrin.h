// intrin.h for the PINT 4.0 SDK: the intrinsics ScummVM uses.
#ifndef PINT_INTRIN_H
#define PINT_INTRIN_H
#ifdef __cplusplus
extern "C" {
#endif
unsigned char _BitScanReverse(unsigned long *index, unsigned long mask);
unsigned char _BitScanForward(unsigned long *index, unsigned long mask);
#pragma intrinsic(_BitScanReverse, _BitScanForward)
#ifdef __cplusplus
}
#endif
#endif
