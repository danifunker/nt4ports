/* rar5shim.cpp: implementation of rar5shim.h. */
#include "rar5shim.h"

void archive_string_free(struct archive_string *a) {
	free(a->s); a->s = NULL; a->length = a->buffer_length = 0;
}
void archive_strcat(struct archive_string *a, const void *p) {
	const char *s = (const char *)p;
	size_t n = strlen(s);
	if (a->length + n + 1 > a->buffer_length) {
		size_t want = (a->length + n + 1) * 2;
		char *q = (char *)realloc(a->s, want);
		if (q == NULL) return;
		a->s = q; a->buffer_length = want;
	}
	memcpy(a->s + a->length, s, n + 1);
	a->length += n;
}
void archive_string_sprintf(struct archive_string *a, const char *f, ...) {
	char tmp[64];
	va_list ap;
	va_start(ap, f);
	vsprintf(tmp, f, ap);
	va_end(ap);
	archive_strcat(a, tmp);
}

static char *dup_str(const char *s) {
	char *r = (char *)malloc(strlen(s) + 1);
	if (r != NULL) strcpy(r, s);
	return r;
}
struct archive_entry *archive_entry_new(void) {
	return (struct archive_entry *)calloc(1, sizeof(struct archive_entry));
}
struct archive_entry *archive_entry_clear(struct archive_entry *e) {
	free(e->pathname); free(e->link);
	memset(e, 0, sizeof(*e));
	return e;
}
void archive_entry_free(struct archive_entry *e) {
	if (e != NULL) { archive_entry_clear(e); free(e); }
}
const char *archive_entry_pathname_utf8(struct archive_entry *e) { return e->pathname; }
int archive_entry_update_pathname_utf8(struct archive_entry *e, const char *s) {
	free(e->pathname); e->pathname = dup_str(s); return 1;
}
int archive_entry_update_symlink_utf8(struct archive_entry *e, const char *s) {
	free(e->link); e->link = dup_str(s); return 1;
}
int archive_entry_update_hardlink_utf8(struct archive_entry *e, const char *s) {
	free(e->link); e->link = dup_str(s); return 1;
}
void archive_entry_set_size(struct archive_entry *e, la_int64_t s) { e->size = s; e->has_size = 1; }
void archive_entry_set_mtime(struct archive_entry *e, time_t t, long) { e->mtime = t; e->has_mtime = 1; }
void archive_entry_set_ctime(struct archive_entry *e, time_t t, long) { e->ctime_ = t; e->has_ctime = 1; }
void archive_entry_set_atime(struct archive_entry *e, time_t t, long) { e->atime = t; e->has_atime = 1; }
void archive_entry_set_mode(struct archive_entry *e, unsigned int m) { e->mode = m; }
void archive_entry_set_filetype(struct archive_entry *e, unsigned int t) {
	e->mode = (e->mode & ~(unsigned int)AE_IFMT) | t;
}
void archive_entry_set_is_data_encrypted(struct archive_entry *e, char v) { e->data_encrypted = v; }
void archive_entry_set_is_metadata_encrypted(struct archive_entry *e, char v) { e->metadata_encrypted = v; }

void archive_set_error(struct archive *a, int num, const char *f, ...) {
	/* The format text alone: no vsprintf, so %zu (unknown to old C runtimes)
	 * and long file names are harmless. */
	a->error_number = num;
	strncpy(a->error, f, sizeof(a->error) - 1);
	a->error[sizeof(a->error) - 1] = 0;
}

const void *__archive_read_ahead(struct archive_read *a, size_t min, ssize_t *avail) {
	size_t have = a->buf_len - a->buf_pos;
	while (have < min && !a->eof) {
		if (a->buf_pos > 0) {
			memmove(a->buf, a->buf + a->buf_pos, have);
			a->buf_pos = 0; a->buf_len = have;
		}
		if (min + 65536 > a->buf_size) {
			size_t want = min + 65536 + 65536;
			unsigned char *q = (unsigned char *)realloc(a->buf, want);
			if (q == NULL) { if (avail) *avail = -1; return NULL; }
			a->buf = q; a->buf_size = want;
		}
		long got = a->reader(a->reader_ctx, a->buf + a->buf_len,
		    (unsigned long)(a->buf_size - a->buf_len));
		if (got < 0) { if (avail) *avail = -1; return NULL; }
		if (got == 0) a->eof = 1;
		a->buf_len += (size_t)got;
		have = a->buf_len - a->buf_pos;
	}
	if (avail) *avail = (ssize_t)have;
	if (have < min) return NULL;
	return a->buf + a->buf_pos;
}

int64_t __archive_read_consume(struct archive_read *a, int64_t n) {
	int64_t left = n;
	if (n < 0) return -1;
	while (left > 0) {
		size_t have = a->buf_len - a->buf_pos;
		if (have == 0) {
			ssize_t av = 0;
			if (__archive_read_ahead(a, 1, &av) == NULL) return -1;
			have = a->buf_len - a->buf_pos;
		}
		size_t take = have;
		if ((int64_t)take > left) take = (size_t)left;
		a->buf_pos += take;
		left -= (int64_t)take;
	}
	a->consumed += (uint64_t)n;
	return n;
}

unsigned long crc32(unsigned long crc, const void *buf, unsigned int len) {
	static uint32_t table[256];
	static int ready = 0;
	const unsigned char *p = (const unsigned char *)buf;
	uint32_t c;
	if (!ready) {
		uint32_t i, j;
		for (i = 0; i < 256; i++) {
			c = i;
			for (j = 0; j < 8; j++)
				c = (c & 1) ? (0xEDB88320U ^ (c >> 1)) : (c >> 1);
			table[i] = c;
		}
		ready = 1;
	}
	if (buf == NULL) return 0;
	c = (uint32_t)crc ^ 0xFFFFFFFFU;
	while (len--) c = table[(c ^ *p++) & 0xFF] ^ (c >> 8);
	return c ^ 0xFFFFFFFFU;
}
