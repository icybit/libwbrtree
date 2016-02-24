#include <stddef.h>
#include <stdlib.h>
#include "hashset.h"
#include "hashset_itr.h"
#include "Rectangle.h"
#include "Node.h"
#include "RTree.h"

void rtree_insert(struct RTree *rtree, struct Entry *entry) 
{
	struct Node *leaf = _rtree_choose_leaf(rtree->root, entry);

	if (node_add_entry(leaf, entry))
	{
		_rtree_adjust_tree(rtree, leaf, NULL);
	}
	else
	{
		struct Node *lleaf = node_split_node(leaf, entry);
		_rtree_adjust_tree(rtree, leaf, lleaf);
	}
}

int rtree_delete(struct RTree *rtree, struct Entry *entry)
{
	struct Node *leaf = _rtree_find_leaf(rtree->root, entry);

	if (leaf != NULL)
	{
		node_delete_entry(leaf, entry);
		_rtree_condense_tree(rtree, leaf);
		if (rtree->root->count == 1)
		{
			struct Node *root = (struct Node *)rtree->root->entries[0];
			node_destroy(rtree->root);
			rtree->root = root;
		}
		return 1;
	}
	return 0;
}

void _rtree_adjust_tree(struct RTree *rtree, struct Node *node, struct Node *nnode)
{
	_rtree_adjust_tree_recursive(rtree, node, nnode, 0);
}

void _rtree_adjust_tree_recursive(struct RTree *rtree, struct Node *node, struct Node *nnode, int level)
{
	if (rtree->root == node)
	{
		if (nnode != NULL)
		{
			struct Rectangle *MBR = malloc(sizeof(struct Rectangle));
			void **entries = malloc(2 * sizeof(struct Node *));
			entries[0] = node;
			entries[1] = nnode;

			node_create(rtree->root, rtree->context, NULL, entries, MBR, level);
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
			struct Node *p_parent = node_split_node(node->parent, nnode);
			_rtree_adjust_tree_recursive(rtree, node->parent, p_parent, level + 1);
		}
	}
	else
	{
		_rtree_adjust_tree_recursive(rtree, node->parent, NULL, level + 1);
	}
}

struct Node * _rtree_choose_leaf(struct Node *node, struct Entry *entry)
{
	return _rtree_choose_leaf_recursive(node, entry);
}

struct Node * _rtree_choose_leaf_recursive(struct Node *node, struct Entry *entry)
{
	if (node_is_leaf(node))
	{
		return node;
	}

	return _rtree_choose_leaf_recursive(node_choose_optimal_entry(node, entry), entry);
}

void _rtree_condense_tree(struct RTree *rtree, struct Node *node)
{
	hashset_t condensed_nodes = hashset_create();
	_rtree_condense_tree_recursive(rtree, node, condensed_nodes);
	hashset_destroy(condensed_nodes);
}

void _rtree_condense_tree_recursive(struct RTree *rtree, struct Node *node, hashset_t condensed_nodes)
{
	if (node_is_root(node))
	{
		hashset_itr_t set_iter = hashset_iterator(condensed_nodes);
		while (hashset_iterator_has_next(set_iter))
		{
			struct Node * condensed_node = (struct Node *)hashset_iterator_value(set_iter);
			if (node_is_leaf(condensed_node))
			{
				int i;
				for (i = 0; i < condensed_node->count; i++)
				{
					rtree_insert(rtree, (struct Entry *)condensed_node->entries[i]);
				}
				node_destroy(condensed_node);
			}
			else
			{
				int i;
				for (i = 0; i < condensed_node->count; i++)
				{
					node_add_entry(condensed_node->parent, (struct Node *)condensed_node->entries[i]);
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

struct Node * _rtree_find_leaf(struct Node *node, struct Entry *entry)
{
	return _rtree_find_leaf_recursive(node, entry);
}

struct Node * _rtree_find_leaf_recursive(struct Node *node, struct Entry *entry)
{
	int i;

	if (node_is_leaf(node))
	{
		for (i = 0; i < node->count; i++)
		{
			if ((struct Entry *)node->entries[i] == entry)
			{
				return node;
			}
		}
	}
	else
	{
		struct Node *result = NULL;

		for (i = 0; i < node->count; i++)
		{
			struct Node *node_entry = (struct Node *)node->entries[i];
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