/* npp_vc4.h: forced include (/FI) first, for Visual C++ 4.x with NT 4.0 headers. */
#ifndef NPP_VC4_H
#define NPP_VC4_H
#if defined(_MSC_VER) && _MSC_VER < 1100
#pragma warning(disable: 4237)
#define STRICT
/* Before the defines below: oaidl.h has a member called bool. */
#include <windows.h>
#include <ole2.h>
#include "BaseTsd.h"
#include <commctrl.h>
/* Not in VC++ 4's comctl32.lib; plain InitCommonControls covers every class. */
#define InitCommonControlsEx(p) (InitCommonControls(), 1)
#define bool int
#define true 1
#define false 0
#define explicit
#define mutable
#define for if (0) {} else for
#endif
#endif
