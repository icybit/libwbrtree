#include <assert.h>
#include <float.h>
#include <math.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <string.h>
#include "Common.h"
#include "Rectangle.h"

double rectangle_area(rt_rect_t *rectangle)
{
	double area = 1.0;
	uint8_t dim;

	for (dim = 0; dim < rectangle->dim; dim++)
	{
		area *= rectangle->high[dim] - rectangle->low[dim];
	}

	return area;
}

void rectangle_combine(rt_rect_t *rectangle, rt_rect_t *other) 
{
	uint8_t dim;

	assert(rectangle->dim == other->dim);

	for (dim = 0; dim < rectangle->dim; dim++)
	{
		rectangle->low[dim] = fminf(rectangle->low[dim], other->low[dim]);
		rectangle->high[dim] = fmaxf(rectangle->high[dim], other->high[dim]);
	}
}

int rectangle_compare(const rt_rect_t *rectangle, const rt_rect_t *other, uint8_t *dimension)
{
	return (int)(rectangle->low[*dimension] - other->low[*dimension]);
}

void rectangle_copy(rt_rect_t *dest, const rt_rect_t *source)
{
	assert(sizeof(*dest) == sizeof(*source));

	dest->dim = source->dim;
	memcpy(dest->low, source->low, source->dim * sizeof(float));
	memcpy(dest->high, source->high, source->dim * sizeof(float));
}

void rectangle_create(rt_rect_t *dest, float *low, float *high, uint8_t dimension)
{
	assert(low && high);

	dest->dim = dimension;
	dest->low = low;
	dest->high = high;
}

void rectangle_extend_infinitely(rt_rect_t *dest)
{
	uint8_t dim;
	for (dim = 0; dim < dest->dim; dim++)
	{
		dest->low[dim] = FLT_MAX;
		dest->high[dim] = -FLT_MAX;
	}
}

double rectangle_intersection_area(rt_rect_t *rectangle, rt_rect_t *other)
{
	double area = 1.0;
	float f0, f1;
	uint8_t dim;

	assert(rectangle->dim == other->dim);

	if (rectangle_overlaps(rectangle, other))
	{
		for (dim = 0; dim < rectangle->dim; dim++)
		{
			f0 = fmaxf(rectangle->low[dim], other->low[dim]);
			f1 = fminf(rectangle->high[dim], other->high[dim]);
			area *= f1 - f0;
		}
	}

	return area;
}

double rectangle_margin(rt_rect_t *rectangle)
{
	uint8_t dim;
	double multiplicity = pow(2.0, (double)(rectangle->dim - 1));
	double margin = 0.0;

	for (dim = 0; dim < rectangle->dim; dim++)
	{
		margin += (rectangle->high[dim] - rectangle->low[dim]) * multiplicity;
	}

	return margin;
}

double rectangle_margin_value(rt_rect_t *rectangle, rt_rect_t *other)
{
	return (rectangle_margin(rectangle) + rectangle_margin(other));
}

double rectangle_min_distance(rt_rect_t *rectangle, rt_rect_t *other)
{
	double result = 0.0;
	uint8_t dim;

	assert(rectangle->dim == other->dim);

	for (dim = 0; dim < rectangle->dim; dim++)
	{
		if (rectangle->low[dim] > other->high[dim])
		{
			result += powf(rectangle->low[dim] - other->high[dim], 2.0);
		}
		else if (rectangle->high[dim] < other->low[dim])
		{
			result += powf(other->low[dim] - rectangle->high[dim], 2.0);
		}
	}

	return sqrt(result);
}

int rectangle_overlaps(rt_rect_t *rectangle, rt_rect_t *other) 
{
	uint8_t dim;

	assert(rectangle->dim == other->dim);

	for (dim = 0; dim < rectangle->dim; dim++)
	{
		if (rectangle->high[dim] < other->low[dim] ||
			rectangle->low[dim] > other->high[dim])
		{
			return 0;
		}
	}

	return 1;
}

#ifdef DEBUG
void rectangle_print(rt_rect_t *rectangle)
{
	uint8_t dim;
	printf("MBR: LOW(");
	for (dim = 0; dim < rectangle->dim; dim++)
	{
		printf("%g", rectangle->low[dim]);
		if (dim < rectangle->dim - 1)
		{
			printf(",");
		}
	}
	printf("), HIGH(");
	for (dim = 0; dim < rectangle->dim; dim++)
	{
		printf("%g", rectangle->high[dim]);
		if (dim < rectangle->dim - 1)
		{
			printf(",");
		}
	}
	printf(")");
}
#endif