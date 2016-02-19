#ifndef _RTREE_RTREE_H_
#define _RTREE_RTREE_H_

struct RTree {
	struct Node *root;
	struct Context *context;
};

void insert(struct RTree *rtree, struct Entry entry);
struct Node * _choose_leaf(struct Node *node, struct Entry entry);
struct Node * _choose_leaf_recursive(struct Node *node, struct Entry entry);
void _adjust_tree(struct RTree *rtree, struct Node *node, struct Node *nnode);
void _adjust_tree_recursive(struct RTree *rtree, struct Node *node, struct Node *nnode, int level);

#endif