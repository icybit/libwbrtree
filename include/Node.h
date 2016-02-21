#ifndef _RTREE_NODE_H_
#define _RTREE_NODE_H_

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define NELEMS(x) (sizeof(x)/sizeof((x)[0]))
#define NALLOC(m, M, f) MIN((int)ceil(m * f), M)

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

int add_entry(struct Node *node, void *entry);
void adjust_MBR(struct Node *node, void *entry);
struct Node * choose_optimal_entry(struct Node *node, struct Entry *entry);
void calculate_MBR(struct Rectangle *MBR, struct Node *node);
void _calculate_node_MBR(struct Rectangle *MBR, struct Node *node);
void _calculate_leaf_MBR(struct Rectangle *MBR, struct Node *leaf);
void create_node(struct Node *dest, struct Context *context, struct Node *parent, void **entries, struct Rectangle *MBR, int level);
int is_leaf(struct Node *node);
struct Node * split_node(struct Node *node, void *entry);

#endif