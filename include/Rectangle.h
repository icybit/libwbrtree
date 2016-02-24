#ifndef _RTREE_RECTANGLE_H_
#define _RTREE_RECTANGLE_H_

struct Point {
	float x;
	float y;
};

struct Rectangle {
	struct Point low;
	struct Point high;
};

void rectangle_combine(struct Rectangle *rectangle, struct Rectangle *other);
int rectangle_overlaps(struct Rectangle *rectangle, struct Rectangle *other);

#endif