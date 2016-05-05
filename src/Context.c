#include "Context.h"
#include "Rectangle.h"
#include <assert.h>
#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif


RTREE_PUBLIC rt_ctx_t * rtree_context_create(uint8_t m, uint8_t M, uint8_t dim, size_t entry_size, float alloc_factor, rt_rect_t *space)
{
	rt_ctx_t *ctx;

	assert(m > 1 && M > 3 && ((M / 2) >= m));
	assert(space != NULL);

	ctx = malloc(sizeof(rt_ctx_t));

	ctx->m = m;
	ctx->M = M;
	ctx->dim = dim;
	ctx->entry_size = entry_size;
	ctx->alloc_factor = (alloc_factor > 1.0f ? alloc_factor : 2.0f);
	ctx->space = space;

	return ctx;
}

RTREE_LOCAL void context_copy(rt_ctx_t *dest, const rt_ctx_t *source)
 {    
	dest->dim = source->dim;
	dest->m = source->m;
	dest->M = source->M;
	dest->entry_size = source->entry_size;
	dest->alloc_factor = source->alloc_factor;
 
    rectangle_copy(dest->space, source->space);
}

RTREE_PUBLIC void rtree_context_destroy(rt_ctx_t *ctx)
{
	assert(ctx);

	rtree_rectangle_destroy(ctx->space);
	free(ctx);
	ctx = NULL;
}

RTREE_LOCAL rt_ctx_t * context_duplicate(const rt_ctx_t *source)
 {  
 	assert(source);

 	return rtree_context_create(source->m, source->M, source->dim, source->entry_size, 
 		source->alloc_factor, rectangle_duplicate(source->space));
}