#include "Node.h"
#include <assert.h>
#include <float.h>
#include <math.h>
#include <stddef.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "hashset.h"
#include "Context.h"
#include "Entry.h"
#include "Rectangle.h"
#include "stdlib_qsort.h"

typedef struct Comparator_Opts {
	uint8_t dim;
	uint8_t low;
} rt_cmp_opts_t;

static void _node_calculate_node_MBR(rt_rect_t *MBR, rt_node_t *node);
static void _node_calculate_leaf_MBR(rt_rect_t *MBR, rt_node_t *leaf);
static uint8_t _node_choose_split_axis(rt_node_t *node, void ***sorted_entries, rt_rect_t *MBR_one, rt_rect_t *MBR_two);
static uint8_t _node_choose_split_index(uint8_t dimension, rt_node_t *node, void ***sorted_entries, rt_rect_t *MBR_one, rt_rect_t *MBR_two);
static void _node_entry_sort(rt_node_t *node, void *entry, void ***sorted_entries);
static double _node_evaluate_distribution(uint8_t k, void ***sorted_entries, uint8_t dimension, rt_node_t *node, rt_rect_t *MBR_one, rt_rect_t *MBR_two, double(*evaluator)(rt_rect_t *MBR_one, rt_rect_t *MBR_two));
static rt_rect_t * _node_get_entry_MBR(rt_node_t *node, void *entry);

RTREE_LOCAL int node_add_entry(rt_node_t *node, void *entry)
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

RTREE_LOCAL void node_adjust_MBR(rt_node_t *node, void *entry)
{
	node_is_leaf(node) ?
		rectangle_combine(node->MBR, ((rt_entry_t *)entry)->MBR) :
		rectangle_combine(node->MBR, ((rt_node_t *)entry)->MBR);
}

RTREE_LOCAL void node_calculate_MBR(rt_rect_t *MBR, rt_node_t *node)
{
	node_is_leaf(node) ? _node_calculate_leaf_MBR(MBR, node) : _node_calculate_node_MBR(MBR, node);
}

RTREE_LOCAL void _node_calculate_node_MBR(rt_rect_t *MBR, rt_node_t *node)
{
	uint8_t i;

	rectangle_extend_infinitely(MBR);
	for (i = 0; i < node->count; i++)
	{
		rectangle_combine(MBR, ((rt_node_t *)(node->entries[i]))->MBR);
	}
}

RTREE_LOCAL void _node_calculate_leaf_MBR(rt_rect_t *MBR, rt_node_t *leaf)
{
	uint8_t i;

	rectangle_extend_infinitely(MBR);
	for (i = 0; i < leaf->count; i++)
	{
		rectangle_combine(MBR, ((rt_entry_t *)(leaf->entries[i]))->MBR);
	}
}

RTREE_LOCAL void * node_choose_optimal_entry(rt_node_t *node, rt_entry_t *entry)
{
	void *optimal_entry = NULL;
	rt_rect_t *optimal_entry_MBR = NULL;
	double optimal_distance = DBL_MAX;
	uint8_t i;

	for (i = 0; i < node->count; i++)
	{
		void *current_entry = node->entries[i];
		rt_rect_t *current_entry_MBR = _node_get_entry_MBR(node, current_entry);

		double distance = rectangle_min_distance(current_entry_MBR, entry->MBR);
		
		if (distance < optimal_distance)
		{
			optimal_distance = distance;
			optimal_entry = current_entry;
			optimal_entry_MBR = _node_get_entry_MBR(node, optimal_entry);
		}
		else if (fabs(distance - optimal_distance) < DBL_EPSILON)
		{
			optimal_entry = (rectangle_area(current_entry_MBR) < rectangle_area(optimal_entry_MBR) ? 
				current_entry : 
				optimal_entry);
			optimal_entry_MBR = _node_get_entry_MBR(node, optimal_entry);
		}
	}

	return optimal_entry;
}

RTREE_LOCAL uint8_t _node_choose_split_axis(rt_node_t *node, void ***sorted_entries, rt_rect_t *MBR_one, rt_rect_t *MBR_two)
{
	uint8_t dim, optimal_axis = 0;
	double optimal_margin_value = DBL_MAX;
	double *margin_values = malloc(node->context->dim * sizeof(double));

	for (dim = 0; dim < node->context->dim; dim++)
	{
		uint8_t k;
		for (k = 1; k <= (node->context->M - (2 * node->context->m) + 2); k++)
		{
			margin_values[dim] += _node_evaluate_distribution(k, sorted_entries, dim, node, MBR_one, MBR_two, rectangle_margin_value);
		}
	}

	for (dim = 0; dim < node->context->dim; dim++)
	{
		if (margin_values[dim] < optimal_margin_value)
		{
			optimal_axis = dim;
			optimal_margin_value = margin_values[dim];
		}
	}

	free(margin_values);

	return optimal_axis;
}

RTREE_LOCAL uint8_t _node_choose_split_index(uint8_t dimension, rt_node_t *node, void ***sorted_entries, rt_rect_t *MBR_one, rt_rect_t *MBR_two)
{
	uint8_t k, optimal_distribution_index;
	double optimal_overlap_value = DBL_MAX, optimal_area_value = DBL_MAX, overlap_value, area_value;
	rt_rect_t *MBR_group_one, *MBR_group_two;

	MBR_group_one = rectangle_duplicate(MBR_one);
	MBR_group_two = rectangle_duplicate(MBR_two);

	for (k = 1; k <= (node->context->M - (2 * node->context->m) + 2); k++)
	{
		overlap_value = _node_evaluate_distribution(k, sorted_entries, dimension, node, MBR_group_one, MBR_group_two, rectangle_intersection_area);
		area_value = rectangle_area(MBR_group_one) + rectangle_area(MBR_group_two);
		if (overlap_value < optimal_overlap_value)
		{
			optimal_overlap_value = overlap_value;
			optimal_area_value = area_value;
			optimal_distribution_index = node->context->m - 1 + k;
			rectangle_copy(MBR_one, MBR_group_one);
			rectangle_copy(MBR_two, MBR_group_two);
		}
		else if (fabs(overlap_value - optimal_overlap_value) < DBL_EPSILON)
		{
			if (area_value < optimal_area_value)
			{
				optimal_overlap_value = overlap_value;
				optimal_area_value = area_value;
				optimal_distribution_index = node->context->m - 1 + k;
				rectangle_copy(MBR_one, MBR_group_one);
				rectangle_copy(MBR_two, MBR_group_two);
			}
		}
	}

	rectangle_destroy(MBR_group_one);
	rectangle_destroy(MBR_group_two);

	return optimal_distribution_index;
}

RTREE_LOCAL int node_compare(const void *entry, const void *other, void *cmp_opts)
{
	rt_cmp_opts_t *opts = (rt_cmp_opts_t *)cmp_opts;

	if (opts->low)
	{
		return rectangle_compare_low((*((rt_node_t **)entry))->MBR, (*((rt_node_t **)other))->MBR, &opts->dim);
	}
	else
	{
		return rectangle_compare_high((*((rt_node_t **)entry))->MBR, (*((rt_node_t **)other))->MBR, &opts->dim);
	}
}

RTREE_LOCAL void node_copy(rt_node_t *dest, const rt_node_t *source)
{
	assert(dest && source);

	dest->capacity = source->capacity;
	dest->context = source->context;
	dest->count = source->count;
	dest->entries = source->entries;
	dest->level = source->level;
	dest->parent = source->parent;
	rectangle_copy(dest->MBR, source->MBR);
}

RTREE_LOCAL rt_node_t * node_create(rt_ctx_t *context, rt_node_t *parent, void **entries, uint8_t entry_count, rt_rect_t *MBR, uint16_t level)
{
	rt_node_t *node;
	uint8_t count;

	assert(context && MBR);

	node = malloc(sizeof(rt_node_t));

	count = (entries != NULL ? entry_count : 0);

	if ((count < context->m && parent != NULL) || count > context->M)
	{
		return NULL;
	}

	node->count = count;
	node->capacity = MAX(count, NALLOC(context->m, context->M, context->alloc_factor));
	node->context = context;
	node->parent = parent;
	node->entries = entries;
	node->level = level;

	if (node->capacity > node->count)
	{
		node->entries = realloc(node->entries, node->capacity * sizeof(void *));
	}
	
	node->MBR = MBR;
	node_calculate_MBR(node->MBR, node);

	return node;
}

RTREE_LOCAL void node_delete_entry(rt_node_t *node, void *entry)
{
	uint8_t i;
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
				memmove(temp, node->entries, i * sizeof(void *));
				memmove(temp + i, node->entries + (i + 1), (node->count - i - 1) * sizeof(void *));
			}
			else if (i == (node->count - 1))
			{
				memmove(temp, node->entries, (node->count - 1) * sizeof(void *));
			}
				
			entry_destroy(node->entries[i]);

			free(node->entries);
			node->entries = temp;
			node->count--;
			node->capacity--;
			return;
		}
	}
}

RTREE_LOCAL void node_destroy(rt_node_t *node)
{
	uint8_t index;

	assert(node);

	rectangle_destroy(node->MBR);
	
	if (node_is_leaf(node)) 
	{
		for(index = 0; index < node->count; index++) 
		{
			entry_destroy(node->entries[index]);
		}
	}

	free(node->entries);
	free(node);
	node = NULL;
}

RTREE_LOCAL void _node_entry_sort(rt_node_t *node, void *entry, void ***sorted_entries)
{
	uint8_t dim, order_shift = 0;
	rt_cmp_opts_t opts;
	int(*comparator)(const void *, const void *, void *) = (node_is_leaf(node) ? entry_compare : node_compare);

	/* Perform ASC sort of MBR low points */
	opts.low = 1;
	for (dim = 0; dim < node->context->dim; dim++)
	{
		opts.dim = dim;
		sorted_entries[dim] = malloc((node->context->M + 1) * sizeof(void *));
		memcpy(sorted_entries[dim], node->entries, node->count * sizeof(void *));
		sorted_entries[dim][node->context->M] = entry;
		rt_qsort_r_fn(sorted_entries[dim], node->context->M + 1, sizeof(void *), comparator, &opts);
	}

	/* Perform ASC sort of MBR high points */
	opts.low = 0;
	for (dim = 0; dim < node->context->dim; dim++)
	{
		opts.dim = dim;
		order_shift = dim + node->context->dim;
		sorted_entries[order_shift] = malloc((node->context->M + 1) * sizeof(void *));
		memcpy(sorted_entries[order_shift], node->entries, node->count * sizeof(void *));
		sorted_entries[order_shift][node->context->M] = entry;
		rt_qsort_r_fn(sorted_entries[order_shift], node->context->M + 1, sizeof(void *), comparator, &opts);
	}
}

RTREE_LOCAL double _node_evaluate_distribution(uint8_t k, void ***sorted_entries, uint8_t dimension, rt_node_t *node, rt_rect_t *MBR_one, rt_rect_t *MBR_two, double(*evaluator)(rt_rect_t *MBR_one, rt_rect_t *MBR_two))
{
	uint8_t split_index = 0, sub_dim = 0, order_shift = 0;
	hashset_t split_group = hashset_create();

	memset(MBR_one->low, 0, MBR_one->dim * sizeof(float));
	memset(MBR_two->low, 0, MBR_two->dim * sizeof(float));

	while (split_index < (node->context->m - 1 + k))
	{
		hashset_add(split_group, sorted_entries[dimension][split_index]);
		split_index++;
	}

	while (sub_dim < node->context->dim)
	{
		int j = 0;
		int first_served = 0, second_served = 0;

		while ((j < node->context->M + 1) && !(first_served && second_served))
		{
			if (hashset_is_member(split_group, sorted_entries[sub_dim][j]))
			{
				if (!first_served)
				{
					MBR_one->low[sub_dim] = _node_get_entry_MBR(node, sorted_entries[sub_dim][j])->low[sub_dim];
					first_served = 1;
				}
			}
			else
			{
				if (!second_served)
				{
					MBR_two->low[sub_dim] = _node_get_entry_MBR(node, sorted_entries[sub_dim][j])->low[sub_dim];
					second_served = 1;
				}
			}
			j++;
		}

		first_served = 0; 
		second_served = 0;
		order_shift = sub_dim + node->context->dim;
		j = node->context->M;
		while (j >= 0 && !(first_served && second_served))
		{
			if (hashset_is_member(split_group, sorted_entries[order_shift][j]))
			{
				if (!first_served)
				{
					MBR_one->high[sub_dim] = _node_get_entry_MBR(node, sorted_entries[order_shift][j])->high[sub_dim];
					first_served = 1;
				}
			}
			else
			{
				if (!second_served)
				{
					MBR_two->high[sub_dim] = _node_get_entry_MBR(node, sorted_entries[order_shift][j])->high[sub_dim];
					second_served = 1;
				}
			}
			j--;
		}
		sub_dim++;
	}
	hashset_destroy(split_group);

	return (*evaluator)(MBR_one, MBR_two);
}

RTREE_LOCAL rt_rect_t * _node_get_entry_MBR(rt_node_t *node, void *entry)
{
	rt_rect_t *MBR = (node_is_leaf(node) ? ((rt_entry_t *)entry)->MBR : ((rt_node_t *)entry)->MBR);

	return MBR;
}

RTREE_LOCAL int node_is_leaf(rt_node_t *node)
{
	return (node->level == 0 ? 1 : 0);
}

RTREE_LOCAL int node_is_root(rt_node_t *node)
{
	return (node->parent == NULL ? 1 : 0);
}

RTREE_LOCAL rt_node_t * node_split(rt_node_t *node, void *entry)
{
	uint8_t split_axis, split_index, split_size, dim;
	rt_node_t *nnode = malloc(sizeof(rt_node_t));
	void ***sorted_entries = malloc(node->context->dim * 2 * sizeof(void **));
	void **nentries;
	rt_rect_t *MBR_one, *MBR_two;

	MBR_one = rectangle_duplicate(node->MBR);
	MBR_two = rectangle_duplicate(node->MBR);

	_node_entry_sort(node, entry, sorted_entries);

	split_axis = _node_choose_split_axis(node, sorted_entries, MBR_one, MBR_two);
	split_index = _node_choose_split_index(split_axis, node, sorted_entries, MBR_one, MBR_two);

	split_size = split_index;
	node->entries = realloc(node->entries, split_size * sizeof(void *));
	memmove(node->entries, sorted_entries[split_axis], split_size * sizeof(void *));
	node->capacity = node->count = split_size;
	rectangle_copy(node->MBR, MBR_one);

	split_size = node->context->M + 1 - split_index;
	nentries = malloc(split_size * sizeof(void *));
	memmove(nentries, sorted_entries[split_axis] + split_index, split_size * sizeof(void *));
	nnode = node_create(node->context, node->parent, nentries, split_size, MBR_two, node->level);

	rectangle_destroy(MBR_one);

	for (dim = 0; dim < node->context->dim; dim++)
	{
		free(sorted_entries[dim]);
	}
	free(sorted_entries);

	return nnode;
}