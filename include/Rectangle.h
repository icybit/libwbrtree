#ifndef _RTREE_RECTANGLE_H_
#define _RTREE_RECTANGLE_H_

#include <stdint.h>

#define MIN(a, b) ((a) < (b) ? (a) : (b))
#define MAX(a, b) ((a) > (b) ? (a) : (b))
#define NELEMS(x) (sizeof(x)/sizeof((x)[0]))

struct Point {
	uint8_t dim;
	float *coords;
};

struct Rectangle {
	uint8_t dim;
	struct Point *low;
	struct Point *high;
};

void point_create(struct Point *dest, uint8_t dimension, float *coordinates);

double rectangle_area(struct Rectangle *rectangle);
void rectangle_combine(struct Rectangle *rectangle, struct Rectangle *other);
void rectangle_create(struct Rectangle *dest, struct Point *low, struct Point *high);
void rectangle_extend_infinitely(struct Rectangle *dest);
double rectangle_min_distance(struct Rectangle *rectangle, struct Rectangle *other);
int rectangle_overlaps(struct Rectangle *rectangle, struct Rectangle *other);

#endif