/* SDL_config.h for SDL 1.2.15 on Windows NT 4.0 (Alpha, MIPS, PowerPC and ARM), built
 * with Visual C++ 4.1 (tenox7/ntsdk) or the PINT 4.0 SDK. The build copies it to
 * include/SDL_config.h. Like SDL_config_win32.h, without DirectX, OpenGL or assembly:
 * GDI (windib) video and waveOut sound only. */
#ifndef _SDL_config_h
#define _SDL_config_h

#include "SDL_platform.h"

typedef signed __int8		int8_t;
typedef unsigned __int8		uint8_t;
typedef signed __int16		int16_t;
typedef unsigned __int16	uint16_t;
typedef signed __int32		int32_t;
typedef unsigned __int32	uint32_t;
typedef signed __int64		int64_t;
typedef unsigned __int64	uint64_t;
#ifndef _UINTPTR_T_DEFINED
typedef unsigned int		uintptr_t;
#define _UINTPTR_T_DEFINED
#endif
#if !defined(DWORD_PTR)
#define DWORD_PTR DWORD
#endif
#if !defined(LONG_PTR)
#define LONG_PTR LONG
#endif
#define SDL_HAS_64BIT_TYPE	1

#define HAVE_LIBC	1
#define HAVE_STDIO_H 1
#define STDC_HEADERS 1
#define HAVE_STRING_H 1
#define HAVE_CTYPE_H 1
#define HAVE_MATH_H 1
#define HAVE_SIGNAL_H 1
#define HAVE_MALLOC 1
#define HAVE_CALLOC 1
#define HAVE_REALLOC 1
#define HAVE_FREE 1
#define HAVE_ALLOCA 1
#define HAVE_QSORT 1
#define HAVE_ABS 1
#define HAVE_MEMSET 1
#define HAVE_MEMCPY 1
#define HAVE_MEMMOVE 1
#define HAVE_MEMCMP 1
#define HAVE_STRLEN 1
#define HAVE__STRREV 1
#define HAVE__STRUPR 1
#define HAVE__STRLWR 1
#define HAVE_STRCHR 1
#define HAVE_STRRCHR 1
#define HAVE_STRSTR 1
#define HAVE_ITOA 1
#define HAVE__LTOA 1
#define HAVE__ULTOA 1
#define HAVE_STRTOL 1
#define HAVE_STRTOUL 1
#define HAVE_STRTOD 1
#define HAVE_ATOI 1
#define HAVE_ATOF 1
#define HAVE_STRCMP 1
#define HAVE_STRNCMP 1
#define HAVE__STRICMP 1
#define HAVE__STRNICMP 1
#define HAVE_SSCANF 1

#define SDL_AUDIO_DRIVER_WAVEOUT	1
#define SDL_AUDIO_DRIVER_DISK	1
#define SDL_AUDIO_DRIVER_DUMMY	1
#define SDL_CDROM_WIN32		1
#define SDL_JOYSTICK_WINMM	1
#define SDL_LOADSO_WIN32	1
#define SDL_THREAD_WIN32	1
#define SDL_TIMER_WIN32	1
#define SDL_VIDEO_DRIVER_DUMMY	1
#define SDL_VIDEO_DRIVER_WINDIB	1
#define SDL_VIDEO_DISABLE_SCREENSAVER	1

/* Windows 2000's; NT 4.0 never returns it, so SDL keeps black and white in place. */
#ifndef SYSPAL_NOSTATIC256
#define SYSPAL_NOSTATIC256 3
#endif

#endif
