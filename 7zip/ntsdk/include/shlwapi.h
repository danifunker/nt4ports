/* shlwapi.h for building with tenox7/ntsdk: the NT 4.0 SDK has none. The file manager
 * includes it only for DllGetVersion's types, which it uses to ask comctl32 its version
 * (through GetProcAddress, so NT 4.0 without Internet Explorer 4 is fine). */
#ifndef P7_NTSDK_SHLWAPI_H
#define P7_NTSDK_SHLWAPI_H

#ifndef DLLVER_PLATFORM_WINDOWS
typedef struct _DLLVERSIONINFO {
    DWORD cbSize;
    DWORD dwMajorVersion;
    DWORD dwMinorVersion;
    DWORD dwBuildNumber;
    DWORD dwPlatformID;
} DLLVERSIONINFO;
#define DLLVER_PLATFORM_WINDOWS 0x00000001
#define DLLVER_PLATFORM_NT      0x00000002
typedef HRESULT (CALLBACK *DLLGETVERSIONPROC)(DLLVERSIONINFO *);
#endif

#endif
