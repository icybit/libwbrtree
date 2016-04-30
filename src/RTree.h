#ifndef __RTREE_RTREE_H__
#define __RTREE_RTREE_H__

#include <stdint.h>
#include <stddef.h>
#include "wbdrtree/wbdrtreeapi.h"
#include "Common.h"

struct RTree {
	rt_node_t *root;
	rt_ctx_t *context;
};


#endif /* __RTREE_RTREE_H__ */