/* ntsecapi.h for building with tenox7/ntsdk: the NT 4.0 SDK has none, so build.sh copies
 * the Windows 2000 SDK's to ie3/. It needs ULONG_PTR, which came with the 64-bit types. */
#ifndef P7_NTSDK_NTSECAPI_H
#define P7_NTSDK_NTSECAPI_H
#ifndef _BASETSD_H_
typedef unsigned long ULONG_PTR, *PULONG_PTR;
#endif
#include "ie3/NTSecAPI.h"
#endif
