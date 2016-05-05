#ifndef __RTREE_CONTEXT_H__
#define __RTREE_CONTEXT_H__

#include <stdint.h>
#include <stddef.h>
#include "wbdrtree/wbdrtreeapi.h"
#include "Common.h"

struct Context {
	uint8_t m;
	uint8_t M;
	uint8_t dim;
	size_t entry_size;
	float alloc_factor;
	rt_rect_t *space;
};

void context_copy(rt_ctx_t *dest, const rt_ctx_t *source);
rt_ctx_t * context_duplicate(const rt_ctx_t *source);

#endif /* __RTREE_CONTEXT_H__ */