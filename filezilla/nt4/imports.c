/* Functions FileZilla (through MFC) imports that NT 4.0's own comctl32.dll lacks: they
   came with Internet Explorer 3 and 4. Each is defined here under its import name
   (__imp_NAME), so the linker takes this, not comctl32.lib's: it calls comctl32's own
   when there is one, and otherwise does the same with what NT 4.0 has.
   For Alpha, MIPS, PowerPC and ARM, where C names have no leading underscore.
   Written for the NT 4.0 port. Licence: GPL v2 or later, as FileZilla. */
#include <windows.h>

typedef struct _IMAGELIST *HIMAGELIST_;

/* commctrl.h's IMAGELISTDRAWPARAMS, as Internet Explorer 3 has it */
typedef struct {
	DWORD cbSize;
	HIMAGELIST_ himl;
	int i;
	HDC hdcDst;
	int x, y, cx, cy, xBitmap, yBitmap;
	COLORREF rgbBk, rgbFg;
	UINT fStyle;
	DWORD dwRop;
} FZ_ILDRAWPARAMS;

#define ILC_MASK_ 0x0001
#define ILC_COLOR_ 0x0000

__declspec(dllimport) HIMAGELIST_ WINAPI ImageList_Create(int, int, UINT, int, int);
__declspec(dllimport) int WINAPI ImageList_GetImageCount(HIMAGELIST_);
__declspec(dllimport) BOOL WINAPI ImageList_GetIconSize(HIMAGELIST_, int *, int *);
__declspec(dllimport) HICON WINAPI ImageList_GetIcon(HIMAGELIST_, int, UINT);
__declspec(dllimport) int WINAPI ImageList_ReplaceIcon(HIMAGELIST_, int, HICON);
__declspec(dllimport) COLORREF WINAPI ImageList_GetBkColor(HIMAGELIST_);
__declspec(dllimport) COLORREF WINAPI ImageList_SetBkColor(HIMAGELIST_, COLORREF);
__declspec(dllimport) BOOL WINAPI ImageList_DrawEx(HIMAGELIST_, int, HDC, int, int, int, int,
	COLORREF, COLORREF, UINT);
__declspec(dllimport) BOOL WINAPI ImageList_Remove(HIMAGELIST_, int);
__declspec(dllimport) int WINAPI ImageList_Add(HIMAGELIST_, HBITMAP, HBITMAP);

static FARPROC comctl(const char *name)
{
	HMODULE h = GetModuleHandleA("comctl32.dll");
	return h ? GetProcAddress(h, name) : NULL;
}

static HIMAGELIST_ WINAPI fz_ImageList_Duplicate(HIMAGELIST_ himl)
{
	typedef HIMAGELIST_ (WINAPI *fn)(HIMAGELIST_);
	static fn own = (fn)-1;
	HIMAGELIST_ copy;
	int n, i, cx, cy;
	if (own == (fn)-1)
		own = (fn)comctl("ImageList_Duplicate");
	if (own)
		return own(himl);
	/* The images again, one icon at a time: colours and masks, not overlays. */
	if (!himl || !ImageList_GetIconSize(himl, &cx, &cy))
		return NULL;
	n = ImageList_GetImageCount(himl);
	copy = ImageList_Create(cx, cy, ILC_COLOR_ | ILC_MASK_, n, 4);
	if (!copy)
		return NULL;
	ImageList_SetBkColor(copy, ImageList_GetBkColor(himl));
	for (i = 0; i < n; i++) {
		HICON icon = ImageList_GetIcon(himl, i, 0);
		ImageList_ReplaceIcon(copy, -1, icon);
		if (icon)
			DestroyIcon(icon);
	}
	return copy;
}

static BOOL WINAPI fz_ImageList_DrawIndirect(FZ_ILDRAWPARAMS *p)
{
	typedef BOOL (WINAPI *fn)(FZ_ILDRAWPARAMS *);
	static fn own = (fn)-1;
	if (own == (fn)-1)
		own = (fn)comctl("ImageList_DrawIndirect");
	if (own)
		return own(p);
	if (!p || p->xBitmap || p->yBitmap)
		return FALSE;
	return ImageList_DrawEx(p->himl, p->i, p->hdcDst, p->x, p->y, p->cx, p->cy,
		p->rgbBk, p->rgbFg, p->fStyle);
}

static BOOL WINAPI fz_ImageList_SetImageCount(HIMAGELIST_ himl, UINT n)
{
	typedef BOOL (WINAPI *fn)(HIMAGELIST_, UINT);
	static fn own = (fn)-1;
	int cx, cy;
	if (own == (fn)-1)
		own = (fn)comctl("ImageList_SetImageCount");
	if (own)
		return own(himl, n);
	/* Fewer: the last ones go. More: blank ones are added. */
	while ((UINT)ImageList_GetImageCount(himl) > n)
		if (!ImageList_Remove(himl, ImageList_GetImageCount(himl) - 1))
			return FALSE;
	if ((UINT)ImageList_GetImageCount(himl) < n && ImageList_GetIconSize(himl, &cx, &cy)) {
		HDC dc = GetDC(NULL);
		HBITMAP bm = CreateCompatibleBitmap(dc, cx, cy);
		ReleaseDC(NULL, dc);
		while (bm && (UINT)ImageList_GetImageCount(himl) < n)
			if (ImageList_Add(himl, bm, NULL) < 0)
				break;
		if (bm)
			DeleteObject(bm);
	}
	return (UINT)ImageList_GetImageCount(himl) == n;
}

/* comctl32's _TrackMouseEvent came with Internet Explorer 3; NT 4.0's user32 has the
   function it wraps. */
static BOOL WINAPI fz_TrackMouseEvent(void *tme)
{
	typedef BOOL (WINAPI *fn)(void *);
	static fn own = (fn)-1;
	if (own == (fn)-1) {
		own = (fn)comctl("_TrackMouseEvent");
		if (!own)
			own = (fn)GetProcAddress(GetModuleHandleA("user32.dll"), "TrackMouseEvent");
	}
	return own ? own(tme) : FALSE;
}

void *__imp_ImageList_Duplicate = (void *)fz_ImageList_Duplicate;
void *__imp_ImageList_SetImageCount = (void *)fz_ImageList_SetImageCount;
void *__imp__TrackMouseEvent = (void *)fz_TrackMouseEvent;
void *__imp_ImageList_DrawIndirect = (void *)fz_ImageList_DrawIndirect;

/* An interface of Internet Explorer 4's shell, which FileZilla asks for and NT 4.0's
   shell doesn't have: not in the NT 4.0 SDK's uuid.lib. Visual C++ 6.0's has it. */
#ifndef _M_ALPHA
const GUID IID_IShellIconOverlay = { 0x7d688a70, 0xc613, 0x11d0, { 0x99, 0x9b, 0x00, 0xc0, 0x4f, 0xd6, 0x55, 0xe1 } };
#endif
