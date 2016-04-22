#include "Entry.h"
#include <assert.h>
#include <string.h>
#include "Context.h"
#include "Rectangle.h"

RTREE_PUBLIC void rtree_entry_create(rt_entry_t *dest, void *tuple, rt_rect_t *MBR)
{
	assert(MBR != NULL);

	dest->tuple = tuple;
	dest->MBR = MBR;
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