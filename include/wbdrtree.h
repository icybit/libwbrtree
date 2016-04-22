#ifndef __WBD_RTREE_H__
#define __WBD_RTREE_H__

#define __WBD_RTREE_H_INSIDE__

#define RTREE_VERSION_MAJOR 0
#define RTREE_VERSION_MINOR 1
#define RTREE_VERSION_PATCH 0

#define RTREE_MAKE_VERSION(major, minor, patch) \
    ((major) * 10000 + (minor) * 100 + (patch))
#define RTREE_VERSION \
	RTREE_MAKE_VERSION(RTREE_VERSION_MAJOR, RTREE_VERSION_MINOR, RTREE_VERSION_PATCH)

#include "wbdrtree/wbdrtreeapi.h"

#undef __WBD_RTREE_H_INSIDE__

#endif /* __WBD_RTREE_H__ */