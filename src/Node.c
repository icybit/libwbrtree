#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include "Node.h"

int add_entry(struct Node *node, struct Entry entry)
{
	if (node->count < node->context->M)
	{
		if ((node->count + 1) > node->capacity)
		{
			node->capacity = NALLOC(node->capacity, node->context->M, node->context->alloc_factor);
			node->entries = realloc(node->entries, node->capacity);
		}
		node->entries[node->count++] = entry;
		return 1;
	}
	return 0;
}

struct Entry * choose_optimal_entry(struct Node *node, struct Entry entry)
{
	return NULL;
}

void create_entry(struct Entry *dest, struct Node *child)
{
	dest->child = child;
	dest->rectangle = get_bounding_rectangle(child);
}

void create_node(struct Node *dest, struct Context *context, struct Node *parent, struct Entry *entries, int level)
{
	int count = NELEMS(entries);

	if (count <= context->M)
	{
		dest->capacity = MAX(count, NALLOC(context->m, context->M, context->alloc_factor));
		dest->count = count;
		dest->entries = entries;
		dest->context = context;
		dest->parent = parent;
		dest->level = level;
	}
}

struct Rectangle get_bounding_rectangle(struct Node *node)
{
	struct Rectangle rectangle = { { 0.0, 0.0 } };
	return rectangle;
}
struct Entry * get_child_entry(struct Node *parent, struct Node *child)
{
	return NULL;
}

int is_leaf(struct Node *node)
{
	return (node->level == 0 ? 1 : 0);
}

struct Node split_node(struct Node *node, struct Entry entry)
{
	struct Node nnode = { NULL, NULL, NULL, 0, 0, 0};
	return nnode;
}