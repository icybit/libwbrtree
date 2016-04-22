#ifndef __RTREE_RECTANGLE_H__
#define __RTREE_RECTANGLE_H__

#include <stdint.h>
#include <stddef.h>
#include "wbdrtree/wbdrtreeapi.h"
#include "Common.h"

struct Rectangle {
	uint8_t dim;
	float *low;
	float *high;
};

double rectangle_area(rt_rect_t *rectangle);
void rectangle_combine(rt_rect_t *rectangle, rt_rect_t *other);
int rectangle_compare(const rt_rect_t *rectangle, const rt_rect_t *other, uint8_t *dimension);
void rectangle_copy(rt_rect_t *dest, const rt_rect_t *source);
void rectangle_extend_infinitely(rt_rect_t *dest);
double rectangle_intersection_area(rt_rect_t *rectangle, rt_rect_t *other);
double rectangle_margin(rt_rect_t *rectangle);
double rectangle_margin_value(rt_rect_t *rectangle, rt_rect_t *other);
double rectangle_min_distance(rt_rect_t *rectangle, rt_rect_t *other);
int rectangle_overlaps(rt_rect_t *rectangle, rt_rect_t *other);
#ifdef DEBUG
void rectangle_print(rt_rect_t *rectangle);
#endif

#endif /* __RTREE_RECTANGLE_H__ */