#include <assert.h>
#include <stddef.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "hashset.h"
#include "hashset_itr.h"
#include "Rectangle.h"
#include "Node.h"
#include "RTree.h"

void rtree_insert(rt_rtree_t *rtree, rt_entry_t *entry) 
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

void rtree_create(rt_rtree_t *dest, rt_ctx_t *context)
{
	rt_node_t *root = malloc(sizeof(rt_node_t));
	node_create(root, context, NULL, NULL, 0, context->space, 0);
	dest->context = context;
	dest->root = root;
}

int rtree_delete(rt_rtree_t *rtree, rt_entry_t *entry)
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

void rtree_destroy(rt_rtree_t *rtree)
{
	node_destroy(rtree->root);
}

void _rtree_adjust_tree(rt_rtree_t *rtree, rt_node_t *node, rt_node_t *nnode)
{
	_rtree_adjust_tree_recursive(rtree, node, nnode, 0);
}

void _rtree_adjust_tree_recursive(rt_rtree_t *rtree, rt_node_t *node, rt_node_t *nnode, int level)
{
	if (rtree->root == node)
	{
		if (nnode != NULL)
		{
			rt_node_t *root = malloc(sizeof(*(rtree->root)));
			void **entries = malloc(2 * sizeof(rt_node_t *));
			entries[0] = node;
			entries[1] = nnode;

			node_copy(root, rtree->root);
			node_create(root, rtree->context, NULL, entries, 2, node->MBR, level);
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

rt_node_t * _rtree_choose_leaf(rt_node_t *node, rt_entry_t *entry)
{
	return _rtree_choose_leaf_recursive(node, entry);
}

rt_node_t * _rtree_choose_leaf_recursive(rt_node_t *node, rt_entry_t *entry)
{
	if (node_is_leaf(node))
	{
		return node;
	}

	return _rtree_choose_leaf_recursive(node_choose_optimal_entry(node, entry), entry);
}

void _rtree_condense_tree(rt_rtree_t *rtree, rt_node_t *node)
{
	hashset_t condensed_nodes = hashset_create();
	_rtree_condense_tree_recursive(rtree, node, condensed_nodes);
	hashset_destroy(condensed_nodes);
}

void _rtree_condense_tree_recursive(rt_rtree_t *rtree, rt_node_t *node, struct hashset_st *condensed_nodes)
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

rt_node_t * _rtree_find_leaf(rt_node_t *node, rt_entry_t *entry)
{
	return _rtree_find_leaf_recursive(node, entry);
}

rt_node_t * _rtree_find_leaf_recursive(rt_node_t *node, rt_entry_t *entry)
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

void rtree_search(rt_rtree_t *rtree, rt_rect_t *search_rectangle, struct hashset_st *results)
{
	_rtree_search_recursive(rtree->root, search_rectangle, results);
}

void _rtree_search_recursive(rt_node_t *node, rt_rect_t *search_rectangle, struct hashset_st *results)
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

void rtree_serialize(rt_rtree_t *rtree, unsigned char *buffer)
{
	size_t buffer_index = 0;

	_rtree_serialize_recursive(rtree->root, buffer, &buffer_index);
}

void _rtree_serialize_recursive(rt_node_t *node, unsigned char *buffer, size_t *index)
{
	if (node_is_leaf(node))
	{
		uint8_t i;
		unsigned char * entry_buffer = malloc(node->context->buffer_size);
		
		for (i = 0; i < node->count; i++)
		{
			rt_entry_t *entry = (rt_entry_t *)node->entries[i];
			size_t buffer_size = entry_serialize(entry, entry_buffer);
			assert(node->context->buffer_size == buffer_size);
			memmove(&buffer[*index], entry_buffer, buffer_size);
			*index += buffer_size;
		}
		free(entry_buffer);
	}
	else
	{
		uint8_t i;
		for (i = 0; i < node->count; i++)
		{
			rt_node_t *entry = (rt_node_t *)node->entries[i];
			_rtree_serialize_recursive(entry, buffer, index);
		}
	}
}

#ifdef DEBUG
void rtree_visualize(rt_rtree_t *rtree)
{
	printf("RTREE: CONTEXT(m = %u, M = %u, dim = %u, buffer_size = %zu, alloc_factor = %3.2f, space = ", 
		rtree->context->m, 
		rtree->context->M,
		rtree->context->dim,
		rtree->context->buffer_size,
		rtree->context->alloc_factor);

	rectangle_print(rtree->context->space);
	puts(")\n");

	_rtree_visualize_recursive(rtree->root, rtree->root->level);
}
void _rtree_visualize_recursive(rt_node_t *node, uint16_t max_level) 
{
	uint16_t level_pad = max_level - node->level;
	char *padding = malloc(level_pad * sizeof(char));

	memset(padding, ' ', level_pad);
	padding[level_pad] = '\0';
	puts(padding);
	free(padding);

	if (node_is_leaf(node))
	{
		uint8_t i;
		for (i = 0; i < node->count; i++)
		{
			entry_print((rt_entry_t *)node->entries[i]);
		}
	}
	else
	{
		uint8_t i;

		rectangle_print(node->MBR);
		
		for (i = 0; i < node->count; i++)
		{
			_rtree_visualize_recursive((rt_node_t *)node->entries[i], max_level);
		}
	}
}
#endif