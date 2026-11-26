// pint_crt.cpp: the C99 and float functions of a modern C run-time that ScummVM uses and
// NT 4.0's (PINT's) lacks. Declared in pint_svm.h, which every ScummVM file includes; this
// file is built without it, so that float arguments here go to the double functions.
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <errno.h>
#include <limits.h>


extern "C" {

// MSVC 14 calls it where a noexcept function would throw; ScummVM throws nothing.
void __cdecl __std_terminate(void) { abort(); }

float acosf(float x) { return (float)acos(x); }
float asinf(float x) { return (float)asin(x); }
float atanf(float x) { return (float)atan(x); }
float atan2f(float y, float x) { return (float)atan2(y, x); }
float cosf(float x) { return (float)cos(x); }
float sinf(float x) { return (float)sin(x); }
float tanf(float x) { return (float)tan(x); }
float coshf(float x) { return (float)cosh(x); }
float sinhf(float x) { return (float)sinh(x); }
float tanhf(float x) { return (float)tanh(x); }
float expf(float x) { return (float)exp(x); }
float logf(float x) { return (float)log(x); }
float log10f(float x) { return (float)log10(x); }
float powf(float x, float y) { return (float)pow(x, y); }
float sqrtf(float x) { return (float)sqrt(x); }
float ceilf(float x) { return (float)ceil(x); }
float floorf(float x) { return (float)floor(x); }
float fabsf(float x) { return (float)fabs(x); }
float fmodf(float x, float y) { return (float)fmod(x, y); }
float hypotf(float x, float y) { return (float)_hypot(x, y); }
double hypot(double x, double y) { return _hypot(x, y); }
float ldexpf(float x, int e) { return (float)ldexp(x, e); }
float frexpf(float x, int *e) { return (float)frexp(x, e); }
float modff(float x, float *ip) { double d; float r = (float)modf(x, &d); *ip = (float)d; return r; }
double trunc(double x) { return x < 0 ? ceil(x) : floor(x); }
float truncf(float x) { return (float)trunc(x); }
double round(double x) { return x < 0 ? ceil(x - 0.5) : floor(x + 0.5); }
float roundf(float x) { return (float)round(x); }
long lround(double x) { return (long)round(x); }
long lroundf(float x) { return (long)round(x); }
double rint(double x) { return round(x); }
float rintf(float x) { return (float)round(x); }
long lrint(double x) { return (long)round(x); }
long lrintf(float x) { return (long)round(x); }
double nearbyint(double x) { return round(x); }
double log2(double x) { return log(x) * 1.4426950408889634; }
float log2f(float x) { return (float)log2(x); }
double exp2(double x) { return pow(2.0, x); }
float exp2f(float x) { return (float)pow(2.0, x); }
double cbrt(double x) { return x < 0 ? -pow(-x, 1.0 / 3) : pow(x, 1.0 / 3); }
float cbrtf(float x) { return (float)cbrt(x); }
double fmin(double a, double b) { return a < b ? a : b; }
double fmax(double a, double b) { return a > b ? a : b; }
float fminf(float a, float b) { return a < b ? a : b; }
float fmaxf(float a, float b) { return a > b ? a : b; }
double copysign(double x, double y) { return _copysign(x, y); }
float copysignf(float x, float y) { return (float)_copysign(x, y); }
long long llabs(long long x) { return x < 0 ? -x : x; }

// C99 snprintf: always terminates, and returns the length the whole output needs.
// NT 4.0's _vsnprintf returns -1 when the buffer is too small and then doesn't terminate.
int __cdecl vsnprintf(char *buf, size_t size, const char *fmt, va_list ap) {
	int n = size ? _vsnprintf(buf, size, fmt, ap) : -1;
	if (n >= 0 && (size_t)n < size)
		return n;
	if (size)
		buf[size - 1] = 0;
	// Too small: find the length with ever larger scratch buffers.
	for (size_t cap = size < 256 ? 256 : size * 2; cap < 0x10000000; cap *= 2) {
		char *tmp = (char *)malloc(cap);
		if (!tmp)
			return -1;
		n = _vsnprintf(tmp, cap, fmt, ap);
		free(tmp);
		if (n >= 0 && (size_t)n < cap)
			return n;
	}
	return -1;
}

int __cdecl snprintf(char *buf, size_t size, const char *fmt, ...) {
	va_list ap;
	va_start(ap, fmt);
	int n = vsnprintf(buf, size, fmt, ap);
	va_end(ap);
	return n;
}

static unsigned long long strtoull_(const char *s, char **end, int base, int *neg) {
	const char *p = s;
	while (isspace((unsigned char)*p))
		p++;
	*neg = 0;
	if (*p == '-' || *p == '+')
		*neg = *p++ == '-';
	if ((base == 0 || base == 16) && p[0] == '0' && (p[1] == 'x' || p[1] == 'X') && isxdigit((unsigned char)p[2])) {
		p += 2;
		base = 16;
	} else if (base == 0)
		base = *p == '0' ? 8 : 10;
	unsigned long long v = 0;
	const char *start = p;
	for (;; p++) {
		int c = (unsigned char)*p, d;
		if (c >= '0' && c <= '9')
			d = c - '0';
		else if (c >= 'a' && c <= 'z')
			d = c - 'a' + 10;
		else if (c >= 'A' && c <= 'Z')
			d = c - 'A' + 10;
		else
			break;
		if (d >= base)
			break;
		v = v * base + d;
	}
	if (end)
		*end = (char *)(p == start ? s : p);
	return v;
}

unsigned long long __cdecl strtoull(const char *s, char **end, int base) {
	int neg;
	unsigned long long v = strtoull_(s, end, base, &neg);
	return neg ? (unsigned long long)-(long long)v : v;
}

long long __cdecl strtoll(const char *s, char **end, int base) {
	int neg;
	unsigned long long v = strtoull_(s, end, base, &neg);
	return neg ? -(long long)v : (long long)v;
}

long long __cdecl atoll(const char *s) { return strtoll(s, 0, 10); }

float __cdecl strtof(const char *s, char **end) { return (float)strtod(s, end); }

}
