#ifndef _RTREE_COMMON_H_
#define _RTREE_COMMON_H_

#if __GNUC__ >= 4
	#define RTREE_PUBLIC __attribute__ ((visibility ("default")))
	#define RTREE_LOCAL  __attribute__ ((visibility ("hidden")))
#else
	#define RTREE_PUBLIC
	#define RTREE_LOCAL
#endif

typedef struct Context rt_ctx_t;
typedef struct Entry rt_entry_t;
typedef struct Node rt_node_t;
typedef struct Rectangle rt_rect_t;
typedef struct RTree rt_rtree_t;

#include <math.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define NELEMS(x) (sizeof(x)/sizeof((x)[0]))
#define NALLOC(m, M, f) MIN((int)ceil(m * f), M)

#endif