/* fz_vc4.h: forced include (-FI) first, for Visual C++ 4.x (MIPS and PowerPC). The C++
   of Visual C++ 5 that FileZilla and MFC 6 use, as far as macros reach. As the NT 4.0
   ports of Notepad++ and 7-Zip in this repository do. Written for this port. */
#ifndef FZ_VC4_H
#define FZ_VC4_H
#if defined(__cplusplus) && defined(_MSC_VER) && _MSC_VER < 1100
#pragma warning(disable: 4237) /* bool, true, false: reserved words */
#define bool int
#define true 1
#define false 0
#define explicit
#define mutable
#define typename
/* The for scope stays Visual C++ 4's, which is Visual C++ 6.0's: a variable of the for
   statement lasts past it. MFC 6 and FileZilla rely on that. */
#endif

/* Visual C++ 4.1's C library prints __int64 (%I64d) but can't read one. */
#if defined(_MSC_VER) && _MSC_VER < 1100
static __inline __int64 fz_atoi64(const char *s)
{
	__int64 v = 0;
	int neg = 0;
	while (*s == ' ' || *s == '\t')
		s++;
	if (*s == '-' || *s == '+')
		neg = *s++ == '-';
	while (*s >= '0' && *s <= '9')
		v = v * 10 + (*s++ - '0');
	return neg ? -v : v;
}
#define _atoi64 fz_atoi64
#define _ttoi64 fz_atoi64
#endif
#endif
