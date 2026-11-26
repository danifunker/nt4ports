/* uxtheme.h for the NT 4.0 build of FileZilla: just the types misc/VisualStylesXP.h
   names. NT 4.0 has no UxTheme.dll, so its loader finds nothing and every call fails
   as the class's "Fail" functions do. Written for this port. */
#ifndef FZ_NT4_UXTHEME_H
#define FZ_NT4_UXTHEME_H
typedef HANDLE HTHEME;
enum THEMESIZE { TS_MIN, TS_TRUE, TS_DRAW };
enum PROPERTYORIGIN { PO_STATE, PO_PART, PO_CLASS, PO_GLOBAL, PO_NOTFOUND };
typedef struct _MARGINS { int cxLeftWidth, cxRightWidth, cyTopHeight, cyBottomHeight; } MARGINS, *PMARGINS;
#define MAX_INTLIST_COUNT 10
typedef struct _INTLIST { int iValueCount; int iValues[MAX_INTLIST_COUNT]; } INTLIST, *PINTLIST;
#endif
