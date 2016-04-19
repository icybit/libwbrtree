#ifndef _RTREE_RTREE_H_
#define _RTREE_RTREE_H_

#include "Common.h"
#include "hashset.h"

struct RTree {
	rt_node_t *root;
	rt_ctx_t *context;
};

RTREE_PUBLIC void rtree_insert(rt_rtree_t *rtree, rt_entry_t *entry);
RTREE_PUBLIC void rtree_create(rt_rtree_t *dest, rt_ctx_t *context);
RTREE_PUBLIC int rtree_delete(rt_rtree_t *rtree, rt_entry_t *entry);
RTREE_PUBLIC void rtree_destroy(rt_rtree_t *rtree);
RTREE_PUBLIC void rtree_search(rt_rtree_t *rtree, rt_rect_t *search_rectangle, struct hashset_st *results);
RTREE_PUBLIC size_t rtree_serialize(rt_rtree_t *rtree, uint8_t *buffer);
#ifdef DEBUG
RTREE_PUBLIC void rtree_visualize(rt_rtree_t *rtree);
#endif

#endif