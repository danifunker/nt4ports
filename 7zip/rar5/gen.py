#!/usr/bin/env python3
# Turns libarchive's reader into rar5.cpp and BLAKE2 into C++-compilable copies.
import sys,re
src,out=sys.argv[1],sys.argv[2]
s=open(src+'/archive_read_support_format_rar5.c').read()
a=s.index('#include "archive_platform.h"'); b=s.index('/*#define CHECK_CRC_ON_SOLID_SKIP*/')
s=s[:a]+'#include "rar5shim.h"\n\n'+s[b:]
s=re.sub(r'\b(\d+)LL\b',r'RAR5_C64(\1)',s)
s=re.sub(r'\b(\d+)ULL\b',r'RAR5_U64(\1)',s)
s=s.replace('archive_string_sprintf(&version_string, ";%zu", version);','archive_string_sprintf(&version_string, ";%lu", (unsigned long)version);')
for old,new in [
 ("d->arr = malloc(sizeof(void*) * max_capacity_power_of_2);","d->arr = (size_t *)malloc(sizeof(void*) * max_capacity_power_of_2);"),
 ("struct filter_info* f = calloc(1, sizeof(*f));","struct filter_info* f = (struct filter_info *)calloc(1, sizeof(*f));"),
 ("rar->cstate.filtered_buf = malloc(flt->block_length);","rar->cstate.filtered_buf = (uint8_t *)malloc(flt->block_length);"),
 ("*ptr = __archive_read_ahead(a, how_many, &avail);","*ptr = (const uint8_t *)__archive_read_ahead(a, how_many, &avail);"),
 ("if ((p = __archive_read_ahead(a, 7, NULL)) == NULL)","if ((p = (const char *)__archive_read_ahead(a, 7, NULL)) == NULL)"),
 ("const char *buff = __archive_read_ahead(a, offset + window, &bytes_avail);","const char *buff = (const char *)__archive_read_ahead(a, offset + window, &bytes_avail);"),
 ("fflags_text = malloc(22 * sizeof(*fflags_text));","fflags_text = (char *)malloc(22 * sizeof(*fflags_text));"),
 ("rar->cstate.window_buf = calloc(1, rar->cstate.window_size);","rar->cstate.window_buf = (uint8_t *)calloc(1, rar->cstate.window_size);"),
 ("rar->cstate.filtered_buf = calloc(1, rar->cstate.window_size);","rar->cstate.filtered_buf = (uint8_t *)calloc(1, rar->cstate.window_size);"),
 ("rar->vol.push_buf = malloc(block_size + 8);","rar->vol.push_buf = (uint8_t *)malloc(block_size + 8);"),
 ("h = __archive_read_ahead(a, window, &bytes);","h = (const char *)__archive_read_ahead(a, window, &bytes);"),
]:
    assert old in s, old
    s=s.replace(old,new)
s=re.sub(r'(uint8_t\* new_window_buf =\s*)(realloc\()',r'\1(uint8_t *)\2',s)
s=s.replace('\t\t\tp = h;\n','\t\t\tp = (const char *)h;\n')
old="""				if (ARCHIVE_OK != consume(a, extra_field_size)) {
					return ARCHIVE_EOF;
				}
		}
	}
"""
new="""				if (ARCHIVE_OK != consume(a, extra_field_size)) {
					return ARCHIVE_EOF;
				}
				/* A skipped field is not an error: a recovery
				 * record's header has only EX_SUBDATA. */
				ret = ARCHIVE_OK;
		}
	}
"""
assert s.count(old)==1; s=s.replace(old,new)
old="	if(rar->file.solid > 0 && rar->cstate.data_encrypted == 0 &&\n	    rar->cstate.window_buf == NULL) {\n"
new="	if(rar->file.solid > 0 && rar->cstate.data_encrypted == 0 &&\n	    rar->cstate.window_buf == NULL && !rar->listing) {\n"
assert s.count(old)==1; s=s.replace(old,new)
old="struct rar5 {\n"
new="struct rar5 {\n	/* Header walk only: nothing is decoded, so no window exists. */\n	int listing;\n\n"
assert s.count(old)==1; s=s.replace(old,new)
old="			init_unpack(rar);\n"
new="""			init_unpack(rar);
			if(rar->cstate.window_size > 0 &&
			    (rar->cstate.window_buf == NULL ||
			    rar->cstate.filtered_buf == NULL)) {
				archive_set_error(&a->archive, ENOMEM,
				    "Not enough memory for the RAR5 dictionary");
				return ARCHIVE_FATAL;
			}
"""
assert s.count(old)==1; s=s.replace(old,new)
i=s.index('int archive_read_support_format_rar5(struct archive *_a)')
s=s[:i]+open('rar5api.inc').read()
i=s.index('static int rar5_capabilities'); j=s.index('static int rar5_init(struct rar5* rar)')
open(out+'/rar5.cpp','w').write(s)
for f in ('archive_blake2.h','archive_blake2_impl.h','archive_blake2s_ref.c','archive_blake2sp_ref.c'):
    t=open(src+'/'+f).read()
    t=t.replace('#include "archive_platform.h"\n','').replace('#include <stdint.h>','#include "rar5types.h"')
    t=t.replace('#define BLAKE2_PACKED(x) __pragma(pack(push, 1)) x __pragma(pack(pop))','#if _MSC_VER < 1300\n#define BLAKE2_PACKED(x) x /* layout is naturally packed; size checked below */\n#else\n#define BLAKE2_PACKED(x) __pragma(pack(push, 1)) x __pragma(pack(pop))\n#endif')
    if f=='archive_blake2_impl.h':
        i=t.index('static BLAKE2_INLINE void secure_zero_memory'); j=t.index('#endif',i)
        t=t[:i]+'static BLAKE2_INLINE void secure_zero_memory(void *v, size_t n)\n{\n  volatile unsigned char *p = (volatile unsigned char *)v;\n  while (n--) *p++ = 0;\n}\n\n'+t[j:]
    open(out+'/'+f.replace('_ref.c','.cpp'),'w').write(t)
