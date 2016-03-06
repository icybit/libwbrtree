#include <assert.h>
#include <float.h>
#include <math.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include "Common.h"
#include "Rectangle.h"

int point_compare(const struct Point *point, const struct Point *other, uint8_t *dimension)
{
	return (int)(point->coords[*dimension] - other->coords[*dimension]);
}

void point_create(struct Point *dest, uint8_t dimension, float *coordinates)
{
	assert(coordinates && dimension);

	dest->dim = dimension;
	dest->coords = coordinates;
}

#ifdef DEBUG
void point_print(struct Point *point)
{
	uint8_t dim;
	for (dim = 0; dim < point->dim; dim++)
	{
		printf("%g", point->coords[dim]);
		if (dim < point->dim - 1)
		{
			puts(",");
		}
	}
}
#endif

double rectangle_area(struct Rectangle *rectangle)
{
	double area = 1.0;
	uint8_t dim;

	for (dim = 0; dim < rectangle->dim; dim++)
	{
		area *= rectangle->high->coords[dim] - rectangle->low->coords[dim];
	}

	return area;
}

void rectangle_combine(struct Rectangle *rectangle, struct Rectangle *other) 
{
	uint8_t dim;

	assert(rectangle->dim == other->dim);

	for (dim = 0; dim < rectangle->dim; dim++)
	{
		rectangle->low->coords[dim] = fminf(rectangle->low->coords[dim], other->low->coords[dim]);
		rectangle->high->coords[dim] = fmaxf(rectangle->high->coords[dim], other->high->coords[dim]);
	}
}

int rectangle_compare(const struct Rectangle *rectangle, const struct Rectangle *other, uint8_t *dimension)
{
	return point_compare(rectangle->low, other->low, dimension);
}

void rectangle_create(struct Rectangle *dest, struct Point *low, struct Point *high)
{
	assert(low && high);
	assert(low->dim == high->dim);

	dest->dim = low->dim;
	dest->low = low;
	dest->high = high;
}

void rectangle_extend_infinitely(struct Rectangle *dest)
{
	uint8_t dim;
	for (dim = 0; dim < dest->dim; dim++)
	{
		dest->low->coords[dim] = -FLT_MAX;
		dest->high->coords[dim] = FLT_MAX;
	}
}

double rectangle_intersection_area(struct Rectangle *rectangle, struct Rectangle *other)
{
	double area = 1.0;
	float f0, f1;
	uint8_t dim;

	assert(rectangle->dim == other->dim);

	if (rectangle_overlaps(rectangle, other))
	{
		for (dim = 0; dim < rectangle->dim; dim++)
		{
			f0 = fmaxf(rectangle->low->coords[dim], other->low->coords[dim]);
			f1 = fminf(rectangle->high->coords[dim], other->high->coords[dim]);
			area *= f1 - f0;
		}
	}

	return area;
}

double rectangle_margin(struct Rectangle *rectangle)
{
	uint8_t dim;
	double multiplicity = pow(2.0, (double)(rectangle->dim - 1));
	double margin = 0.0;

	for (dim = 0; dim < rectangle->dim; dim++)
	{
		margin += (rectangle->high->coords[dim] - rectangle->low->coords[dim]) * multiplicity;
	}

	return margin;
}

double rectangle_margin_value(struct Rectangle *rectangle, struct Rectangle *other)
{
	return (rectangle_margin(rectangle) + rectangle_margin(other));
}

double rectangle_min_distance(struct Rectangle *rectangle, struct Rectangle *other)
{
	double result = 0.0;
	uint8_t dim;

	assert(rectangle->dim == other->dim);

	for (dim = 0; dim < rectangle->dim; dim++)
	{
		if (rectangle->low->coords[dim] > other->high->coords[dim])
		{
			result += powf(rectangle->low->coords[dim] - other->high->coords[dim], 2.0);
		}
		else if (rectangle->high->coords[dim] < other->low->coords[dim])
		{
			result += powf(other->low->coords[dim] - rectangle->high->coords[dim], 2.0);
		}
	}

	return sqrt(result);
}

int rectangle_overlaps(struct Rectangle *rectangle, struct Rectangle *other) 
{
	uint8_t dim;

	assert(rectangle->dim == other->dim);

	for (dim = 0; dim < rectangle->dim; dim++)
	{
		if (rectangle->high->coords[dim] < other->low->coords[dim] ||
			rectangle->low->coords[dim] > other->high->coords[dim])
		{
			return 0;
		}
	}

	return 1;
}

#ifdef DEBUG
void rectangle_print(struct Rectangle *rectangle)
{
	puts("MBR: LOW(");
	point_print(rectangle->low);
	puts("), HIGH(");
	point_print(rectangle->high);
	puts(")\n");
}
#endif