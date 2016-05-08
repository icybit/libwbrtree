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

RTREE_LOCAL rt_entry_t * entry_create(void *tuple, rt_rect_t *MBR)
{
	rt_entry_t *entry;

	assert(tuple && MBR);

	entry = malloc(sizeof(entry));

	entry->tuple = tuple;
	entry->MBR = MBR;

	return entry;
}

RTREE_LOCAL void entry_destroy(rt_entry_t *entry)
{
	assert(entry);

	rectangle_destroy(entry->MBR);
	free(entry);
	entry = NULL;
}

RTREE_LOCAL void * entry_get_tuple(rt_entry_t *entry)
{
	assert(entry);

	return entry->tuple;
}

#ifdef DEBUG
RTREE_LOCAL void entry_print(rt_entry_t *entry)
{
	printf("ENTRY: [VALUE: %p,", entry->tuple);
	rectangle_print(entry->MBR);
	puts("]");
}
#endif