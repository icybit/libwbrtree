#include <stddef.h>
#include <stdlib.h>
#include "Rectangle.h"
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

void adjust_parent_entry(struct Node *node)
{
	struct Entry *entry = get_child_entry(node->parent, node);
	combine_rectangle(node->parent->MBR, entry->MBR);
}

struct Entry * choose_optimal_entry(struct Node *node, struct Entry entry)
{
	return NULL;
}

void calculate_MBR(struct Rectangle *MBR, struct Node *node)
{
	int i;
	for (i = 0; i < node->count; i++)
	{
		combine_rectangle(MBR, node->entries[i].MBR);
	}
}

void create_entry(struct Entry *dest, struct Node *child)
{
	dest->child = child;
	dest->MBR = child->MBR;
}

void create_node(struct Node *dest, struct Context *context, struct Node *parent, struct Entry *entries, struct Rectangle *MBR, int level)
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
		dest->MBR = MBR;
		calculate_MBR(dest->MBR, dest);
	}
}

struct Entry * get_child_entry(struct Node *parent, struct Node *child)
{
	struct Entry *entry = NULL;
	int i;
	for (i = 0; i < parent->count; i++)
	{
		entry = &parent->entries[i];
		if (entry->child == child)
		{
			return entry;
		}
	}
	return entry;
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