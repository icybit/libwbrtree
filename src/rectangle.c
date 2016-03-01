#include <assert.h>
#include <float.h>
#include <math.h>
#include <stdlib.h>
#include "Common.h"
#include "Rectangle.h"

void point_create(struct Point *dest, uint8_t dimension, float *coordinates)
{
	assert(coordinates);

	dest->dim = dimension;
	dest->coords = coordinates;
}

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