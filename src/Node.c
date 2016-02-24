#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "Rectangle.h"
#include "Node.h"

int node_add_entry(struct Node *node, void *entry)
{
	if (node->count < node->context->M)
	{
		if ((node->count + 1) > node->capacity)
		{
			node->capacity = NALLOC(node->capacity, node->context->M, node->context->alloc_factor);
			node->entries = realloc(node->entries, node->capacity * sizeof(void *));
		}
		node->entries[node->count++] = entry;
		node_adjust_MBR(node, entry);
		return 1;
	}
	return 0;
}

void node_adjust_MBR(struct Node *node, void *entry)
{
	node_is_leaf(node) ?
		rectangle_combine(node->MBR, ((struct Entry *)entry)->MBR) :
		rectangle_combine(node->MBR, ((struct Node *)entry)->MBR);
}

struct Node * node_choose_optimal_entry(struct Node *node, struct Entry *entry)
{
	return NULL;
}

void node_calculate_MBR(struct Rectangle *MBR, struct Node *node)
{
	node_is_leaf(node) ? _node_calculate_leaf_MBR(MBR, node) : _node_calculate_node_MBR(MBR, node);
}

void _node_calculate_node_MBR(struct Rectangle *MBR, struct Node *node)
{
	int i;
	for (i = 0; i < node->count; i++)
	{
		rectangle_combine(MBR, ((struct Node *)(node->entries[i]))->MBR);
	}
}

void _node_calculate_leaf_MBR(struct Rectangle *MBR, struct Node *leaf)
{
	int i;
	for (i = 0; i < leaf->count; i++)
	{
		rectangle_combine(MBR, ((struct Entry *)(leaf->entries[i]))->MBR);
	}
}

void node_create(struct Node *dest, struct Context *context, struct Node *parent, void **entries, struct Rectangle *MBR, int level)
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
	node_calculate_MBR(dest->MBR, dest);
}

void node_delete_entry(struct Node *node, void *entry)
{
	int i;
	for (i = 0; i < node->count; i++)
	{
		if (node->entries[i] == entry)
		{
			void **temp = malloc((node->capacity - 1) * sizeof(void *));

			if (i == 0)
			{
				memmove(temp, node->entries + 1, (node->count - 1) * sizeof(void *));
			}
			else if (i > 0 && i < (node->count - 1))
			{
				memmove(temp, node->entries, (i - 1) * sizeof(void *));
				memmove(temp, node->entries + i, (node->count - i) * sizeof(void *));
			}
			else if (i == (node->count - 1))
			{
				memmove(temp, node->entries, (node->count - 1) * sizeof(void *));
			}

			free(node->entries);
			node->entries = temp;
			node->count--;
			node->capacity--;
			return;
		}
	}
}

void node_destroy(struct Node *node)
{
	free(node->entries);
	free(node->MBR);
	free(node);
}

int node_is_leaf(struct Node *node)
{
	return (node->level == 0 ? 1 : 0);
}

int node_is_root(struct Node *node)
{
	return (node->parent == NULL ? 1 : 0);
}

struct Node * node_split_node(struct Node *node, void *entry)
{
	return NULL;
}