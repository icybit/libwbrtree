#ifndef __RTREE_STD_QSORT_R_H__
#define __RTREE_STD_QSORT_R_H__

#include <stdint.h>
#include <stddef.h>
#include "Common.h"

typedef int(*compar_d_fn_t)(const void *, const void *, void *);

void rt_qsort_r_fn(void *const pbase, size_t total_elems, size_t size, compar_d_fn_t cmp, void *arg);

#endif /* !__RTREE_STD_QSORT_R_H__ */