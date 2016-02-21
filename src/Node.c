#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include "Rectangle.h"
#include "Node.h"

int add_entry(struct Node *node, void *entry)
{
	if (node->count < node->context->M)
	{
		if ((node->count + 1) > node->capacity)
		{
			node->capacity = NALLOC(node->capacity, node->context->M, node->context->alloc_factor);
			node->entries = realloc(node->entries, node->capacity * sizeof(void *));
		}
		node->entries[node->count++] = entry;
		adjust_MBR(node, entry);
		return 1;
	}
	return 0;
}

void adjust_MBR(struct Node *node, void *entry)
{
	is_leaf(node) ?
		combine_rectangle(node->MBR, ((struct Entry *)entry)->MBR) :
		combine_rectangle(node->MBR, ((struct Node *)entry)->MBR);
}

struct Node * choose_optimal_entry(struct Node *node, struct Entry *entry)
{
	return NULL;
}

void calculate_MBR(struct Rectangle *MBR, struct Node *node)
{
	is_leaf(node) ? _calculate_leaf_MBR(MBR, node) : _calculate_node_MBR(MBR, node);
}

void _calculate_node_MBR(struct Rectangle *MBR, struct Node *node)
{
	int i;
	for (i = 0; i < node->count; i++)
	{
		combine_rectangle(MBR, ((struct Node *)(node->entries[i]))->MBR);
	}
}

void _calculate_leaf_MBR(struct Rectangle *MBR, struct Node *leaf)
{
	int i;
	for (i = 0; i < leaf->count; i++)
	{
		combine_rectangle(MBR, ((struct Entry *)(leaf->entries[i]))->MBR);
	}
}

void create_node(struct Node *dest, struct Context *context, struct Node *parent, void **entries, struct Rectangle *MBR, int level)
{
	int count = NELEMS(entries);

	if ((count < context->m && parent != NULL) || count > context->M)
	{
		return;
	}

	dest->count = count;
	dest->capacity = MAX(count, NALLOC(context->m, context->M, context->alloc_factor));
	dest->context = context;
	dest->parent = parent;
	dest->entries = entries;
	dest->level = level;

	if (dest->capacity > dest->count)
	{
		dest->entries = realloc(dest->entries, dest->capacity * sizeof(void *));
	}
	
	dest->MBR = MBR;
	calculate_MBR(dest->MBR, dest);
}

int is_leaf(struct Node *node)
{
	return (node->level == 0 ? 1 : 0);
}

struct Node * split_node(struct Node *node, void *entry)
{
	return NULL;
}