#include <assert.h>
#include "Context.h"

void context_create(rt_ctx_t *dest, uint8_t m, uint8_t M, uint8_t dim, size_t entry_size, float alloc_factor, rt_rect_t *space_MBR)
{
	assert(space_MBR != NULL);

	dest->m = (m <= M / 2 ? m : 2);
	dest->M = (M > 2 ? M : 3);
	dest->dim = dim;
	dest->entry_size = entry_size;
	dest->alloc_factor = (alloc_factor > 1.0f ? alloc_factor : 2.0f);
	dest->space = space_MBR;
}