#include <float.h>
#include <math.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#include "../include/Rectangle.h"
#include "TestRectangle.h"

void _test_rectangle_area() {
	rt_rect_t rectangle;
	uint8_t dimension = 2;
	float low[] = { 0.0f, 0.0f };
	float high[] = { 2.0f, 2.0f };

	rectangle_create(&rectangle, low, high, dimension);

	assert_true(fabs(rectangle_area(&rectangle) - 4.0) < DBL_EPSILON);
}

void _test_rectangle_create() {
	rt_rect_t rectangle;
	uint8_t index, dimension = 2;
	float low[] = {0.0f, 0.0f};
	float high[] = {2.0f, 2.0f};

	rectangle_create(&rectangle, low, high, dimension);

	for (index = 0; index < dimension; index++)
	{
		assert_true(rectangle.low[index] == low[index]);
		assert_true(rectangle.high[index] == high[index]);
	}
	assert_int_equal(rectangle.dim, dimension);	
}

void _test_rectangle_combine_1() {
	uint8_t index, dim = 2;

	rt_rect_t rectangle_a;	
	float low_a[] = { 0.0f, 0.0f };
	float high_a[] = { 2.0f, 2.0f };

	rt_rect_t rectangle_b;
	float low_b[] = { 1.0f, 1.0f };
	float high_b[] = { 3.0f, 3.0f };

	rectangle_create(&rectangle_a, low_a, high_a, dim);
	rectangle_create(&rectangle_b, low_b, high_b, dim);
		
	rectangle_combine(&rectangle_a, &rectangle_b);
	
	assert_int_equal(rectangle_a.dim, dim);
	for (index = 0; index < dim; index++) {
		assert_true(rectangle_a.low[index] == low_a[index]);
		assert_true(rectangle_a.high[index] == high_b[index]);
	}	
}

void _test_rectangle_combine_2() {
	uint8_t index, dim = 2;

	rt_rect_t rectangle_a;
	float low_a[] = { 1.0f, 1.0f };
	float high_a[] = { 3.0f, 3.0f };

	rt_rect_t rectangle_b;
	float low_b[] = { 1.0f, 0.0f };
	float high_b[] = { 4.0f, 4.0f };

	rectangle_create(&rectangle_a, low_a, high_a, dim);
	rectangle_create(&rectangle_b, low_b, high_b, dim);
	
	rectangle_combine(&rectangle_a, &rectangle_b);
	assert_int_equal(rectangle_a.dim, dim);
	for (index = 0; index < dim; index++) {
		assert_true(rectangle_a.low[index] == low_b[index]);
		assert_true(rectangle_a.high[index] == high_b[index]);
	}	
}

void _test_rectangle_extend_infinitely() {
	rt_rect_t rectangle;
	uint8_t index, dim = 2;
	float low[] = { 0.0f, 0.0f };
	float high[] = { 2.0f, 2.0f };

	rectangle_create(&rectangle, low, high, dim);	

	rectangle_extend_infinitely(&rectangle);
	assert_int_equal(rectangle.dim, dim);
	
	for (index = 0; index < dim; index++) {
		assert_true(rectangle.low[index] == FLT_MAX);
		assert_true(rectangle.high[index] == -FLT_MAX);
	}
}

void _test_rectangle_min_distance() {
	uint8_t dim = 2;

	rt_rect_t rectangle_a;
	float low_a[] = { 0.0f, 0.0f };
	float high_a[] = { 2.0f, 2.0f };

	rt_rect_t rectangle_b;
	float low_b[] = { 5.0f, 6.0f };
	float high_b[] = { 8.0f, 8.0f };

	rectangle_create(&rectangle_a, low_a, high_a, dim);
	rectangle_create(&rectangle_b, low_b, high_b, dim);
	
	assert_int_equal(rectangle_min_distance(&rectangle_a, &rectangle_b), 5);
	assert_int_equal(rectangle_min_distance(&rectangle_b, &rectangle_a), 5);
}

void _test_rectangle_overlaps() {
	uint8_t dim = 2;

	rt_rect_t rectangle_a;
	float low_a[] = { 0.0f, 0.0f };
	float high_a[] = { 2.0f, 2.0f };

	rt_rect_t rectangle_b;
	float low_b[] = { 1.0f, 1.0f };
	float high_b[] = { 3.0f, 3.0f };

	rt_rect_t rectangle_c;
	float low_c[] = { 3.0f, 3.0f };
	float high_c[] = { 5.0f, 5.0f };

	rectangle_create(&rectangle_a, low_a, high_a, dim);
	rectangle_create(&rectangle_b, low_b, high_b, dim);
	rectangle_create(&rectangle_c, low_c, high_c, dim);

	assert_true(rectangle_overlaps(&rectangle_a, &rectangle_b));
	assert_false(rectangle_overlaps(&rectangle_a, &rectangle_c));
}

void _test_rectangle_compare_low() {
	uint8_t dimensions = 2, dim[] = { 0, 1 };
	rt_rect_t rectangle_a, rectangle_b;
	float low_a[] = { 1.0f, 2.0f };
	float high_a[] = { 3.0f, 4.0f };
	float low_b[] = { 2.0f, 1.0f };
	float high_b[] = { 4.0f, 3.0f };

	rectangle_create(&rectangle_a, low_a, high_a, dimensions);
	rectangle_create(&rectangle_b, low_b, high_b, dimensions);
	
	assert_true(rectangle_compare_low(&rectangle_a, &rectangle_b, &dim[0]) < 0);
	assert_true(rectangle_compare_low(&rectangle_a, &rectangle_b, &dim[1]) > 0);
	assert_true(rectangle_compare_low(&rectangle_b, &rectangle_a, &dim[0]) > 0);
	assert_true(rectangle_compare_low(&rectangle_b, &rectangle_a, &dim[1]) < 0);
	assert_true(rectangle_compare_low(&rectangle_b, &rectangle_b, &dim[0]) == 0);
	assert_true(rectangle_compare_low(&rectangle_b, &rectangle_b, &dim[1]) == 0);
}

void _test_rectangle_compare_high() {
	uint8_t dimensions = 2, dim[] = { 0, 1 };
	rt_rect_t rectangle_a, rectangle_b;
	float low_a[] = { 1.0f, 2.0f };
	float high_a[] = { 3.0f, 4.0f };
	float low_b[] = { 2.0f, 1.0f };
	float high_b[] = { 4.0f, 3.0f };

	rectangle_create(&rectangle_a, low_a, high_a, dimensions);
	rectangle_create(&rectangle_b, low_b, high_b, dimensions);

	assert_true(rectangle_compare_high(&rectangle_a, &rectangle_b, &dim[0]) < 0);
	assert_true(rectangle_compare_high(&rectangle_a, &rectangle_b, &dim[1]) > 0);
	assert_true(rectangle_compare_high(&rectangle_b, &rectangle_a, &dim[0]) > 0);
	assert_true(rectangle_compare_high(&rectangle_b, &rectangle_a, &dim[1]) < 0);
	assert_true(rectangle_compare_high(&rectangle_b, &rectangle_b, &dim[0]) == 0);
	assert_true(rectangle_compare_high(&rectangle_b, &rectangle_b, &dim[1]) == 0);
}

void _test_rectangle_intersection_area() {
	uint8_t dim = 2;

	rt_rect_t rectangle_a;
	float low_a[] = { 0.0f, 0.0f };
	float high_a[] = { 2.0f, 2.0f };

	rt_rect_t rectangle_b;
	float low_b[] = { 1.0f, 1.0f };
	float high_b[] = { 3.0f, 3.0f };

	rectangle_create(&rectangle_a, low_a, high_a, dim);
	rectangle_create(&rectangle_b, low_b, high_b, dim);	

	assert_int_equal(rectangle_intersection_area(&rectangle_a, &rectangle_b), 1);
	assert_int_equal(rectangle_intersection_area(&rectangle_b, &rectangle_a), 1);
}

void _test_rectangle_margin() {
	rt_rect_t rectangle;
	uint8_t dim = 2;
	float low[] = { 0.0f, 0.0f };
	float high[] = { 2.0f, 3.0f };

	rectangle_create(&rectangle, low, high, dim);
	
	assert_int_equal(rectangle_margin(&rectangle), 10);
}

void _test_rectangle_margin_value() {
	uint8_t dim = 2;

	rt_rect_t rectangle_a;
	float low_a[] = { 0.0f, 0.0f };
	float high_a[] = { 2.0f, 2.0f };

	rt_rect_t rectangle_b;
	float low_b[] = { 1.0f, 1.0f };
	float high_b[] = { 5.0f, 3.0f };

	rectangle_create(&rectangle_a, low_a, high_a, dim);
	rectangle_create(&rectangle_b, low_b, high_b, dim);
		
	assert_int_equal(rectangle_margin_value(&rectangle_a, &rectangle_b), 20);
	assert_int_equal(rectangle_margin_value(&rectangle_b, &rectangle_a), 20);
}

void _test_rectangle_copy() {
	uint8_t index, dim = 2;

	rt_rect_t rectangle_a;
	float low_a[] = { 0.0f, 0.0f };
	float high_a[] = { 2.0f, 2.0f };

	rt_rect_t rectangle_b;	
	float low_b[] = { 1.0f, 1.0f };
	float high_b[] = { 5.0f, 3.0f };

	rectangle_create(&rectangle_a, low_a, high_a, dim);
	rectangle_create(&rectangle_b, low_b, high_b, dim);

	rectangle_copy(&rectangle_b, &rectangle_a);
	for (index = 0; index < dim; index++) {
		assert_true(rectangle_a.low[index] == rectangle_b.low[index]);
		assert_true(rectangle_a.high[index] == rectangle_b.high[index]);
	}
}

int test_rectangle(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(_test_rectangle_area),
		cmocka_unit_test(_test_rectangle_create),
		cmocka_unit_test(_test_rectangle_combine_1),
		cmocka_unit_test(_test_rectangle_combine_2),
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