#ifndef _RTREE_NODE_H_
#define _RTREE_NODE_H_

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define NELEMS(x) (sizeof(x)/sizeof((x)[0]))
#define NALLOC(m, M, f) MIN((int)ceil(m * f), M)

#include "Rectangle.h"

struct Context {
	int m;
	int M;
	float alloc_factor;
};

struct Entry {
	struct Node *child;
	struct Rectangle rectangle;
};

struct Node {
	struct Context *context;
	struct Node *parent;
	struct Entry *entries;
	int capacity;
	int count;
	int level;
};

int add_entry(struct Node *node, struct Entry entry);
struct Entry * choose_optimal_entry(struct Node *node, struct Entry entry);
void create_entry(struct Entry *dest, struct Node *child);
void create_node(struct Node *dest, struct Context *context, struct Node *parent, struct Entry *entries, int level);
struct Rectangle get_bounding_rectangle(struct Node *node);
struct Entry * get_child_entry(struct Node *parent, struct Node *child);
int is_leaf(struct Node *node);
struct Node split_node(struct Node *node, struct Entry entry);

#endif