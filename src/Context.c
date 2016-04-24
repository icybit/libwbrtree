#include "Context.h"
#include <assert.h>
#include <stdlib.h>

RTREE_PUBLIC rt_ctx_t * rtree_context_create(uint8_t m, uint8_t M, uint8_t dim, size_t entry_size, float alloc_factor, rt_rect_t *MBR)
{
	rt_ctx_t *ctx;

	assert(MBR);

	ctx = malloc(sizeof(rt_ctx_t));

	ctx->m = (m <= M / 2 ? m : 2);
	ctx->M = (M > 2 ? M : 3);
	ctx->dim = dim;
	ctx->entry_size = entry_size;
	ctx->alloc_factor = (alloc_factor > 1.0f ? alloc_factor : 2.0f);
	ctx->space = MBR;

	return ctx;
}

RTREE_PUBLIC void rtree_context_destroy(rt_ctx_t *ctx)
{
	assert(ctx);

	rtree_rectangle_destroy(ctx->space);
	free(ctx);
	ctx = NULL;
}