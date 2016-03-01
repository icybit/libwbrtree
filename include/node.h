#ifndef _RTREE_NODE_H_
#define _RTREE_NODE_H_

struct Context {
	uint8_t m;
	uint8_t M;
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
	uint8_t capacity;
	uint8_t count;
	uint16_t level;
};

void context_create(struct Context *dest, uint8_t m, uint8_t M, float alloc_factor, struct Rectangle *space_MBR);

void entry_create(struct Entry *dest, void *tuple, struct Rectangle *MBR);

int node_add_entry(struct Node *node, void *entry);
void node_adjust_MBR(struct Node *node, void *entry);
struct Node * node_choose_optimal_entry(struct Node *node, struct Entry *entry);
void node_calculate_MBR(struct Rectangle *MBR, struct Node *node);
void _node_calculate_node_MBR(struct Rectangle *MBR, struct Node *node);
void _node_calculate_leaf_MBR(struct Rectangle *MBR, struct Node *leaf);
int _node_choose_split_axis(void **entries);
void _node_choose_split_index(uint8_t axis, void **entries, void *group_one, void *group_two);
void node_create(struct Node *dest, struct Context *context, struct Node *parent, void **entries, uint8_t entry_count, struct Rectangle *MBR, uint16_t level);
void node_delete_entry(struct Node *node, void *entry);
void node_destroy(struct Node *node);
int node_is_leaf(struct Node *node);
int node_is_root(struct Node *node);
struct Node * node_split_node(struct Node *node, void *entry);

#endif