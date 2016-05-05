#include "Entry.h"
#include <assert.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include <string.h>
#include <stdlib.h>
#include "Context.h"
#include "Rectangle.h"

typedef struct Comparator_Opts {
	uint8_t dim;
	uint8_t low;
} rt_cmp_opts_t;

RTREE_PUBLIC rt_entry_t * rtree_entry_create(void *tuple, rt_rect_t *MBR)
{
	rt_entry_t *entry;

	assert(tuple && MBR);

	entry = malloc(sizeof(entry));

	entry->tuple = tuple;
	entry->MBR = MBR;

	return entry;
}

RTREE_PUBLIC void * rtree_entry_get_tuple(rt_entry_t *entry)
{
	assert(entry);

	return entry->tuple;
}

RTREE_PUBLIC void rtree_entry_destroy(rt_entry_t *entry)
{
	assert(entry);

	rtree_rectangle_destroy(entry->MBR);
	free(entry->tuple);
	free(entry);
	entry = NULL;
}

RTREE_LOCAL int entry_compare(const void *entry, const void *other, void *cmp_opts)
{
	rt_cmp_opts_t *opts = (rt_cmp_opts_t *)cmp_opts;

	if (opts->low)
	{
		return rectangle_compare_low((*((rt_entry_t **)entry))->MBR, (*((rt_entry_t **)other))->MBR, &opts->dim);
	}
	else
	{
		return rectangle_compare_high((*((rt_entry_t **)entry))->MBR, (*((rt_entry_t **)other))->MBR, &opts->dim);
	}
}

#ifdef DEBUG
RTREE_LOCAL void entry_print(rt_entry_t *entry)
{
	printf("ENTRY: [VALUE: %p,", entry->tuple);
	rectangle_print(entry->MBR);
	puts("]");
}
#endif

RTREE_LOCAL size_t entry_serialize(rt_entry_t *entry, uint8_t *buffer, size_t entry_size)
{
	size_t index = 0;
	memcpy(&buffer[index], &entry, sizeof(rt_entry_t *));
	index += sizeof(rt_entry_t *);
	memcpy(&buffer[index], entry->tuple, sizeof(entry_size));
	index += sizeof(entry_size);
	memcpy(&buffer[index], entry->MBR->low, entry->MBR->dim * sizeof(float));
	index += entry->MBR->dim * sizeof(float);

	return index;
}