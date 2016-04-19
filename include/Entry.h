#ifndef _RTREE_ENTRY_H_
#define _RTREE_ENTRY_H_

#include <stdint.h>
#include <stddef.h>
#include "Common.h"

struct Entry {
	void *tuple;
	rt_rect_t *MBR;
};

RTREE_LOCAL size_t entry_calculate_buffer_size(rt_ctx_t *context);
RTREE_LOCAL int entry_compare(const void *entry, const void *other, void *dimension);
RTREE_PUBLIC void entry_create(rt_entry_t *dest, void *tuple, rt_rect_t *MBR);
#ifdef DEBUG
RTREE_LOCAL void entry_print(rt_entry_t *entry);
#endif
RTREE_LOCAL size_t entry_serialize(rt_entry_t *entry, uint8_t *buffer, size_t entry_size);

#endif