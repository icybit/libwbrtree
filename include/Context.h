#ifndef _RTREE_CONTEXT_H_
#define _RTREE_CONTEXT_H_

#include <stdint.h>
#include <stddef.h>
#include "Common.h"

struct Context {
	uint8_t m;
	uint8_t M;
	uint8_t dim;
	size_t entry_size;
	float alloc_factor;
	rt_rect_t *space;
};

RTREE_PUBLIC void context_create(rt_ctx_t *dest, uint8_t m, uint8_t M, uint8_t dim, size_t entry_size, float alloc_factor, rt_rect_t *space_MBR);

#endif