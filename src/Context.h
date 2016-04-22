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

#endif /* __RTREE_CONTEXT_H__ */