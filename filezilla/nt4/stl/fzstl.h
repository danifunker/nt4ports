/* fzstl.h: the common part of a small STL for the NT 4.0 build of FileZilla, for the
   compilers that have none: Visual C++ 4.1 (MIPS, PowerPC) and the PINT SDK (ARM).
   It has what FileZilla 2.2.32 uses, no more: vector, deque, list, map, set, pair,
   less, swap, find and auto_ptr. No exceptions are thrown.

   Visual C++ 4.1 has no member templates, no partial specialization and no default
   template arguments that name an earlier parameter, so there are no range
   constructors, and map and set always order by operator<. It also compiles every
   member function of a class template it instantiates, so a member may only need of
   the element type what every element type FileZilla uses has: list has no remove(),
   which needs ==.

   Written for the NT 4.0 port. Licence: GPL v2 or later, as FileZilla. */
#ifndef FZSTL_H
#define FZSTL_H

#include <stddef.h>
#include <string.h>
#include <new.h>

#ifdef _MSC_VER
#pragma warning(disable: 4284) /* operator-> of an iterator over a non-class type */
#endif

namespace std {

template <class T1, class T2> struct pair {
	typedef T1 first_type;
	typedef T2 second_type;
	T1 first;
	T2 second;
	pair() : first(T1()), second(T2()) {}
	pair(const T1 &a, const T2 &b) : first(a), second(b) {}
};

template <class T1, class T2> inline pair<T1, T2> make_pair(const T1 &a, const T2 &b)
{
	return pair<T1, T2>(a, b);
}

template <class T> struct less {
	bool operator()(const T &a, const T &b) const { return a < b; }
};

template <class T> inline void swap(T &a, T &b)
{
	T t = a;
	a = b;
	b = t;
}

template <class It, class T> inline It find(It first, It last, const T &v)
{
	for (; first != last; ++first)
		if (*first == v)
			break;
	return first;
}

/* Construction in raw storage, and destruction without freeing. */
template <class T> inline void _Construct(T *p, const T &v) { new ((void *)p) T(v); }
template <class T> inline void _Destroy(T *p) { p->~T(); }

}

#endif
