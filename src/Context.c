#include <assert.h>
#include "Context.h"

void context_create(rt_ctx_t *dest, uint8_t m, uint8_t M, uint8_t dim, size_t entry_size, float alloc_factor, rt_rect_t *space_MBR)
{
	assert(m > 1 && M > 3 && ((M/2) >= m));
	assert(space_MBR != NULL);

	dest->m = m;
	dest->M = M;
	dest->dim = dim;
	dest->entry_size = entry_size;
	dest->alloc_factor = (alloc_factor > 1.0f ? alloc_factor : 2.0f);
	dest->space = space_MBR;
}