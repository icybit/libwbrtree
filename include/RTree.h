#ifndef _RTREE_RTREE_H_
#define _RTREE_RTREE_H_

struct RTree {
	struct Node *root;
	struct Context *context;
};

void rtree_insert(struct RTree *rtree, struct Entry *entry);
int rtree_delete(struct RTree *rtree, struct Entry *entry);
void _rtree_adjust_tree(struct RTree *rtree, struct Node *node, struct Node *nnode);
void _rtree_adjust_tree_recursive(struct RTree *rtree, struct Node *node, struct Node *nnode, int level);
struct Node * _rtree_choose_leaf(struct Node *node, struct Entry *entry);
struct Node * _rtree_choose_leaf_recursive(struct Node *node, struct Entry *entry);
void _rtree_condense_tree(struct RTree *rtree, struct Node *node);
void _rtree_condense_tree_recursive(struct RTree *rtree, struct Node *node, struct hashset_st *condensed_nodes);
struct Node * _rtree_find_leaf(struct Node *node, struct Entry *entry);
struct Node * _rtree_find_leaf_recursive(struct Node *node, struct Entry *entry);
struct hashset_st * rtree_search(struct RTree *rtree, struct Rectangle *search_rectangle);
void _rtree_search_recursive(struct Node *node, struct Rectangle *search_rectangle, struct hashset_st *results);
#endif