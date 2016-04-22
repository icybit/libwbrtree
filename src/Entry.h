#ifndef __RTREE_ENTRY_H__
#define __RTREE_ENTRY_H__

#include <stdint.h>
#include <stddef.h>
#include "wbdrtree/wbdrtreeapi.h"
#include "Common.h"

struct Entry {
	void *tuple;
	rt_rect_t *MBR;
};

size_t entry_calculate_buffer_size(rt_ctx_t *context);
int entry_compare(const void *entry, const void *other, void *dimension);
#ifdef DEBUG
void entry_print(rt_entry_t *entry);
#endif
size_t entry_serialize(rt_entry_t *entry, uint8_t *buffer, size_t entry_size);

#endif /* __RTREE_ENTRY_H__ */