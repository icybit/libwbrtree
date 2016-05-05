#include <assert.h>
#include <stddef.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "wbrtree/wbrtreeapi.h"
#include "hashset.h"
#include "hashset_itr.h"
#include "Common.h"
#include "Context.h"
#include "Entry.h"
#include "Node.h"
#include "Rectangle.h"
#include "RTree.h"

static void _rtree_destroy_recursive(rt_node_t *node);
static void _rtree_adjust_tree(rt_rtree_t *rtree, rt_node_t *node, rt_node_t *nnode);
static void _rtree_adjust_tree_recursive(rt_rtree_t *rtree, rt_node_t *node, rt_node_t *nnode, int level);
static rt_node_t * _rtree_choose_leaf(rt_node_t *node, rt_entry_t *entry);
static rt_node_t * _rtree_choose_leaf_recursive(rt_node_t *node, rt_entry_t *entry);
static void _rtree_condense_tree(rt_rtree_t *rtree, rt_node_t *node);
static void _rtree_condense_tree_recursive(rt_rtree_t *rtree, rt_node_t *node, struct hashset_st *condensed_nodes);
static rt_node_t * _rtree_find_leaf(rt_node_t *node, rt_entry_t *entry);
static rt_node_t * _rtree_find_leaf_recursive(rt_node_t *node, rt_entry_t *entry);
static void _rtree_search_recursive(rt_node_t *node, rt_rect_t *search_rectangle, struct hashset_st *results);
static void _rtree_serialize_recursive(rt_node_t *node, uint8_t *buffer, size_t *buffer_size, size_t *index);
#ifdef DEBUG
static void _rtree_visualize_recursive(rt_node_t *node, uint16_t max_level);
#endif

RTREE_PUBLIC void rtree_insert(rt_rtree_t *rtree, rt_entry_t *entry) 
{
	rt_node_t *leaf = _rtree_choose_leaf(rtree->root, entry);

	if (node_add_entry(leaf, entry))
	{
		_rtree_adjust_tree(rtree, leaf, NULL);
	}
	else
	{
		rt_node_t *lleaf = node_split(leaf, entry);
		_rtree_adjust_tree(rtree, leaf, lleaf);
	}
}

RTREE_PUBLIC rt_entry_t * rtree_try_insert(rt_rtree_t *rtree, rt_entry_t *entry) {
	rt_node_t *leaf = _rtree_choose_leaf(rtree->root, entry);
	rt_entry_t *optimal_entry = (rt_entry_t *) node_choose_optimal_entry(leaf, entry);

	return optimal_entry;
}

RTREE_PUBLIC rt_rtree_t * rtree_create(rt_ctx_t *context)
{
	rt_rtree_t *rtree;
	rt_node_t *root;
	rt_rect_t *MBR;
	float *low, *high;

	assert(context);

	low = malloc(context->dim * sizeof(float));
	high = malloc(context->dim * sizeof(float));
	MBR = rtree_rectangle_create(low, high, context->dim);
	rectangle_extend_infinitely(MBR);

	root = node_create(context, NULL, NULL, 0, MBR, 0);

	rtree = malloc(sizeof(rt_rtree_t));
	rtree->context = context;
	rtree->root = root;

	return rtree;
}

RTREE_PUBLIC int rtree_delete(rt_rtree_t *rtree, rt_entry_t *entry)
{
	rt_node_t *leaf = _rtree_find_leaf(rtree->root, entry);

	if (leaf != NULL)
	{
		node_delete_entry(leaf, entry);
		_rtree_condense_tree(rtree, leaf);
		if (rtree->root->count == 1)
		{
			rt_node_t *root = (rt_node_t *)rtree->root->entries[0];
			node_destroy(rtree->root);
			rtree->root = root;
		}
		return 1;
	}
	return 0;
}

RTREE_PUBLIC void rtree_destroy(rt_rtree_t *rtree)
{
	assert(rtree);

	_rtree_destroy_recursive(rtree->root);
	rtree_context_destroy(rtree->context);
	free(rtree);
	rtree = NULL;
}

static void _rtree_destroy_recursive(rt_node_t *node)
{
	if (!node_is_leaf(node))
	{
		uint8_t i;
		for (i = 0; i < node->count; i++)
		{
			_rtree_destroy_recursive((rt_node_t *)node->entries[i]);
		}
	}
	node_destroy(node);
}

static void _rtree_adjust_tree(rt_rtree_t *rtree, rt_node_t *node, rt_node_t *nnode)
{
	_rtree_adjust_tree_recursive(rtree, node, nnode, 0);
}

static void _rtree_adjust_tree_recursive(rt_rtree_t *rtree, rt_node_t *node, rt_node_t *nnode, int level)
{
	if (rtree->root == node)
	{
		if (nnode != NULL)
		{
			rt_node_t *root;
			rt_rect_t *MBR;
			void **entries = malloc(2 * sizeof(rt_node_t *));
			entries[0] = node;
			entries[1] = nnode;

			MBR = rectangle_duplicate(node->MBR);

			root = node_create(rtree->context, NULL, entries, 2, MBR, level + 1);
			rtree->root = node->parent = nnode->parent = root;
		}
		return;
	}

	rectangle_combine(node->parent->MBR, node->MBR);

	if (nnode != NULL)
	{
		if (node_add_entry(node->parent, nnode))
		{
			_rtree_adjust_tree_recursive(rtree, node->parent, NULL, level + 1);
		}
		else
		{
			rt_node_t *p_parent = node_split(node->parent, nnode);
			_rtree_adjust_tree_recursive(rtree, node->parent, p_parent, level + 1);
		}
	}
	else
	{
		_rtree_adjust_tree_recursive(rtree, node->parent, NULL, level + 1);
	}
}

static rt_node_t * _rtree_choose_leaf(rt_node_t *node, rt_entry_t *entry)
{
	return _rtree_choose_leaf_recursive(node, entry);
}

static rt_node_t * _rtree_choose_leaf_recursive(rt_node_t *node, rt_entry_t *entry)
{
	if (node_is_leaf(node))
	{
		return node;
	}

	return _rtree_choose_leaf_recursive((rt_node_t *) node_choose_optimal_entry(node, entry), entry);
}

static void _rtree_condense_tree(rt_rtree_t *rtree, rt_node_t *node)
{
	hashset_t condensed_nodes = hashset_create();
	_rtree_condense_tree_recursive(rtree, node, condensed_nodes);
	hashset_destroy(condensed_nodes);
}

static void _rtree_condense_tree_recursive(rt_rtree_t *rtree, rt_node_t *node, struct hashset_st *condensed_nodes)
{
	if (node_is_root(node))
	{
		hashset_itr_t set_iter = hashset_iterator(condensed_nodes);
		while (hashset_iterator_has_next(set_iter))
		{
			rt_node_t * condensed_node = (rt_node_t *)hashset_iterator_value(set_iter);
			if (node_is_leaf(condensed_node))
			{
				uint8_t i;
				for (i = 0; i < condensed_node->count; i++)
				{
					rtree_insert(rtree, (rt_entry_t *)condensed_node->entries[i]);
				}
				node_destroy(condensed_node);
			}
			else
			{
				uint8_t i;
				for (i = 0; i < condensed_node->count; i++)
				{
					node_add_entry(condensed_node->parent, (rt_node_t *)condensed_node->entries[i]);
				}
			}
			hashset_iterator_next(set_iter);
		}
		free(set_iter);
	}
	else
	{
		if (node->count < node->context->m)
		{
			node_delete_entry(node->parent, node);
			hashset_add(condensed_nodes, node);
		}
		else
		{
			node_calculate_MBR(node->MBR, node);
		}
		_rtree_condense_tree_recursive(rtree, node->parent, condensed_nodes);
	}
}

static rt_node_t * _rtree_find_leaf(rt_node_t *node, rt_entry_t *entry)
{
	return _rtree_find_leaf_recursive(node, entry);
}

static rt_node_t * _rtree_find_leaf_recursive(rt_node_t *node, rt_entry_t *entry)
{
	uint8_t i;

	if (node_is_leaf(node))
	{
		for (i = 0; i < node->count; i++)
		{
			if ((rt_entry_t *)node->entries[i] == entry)
			{
				return node;
			}
		}
	}
	else
	{
		rt_node_t *result = NULL;

		for (i = 0; i < node->count; i++)
		{
			rt_node_t *node_entry = (rt_node_t *)node->entries[i];
			if (rectangle_overlaps(node_entry->MBR, entry->MBR))
			{
				result = _rtree_find_leaf_recursive(node_entry, entry);
				if (result)
				{
					return result;
				}
			}
		}
	}

	return NULL;
}

RTREE_PUBLIC void rtree_search(rt_rtree_t *rtree, rt_rect_t *search_rectangle, rt_hset_t *results)
{
	_rtree_search_recursive(rtree->root, search_rectangle, results);
}

static void _rtree_search_recursive(rt_node_t *node, rt_rect_t *search_rectangle, struct hashset_st *results)
{
	if (node_is_leaf(node))
	{
		uint8_t i;
		for (i = 0; i < node->count; i++)
		{
			rt_entry_t *entry = (rt_entry_t *)node->entries[i];
			if (rectangle_overlaps(search_rectangle, entry->MBR))
			{
				hashset_add(results, entry);
			}
		}
	}
	else
	{
		uint8_t i;
		for (i = 0; i < node->count; i++)
		{
			rt_node_t *entry = (rt_node_t *)node->entries[i];
			if (rectangle_overlaps(search_rectangle, entry->MBR))
			{
				_rtree_search_recursive(entry, search_rectangle, results);
			}
		}
	}
}

RTREE_PUBLIC size_t rtree_serialize(rt_rtree_t *rtree, uint8_t **buffer)
{
	size_t bytes = 0, size = 0;

	_rtree_serialize_recursive(rtree->root, *buffer, &size, &bytes);

	/* Shrink buffer to wrap contents */
	*buffer = realloc(*buffer, bytes);

	return bytes;
}

static void _rtree_serialize_recursive(rt_node_t *node, uint8_t *buffer, size_t *buffer_size, size_t *index)
{
	uint8_t i;

	if (node_is_leaf(node))
	{
		for (i = 0; i < node->count; i++)
		{
			rt_entry_t *entry = (rt_entry_t *)node->entries[i];
			uint8_t *entry_buffer = NULL;
			size_t bytes = node->context->serializer(entry, &entry_buffer);
			if ((*index + bytes) > *buffer_size)
			{
				*buffer_size = *index + (bytes * node->count);
				buffer = realloc(buffer, *buffer_size);
			}
			memmove(&buffer[*index], entry_buffer, bytes);
			*index += bytes;
			free(entry_buffer);
		}
	}
	else
	{
		for (i = 0; i < node->count; i++)
		{
			rt_node_t *entry = (rt_node_t *)node->entries[i];
			_rtree_serialize_recursive(entry, buffer, buffer_size, index);
		}
	}
}

RTREE_PUBLIC rt_rtree_t * rtree_split(rt_rtree_t *rtree)
{
    rt_rtree_t *other;
    rt_node_t *root = rtree->root; 

    if (node_is_leaf(rtree->root) || rtree->root->count != 2) {
    	return NULL;
    }

    other = rtree_create(context_duplicate(rtree->context));

    other->root = rtree->root->entries[1];
	rtree->root = rtree->root->entries[0];

    node_destroy(root);

    return other;
}

#ifdef DEBUG
RTREE_PUBLIC void rtree_visualize(rt_rtree_t *rtree)
{
	printf("RTREE: CONTEXT(m = %u, M = %u, dim = %u, alloc_factor = %3.2f, space = ", 
		rtree->context->m, 
		rtree->context->M,
		rtree->context->dim,
		rtree->context->alloc_factor);

	rectangle_print(rtree->context->space);
	puts(")");

	_rtree_visualize_recursive(rtree->root, rtree->root->level);
}
static void _rtree_visualize_recursive(rt_node_t *node, uint16_t max_level) 
{
	uint8_t i;
	uint16_t level_pad = max_level - node->level;
	char *padding = malloc(level_pad * sizeof(char));

	memset(padding, ' ', level_pad);
	padding[level_pad] = '\0';
	
	printf("%s", padding);
	rectangle_print(node->MBR);
	printf("\n");

	if (node_is_leaf(node))
	{
		for (i = 0; i < node->count; i++)
		{
			printf("%s", padding);
			printf("%s", padding);
			entry_print((rt_entry_t *)node->entries[i]);
		}
	}
	else
	{
		for (i = 0; i < node->count; i++)
		{
			_rtree_visualize_recursive((rt_node_t *)node->entries[i], max_level);
		}
	}
	free(padding);
}
#endif