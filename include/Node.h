#ifndef _RTREE_NODE_H_
#define _RTREE_NODE_H_

struct Context {
	uint8_t m;
	uint8_t M;
	uint8_t dim;
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

int entry_compare(const void *entry, const void *other, void *dimension);
void entry_create(struct Entry *dest, void *tuple, struct Rectangle *MBR);
#ifdef DEBUG
void entry_print(struct Entry *entry);
#endif

int node_add_entry(struct Node *node, void *entry);
void node_adjust_MBR(struct Node *node, void *entry);
struct Node * node_choose_optimal_entry(struct Node *node, struct Entry *entry);
void node_calculate_MBR(struct Rectangle *MBR, struct Node *node);
void _node_calculate_node_MBR(struct Rectangle *MBR, struct Node *node);
void _node_calculate_leaf_MBR(struct Rectangle *MBR, struct Node *leaf);
uint8_t _node_choose_split_axis(struct Node *node, void ***sorted_entries, struct Rectangle *MBR_one, struct Rectangle *MBR_two);
uint8_t _node_choose_split_index(uint8_t dimension, struct Node *node, void ***sorted_entries, struct Rectangle *MBR_one, struct Rectangle *MBR_two);
void node_create(struct Node *dest, struct Context *context, struct Node *parent, void **entries, uint8_t entry_count, struct Rectangle *MBR, uint16_t level);
void node_delete_entry(struct Node *node, void *entry);
void node_destroy(struct Node *node);
double _node_evaluate_distribution(uint8_t k, void ***sorted_entries, uint8_t dimension, struct Node *node, struct Rectangle *MBR_one, struct Rectangle *MBR_two, double(*evaluator)(struct Rectangle *MBR_one, struct Rectangle *MBR_two));
struct Rectangle * _node_get_entry_MBR(struct Node *node, void *entry);
int node_is_leaf(struct Node *node);
int node_is_root(struct Node *node);
struct Node * node_split_node(struct Node *node, void *entry);

#endif