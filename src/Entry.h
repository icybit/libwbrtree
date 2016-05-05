#ifndef __RTREE_ENTRY_H__
#define __RTREE_ENTRY_H__

#include <stdint.h>
#include <stddef.h>
#include "wbrtree/wbrtreeapi.h"
#include "Common.h"

struct Entry {
	void *tuple;
	rt_rect_t *MBR;
};

int entry_compare(const void *entry, const void *other, void *cmp_opts);
#ifdef DEBUG
void entry_print(rt_entry_t *entry);
#endif
size_t entry_serialize(rt_entry_t *entry, uint8_t *buffer, size_t entry_size);

#endif /* __RTREE_ENTRY_H__ */