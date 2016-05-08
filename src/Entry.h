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
rt_entry_t * entry_create(void *tuple, rt_rect_t *MBR);
void entry_destroy(rt_entry_t *entry);
void * entry_get_tuple(rt_entry_t *entry);
#ifdef DEBUG
void entry_print(rt_entry_t *entry);
#endif

#endif /* __RTREE_ENTRY_H__ */