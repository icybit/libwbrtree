#ifndef __RTREE_RTREE_H__
#define __RTREE_RTREE_H__

#include <stdint.h>
#include <stddef.h>
#include "wbrtree/wbrtreeapi.h"
#include "Common.h"

struct RTree {
	rt_node_t *root;
	rt_ctx_t *context;
};

void insert(rt_rtree_t *rtree, rt_entry_t *entry);
rt_entry_t * try_insert(rt_rtree_t *rtree, rt_entry_t *entry);
rt_rtree_t * create(rt_ctx_t *context);
int delete(rt_rtree_t *rtree, rt_entry_t *entry);
int delete_area(rt_rtree_t *rtree, rt_rect_t *MBR);
void destroy(rt_rtree_t **rtree);
void search(rt_rtree_t *rtree, rt_rect_t *search_rectangle, rt_hset_t *results);
size_t serialize(rt_rtree_t *rtree, uint8_t **buffer);
rt_rtree_t * split(rt_rtree_t *rtree);

#ifdef DEBUG
void visualize(rt_rtree_t *rtree);
#endif

#endif /* __RTREE_RTREE_H__ */