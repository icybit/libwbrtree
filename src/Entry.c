#include "Entry.h"
#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include "Context.h"
#include "Rectangle.h"

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

RTREE_LOCAL size_t entry_calculate_buffer_size(rt_ctx_t *context)
{
	return (sizeof(rt_entry_t *) +
		context->entry_size +
		context->dim * sizeof(float));
}

RTREE_LOCAL int entry_compare(const void *entry, const void *other, void *dimension)
{
	return rectangle_compare((*((rt_entry_t **)entry))->MBR, (*((rt_entry_t **)other))->MBR, dimension);
}

#ifdef DEBUG
RTREE_LOCAL void entry_print(rt_entry_t *entry)
{
	printf("ENTRY: [VALUE: %p,", entry->tuple);
	rectangle_print(entry->MBR);
	puts("]\n");
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