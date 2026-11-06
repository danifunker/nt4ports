/* shlwapi.h: local versions of the path functions Notepad++ 3.3 calls, for
 * NT 4.0 systems with no shlwapi.dll (it arrived with IE 4). ANSI only. */
#ifndef NPP_LOCAL_SHLWAPI
#define NPP_LOCAL_SHLWAPI
#include <windows.h>
#include <string.h>

static inline BOOL PathFileExists(const char *p) { return GetFileAttributesA(p) != (DWORD)-1; }
static inline BOOL PathIsDirectory(const char *p) {
	DWORD a = GetFileAttributesA(p);
	return a != (DWORD)-1 && (a & FILE_ATTRIBUTE_DIRECTORY);
}
static inline char *PathFindFileName(const char *p) {
	const char *r = p, *s;
	for (s = p; *s; s = CharNextA(s))
		if ((*s == '\\' || *s == '/' || *s == ':') && s[1]) r = s + 1;
	return (char *)r;
}
static inline char *PathFindExtension(const char *p) {
	const char *d = 0, *s;
	for (s = p; *s; s = CharNextA(s)) {
		if (*s == '\\' || *s == '/' || *s == ' ') d = 0;
		else if (*s == '.') d = s;
	}
	return (char *)(d ? d : s);
}
static inline BOOL PathRemoveFileSpec(char *p) {
	char *f = PathFindFileName(p);
	if (f == p) { if (*p) { *p = 0; return TRUE; } return FALSE; }
	if (f - p == 3 && p[1] == ':') { *f = 0; return TRUE; }
	if (f[-1] == '\\' || f[-1] == '/') f--;
	*f = 0;
	return TRUE;
}
static inline BOOL PathAppend(char *p, const char *more) {
	size_t n = strlen(p);
	while (*more == '\\') more++;
	if (n && p[n - 1] != '\\') { p[n++] = '\\'; p[n] = 0; }
	strcpy(p + n, more);
	return TRUE;
}
/* * and ? patterns, case-insensitive; several patterns separated by ; */
static inline BOOL nppWildMatch(const char *s, const char *p, const char *pe) {
	for (; p != pe; p++) {
		if (*p == '*') {
			for (;; s++) {
				if (nppWildMatch(s, p + 1, pe)) return TRUE;
				if (!*s) return FALSE;
			}
		}
		if (!*s) return FALSE;
		if (*p != '?' && (char)(DWORD)CharLowerA((LPSTR)(DWORD)(unsigned char)*p) !=
		    (char)(DWORD)CharLowerA((LPSTR)(DWORD)(unsigned char)*s)) return FALSE;
		s++;
	}
	return !*s;
}
static inline BOOL PathMatchSpec(const char *file, const char *spec) {
	const char *name = PathFindFileName(file);
	while (*spec) {
		const char *e = spec;
		while (*spec == ' ') e = ++spec;
		while (*e && *e != ';') e++;
		if (nppWildMatch(name, spec, e)) return TRUE;
		spec = *e ? e + 1 : e;
	}
	return FALSE;
}
#endif
