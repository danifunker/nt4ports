/* pint_svm.h: forced include (-FI) for ScummVM with the PINT 4.0 SDK: what ScummVM expects
 * of a modern Microsoft C run-time and headers that NT 4.0's lack. The functions that aren't
 * inline here are in pint_crt.cpp. */
#ifndef PINT_SVM_H
#define PINT_SVM_H

#include <stddef.h>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <math.h>
#include <float.h>
#ifdef __cplusplus
#include "pint_std.h"
#endif

/* SAL annotations of later headers. */
#define _Printf_format_string_
#define _In_
#define _In_z_
#define _Out_
#define _Inout_

#ifdef __cplusplus
extern "C" {
#endif

/* Compiler intrinsics, declared as later headers do. */
unsigned short __cdecl _byteswap_ushort(unsigned short);
unsigned long __cdecl _byteswap_ulong(unsigned long);
unsigned long long __cdecl _byteswap_uint64(unsigned long long);
#pragma intrinsic(_byteswap_ushort, _byteswap_ulong, _byteswap_uint64)

/* C99 functions of the C run-time, in pint_crt.cpp. */
int __cdecl snprintf(char *buf, size_t size, const char *fmt, ...);
int __cdecl vsnprintf(char *buf, size_t size, const char *fmt, va_list ap);
long long __cdecl strtoll(const char *s, char **end, int base);
unsigned long long __cdecl strtoull(const char *s, char **end, int base);
long long __cdecl atoll(const char *s);
float __cdecl strtof(const char *s, char **end);
/* In NT 4.0's C run-time, but not in its stdio.h. */
long long __cdecl _ftelli64(FILE *f);
int __cdecl _fseeki64(FILE *f, long long offset, int origin);

/* Float versions of the double math functions, which NT 4.0's C run-time has only on MIPS,
 * and the C99 math functions: in pint_crt.cpp. */
float __cdecl acosf(float x);
float __cdecl asinf(float x);
float __cdecl atanf(float x);
float __cdecl atan2f(float y, float x);
float __cdecl cosf(float x);
float __cdecl sinf(float x);
float __cdecl tanf(float x);
float __cdecl coshf(float x);
float __cdecl sinhf(float x);
float __cdecl tanhf(float x);
float __cdecl expf(float x);
float __cdecl logf(float x);
float __cdecl log10f(float x);
float __cdecl powf(float x, float y);
float __cdecl sqrtf(float x);
float __cdecl ceilf(float x);
float __cdecl floorf(float x);
float __cdecl fabsf(float x);
float __cdecl fmodf(float x, float y);
float __cdecl hypotf(float x, float y);
double __cdecl hypot(double x, double y);
float __cdecl ldexpf(float x, int e);
float __cdecl frexpf(float x, int *e);
float __cdecl modff(float x, float *ip);
double __cdecl trunc(double x);
float __cdecl truncf(float x);
double __cdecl round(double x);
float __cdecl roundf(float x);
long __cdecl lround(double x);
long __cdecl lroundf(float x);
double __cdecl rint(double x);
float __cdecl rintf(float x);
long __cdecl lrint(double x);
long __cdecl lrintf(float x);
double __cdecl nearbyint(double x);
double __cdecl log2(double x);
float __cdecl log2f(float x);
double __cdecl exp2(double x);
float __cdecl exp2f(float x);
double __cdecl cbrt(double x);
float __cdecl cbrtf(float x);
double __cdecl fmin(double a, double b);
double __cdecl fmax(double a, double b);
float __cdecl fminf(float a, float b);
float __cdecl fmaxf(float a, float b);
double __cdecl copysign(double x, double y);
float __cdecl copysignf(float x, float y);
long long __cdecl llabs(long long x);

#ifdef __cplusplus
}

/* The float and integer overloads of <cmath>. */
inline float abs(float x) { return fabsf(x); }
inline double abs(double x) { return fabs(x); }
inline long abs(long x) { return labs(x); }
inline long long abs(long long x) { return llabs(x); }
inline float fabs(float x) { return fabsf(x); }
inline float sqrt(float x) { return sqrtf(x); }
inline float sin(float x) { return sinf(x); }
inline float cos(float x) { return cosf(x); }
inline float tan(float x) { return tanf(x); }
inline float asin(float x) { return asinf(x); }
inline float acos(float x) { return acosf(x); }
inline float atan(float x) { return atanf(x); }
inline float atan2(float y, float x) { return atan2f(y, x); }
inline float exp(float x) { return expf(x); }
inline float log(float x) { return logf(x); }
inline float log10(float x) { return log10f(x); }
inline float pow(float x, float y) { return powf(x, y); }
// C++11's <cmath> takes integers as doubles, and a mix of float and double as double.
template<class T> struct pint_is_int { enum { v = 0 }; };
#define PINT_INT(T) template<> struct pint_is_int<T> { enum { v = 1 }; };
PINT_INT(bool) PINT_INT(char) PINT_INT(signed char) PINT_INT(unsigned char) PINT_INT(short)
PINT_INT(unsigned short) PINT_INT(int) PINT_INT(unsigned int) PINT_INT(long)
PINT_INT(unsigned long) PINT_INT(long long) PINT_INT(unsigned long long)
#undef PINT_INT
template<class T> struct pint_is_num { enum { v = pint_is_int<T>::v } ; };
template<> struct pint_is_num<float> { enum { v = 1 }; };
template<> struct pint_is_num<double> { enum { v = 1 }; };
template<bool B, class T> struct pint_if {};
template<class T> struct pint_if<true, T> { typedef T type; };
#define PINT_UNARY(f) \
	template<class T> inline typename pint_if<pint_is_int<T>::v != 0, double>::type f(T x) { return ::f((double)x); }
PINT_UNARY(sqrt) PINT_UNARY(fabs) PINT_UNARY(sin) PINT_UNARY(cos) PINT_UNARY(tan)
PINT_UNARY(asin) PINT_UNARY(acos) PINT_UNARY(atan) PINT_UNARY(exp) PINT_UNARY(log)
PINT_UNARY(log10) PINT_UNARY(floor) PINT_UNARY(ceil) PINT_UNARY(round) PINT_UNARY(trunc)
PINT_UNARY(cbrt) PINT_UNARY(log2) PINT_UNARY(exp2) PINT_UNARY(sinh) PINT_UNARY(cosh) PINT_UNARY(tanh)
#undef PINT_UNARY
#define PINT_BINARY(f) \
	template<class A, class B> inline typename pint_if<(pint_is_int<A>::v || pint_is_int<B>::v) && \
		pint_is_num<A>::v && pint_is_num<B>::v, double>::type f(A x, B y) { return ::f((double)x, (double)y); } \
	inline double f(float x, double y) { return ::f((double)x, y); } \
	inline double f(double x, float y) { return ::f(x, (double)y); }
PINT_BINARY(pow) PINT_BINARY(atan2) PINT_BINARY(fmod) PINT_BINARY(hypot) PINT_BINARY(fmin)
PINT_BINARY(fmax) PINT_BINARY(copysign)
#undef PINT_BINARY
inline float floor(float x) { return floorf(x); }
inline float ceil(float x) { return ceilf(x); }
inline float fmod(float x, float y) { return fmodf(x, y); }
inline float round(float x) { return roundf(x); }
inline float trunc(float x) { return truncf(x); }
inline float hypot(float x, float y) { return hypotf(x, y); }
inline float fmin(float x, float y) { return fminf(x, y); }
inline float fmax(float x, float y) { return fmaxf(x, y); }
inline float copysign(float x, float y) { return copysignf(x, y); }
inline float log2(float x) { return log2f(x); }
inline float exp2(float x) { return exp2f(x); }
inline float cbrt(float x) { return cbrtf(x); }
inline float sinh(float x) { return sinhf(x); }
inline float cosh(float x) { return coshf(x); }
inline float tanh(float x) { return tanhf(x); }

/* C99's classification macros, as C++ functions (later headers make them both).
 * isnan and isinf are left to ScummVM: common/formats/json.h defines them for MSVC. */
inline bool isfinite(double x) { return _finite(x) != 0; }
inline bool signbit(double x) { return _copysign(1.0, x) < 0; }
#endif

#endif
