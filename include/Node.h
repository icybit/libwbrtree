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

struct Entry {	// (2) If (1), Non-Leaf Node struct Entry *entries becomes struct Node *entries
	struct Node *child;
	struct Rectangle *MBR;	// (1) Points to child->MBR. COULD BE OPTIMIZED AWAY. IF TRUE, REFER TO (2)
};

struct Node {
	struct Context *context;
	struct Node *parent;
	struct Entry *entries;
	struct Rectangle *MBR;
	int capacity;
	int count;
	int level;
};

int add_entry(struct Node *node, struct Entry entry);
void adjust_parent_entry(struct Node *node);
struct Entry * choose_optimal_entry(struct Node *node, struct Entry entry);
void create_entry(struct Entry *dest, struct Node *child);
void create_node(struct Node *dest, struct Context *context, struct Node *parent, struct Entry *entries, int level);
struct Entry * get_child_entry(struct Node *parent, struct Node *child);
int is_leaf(struct Node *node);
struct Node split_node(struct Node *node, struct Entry entry);

#endif