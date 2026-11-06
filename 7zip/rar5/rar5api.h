/* rar5api.h: small interface over the RAR5 reader, used by the 7-Zip handler. */
#ifndef RAR5API_H
#define RAR5API_H
#include <time.h>
#include "rar5types.h"
typedef long (*rar5_read_fn)(void *ctx, void *buf, unsigned long n);
struct r5handle;
struct r5item {
	const char *path;       /* UTF-8, valid until the next r5_next */
	int64_t size;
	int has_size;
	int64_t packed;
	time_t mtime, ctime_, atime;
	int has_mtime, has_ctime, has_atime;
	int is_dir, is_link, encrypted;
	uint32_t crc;
	int has_crc;
};
struct r5handle *r5_open(rar5_read_fn reader, void *ctx);
int r5_bid(struct r5handle *);                    /* 1 if this is RAR5 */
int r5_next(struct r5handle *, struct r5item *);  /* 0 ok, 1 end, <0 error */
int r5_skip_packed(struct r5handle *);            /* header walk: no decoding */
int r5_skip_decode(struct r5handle *);            /* discard, keeping solid state */
int r5_read(struct r5handle *, const void **buf, size_t *size); /* 0 ok, 1 end, <0 error */
int r5_is_solid(struct r5handle *);
int r5_is_volume(struct r5handle *);
const char *r5_error(struct r5handle *);
void r5_close(struct r5handle *);
#endif
