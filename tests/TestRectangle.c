#include "TestRectangle.h"
#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../src/Rectangle.h"

static float * initialize_coordinates(float x, float y);

void _test_rectangle_area() {
	rt_rect_t *rectangle;
	uint8_t dimension = 2;
	float *low = initialize_coordinates(0.0f, 0.0f);
	float *high = initialize_coordinates(2.0f, 2.0f);

	rectangle = rtree_rectangle_create(low, high, dimension);

	assert_true(fabs(rectangle_area(rectangle) - 4.0) < DBL_EPSILON);

	rtree_rectangle_destroy(rectangle);
}

void _test_rectangle_create() {
	rt_rect_t *rectangle;
	uint8_t index, dimension = 2;
	float *low = initialize_coordinates(0.0f, 0.0f);
	float *high = initialize_coordinates(2.0f, 2.0f);

	rectangle = rtree_rectangle_create(low, high, dimension);

	for (index = 0; index < dimension; index++)
	{
		assert_true(rectangle->low[index] == low[index]);
		assert_true(rectangle->high[index] == high[index]);
	}
	assert_int_equal(rectangle->dim, dimension);

	rtree_rectangle_destroy(rectangle);
}

void _test_rectangle_combine() {
	uint8_t index, dim = 2;
	rt_rect_t *rectangle_a, *rectangle_b;
	float *low_a = initialize_coordinates(0.0f, 0.0f);
	float *high_a = initialize_coordinates(2.0f, 2.0f);
	float *low_b = initialize_coordinates(1.0f, 1.0f);
	float *high_b = initialize_coordinates(3.0f, 3.0f);

	rectangle_a = rtree_rectangle_create(low_a, high_a, dim);
	rectangle_b = rtree_rectangle_create(low_b, high_b, dim);
		
	rectangle_combine(rectangle_a, rectangle_b);
	
	assert_int_equal(rectangle_a->dim, dim);
	for (index = 0; index < dim; index++) {
		assert_true(rectangle_a->low[index] == low_a[index]);
		assert_true(rectangle_a->high[index] == high_b[index]);
	}

	rectangle_a->low[0] = 1.0f;
	rectangle_a->low[1] = 1.0f;
	rectangle_a->high[0] = 3.0f;
	rectangle_a->high[1] = 3.0f;

	rectangle_b->low[0] = 1.0f;
	rectangle_b->low[1] = 0.0f;
	rectangle_b->high[0] = 4.0f;
	rectangle_b->high[1] = 4.0f;

	rectangle_combine(rectangle_a, rectangle_b);
	assert_int_equal(rectangle_a->dim, dim);
	for (index = 0; index < dim; index++) {
		assert_true(rectangle_a->low[index] == low_b[index]);
		assert_true(rectangle_a->high[index] == high_b[index]);
	}

	rtree_rectangle_destroy(rectangle_a);
	rtree_rectangle_destroy(rectangle_b);
}

void _test_rectangle_extend_infinitely() {
	rt_rect_t *rectangle;
	uint8_t index, dim = 2;
	float *low = initialize_coordinates(0.0f, 0.0f);
	float *high = initialize_coordinates(2.0f, 2.0f);

	rectangle = rtree_rectangle_create(low, high, dim);

	rectangle_extend_infinitely(rectangle);
	assert_int_equal(rectangle->dim, dim);
	
	for (index = 0; index < dim; index++) {
		assert_true(rectangle->low[index] == FLT_MAX);
		assert_true(rectangle->high[index] == -FLT_MAX);
	}

	rtree_rectangle_destroy(rectangle);
}

void _test_rectangle_min_distance() {
	rt_rect_t *rectangle_a, *rectangle_b;
	uint8_t dim = 2;
	float *low_a = initialize_coordinates(0.0f, 0.0f);
	float *high_a = initialize_coordinates(2.0f, 2.0f);
	float *low_b = initialize_coordinates(5.0f, 6.0f);
	float *high_b = initialize_coordinates(8.0f, 8.0f);

	rectangle_a = rtree_rectangle_create(low_a, high_a, dim);
	rectangle_b = rtree_rectangle_create(low_b, high_b, dim);
	
	assert_int_equal(rectangle_min_distance(rectangle_a, rectangle_b), 5);
	assert_int_equal(rectangle_min_distance(rectangle_b, rectangle_a), 5);

	rtree_rectangle_destroy(rectangle_a);
	rtree_rectangle_destroy(rectangle_b);
}

void _test_rectangle_overlaps() {
	rt_rect_t *rectangle_a, *rectangle_b, *rectangle_c;
	uint8_t dim = 2;
	float *low_a = initialize_coordinates(0.0f, 0.0f);
	float *high_a = initialize_coordinates(2.0f, 2.0f);
	float *low_b = initialize_coordinates(1.0f, 1.0f);
	float *high_b = initialize_coordinates(3.0f, 3.0f);
	float *low_c = initialize_coordinates(3.0f, 3.0f);
	float *high_c = initialize_coordinates(5.0f, 5.0f);

	rectangle_a = rtree_rectangle_create(low_a, high_a, dim);
	rectangle_b = rtree_rectangle_create(low_b, high_b, dim);
	rectangle_c = rtree_rectangle_create(low_c, high_c, dim);

	assert_true(rectangle_overlaps(rectangle_a, rectangle_b));
	assert_false(rectangle_overlaps(rectangle_a, rectangle_c));

	rtree_rectangle_destroy(rectangle_a);
	rtree_rectangle_destroy(rectangle_b);
	rtree_rectangle_destroy(rectangle_c);
}

void _test_rectangle_compare_low() {
	rt_rect_t *rectangle_a, *rectangle_b;
	uint8_t dimensions = 2, dim[] = { 0, 1 };
	float *low_a = initialize_coordinates(1.0f, 2.0f);
	float *high_a = initialize_coordinates(3.0f, 4.0f);
	float *low_b = initialize_coordinates(2.0f, 1.0f);
	float *high_b = initialize_coordinates(4.0f, 3.0f);

	rectangle_a = rtree_rectangle_create(low_a, high_a, dimensions);
	rectangle_b = rtree_rectangle_create(low_b, high_b, dimensions);
	
	assert_true(rectangle_compare_low(rectangle_a, rectangle_b, &dim[0]) < 0);
	assert_true(rectangle_compare_low(rectangle_a, rectangle_b, &dim[1]) > 0);
	assert_true(rectangle_compare_low(rectangle_b, rectangle_a, &dim[0]) > 0);
	assert_true(rectangle_compare_low(rectangle_b, rectangle_a, &dim[1]) < 0);
	assert_true(rectangle_compare_low(rectangle_b, rectangle_b, &dim[0]) == 0);
	assert_true(rectangle_compare_low(rectangle_b, rectangle_b, &dim[1]) == 0);

	rtree_rectangle_destroy(rectangle_a);
	rtree_rectangle_destroy(rectangle_b);
}

void _test_rectangle_compare_high() {
	rt_rect_t *rectangle_a, *rectangle_b;
	uint8_t dimensions = 2, dim[] = { 0, 1 };
	float *low_a = initialize_coordinates(1.0f, 2.0f);
	float *high_a = initialize_coordinates(3.0f, 4.0f);
	float *low_b = initialize_coordinates(2.0f, 1.0f);
	float *high_b = initialize_coordinates(4.0f, 3.0f);

	rectangle_a = rtree_rectangle_create(low_a, high_a, dimensions);
	rectangle_b = rtree_rectangle_create(low_b, high_b, dimensions);

	assert_true(rectangle_compare_high(rectangle_a, rectangle_b, &dim[0]) < 0);
	assert_true(rectangle_compare_high(rectangle_a, rectangle_b, &dim[1]) > 0);
	assert_true(rectangle_compare_high(rectangle_b, rectangle_a, &dim[0]) > 0);
	assert_true(rectangle_compare_high(rectangle_b, rectangle_a, &dim[1]) < 0);
	assert_true(rectangle_compare_high(rectangle_b, rectangle_b, &dim[0]) == 0);
	assert_true(rectangle_compare_high(rectangle_b, rectangle_b, &dim[1]) == 0);

	rtree_rectangle_destroy(rectangle_a);
	rtree_rectangle_destroy(rectangle_b);
}

void _test_rectangle_intersection_area() {
	rt_rect_t *rectangle_a, *rectangle_b;
	uint8_t dim = 2;
	float *low_a = initialize_coordinates(0.0f, 0.0f);
	float *high_a = initialize_coordinates(2.0f, 2.0f);
	float *low_b = initialize_coordinates(1.0f, 1.0f);
	float *high_b = initialize_coordinates(3.0f, 3.0f);

	rectangle_a = rtree_rectangle_create(low_a, high_a, dim);
	rectangle_b = rtree_rectangle_create(low_b, high_b, dim);

	assert_int_equal(rectangle_intersection_area(rectangle_a, rectangle_b), 1);
	assert_int_equal(rectangle_intersection_area(rectangle_b, rectangle_a), 1);

	rtree_rectangle_destroy(rectangle_a);
	rtree_rectangle_destroy(rectangle_b);
}

void _test_rectangle_margin() {
	rt_rect_t *rectangle;
	uint8_t dim = 2;
	float *low = initialize_coordinates(0.0f, 0.0f);
	float *high = initialize_coordinates(2.0f, 3.0f);

	rectangle = rtree_rectangle_create(low, high, dim);
	
	assert_int_equal(rectangle_margin(rectangle), 10);

	rtree_rectangle_destroy(rectangle);
}

void _test_rectangle_margin_value() {
	rt_rect_t *rectangle_a, *rectangle_b;
	uint8_t dim = 2;
	float *low_a = initialize_coordinates(0.0f, 0.0f);
	float *high_a = initialize_coordinates(2.0f, 2.0f);
	float *low_b = initialize_coordinates(1.0f, 1.0f);
	float *high_b = initialize_coordinates(5.0f, 3.0f);

	rectangle_a = rtree_rectangle_create(low_a, high_a, dim);
	rectangle_b = rtree_rectangle_create(low_b, high_b, dim);
		
	assert_int_equal(rectangle_margin_value(rectangle_a, rectangle_b), 20);
	assert_int_equal(rectangle_margin_value(rectangle_b, rectangle_a), 20);

	rtree_rectangle_destroy(rectangle_a);
	rtree_rectangle_destroy(rectangle_b);
}

void _test_rectangle_copy() {
	rt_rect_t *rectangle_a, *rectangle_b;
	uint8_t index, dim = 2;
	float *low_a = initialize_coordinates(0.0f, 0.0f);
	float *high_a = initialize_coordinates(2.0f, 2.0f);
	float *low_b = initialize_coordinates(1.0f, 1.0f);
	float *high_b = initialize_coordinates(5.0f, 3.0f);

	rectangle_a = rtree_rectangle_create(low_a, high_a, dim);
	rectangle_b = rtree_rectangle_create(low_b, high_b, dim);

	rectangle_copy(rectangle_b, rectangle_a);
	for (index = 0; index < dim; index++) {
		assert_true(rectangle_a->low[index] == rectangle_b->low[index]);
		assert_true(rectangle_a->high[index] == rectangle_b->high[index]);
	}

	rtree_rectangle_destroy(rectangle_a);
	rtree_rectangle_destroy(rectangle_b);
}

static float * initialize_coordinates(float x, float y) 
{
	float *coords = malloc(2 * sizeof(float));

	coords[0] = x;
	coords[1] = y;

	return coords;
}

int test_rectangle(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(_test_rectangle_area),
		cmocka_unit_test(_test_rectangle_create),
		cmocka_unit_test(_test_rectangle_combine),
		cmocka_unit_test(_test_rectangle_extend_infinitely),
		cmocka_unit_test(_test_rectangle_overlaps),
		cmocka_unit_test(_test_rectangle_min_distance),
		cmocka_unit_test(_test_rectangle_intersection_area),
		cmocka_unit_test(_test_rectangle_margin),
		cmocka_unit_test(_test_rectangle_margin_value),
		cmocka_unit_test(_test_rectangle_compare_low),
		cmocka_unit_test(_test_rectangle_compare_high),
		cmocka_unit_test(_test_rectangle_copy)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}