#ifndef _RTREE_RECTANGLE_H_
#define _RTREE_RECTANGLE_H_

#include <stdint.h>

struct Point {
	uint8_t dim;
	float *coords;
};

struct Rectangle {
	uint8_t dim;
	struct Point *low;
	struct Point *high;
};

int point_compare(const struct Point *point, const struct Point *other, uint8_t *dimension);
void point_create(struct Point *dest, uint8_t dimension, float *coordinates);

double rectangle_area(struct Rectangle *rectangle);
void rectangle_combine(struct Rectangle *rectangle, struct Rectangle *other);
int rectangle_compare(const struct Rectangle *rectangle, const struct Rectangle *other, uint8_t *dimension);
void rectangle_create(struct Rectangle *dest, struct Point *low, struct Point *high);
void rectangle_extend_infinitely(struct Rectangle *dest);
double rectangle_intersection_area(struct Rectangle *rectangle, struct Rectangle *other);
double rectangle_margin(struct Rectangle *rectangle);
double rectangle_margin_value(struct Rectangle *rectangle, struct Rectangle *other);
double rectangle_min_distance(struct Rectangle *rectangle, struct Rectangle *other);
int rectangle_overlaps(struct Rectangle *rectangle, struct Rectangle *other);

#endif