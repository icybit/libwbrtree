#ifndef _RTREE_NODE_H_
#define _RTREE_NODE_H_

struct Context {
	int m;
	int M;
	float alloc_factor;
	struct Rectangle *space;
};

struct Entry {
	void *tuple;
	struct Rectangle *MBR;
};

struct Node {
	struct Context *context;
	struct Node *parent;
	void **entries;
	struct Rectangle *MBR;
	int capacity;
	int count;
	int level;
};

void context_create(struct Context *dest, int m, int M, float alloc_factor, struct Rectangle *space_MBR);

void entry_create(struct Entry *dest, void *tuple, struct Rectangle *MBR);

int node_add_entry(struct Node *node, void *entry);
void node_adjust_MBR(struct Node *node, void *entry);
struct Node * node_choose_optimal_entry(struct Node *node, struct Entry *entry);
void node_calculate_MBR(struct Rectangle *MBR, struct Node *node);
void _node_calculate_node_MBR(struct Rectangle *MBR, struct Node *node);
void _node_calculate_leaf_MBR(struct Rectangle *MBR, struct Node *leaf);
void node_create(struct Node *dest, struct Context *context, struct Node *parent, void **entries, struct Rectangle *MBR, int level);
void node_delete_entry(struct Node *node, void *entry);
void node_destroy(struct Node *node);
int node_is_leaf(struct Node *node);
int node_is_root(struct Node *node);
struct Node * node_split_node(struct Node *node, void *entry);

#endif