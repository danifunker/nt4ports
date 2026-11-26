// pint_std.h: the std names of the C headers that ScummVM uses, for the <cxxx> headers here.
#ifndef PINT_STD_H
#define PINT_STD_H
#include <stddef.h>
// C11's, missing from NT 4.0's stddef.h.
typedef double max_align_t;
namespace std {
typedef ::size_t size_t;
typedef ::ptrdiff_t ptrdiff_t;
typedef decltype(nullptr) nullptr_t;
typedef ::max_align_t max_align_t;
}
#endif
