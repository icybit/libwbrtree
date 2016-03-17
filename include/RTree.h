#ifndef _RTREE_RTREE_H_
#define _RTREE_RTREE_H_

typedef struct RTree {
	rt_node_t *root;
	rt_ctx_t *context;
} rt_rtree_t;

void rtree_insert(rt_rtree_t *rtree, rt_entry_t *entry);
void rtree_create(rt_rtree_t *dest, rt_ctx_t *context);
int rtree_delete(rt_rtree_t *rtree, rt_entry_t *entry);
void rtree_destroy(rt_rtree_t *rtree);
void _rtree_adjust_tree(rt_rtree_t *rtree, rt_node_t *node, rt_node_t *nnode);
void _rtree_adjust_tree_recursive(rt_rtree_t *rtree, rt_node_t *node, rt_node_t *nnode, int level);
rt_node_t * _rtree_choose_leaf(rt_node_t *node, rt_entry_t *entry);
rt_node_t * _rtree_choose_leaf_recursive(rt_node_t *node, rt_entry_t *entry);
void _rtree_condense_tree(rt_rtree_t *rtree, rt_node_t *node);
void _rtree_condense_tree_recursive(rt_rtree_t *rtree, rt_node_t *node, struct hashset_st *condensed_nodes);
rt_node_t * _rtree_find_leaf(rt_node_t *node, rt_entry_t *entry);
rt_node_t * _rtree_find_leaf_recursive(rt_node_t *node, rt_entry_t *entry);
void rtree_search(rt_rtree_t *rtree, rt_rect_t *search_rectangle, struct hashset_st *results);
void _rtree_search_recursive(rt_node_t *node, rt_rect_t *search_rectangle, struct hashset_st *results);
void rtree_serialize(rt_rtree_t *rtree, unsigned char *buffer);
void _rtree_serialize_recursive(rt_node_t *node, unsigned char *buffer, size_t *index);
#ifdef DEBUG
void rtree_visualize(rt_rtree_t *rtree);
void _rtree_visualize_recursive(rt_node_t *node, uint16_t max_level);
#endif

#endif