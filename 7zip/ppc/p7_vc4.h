/* p7_vc4.h: forced include (/FI) for building 7-Zip 9.20 with Visual C++ 4.
 * bool is a long, not an int: CPropVariant has bool, Int32 and UInt32 overloads. */
#ifndef P7_VC4_H
#define P7_VC4_H
#if defined(__cplusplus) && defined(_MSC_VER) && _MSC_VER < 1100
#pragma warning(disable: 4237)
#define _7ZIP_VC4
#define bool long
#define true ((long)1)
#define false ((long)0)
#define explicit
#define mutable
#define for if (0) {} else for
#endif
#endif
