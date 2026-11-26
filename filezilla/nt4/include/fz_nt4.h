/* What FileZilla 2.2.32 takes from the Platform SDK and MFC 7 that the NT 4.0
   compilers lack. Included by config.h when FZ_NT4 is defined. Written for this port. */
#ifndef FZ_NT4_H
#define FZ_NT4_H

#ifndef INVALID_SET_FILE_POINTER
#define INVALID_SET_FILE_POINTER ((DWORD)-1)
#endif

/* The pointer-sized types and window functions of the 64-bit SDKs: on 32-bit NT 4.0
   they are the plain ones. */
#ifndef GWLP_WNDPROC
typedef long LONG_PTR;
typedef unsigned long ULONG_PTR;
#ifndef DWORD_PTR /* misc/htmlhelp.h makes it a macro */
typedef unsigned long DWORD_PTR;
#endif
#ifndef _BASETSD_H_ /* Visual C++ 6.0's has these two */
/* int, as in the 32-bit SDKs since: FileZilla's OnTimer(UINT_PTR) is then MFC 6's
   OnTimer(UINT), which Visual C++ 4.1 holds message maps to. */
typedef int INT_PTR;
typedef unsigned int UINT_PTR;
#endif
#define GWLP_WNDPROC GWL_WNDPROC
#define GWLP_HINSTANCE GWL_HINSTANCE
#define GWLP_USERDATA GWL_USERDATA
#define GWLP_ID GWL_ID
#define DWLP_MSGRESULT DWL_MSGRESULT
#define SetWindowLongPtr SetWindowLong
#define GetWindowLongPtr GetWindowLong
#define GetClassLongPtr GetClassLong
#define SetClassLongPtr SetClassLong
#endif

#ifndef SHGFP_TYPE_CURRENT
#define SHGFP_TYPE_CURRENT 0
#endif

/* 64-bit time: the NT 4.0 C libraries have 32-bit time_t only. */
#ifndef _TIME64_T_DEFINED
#define __time64_t time_t
#define _time64 time
#define _localtime64 localtime
#endif

/* EndMenu is Windows 98's and 2000's. On NT 4.0 the window that holds the menu is told
   to leave its menu loop, which is what EndMenu does. */
inline BOOL EndMenu()
{
	HWND hwnd = ::GetCapture();
	if (!hwnd)
		hwnd = ::GetForegroundWindow();
	if (hwnd)
		::SendMessage(hwnd, WM_CANCELMODE, 0, 0);
	return TRUE;
}

/* Internet Explorer 4's shell interface for overlay icons, which FileZilla asks a folder
   for; the PINT SDK's shlobj.h is NT 4.0's and lacks it. Its GUID is in imports.c. */
#if defined(_M_ARM) && defined(__cplusplus)
#include <shlobj.h>
#undef INTERFACE
#define INTERFACE IShellIconOverlay
DECLARE_INTERFACE_(IShellIconOverlay, IUnknown)
{
	STDMETHOD(QueryInterface)(THIS_ REFIID riid, LPVOID *ppv) PURE;
	STDMETHOD_(ULONG, AddRef)(THIS) PURE;
	STDMETHOD_(ULONG, Release)(THIS) PURE;
	STDMETHOD(GetOverlayIndex)(THIS_ LPCITEMIDLIST pidl, int *pIndex) PURE;
	STDMETHOD(GetOverlayIconIndex)(THIS_ LPCITEMIDLIST pidl, int *pIconIndex) PURE;
};
#undef INTERFACE
extern "C" const GUID IID_IShellIconOverlay;
#endif

/* comctl32's version, (major << 16) | minor: nt4compat.cpp. */
#ifdef __cplusplus
extern "C"
#endif
DWORD fz_comctl32_version(void);

/* MFC 7's 8-bit string. This is an ANSI build, where it is MFC's CString. */
#ifdef __AFX_H__
typedef CString CStringA;
#endif

#endif
