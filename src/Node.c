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
#include "Common.h"
#include "Rectangle.h"
#include "Node.h"

void context_create(struct Context *dest, uint8_t m, uint8_t M, uint8_t dim, float alloc_factor, struct Rectangle *space_MBR)
{
	assert(space_MBR != NULL);

	dest->m = (m <= M/2 ? m : 2);
	dest->M = (M > 2 ? M : 3);
	dest->dim = dim;
	dest->alloc_factor = (alloc_factor > 1.0f ? alloc_factor : 2.0f);
	dest->space = space_MBR;
}

#ifdef _QSORT_LINUX
int entry_compare(const void *entry, const void *other, void *dimension)
{
	return rectangle_compare(((struct Entry *)entry)->MBR, ((struct Entry *)other)->MBR, dimension);
}
#endif

#ifdef _QSORT_WINDOWS
int entry_compare(void *dimension, const void *entry, const void *other)
{
	return rectangle_compare(((struct Entry *)entry)->MBR, ((struct Entry *)other)->MBR, dimension);
}
#endif

void entry_create(struct Entry *dest, void *tuple, struct Rectangle *MBR)
{
	assert(MBR != NULL);

	dest->tuple = tuple;
	dest->MBR = MBR;
}

#ifdef DEBUG
void entry_print(struct Entry *entry)
{
	printf("ENTRY: [VALUE: %p,", entry->tuple);
	rectangle_print(entry->MBR);
	puts("]\n");
}
#endif

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

void node_calculate_MBR(struct Rectangle *MBR, struct Node *node)
{
	node_is_leaf(node) ? _node_calculate_leaf_MBR(MBR, node) : _node_calculate_node_MBR(MBR, node);
}

void _node_calculate_node_MBR(struct Rectangle *MBR, struct Node *node)
{
	uint8_t i;
	for (i = 0; i < node->count; i++)
	{
		rectangle_combine(MBR, ((struct Node *)(node->entries[i]))->MBR);
	}
}

void _node_calculate_leaf_MBR(struct Rectangle *MBR, struct Node *leaf)
{
	uint8_t i;
	for (i = 0; i < leaf->count; i++)
	{
		rectangle_combine(MBR, ((struct Entry *)(leaf->entries[i]))->MBR);
	}
}

struct Node * node_choose_optimal_entry(struct Node *node, struct Entry *entry)
{
	struct Node *optimal_entry = NULL;
	double optimal_distance = DBL_MAX;
	uint8_t i;

	for (i = 0; i < node->count; i++)
	{
		struct Node *current_entry = (struct Node *)node->entries[i];
		double distance = rectangle_min_distance(current_entry->MBR, entry->MBR);
		if (distance < optimal_distance)
		{
			optimal_distance = distance;
			optimal_entry = current_entry;
		}
		else if (fabs(distance - optimal_distance) < DBL_EPSILON)
		{
			optimal_entry = (rectangle_area(current_entry->MBR) < rectangle_area(optimal_entry->MBR) ? 
				current_entry : 
				optimal_entry);
		}
	}

	return optimal_entry;
}

uint8_t _node_choose_split_axis(struct Node *node, void ***sorted_entries, struct Rectangle *MBR_one, struct Rectangle *MBR_two)
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

uint8_t _node_choose_split_index(uint8_t dimension, struct Node *node, void ***sorted_entries, struct Rectangle *MBR_one, struct Rectangle *MBR_two)
{
	uint8_t k, optimal_distribution_index;
	double optimal_overlap_value = DBL_MAX, optimal_area_value = DBL_MAX, overlap_value, area_value;
	struct Rectangle *MBR_group_one = malloc(sizeof(*MBR_one));
	struct Rectangle *MBR_group_two = malloc(sizeof(*MBR_two));

	for (k = 1; k <= (node->context->M - (2 * node->context->m) + 2); k++)
	{
		overlap_value = _node_evaluate_distribution(k, sorted_entries, dimension, node, MBR_group_one, MBR_group_two, rectangle_intersection_area);
		area_value = rectangle_area(MBR_group_one) + rectangle_area(MBR_group_two);
		if (overlap_value < optimal_overlap_value)
		{
			optimal_overlap_value = overlap_value;
			optimal_area_value = area_value;
			optimal_distribution_index = node->context->m - 1 + k;
			memmove(MBR_one, MBR_group_one, sizeof(*MBR_group_one));
			memmove(MBR_two, MBR_group_two, sizeof(*MBR_group_two));
		}
		else if (fabs(overlap_value - optimal_overlap_value) < DBL_EPSILON)
		{
			if (area_value < optimal_area_value)
			{
				optimal_overlap_value = overlap_value;
				optimal_area_value = area_value;
				optimal_distribution_index = node->context->m - 1 + k;
				memmove(MBR_one, MBR_group_one, sizeof(*MBR_group_one));
				memmove(MBR_two, MBR_group_two, sizeof(*MBR_group_two));
			}
		}
	}

	free(MBR_group_one);
	free(MBR_group_two);

	return optimal_distribution_index;
}

#ifdef _QSORT_LINUX
int node_compare(const void *entry, const void *other, void *dimension)
{
	return rectangle_compare(((struct Node *)entry)->MBR, ((struct Node *)other)->MBR, dimension);
}
#endif

#ifdef _QSORT_WINDOWS
int node_compare(void *dimension, const void *entry, const void *other)
{
	return rectangle_compare(((struct Node *)entry)->MBR, ((struct Node *)other)->MBR, dimension);
}
#endif

void node_create(struct Node *dest, struct Context *context, struct Node *parent, void **entries, uint8_t entry_count, struct Rectangle *MBR, uint16_t level)
{
	uint8_t count = (entries != NULL ? entry_count : 0);

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

double _node_evaluate_distribution(uint8_t k, void ***sorted_entries, uint8_t dimension, struct Node *node, struct Rectangle *MBR_one, struct Rectangle *MBR_two, double (*evaluator)(struct Rectangle *MBR_one, struct Rectangle *MBR_two))
{
	uint8_t split_index = 0, sub_dim = 0;
	hashset_t split_group = hashset_create();

	MBR_one->low->coords[dimension] = _node_get_entry_MBR(node, sorted_entries[dimension][split_index])->low->coords[dimension];
	while (split_index < (node->context->m - 1 + k))
	{
		hashset_add(split_group, sorted_entries[dimension][split_index]);
		/* TODO: Solve the case when sorted entries have the same lower values */
		if ((split_index + 1) == (node->context->m - 1 + k))
		{
			MBR_one->high->coords[dimension] = _node_get_entry_MBR(node, sorted_entries[dimension][split_index])->high->coords[dimension];
		}
		split_index++;
	}
	MBR_two->low->coords[dimension] = _node_get_entry_MBR(node, sorted_entries[dimension][split_index])->low->coords[dimension];
	MBR_two->high->coords[dimension] = _node_get_entry_MBR(node, sorted_entries[dimension][node->context->M])->high->coords[dimension];

	while (sub_dim < node->context->dim)
	{
		uint8_t j = 0, match = 0;

		if (sub_dim == dimension)
		{
			sub_dim++;
			continue;
		}

		while ((j < node->context->M + 1) && (match < 2))
		{
			if (hashset_is_member(split_group, sorted_entries[sub_dim][j]))
			{
				if (MBR_one->low->coords[sub_dim] == 0.0f)
				{
					MBR_one->low->coords[sub_dim] = _node_get_entry_MBR(node, sorted_entries[sub_dim][j])->low->coords[sub_dim];
					match++;
				}
			}
			else
			{
				if (MBR_two->low->coords[sub_dim] == 0.0f)
				{
					MBR_two->low->coords[sub_dim] = _node_get_entry_MBR(node, sorted_entries[sub_dim][j])->low->coords[sub_dim];
					match++;
				}
			}
			j++;
		}

		match = 0;
		j = node->context->M;
		while (j >= 0 && match < 2)
		{
			if (hashset_is_member(split_group, sorted_entries[sub_dim][j]))
			{
				if (MBR_one->high->coords[sub_dim] == 0.0f)
				{
					MBR_one->high->coords[sub_dim] = _node_get_entry_MBR(node, sorted_entries[sub_dim][j])->high->coords[sub_dim];
					match++;
				}
			}
			else
			{
				if (MBR_two->high->coords[sub_dim] == 0.0f)
				{
					MBR_two->high->coords[sub_dim] = _node_get_entry_MBR(node, sorted_entries[sub_dim][j])->high->coords[sub_dim];
					match++;
				}
			}
			j--;
		}
		sub_dim++;
	}
	hashset_destroy(split_group);

	return (*evaluator)(MBR_one, MBR_two);
}

struct Rectangle * _node_get_entry_MBR(struct Node *node, void *entry)
{
	struct Rectangle *MBR;
	MBR = (node_is_leaf(node) ? ((struct Entry *)entry)->MBR : ((struct Node *)entry)->MBR);

	return MBR;
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
	uint8_t split_axis, split_index, split_size, dim;
	struct Node *nnode = malloc(sizeof(struct Node));
	struct Rectangle *MBR_one = malloc(sizeof(node->MBR));
	struct Rectangle *MBR_two = malloc(sizeof(node->MBR));
	void ***sorted_entries = malloc(node->context->dim * sizeof(void **));

#ifdef _QSORT_LINUX
	int(*comparator)(const void *, const void *, void *) = (node_is_leaf(node) ? entry_compare : node_compare);
#endif
#ifdef _QSORT_WINDOWS
	int(*comparator)(void *, const void *, const void *) = (node_is_leaf(node) ? entry_compare : node_compare);
#endif

	for (dim = 0; dim < node->context->dim; dim++)
	{
		sorted_entries[dim] = malloc((node->context->M + 1) * sizeof(void *));
		memcpy(sorted_entries[dim], node->entries, node->count);
		memcpy(sorted_entries[dim] + node->count, entry, 1);
#ifdef _QSORT_LINUX
		qsort_r(sorted_entries[dim], node->context->M + 1, sizeof(void *), comparator, dim);
#endif
#ifdef _QSORT_WINDOWS
		qsort_s(sorted_entries[dim], node->context->M + 1, sizeof(void *), comparator, &dim);
#endif
	}

	split_axis = _node_choose_split_axis(node, sorted_entries, MBR_one, MBR_two);
	split_index = _node_choose_split_index(split_axis, node, sorted_entries, MBR_one, MBR_two);

	free(node->entries);
	split_size = split_index;
	node->entries = malloc(split_size * sizeof(void *));
	memmove(node->entries, sorted_entries[split_axis], split_size);
	node->capacity = node->count = split_size;
	memmove(node->MBR, MBR_one, sizeof(*MBR_one));
	free(MBR_one);

	node_create(nnode, node->context, node->parent, NULL, 0, MBR_two, node->level);
	split_size = node->context->M + 1 - split_index;
	nnode->entries = malloc(split_size * sizeof(void *));
	memmove(nnode->entries, sorted_entries[split_axis] + split_index, split_size);
	nnode->capacity = nnode->count = split_size;

	for (dim = 0; dim < node->context->dim; dim++)
	{
		free(sorted_entries[dim]);
	}
	free(sorted_entries);

	return nnode;
}