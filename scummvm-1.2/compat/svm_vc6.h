/* svm_vc6.h: included first in every ScummVM file (-FI) for Visual C++ 6.0 for Alpha. */
#ifndef SVM_VC6_H
#define SVM_VC6_H

/* The scope of a for statement's variable as in standard C++, not VC6's enclosing block. */
#define for if (0) {} else for

/* VC6 can't name a template friend or a nested class's friend of another nested class, which
 * ScummVM's headers have for their iterators and save handlers. Every member is public
 * instead: access is only checked, and every file is built so, so it changes no code. */
#define private public
#define protected public

#ifdef __cplusplus
/* VC6 gives every instantiation of a function template the same name when its template
 * arguments aren't in its parameters' types, and the linker keeps one of them: f<1>() and
 * f<2>() both run f<2>. The patch gives those templates an unused parameter of one of these
 * types, defaulted to 0, which puts the arguments in the name. */
template<int N> struct VC6Int { };
template<class T> struct VC6Type { };
#endif

/* Names of later Windows headers than VC6's. */
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#define DWORD_PTR DWORD

#endif
