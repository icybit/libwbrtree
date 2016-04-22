#ifndef __RTREE_COMMON_H__
#define __RTREE_COMMON_H__

#include <math.h>

#if __GNUC__ >= 4
	#define RTREE_PUBLIC __attribute__ ((visibility ("default")))
	#define RTREE_LOCAL  __attribute__ ((visibility ("hidden")))
#else
	#define RTREE_PUBLIC
	#define RTREE_LOCAL
#endif

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define NELEMS(x) (sizeof(x)/sizeof((x)[0]))
#define NALLOC(m, M, f) MIN((int)ceil(m * f), M)

#endif /* __RTREE_COMMON_H__ */