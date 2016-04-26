#ifndef _RTREE_NODE_H_
#define _RTREE_NODE_H_

#include <stdint.h>
#include "Common.h"

struct Node {
	rt_ctx_t *context;
	struct Node *parent;
	void **entries;
	rt_rect_t *MBR;
	uint8_t capacity;
	uint8_t count;
	uint16_t level;
};

RTREE_LOCAL int node_add_entry(rt_node_t *node, void *entry);
RTREE_LOCAL void node_adjust_MBR(rt_node_t *node, void *entry);
RTREE_LOCAL void node_calculate_MBR(rt_rect_t *MBR, rt_node_t *node);
RTREE_LOCAL void * node_choose_optimal_entry(rt_node_t *node, rt_entry_t *entry);
RTREE_LOCAL int node_compare(const void *entry, const void *other, void *dimension);
RTREE_LOCAL void node_copy(rt_node_t *dest, const rt_node_t *source);
RTREE_LOCAL void node_create(rt_node_t *dest, rt_ctx_t *context, rt_node_t *parent, void **entries, uint8_t entry_count, rt_rect_t *MBR, uint16_t level);
RTREE_LOCAL void node_delete_entry(rt_node_t *node, void *entry);
RTREE_LOCAL void node_destroy(rt_node_t *node);
RTREE_LOCAL int node_is_leaf(rt_node_t *node);
RTREE_LOCAL int node_is_root(rt_node_t *node);
RTREE_LOCAL rt_node_t * node_split(rt_node_t *node, void *entry);

#endif