/* windows.h for ScummVM with the PINT 4.0 SDK: the SDK's NT 4.0 windows.h, then the
 * Windows 2000 names ScummVM's Win32 code uses. That code calls the Windows 2000
 * functions (VerifyVersionInfo, SHGetFolderPath) only through GetProcAddress, so their
 * types are all it needs. The SDK is at Z:\opt\pint40sdk in its Docker image and in
 * filezilla/nt4/sdkrun.sh's bubblewrap copy. */
#ifndef PINT_WINDOWS_WRAPPER_H
#define PINT_WINDOWS_WRAPPER_H
#include <Z:\opt\pint40sdk\include\windows.h>
#ifndef RC_INVOKED

/* winnt.h has HRESULT in later SDKs; NT 4.0's has it only in wtypes.h, which
 * WIN32_LEAN_AND_MEAN leaves out. */
#ifndef _HRESULT_DEFINED
#define _HRESULT_DEFINED
typedef LONG HRESULT;
#endif
#ifndef _BASETSD_H_
typedef long LONG_PTR, *PLONG_PTR;
typedef unsigned long ULONG_PTR, *PULONG_PTR;
typedef unsigned long DWORD_PTR, *PDWORD_PTR;
typedef int INT_PTR, *PINT_PTR;
typedef unsigned int UINT_PTR, *PUINT_PTR;
typedef ULONG_PTR SIZE_T, *PSIZE_T;
typedef LONG_PTR SSIZE_T, *PSSIZE_T;
#endif

#ifndef IS_INTRESOURCE
#define IS_INTRESOURCE(r) ((((ULONG_PTR)(r)) >> 16) == 0)
#endif

#ifndef GWLP_WNDPROC
#define GWLP_WNDPROC GWL_WNDPROC
#define GWLP_HINSTANCE GWL_HINSTANCE
#define GWLP_USERDATA GWL_USERDATA
#define GWLP_ID GWL_ID
#define GCLP_HICON GCL_HICON
#define GCLP_HICONSM (-34)
#define GCLP_HCURSOR GCL_HCURSOR
#define GetWindowLongPtr GetWindowLong
#define SetWindowLongPtr SetWindowLong
#define GetClassLongPtr GetClassLong
#define SetClassLongPtr SetClassLong
#endif

#ifndef MAPVK_VK_TO_VSC
#define MAPVK_VK_TO_VSC 0
#define MAPVK_VSC_TO_VK 1
#define MAPVK_VK_TO_CHAR 2
#endif

#ifndef INVALID_FILE_ATTRIBUTES
#define INVALID_FILE_ATTRIBUTES ((DWORD)-1)
#endif
#ifndef WC_NO_BEST_FIT_CHARS
#define WC_NO_BEST_FIT_CHARS 0x400
#endif

#ifndef VER_MAJORVERSION
typedef struct _OSVERSIONINFOEXA {
	DWORD dwOSVersionInfoSize, dwMajorVersion, dwMinorVersion, dwBuildNumber, dwPlatformId;
	CHAR szCSDVersion[128];
	WORD wServicePackMajor, wServicePackMinor, wSuiteMask;
	BYTE wProductType, wReserved;
} OSVERSIONINFOEXA, *POSVERSIONINFOEXA, *LPOSVERSIONINFOEXA;
#define VER_MINORVERSION 0x0000001
#define VER_MAJORVERSION 0x0000002
#define VER_BUILDNUMBER 0x0000004
#define VER_PLATFORMID 0x0000008
#define VER_SERVICEPACKMINOR 0x0000010
#define VER_SERVICEPACKMAJOR 0x0000020
#define VER_EQUAL 1
#define VER_GREATER 2
#define VER_GREATER_EQUAL 3
#define VER_LESS 4
#define VER_LESS_EQUAL 5
#endif

/* shlobj.h's, from Internet Explorer 5. */
#ifndef CSIDL_FLAG_CREATE
#define CSIDL_FLAG_CREATE 0x8000
#define CSIDL_FLAG_DONT_VERIFY 0x4000
#define CSIDL_FLAG_MASK 0xFF00
#define SHGFP_TYPE_CURRENT 0
#define SHGFP_TYPE_DEFAULT 1
#endif
#ifndef CSIDL_PROGRAM_FILES
#define CSIDL_PROGRAM_FILES 0x0026
#endif
#ifndef CSIDL_LOCAL_APPDATA
#define CSIDL_LOCAL_APPDATA 0x001c
#define CSIDL_COMMON_APPDATA 0x0023
#define CSIDL_MYPICTURES 0x0027
#define CSIDL_PERSONAL 0x0005
#endif
#endif /* RC_INVOKED */
#endif
