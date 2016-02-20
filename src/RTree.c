#include <stddef.h>
#include <stdlib.h>
#include "Rectangle.h"
#include "Node.h"
#include "RTree.h"

void insert(struct RTree *rtree, struct Entry entry) 
{
	struct Node *leaf = _choose_leaf(rtree->root, entry);

	if (add_entry(leaf, entry))
	{
		_adjust_tree(rtree, leaf, NULL);
	}
	else
	{
		struct Node lleaf = split_node(leaf, entry);
		_adjust_tree(rtree, leaf, &lleaf);
	}
}

struct Node * _choose_leaf(struct Node *node, struct Entry entry)
{
	return _choose_leaf_recursive(node, entry);
}

struct Node * _choose_leaf_recursive(struct Node *node, struct Entry entry)
{
	struct Entry *optimal = NULL;

	if (is_leaf(node))
	{
		return node;
	}

	optimal = choose_optimal_entry(node, entry);
	return _choose_leaf_recursive(optimal->child, entry);
}

void _adjust_tree(struct RTree *rtree, struct Node *node, struct Node *nnode)
{
	_adjust_tree_recursive(rtree, node, nnode, 0);
}

void _adjust_tree_recursive(struct RTree *rtree, struct Node *node, struct Node *nnode, int level)
{
	struct Entry *entries, *new_entry;
	struct Node p_parent;

	if (rtree->root == node)
	{
		if (nnode != NULL)
		{
			entries = malloc(2 * sizeof(struct Entry));
			create_entry(&entries[0], node);
			create_entry(&entries[1], nnode);

			create_node(rtree->root, rtree->context, NULL, entries, level);
		}
		return;
	}

	adjust_parent_entry(node);

	if (nnode != NULL)
	{
		new_entry = malloc(sizeof(struct Entry));
		create_entry(new_entry, nnode);

		if (add_entry(node->parent, *new_entry))
		{
			_adjust_tree_recursive(rtree, node->parent, NULL, level + 1);
		}
		else
		{
			p_parent = split_node(node->parent, *new_entry);
			_adjust_tree_recursive(rtree, node->parent, &p_parent, level + 1);
		}
	}
	else
	{
		_adjust_tree_recursive(rtree, node->parent, NULL, level + 1);
	}
}