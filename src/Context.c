#include "Context.h"
#include "Rectangle.h"
#include <assert.h>
#include <stdlib.h>
#ifdef DEBUG
#include <stdio.h>
#endif


RTREE_LOCAL rt_ctx_t * context_create(uint8_t m, uint8_t M, uint8_t dim, size_t(*serialize_fn)(rt_entry_t *, uint8_t **), float alloc_factor, rt_rect_t *space)
{
	rt_ctx_t *ctx;

	assert(m > 1 && M > 3 && ((M / 2) >= m));
	assert(space != NULL);

	ctx = malloc(sizeof(rt_ctx_t));

	ctx->m = m;
	ctx->M = M;
	ctx->dim = dim;
	ctx->serializer = serialize_fn;
	ctx->alloc_factor = (alloc_factor > 1.0f ? alloc_factor : 2.0f);
	ctx->space = space;

	return ctx;
}

RTREE_LOCAL void context_copy(rt_ctx_t *dest, const rt_ctx_t *source)
 {    
	dest->dim = source->dim;
	dest->m = source->m;
	dest->M = source->M;
	dest->serializer = source->serializer;
	dest->alloc_factor = source->alloc_factor;
 
    rectangle_copy(dest->space, source->space);
}

RTREE_LOCAL void context_destroy(rt_ctx_t *ctx)
{
	assert(ctx);

	rectangle_destroy(ctx->space);
	free(ctx);
	ctx = NULL;
}

RTREE_LOCAL rt_ctx_t * context_duplicate(const rt_ctx_t *source)
 {  
 	assert(source);

 	return context_create(source->m, source->M, source->dim, source->serializer, 
 		source->alloc_factor, rectangle_duplicate(source->space));
}