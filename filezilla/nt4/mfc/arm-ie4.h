/* arm-ie4.h: forced include (-FI) for the ARM build of MFC 6 and FileZilla with the PINT
   SDK, whose headers are NT 4.0's. MFC 6 includes some of Internet Explorer 4's COM
   headers (prepare.py's ie4/), which want the newer rpcndr.h's version and macros:
   those are given here, PINT's rpcndr.h being used. Written for this port. */
#ifndef FZ_ARM_IE4_H
#define FZ_ARM_IE4_H
#define __RPCNDR_H_VERSION__ 440
#ifndef DECLSPEC_UUID
#define DECLSPEC_UUID(x) __declspec(uuid(x))
#endif
#ifndef DECLSPEC_NOVTABLE
#define DECLSPEC_NOVTABLE __declspec(novtable)
#endif
#ifndef MIDL_INTERFACE
#define MIDL_INTERFACE(x) struct DECLSPEC_UUID(x) DECLSPEC_NOVTABLE
#endif

/* PINT's C library has _atoi64; its headers don't say so. */
#ifdef __cplusplus
extern "C"
#endif
__int64 __cdecl _atoi64(const char *);
#define _ttoi64 _atoi64
#endif
