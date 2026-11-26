// The pieces FileZilla 2.2.32 takes from libraries that NT 4.0 doesn't have.
// Written for the NT 4.0 port. Licence: GPL v2 or later, as FileZilla.
#include "stdafx.h"
#include <idna.h>

// libidn's entry point: ASCII names pass, others fail (see include/idna.h).
extern "C" int idna_to_ascii_8z(const char *input, char **output, int flags)
{
	const unsigned char *p;
	for (p = (const unsigned char *)input; *p; p++)
		if (*p >= 0x80)
			return 1;
	*output = (char *)malloc(strlen(input) + 1);
	if (!*output)
		return 1;
	strcpy(*output, input);
	return IDNA_SUCCESS;
}

// comctl32's version as (major << 16) | minor. DllGetVersion came with 4.71
// (Internet Explorer 4): without it, it is NT 4.0's own, taken as 4.70.
extern "C" DWORD fz_comctl32_version(void)
{
	typedef struct { DWORD cbSize, dwMajorVersion, dwMinorVersion, dwBuildNumber, dwPlatformID; } DLLVERSIONINFO_;
	typedef HRESULT (CALLBACK *DLLGETVERSIONPROC_)(DLLVERSIONINFO_ *);
	HMODULE h = GetModuleHandle(_T("comctl32.dll"));
	DLLGETVERSIONPROC_ get = h ? (DLLGETVERSIONPROC_)GetProcAddress(h, "DllGetVersion") : NULL;
	DLLVERSIONINFO_ v = { sizeof(DLLVERSIONINFO_) };
	if (!get || FAILED(get(&v)))
		return 0x00040046;
	return (v.dwMajorVersion << 16) | v.dwMinorVersion;
}

#ifdef _M_ARM
// Microsoft C 19 calls this when an exception leaves a function that may not throw;
// the PINT SDK's older C library hasn't it (as in this repository's other PINT ports).
extern "C" void __std_terminate(void)
{
	abort();
}
#endif
