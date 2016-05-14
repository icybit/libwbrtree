#ifndef __RTREE_CONTEXT_H__
#define __RTREE_CONTEXT_H__

#include <stdint.h>
#include <stddef.h>
#include "wbrtree/wbrtreeapi.h"
#include "Common.h"

typedef size_t(*serialize_fn)(rt_entry_t *, uint8_t **);

struct Context {
	uint8_t m;
	uint8_t M;
	uint8_t dim;
	serialize_fn serializer;
	float alloc_factor;
	rt_rect_t *space;
};

void context_copy(rt_ctx_t *dest, const rt_ctx_t *source);
rt_ctx_t * context_create(uint8_t m, uint8_t M, uint8_t dim, size_t(*serialize_fn)(rt_entry_t *, uint8_t **), float alloc_factor, rt_rect_t *space);
void context_destroy(rt_ctx_t **ctx);
rt_ctx_t * context_duplicate(const rt_ctx_t *source);

#endif /* __RTREE_CONTEXT_H__ */