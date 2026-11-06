/* rar5types.h: fixed-size types for compilers without <stdint.h> (VC6). */
#ifndef RAR5TYPES_H
#define RAR5TYPES_H
#include <stddef.h>
#if defined(_MSC_VER)
typedef unsigned char uint8_t;
typedef signed char int8_t;
typedef unsigned short uint16_t;
typedef short int16_t;
typedef unsigned int uint32_t;
typedef int int32_t;
typedef unsigned __int64 uint64_t;
typedef __int64 int64_t;
#define RAR5_C64(x) x##i64
#define RAR5_U64(x) x##ui64
#define PRIx32 "x"
#else
#define __STDC_FORMAT_MACROS
#include <stdint.h>
#include <inttypes.h>
#define RAR5_C64(x) x##LL
#define RAR5_U64(x) x##ULL
#endif
#if defined(_MSC_VER)
typedef long ssize_t;
#else
#include <sys/types.h>
#endif
/* VC6 keeps a for-loop variable in the enclosing scope. */
#if defined(_MSC_VER) && _MSC_VER < 1300
#define for if (0) {} else for
#endif
#endif
