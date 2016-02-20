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

void combine_rectangle(struct Rectangle *rectangle, struct Rectangle *other);

#endif