
/* ---- rar5api.h: replaces archive_read_support_format_rar5(). ---- */
#include "rar5api.h"

struct r5handle {
	struct archive_read a;
	struct archive_entry *entry;
};

struct r5handle *r5_open(rar5_read_fn reader, void *ctx) {
	struct r5handle *h = (struct r5handle *)calloc(1, sizeof(*h));
	struct rar5 *rar;
	if (h == NULL) return NULL;
	rar = (struct rar5 *)malloc(sizeof(*rar));
	h->entry = archive_entry_new();
	if (rar == NULL || h->entry == NULL || ARCHIVE_OK != rar5_init(rar)) {
		free(rar); archive_entry_free(h->entry); free(h);
		return NULL;
	}
	h->a.format = &h->a.desc;
	h->a.desc.data = rar;
	h->a.reader = reader;
	h->a.reader_ctx = ctx;
	return h;
}

void r5_close(struct r5handle *h) {
	if (h == NULL) return;
	rar5_cleanup(&h->a);
	archive_entry_free(h->entry);
	free(h->a.buf);
	free(h);
}

int r5_next(struct r5handle *h, struct r5item *it) {
	struct rar5 *rar = get_context(&h->a);
	struct archive_entry *e = h->entry;
	int ret = rar5_read_header(&h->a, e);
	if (ret == ARCHIVE_EOF) return 1;
	if (ret < ARCHIVE_WARN) return -1;
	memset(it, 0, sizeof(*it));
	it->path = e->pathname != NULL ? e->pathname : "";
	it->size = e->size;
	it->has_size = e->has_size;
	it->packed = (int64_t)rar->file.bytes_remaining;
	it->mtime = e->mtime; it->has_mtime = e->has_mtime;
	it->ctime_ = e->ctime_; it->has_ctime = e->has_ctime;
	it->atime = e->atime; it->has_atime = e->has_atime;
	it->is_dir = (e->mode & AE_IFMT) == AE_IFDIR;
	it->is_link = (e->mode & AE_IFMT) == AE_IFLNK || e->link != NULL;
	it->encrypted = e->data_encrypted || e->metadata_encrypted;
	it->crc = rar->file.stored_crc32;
	it->has_crc = rar->file.stored_crc32 != 0;
	return 0;
}

int r5_skip_packed(struct r5handle *h) {
	struct rar5 *rar = get_context(&h->a);
	rar->listing = 1;
	if (ARCHIVE_OK != consume(&h->a, rar->file.bytes_remaining)) return -1;
	rar->file.bytes_remaining = 0;
	return 0;
}

int r5_skip_decode(struct r5handle *h) {
	int ret = rar5_read_data_skip(&h->a);
	return ret < ARCHIVE_WARN ? -1 : 0;
}

int r5_read(struct r5handle *h, const void **buf, size_t *size) {
	int64_t off = 0;
	int ret = rar5_read_data(&h->a, buf, size, &off);
	if (ret == ARCHIVE_EOF) return 1;
	if (ret < ARCHIVE_WARN) return -1;
	return 0;
}

int r5_bid(struct r5handle *h) { return rar5_bid(&h->a, 0) > 0; }
int r5_is_solid(struct r5handle *h) { return get_context(&h->a)->main.solid; }
int r5_is_volume(struct r5handle *h) { return get_context(&h->a)->main.volume; }
const char *r5_error(struct r5handle *h) { return h->a.archive.error; }
