/* rar5shim.h: the small part of libarchive's internal API that
 * archive_read_support_format_rar5.c uses, so that file can run on its own. */
#ifndef RAR5SHIM_H
#define RAR5SHIM_H
#ifdef _MSC_VER
/* ssize_t is 32 bits here, as in any 32-bit libarchive build. */
#pragma warning(disable: 4244)
#endif
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdarg.h>
#include <limits.h>
#include <errno.h>
#include <time.h>
#include "rar5types.h"
#include "archive_blake2.h"

#define ARCHIVE_EOF 1
#define ARCHIVE_OK 0
#define ARCHIVE_RETRY (-10)
#define ARCHIVE_WARN (-20)
#define ARCHIVE_FAILED (-25)
#define ARCHIVE_FATAL (-30)
#define ARCHIVE_ERRNO_FILE_FORMAT 84
#define ARCHIVE_ERRNO_PROGRAMMER 22
#define ARCHIVE_FORMAT_RAR_V5 0x100000
#define ARCHIVE_READ_MAGIC 0xdeb0c5U
#define ARCHIVE_STATE_NEW 1U
#define ARCHIVE_READ_FORMAT_CAPS_ENCRYPT_DATA 1
#define ARCHIVE_READ_FORMAT_CAPS_ENCRYPT_METADATA 2
#define ARCHIVE_READ_FORMAT_ENCRYPTION_UNSUPPORTED (-2)
#define ARCHIVE_READ_FORMAT_ENCRYPTION_DONT_KNOW (-1)
#define AE_IFMT 0170000
#define AE_IFREG 0100000
#define AE_IFLNK 0120000
#define AE_IFDIR 0040000
#define AE_SYMLINK_TYPE_UNDEFINED 0
#define AE_SYMLINK_TYPE_FILE 1
#define AE_SYMLINK_TYPE_DIRECTORY 2
#define __LA_MODE_T unsigned int
typedef int64_t la_int64_t;
typedef ssize_t la_ssize_t;

#define archive_check_magic(a, m, s, f) ((void)0)

struct archive {
	int archive_format;
	const char *archive_format_name;
	char error[256];
	int error_number;
};
struct archive_read;
struct archive_format_descriptor { void *data; };

/* Reads up to n bytes; returns the count, 0 at end, negative on error. */
typedef long (*rar5_read_fn)(void *ctx, void *buf, unsigned long n);

struct archive_read {
	struct archive archive;
	struct archive_format_descriptor desc;
	struct archive_format_descriptor *format;
	rar5_read_fn reader;
	void *reader_ctx;
	unsigned char *buf;
	size_t buf_size, buf_pos, buf_len;
	int eof;
	uint64_t consumed;
};

struct archive_string { char *s; size_t length; size_t buffer_length; };
#define archive_string_init(a) \
	do { (a)->s = NULL; (a)->length = 0; (a)->buffer_length = 0; } while (0)
void archive_string_free(struct archive_string *);
void archive_strcat(struct archive_string *, const void *);
void archive_string_sprintf(struct archive_string *, const char *, ...);

struct archive_entry {
	char *pathname;
	char *link;
	int64_t size;
	int has_size;
	time_t mtime, ctime_, atime;
	int has_mtime, has_ctime, has_atime;
	unsigned int mode;
	int data_encrypted, metadata_encrypted;
};
struct archive_entry *archive_entry_new(void);
void archive_entry_free(struct archive_entry *);
struct archive_entry *archive_entry_clear(struct archive_entry *);
const char *archive_entry_pathname_utf8(struct archive_entry *);
int archive_entry_update_pathname_utf8(struct archive_entry *, const char *);
int archive_entry_update_symlink_utf8(struct archive_entry *, const char *);
int archive_entry_update_hardlink_utf8(struct archive_entry *, const char *);
void archive_entry_set_size(struct archive_entry *, la_int64_t);
void archive_entry_set_mtime(struct archive_entry *, time_t, long);
void archive_entry_set_ctime(struct archive_entry *, time_t, long);
void archive_entry_set_atime(struct archive_entry *, time_t, long);
void archive_entry_set_mode(struct archive_entry *, unsigned int);
void archive_entry_set_filetype(struct archive_entry *, unsigned int);
void archive_entry_set_is_data_encrypted(struct archive_entry *, char);
void archive_entry_set_is_metadata_encrypted(struct archive_entry *, char);
#define archive_entry_set_symlink_type(e, t) ((void)0)
#define archive_entry_set_uname(e, s) ((void)0)
#define archive_entry_set_gname(e, s) ((void)0)
#define archive_entry_set_uid(e, i) ((void)0)
#define archive_entry_set_gid(e, i) ((void)0)
#define archive_entry_copy_fflags_text(e, t) ((const char *)0)

void archive_set_error(struct archive *, int, const char *, ...);
const void *__archive_read_ahead(struct archive_read *, size_t, ssize_t *);
int64_t __archive_read_consume(struct archive_read *, int64_t);
unsigned long crc32(unsigned long, const void *, unsigned int);

static inline uint16_t archive_le16dec(const void *pp) {
	const unsigned char *p = (const unsigned char *)pp;
	return (uint16_t)(p[0] | (p[1] << 8));
}
static inline uint32_t archive_le32dec(const void *pp) {
	const unsigned char *p = (const unsigned char *)pp;
	return (uint32_t)p[0] | ((uint32_t)p[1] << 8) |
	    ((uint32_t)p[2] << 16) | ((uint32_t)p[3] << 24);
}
static inline uint64_t archive_le64dec(const void *pp) {
	const unsigned char *p = (const unsigned char *)pp;
	return (uint64_t)archive_le32dec(p) |
	    ((uint64_t)archive_le32dec(p + 4) << 32);
}
static inline void archive_le32enc(void *pp, uint32_t u) {
	unsigned char *p = (unsigned char *)pp;
	p[0] = (unsigned char)u; p[1] = (unsigned char)(u >> 8);
	p[2] = (unsigned char)(u >> 16); p[3] = (unsigned char)(u >> 24);
}
#endif
