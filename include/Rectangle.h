#ifndef _RTREE_RECTANGLE_H_
#define _RTREE_RECTANGLE_H_

#include <stdint.h>
#include "Common.h"

struct Rectangle {
	uint8_t dim;
	float *low;
	float *high;
};

RTREE_LOCAL double rectangle_area(rt_rect_t *rectangle);
RTREE_LOCAL void rectangle_combine(rt_rect_t *rectangle, rt_rect_t *other);
RTREE_LOCAL int rectangle_compare_low(const rt_rect_t *rectangle, const rt_rect_t *other, uint8_t *dimension);
RTREE_LOCAL int rectangle_compare_high(const rt_rect_t *rectangle, const rt_rect_t *other, uint8_t *dimension);
RTREE_LOCAL void rectangle_copy(rt_rect_t *dest, const rt_rect_t *source);
RTREE_PUBLIC void rectangle_create(rt_rect_t *dest, float *low, float *high, uint8_t dimension);
RTREE_LOCAL void rectangle_extend_infinitely(rt_rect_t *dest);
RTREE_LOCAL double rectangle_intersection_area(rt_rect_t *rectangle, rt_rect_t *other);
RTREE_LOCAL double rectangle_margin(rt_rect_t *rectangle);
RTREE_LOCAL double rectangle_margin_value(rt_rect_t *rectangle, rt_rect_t *other);
RTREE_LOCAL double rectangle_min_distance(rt_rect_t *rectangle, rt_rect_t *other);
RTREE_LOCAL int rectangle_overlaps(rt_rect_t *rectangle, rt_rect_t *other);
#ifdef DEBUG
RTREE_LOCAL void rectangle_print(rt_rect_t *rectangle);
#endif

#endif