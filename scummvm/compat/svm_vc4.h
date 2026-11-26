/* svm_vc4.h: forced include (-FI) for ScummVM with Visual C++ 4.1, which has no bool,
 * explicit, mutable, typename or __forceinline, and the old scope of for. */
#ifndef SVM_VC4_H
#define SVM_VC4_H
#if defined(_MSC_VER) && _MSC_VER < 1100
#pragma warning(disable: 4237)
#define __forceinline inline
#define SVM_NO_BOOL /* bool is int: no overloads on bool and int */
#define bool int
#define true 1
#define false 0
#define explicit
#define mutable
#define typename class
#define for if (0) {} else for
#endif
#endif
