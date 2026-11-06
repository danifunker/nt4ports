/* rar5test: list, then extract with the RAR5 reader. Mac-side test only. */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include "rar5api.h"
static long rd(void *c, void *b, unsigned long n) { return (long)fread(b, 1, n, (FILE *)c); }
static void mk(const char *p) { char t[4096]; strcpy(t, p); for (char *s = t + 1; *s; s++) if (*s == '/') { *s = 0; mkdir(t, 0777); *s = '/'; } }
int main(int argc, char **argv) {
	FILE *f = fopen(argv[1], "rb");
	const char *out = argc > 2 ? argv[2] : NULL;
	if (!f) return 2;
	struct r5handle *h = r5_open(rd, f);
	struct r5item it; int r, n = 0;
	while ((r = r5_next(h, &it)) == 0) {
		printf("L %10lld %10lld %s%s%s %s\n", (long long)it.size, (long long)it.packed, it.is_dir ? "D" : "-", it.is_link ? "L" : "-", it.encrypted ? "E" : "-", it.path);
		n++;
		if (r5_skip_packed(h)) { printf("skip failed\n"); break; }
	}
	printf("list r=%d n=%d solid=%d vol=%d err=[%s]\n", r, n, r5_is_solid(h), r5_is_volume(h), r5_error(h));
	r5_close(h);
	if (!out) return 0;
	fseek(f, 0, SEEK_SET);
	h = r5_open(rd, f);
	int bad = 0, files = 0;
	while ((r = r5_next(h, &it)) == 0) {
		if (strstr(it.path, "..") || it.path[0] == '/' || strlen(it.path) > 1000) { printf("refused path\n"); if (r5_skip_decode(h)) break; continue; }
		char p[4096]; snprintf(p, sizeof p, "%s/%s", out, it.path);
		if (it.is_dir) { mk(p); strcat(p, "/"); mk(p); continue; }
		mk(p);
		if (it.is_link) { if (r5_skip_decode(h)) break; continue; }
		FILE *o = fopen(p, "wb");
		const void *b; size_t s; int x; long long tot = 0;
		while ((x = r5_read(h, &b, &s)) == 0) { if (s && o) { fwrite(b, 1, s, o); tot += s; } }
		if (o) fclose(o);
		files++;
		if (x < 0) { bad++; printf("X %s: %s\n", it.path, r5_error(h)); }
	}
	printf("extract r=%d files=%d bad=%d err=[%s]\n", r, files, bad, r5_error(h));
	r5_close(h); fclose(f);
	return bad != 0;
}
