#include <assert.h>
#include <float.h>
#include <math.h>
#include <stddef.h>
#include <stdlib.h>
#include <string.h>
#include "hashset.h"
#include "Common.h"
#include "Rectangle.h"
#include "Node.h"

void context_create(struct Context *dest, uint8_t m, uint8_t M, float alloc_factor, struct Rectangle *space_MBR)
{
	assert(space_MBR != NULL);

	dest->m = (m <= M/2 ? m : 2);
	dest->M = (M > 2 ? M : 3);
	dest->alloc_factor = (alloc_factor > 1.0f ? alloc_factor : 2.0f);
	dest->space = space_MBR;
}

int entry_compare(const void *entry, const void *other, void *dimension)
{
	return rectangle_compare(((struct Entry *)entry)->MBR, ((struct Entry *)other)->MBR, dimension);
}

void entry_create(struct Entry *dest, void *tuple, struct Rectangle *MBR)
{
	assert(MBR != NULL);

	dest->tuple = tuple;
	dest->MBR = MBR;
}

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

uint8_t _node_choose_split_axis(struct Node *node, void *entry, void ***sorted_entries)
{
	uint8_t dim, optimal_axis;
	double optimal_margin_value;
	double *margin_values = malloc(node->context->dim * sizeof(double));

	for (dim = 0; dim < node->context->dim; dim++)
	{
		sorted_entries[dim] = malloc((node->context->M + 1) * sizeof(void *));
		memcpy(sorted_entries[dim], node->entries, node->count);
		memcpy(sorted_entries[dim] + node->count, entry, 1);
		qsort_r(sorted_entries[dim], node->context->M + 1, sizeof(void *), entry_compare, dim);
	}

	for (dim = 0; dim < node->context->dim; dim++)
	{
		uint8_t k;
		float *low_group_one = calloc(node->context->dim, sizeof(float));
		float *low_group_two = calloc(node->context->dim, sizeof(float));
		float *high_group_one = calloc(node->context->dim, sizeof(float));
		float *high_group_two = calloc(node->context->dim, sizeof(float));

		for (k = 1; k < (node->context->M - (2 * node->context->m) + 2); k++)
		{
			uint8_t i, sub_dim;	
			hashset_t split_group = hashset_create();

			i = 0;
			low_group_one[dim] = ((struct Entry *)sorted_entries[dim][i])->MBR->low->coords[dim];
			while (i < (node->context->m - 1 + k))
			{
				hashset_add(split_group, sorted_entries[dim][i]);
				/* TODO: Solve the case when sorted entries have the same lower values */
				if (i == (node->context->m + k))
				{
					high_group_one[dim] = ((struct Entry *)sorted_entries[dim][i])->MBR->high->coords[dim];
				}
				i++;
			}
			low_group_two[dim] = ((struct Entry *)sorted_entries[dim][i])->MBR->low->coords[dim];
			high_group_two[dim] = ((struct Entry *)sorted_entries[dim][node->context->M])->MBR->high->coords[dim];

			sub_dim = 0;
			while (sub_dim < node->context->dim)
			{
				uint8_t j = 0, match = 0;

				if (sub_dim == dim)
				{
					sub_dim++;
					continue;
				}

				while ((j < node->context->M + 1) && (match < 2))
				{
					if (hashset_is_member(split_group, sorted_entries[sub_dim][j]))
					{
						if (low_group_one[sub_dim] == 0.0f)
						{
							low_group_one[sub_dim] = ((struct Entry *)sorted_entries[sub_dim][j])->MBR->low->coords[sub_dim];
							match++;
						}
						
					}
					else
					{
						if (low_group_two[sub_dim] == 0.0f)
						{
							low_group_two[sub_dim] = ((struct Entry *)sorted_entries[sub_dim][j])->MBR->low->coords[sub_dim];
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
						if (high_group_one[sub_dim] == 0.0f)
						{
							high_group_one[sub_dim] = ((struct Entry *)sorted_entries[sub_dim][j])->MBR->high->coords[sub_dim];
							match++;
						}

					}
					else
					{
						if (high_group_two[sub_dim] == 0.0f)
						{
							high_group_two[sub_dim] = ((struct Entry *)sorted_entries[sub_dim][j])->MBR->high->coords[sub_dim];
							match++;
						}
					}
					j--;
				}
				sub_dim++;
			}

			struct Point p_low_group_one, p_low_group_two, p_high_group_one, p_high_group_two;
			point_create(&p_low_group_one, node->context->dim, low_group_one);
			point_create(&p_low_group_two, node->context->dim, low_group_two);
			point_create(&p_high_group_one, node->context->dim, high_group_one);
			point_create(&p_high_group_two, node->context->dim, high_group_two);

			struct Rectangle MBR_group_one, MBR_group_two;
			rectangle_create(&MBR_group_one, &p_low_group_one, &p_high_group_one);
			rectangle_create(&MBR_group_two, &p_low_group_two, &p_high_group_two);

			margin_values[dim] += (rectangle_margin(&MBR_group_one) + rectangle_margin(&MBR_group_two));

			hashset_destroy(split_group);
		}
		free(low_group_one);
		free(low_group_two);
		free(high_group_one);
		free(high_group_two);
	}

	optimal_axis = 0;
	optimal_margin_value = DBL_MAX;
	for (dim = 0; dim < node->context->dim; dim++)
	{
		if (margin_values[dim] < optimal_margin_value)
		{
			optimal_axis = dim;
			optimal_margin_value = margin_values[dim];
		}
	}

	for (dim = 0; dim < node->context->dim; dim++)
	{
		if (dim != optimal_axis)
		{
			free(sorted_entries[dim]);
		}
	}

	return optimal_axis;
}

uint8_t _node_choose_split_index(uint8_t dim, struct Node *node, void ***sorted_entries, struct Rectangle *MBR_one, struct Rectangle *MBR_two)
{
	uint8_t k, optimal_distribution_index;
	double optimal_overlap_value, optimal_area_value, overlap_value, area_value;
	float *low_group_one = calloc(node->context->dim, sizeof(float));
	float *low_group_two = calloc(node->context->dim, sizeof(float));
	float *high_group_one = calloc(node->context->dim, sizeof(float));
	float *high_group_two = calloc(node->context->dim, sizeof(float));

	for (k = 1; k < (node->context->M - (2 * node->context->m) + 2); k++)
	{
		uint8_t i, sub_dim;
		hashset_t split_group = hashset_create();

		i = 0;
		low_group_one[dim] = ((struct Entry *)sorted_entries[dim][i])->MBR->low->coords[dim];
		while (i < (node->context->m - 1 + k))
		{
			hashset_add(split_group, sorted_entries[dim][i]);
			/* TODO: Solve the case when sorted entries have the same lower values */
			if (i == (node->context->m + k))
			{
				high_group_one[dim] = ((struct Entry *)sorted_entries[dim][i])->MBR->high->coords[dim];
			}
			i++;
		}
		low_group_two[dim] = ((struct Entry *)sorted_entries[dim][i])->MBR->low->coords[dim];
		high_group_two[dim] = ((struct Entry *)sorted_entries[dim][node->context->M])->MBR->high->coords[dim];

		sub_dim = 0;
		while (sub_dim < node->context->dim)
		{
			uint8_t j = 0, match = 0;

			if (sub_dim == dim)
			{
				sub_dim++;
				continue;
			}

			while ((j < node->context->M + 1) && (match < 2))
			{
				if (hashset_is_member(split_group, sorted_entries[sub_dim][j]))
				{
					if (low_group_one[sub_dim] == 0.0f)
					{
						low_group_one[sub_dim] = ((struct Entry *)sorted_entries[sub_dim][j])->MBR->low->coords[sub_dim];
						match++;
					}

				}
				else
				{
					if (low_group_two[sub_dim] == 0.0f)
					{
						low_group_two[sub_dim] = ((struct Entry *)sorted_entries[sub_dim][j])->MBR->low->coords[sub_dim];
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
					if (high_group_one[sub_dim] == 0.0f)
					{
						high_group_one[sub_dim] = ((struct Entry *)sorted_entries[sub_dim][j])->MBR->high->coords[sub_dim];
						match++;
					}

				}
				else
				{
					if (high_group_two[sub_dim] == 0.0f)
					{
						high_group_two[sub_dim] = ((struct Entry *)sorted_entries[sub_dim][j])->MBR->high->coords[sub_dim];
						match++;
					}
				}
				j--;
			}
			sub_dim++;
		}

		struct Point p_low_group_one, p_low_group_two, p_high_group_one, p_high_group_two;
		point_create(&p_low_group_one, node->context->dim, low_group_one);
		point_create(&p_low_group_two, node->context->dim, low_group_two);
		point_create(&p_high_group_one, node->context->dim, high_group_one);
		point_create(&p_high_group_two, node->context->dim, high_group_two);

		struct Rectangle MBR_group_one, MBR_group_two;
		rectangle_create(&MBR_group_one, &p_low_group_one, &p_high_group_one);
		rectangle_create(&MBR_group_two, &p_low_group_two, &p_high_group_two);

		optimal_overlap_value = DBL_MAX;
		optimal_area_value = DBL_MAX;
		overlap_value = rectangle_intersection_area(&MBR_group_one, &MBR_group_two);
		area_value = rectangle_area(&MBR_group_one) + rectangle_area(&MBR_group_two);
		if (overlap_value < optimal_overlap_value)
		{
			optimal_overlap_value = overlap_value;
			optimal_area_value = area_value;
			optimal_distribution_index = i;
			memcpy(MBR_one, &MBR_group_one, sizeof(MBR_group_one));
			memcpy(MBR_two, &MBR_group_two, sizeof(MBR_group_two));
		}
		else if (fabs(overlap_value - optimal_overlap_value) < DBL_EPSILON)
		{
			if (area_value < optimal_area_value)
			{
				optimal_overlap_value = overlap_value;
				optimal_area_value = area_value;
				optimal_distribution_index = i;
				memcpy(MBR_one, &MBR_group_one, sizeof(MBR_group_one));
				memcpy(MBR_two, &MBR_group_two, sizeof(MBR_group_two));
			}
		}
		hashset_destroy(split_group);
	}
	free(low_group_one);
	free(low_group_two);
	free(high_group_one);
	free(high_group_two);

	return optimal_distribution_index;
}

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
	struct Node *split_node = malloc(sizeof(struct Node));
	struct Rectangle *MBR_group_one = malloc(sizeof(struct Rectangle));
	struct Rectangle *MBR_group_two = malloc(sizeof(struct Rectangle));
	void ***sorted_entries = malloc(node->context->dim * sizeof(void **));
	uint8_t axis = _node_choose_split_axis(node, entry, sorted_entries);
	uint8_t index = _node_choose_split_index(axis, node, sorted_entries, MBR_group_one, MBR_group_two);

	free(node->entries);
	node->entries = malloc((index - 1) * sizeof(void *));
	memmove(node->entries, sorted_entries[axis], index - 1);
	node->capacity = node->count = index - 1;
	memmove(node->MBR, MBR_group_one, sizeof(MBR_group_one));

	node_create(split_node, node->context, node->parent, NULL, 0, MBR_group_two, node->level);
	split_node->entries = malloc((node->context->M + 1 - index) * sizeof(void *));
	memmove(split_node->entries, sorted_entries[axis], node->context->M + 1 - index);
	split_node->capacity = split_node->count = node->context->M + 1 - index;

	return split_node;
}