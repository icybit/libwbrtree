#ifndef __RTREE_CONTEXT_H__
#define __RTREE_CONTEXT_H__

#include <stdint.h>
#include <stddef.h>
#include "wbdrtree/wbdrtreeapi.h"
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

#endif /* __RTREE_CONTEXT_H__ */