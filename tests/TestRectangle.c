#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <cmocka.h>

#include <float.h>
#include <stdlib.h>
#include "Helpers.h"
#include "Rectangle.h"
#include "TestRectangle.h"

void _test_point_create() {
	int dim = 2;
	struct Point *point = malloc(sizeof(struct Point));
	float coords[] = { 0, 0 };
	
	point_create(point, dim, coords);

	assert_int_equal(dim, point->dim);
	assert_ptr_equal(coords, point->coords);
			
	free(point);	
}

void _test_rectangle_create() {
	struct Rectangle *rectangle = malloc(sizeof(struct Rectangle));
	
	int dim = 2;
	struct Point *low = malloc(sizeof(struct Point));
	struct Point *high = malloc(sizeof(struct Point));
	float coords_low[] = {0, 0};
	float coords_high[] = {2, 2};

	point_create(low, dim, coords_low);
	point_create(high, dim, coords_high);
	

	rectangle_create(rectangle, low, high);
	assert_ptr_equal(rectangle->low, low);
	assert_ptr_equal(rectangle->high, high);
	assert_int_equal(rectangle->dim, dim);	
		
	free(low);
	free(high);
	free(rectangle);
}

void _test_rectangle_area() {
	struct Rectangle *rectangle = malloc(sizeof(struct Rectangle));

	int dim = 2;
	struct Point *low = malloc(sizeof(struct Point));
	struct Point *high = malloc(sizeof(struct Point));
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 2 };

	point_create(low, dim, coords_low);
	point_create(high, dim, coords_high);
	
	rectangle_create(rectangle, low, high);
	assert_true(rectangle_area(rectangle) == 4.0);

	free(low);
	free(high);
	free(rectangle);
}

void _test_rectangle_combine_1() {
	int dim = 2;
	// Create rectangle A
	struct Rectangle *rectangle_a = malloc(sizeof(struct Rectangle));
	struct Point *low_a = malloc(sizeof(struct Point));
	struct Point *high_a = malloc(sizeof(struct Point));
	float coords_low_a[] = { 0, 0 };
	float coords_high_a[] = { 2, 2 };

	point_create(low_a, dim, coords_low_a);
	point_create(high_a, dim, coords_high_a);

	rectangle_create(rectangle_a, low_a, high_a);

	// Create rectangle B	
	struct Rectangle *rectangle_b = malloc(sizeof(struct Rectangle));
	struct Point *low_b = malloc(sizeof(struct Point));
	struct Point *high_b = malloc(sizeof(struct Point));
	float coords_low_b[] = { 1, 1 };
	float coords_high_b[] = { 3, 3 };

	point_create(low_b, dim, coords_low_b);
	point_create(high_b, dim, coords_high_b);

	rectangle_create(rectangle_b, low_b, high_b);
	
	// Test rectangle_combine
	rectangle_combine(rectangle_a, rectangle_b);
	assert_int_equal(rectangle_a->dim, dim);
	for (int i = 0; i < dim; i++) {
		assert_ptr_equal(rectangle_a->low->coords[i], low_a->coords[i]);		
		assert_ptr_equal(rectangle_a->high->coords[i], high_b->coords[i]);
	}	

	free(low_a);
	free(high_a);
	free(rectangle_a);

	free(low_b);
	free(high_b);
	free(rectangle_b);
}

void _test_rectangle_combine_2() {
	int dim = 2;
	// Create rectangle A
	struct Rectangle *rectangle_a = malloc(sizeof(struct Rectangle));
	struct Point *low_a = malloc(sizeof(struct Point));
	struct Point *high_a = malloc(sizeof(struct Point));
	float coords_low_a[] = { 1, 1 };
	float coords_high_a[] = { 3, 3 };

	point_create(low_a, dim, coords_low_a);
	point_create(high_a, dim, coords_high_a);

	rectangle_create(rectangle_a, low_a, high_a);

	// Create rectangle B	
	struct Rectangle *rectangle_b = malloc(sizeof(struct Rectangle));
	struct Point *low_b = malloc(sizeof(struct Point));
	struct Point *high_b = malloc(sizeof(struct Point));
	float coords_low_b[] = { 1, 0 };
	float coords_high_b[] = { 4, 4 };

	point_create(low_b, dim, coords_low_b);
	point_create(high_b, dim, coords_high_b);

	rectangle_create(rectangle_b, low_b, high_b);

	// Test rectangle_combine
	rectangle_combine(rectangle_a, rectangle_b);
	assert_int_equal(rectangle_a->dim, dim);
	assert_true(rectangle_a->low->coords[0] == 1);
	assert_true(rectangle_a->low->coords[1] == 0);
	assert_true(rectangle_a->high->coords[0] == 4);
	assert_true(rectangle_a->high->coords[1] == 4);

	free(low_a);
	free(high_a);
	free(rectangle_a);

	free(low_b);
	free(high_b);
	free(rectangle_b);
}

void _test_rectangle_extend_infinitely() {
	struct Rectangle *rectangle = malloc(sizeof(struct Rectangle));

	int dim = 2;
	struct Point *low = malloc(sizeof(struct Point));
	struct Point *high = malloc(sizeof(struct Point));
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 2 };

	point_create(low, dim, coords_low);
	point_create(high, dim, coords_high);

	rectangle_create(rectangle, low, high);

	rectangle_extend_infinitely(rectangle);
	assert_int_equal(rectangle->dim, dim);
	
	for (int i = 0; i < dim; i++) {
		assert_true(rectangle->low->coords[i] == -FLT_MAX);
		assert_true(rectangle->high->coords[i] == FLT_MAX);
	}

	free(low);
	free(high);
	free(rectangle);
}

void _test_rectangle_min_distance() {
	int dim = 2;
	// Create rectangle A
	struct Rectangle *rectangle_a = malloc(sizeof(struct Rectangle));
	struct Point *low_a = malloc(sizeof(struct Point));
	struct Point *high_a = malloc(sizeof(struct Point));
	float coords_low_a[] = { 0, 0 };
	float coords_high_a[] = { 2, 2 };

	point_create(low_a, dim, coords_low_a);
	point_create(high_a, dim, coords_high_a);

	rectangle_create(rectangle_a, low_a, high_a);

	// Create rectangle B	
	struct Rectangle *rectangle_b = malloc(sizeof(struct Rectangle));
	struct Point *low_b = malloc(sizeof(struct Point));
	struct Point *high_b = malloc(sizeof(struct Point));
	float coords_low_b[] = { 5, 6 };
	float coords_high_b[] = { 8, 8 };

	point_create(low_b, dim, coords_low_b);
	point_create(high_b, dim, coords_high_b);

	rectangle_create(rectangle_b, low_b, high_b);

	// Test rectangle_min_distance
	assert_int_equal(rectangle_min_distance(rectangle_a, rectangle_b), 5);
	assert_int_equal(rectangle_min_distance(rectangle_b, rectangle_a), 5);

	free(low_a);
	free(high_a);
	free(rectangle_a);

	free(low_b);
	free(high_b);
	free(rectangle_b);
}

void _test_rectangle_overlaps() {
	int dim = 2;
	// Create rectangle A
	struct Rectangle *rectangle_a = malloc(sizeof(struct Rectangle));
	struct Point *low_a = malloc(sizeof(struct Point));
	struct Point *high_a = malloc(sizeof(struct Point));
	float coords_low_a[] = { 0, 0 };
	float coords_high_a[] = { 2, 2 };

	point_create(low_a, dim, coords_low_a);
	point_create(high_a, dim, coords_high_a);

	rectangle_create(rectangle_a, low_a, high_a);

	// Create rectangle B	
	struct Rectangle *rectangle_b = malloc(sizeof(struct Rectangle));
	struct Point *low_b = malloc(sizeof(struct Point));
	struct Point *high_b = malloc(sizeof(struct Point));
	float coords_low_b[2] = { 1, 1 };
	float coords_high_b[2] = { 3, 3 };

	point_create(low_b, dim, coords_low_b);
	point_create(high_b, dim, coords_high_b);

	rectangle_create(rectangle_b, low_b, high_b);

	// Create rectangle C	
	struct Rectangle *rectangle_c = malloc(sizeof(struct Rectangle));
	struct Point *low_c = malloc(sizeof(struct Point));
	struct Point *high_c = malloc(sizeof(struct Point));
	float coords_low_c[2] = { 3, 3 };
	float coords_high_c[2] = { 5, 5 };

	point_create(low_c, dim, coords_low_c);
	point_create(high_c, dim, coords_high_c);

	rectangle_create(rectangle_c, low_c, high_c);

	// Test rectangle_overlap
	assert_true(rectangle_overlaps(rectangle_a, rectangle_b));
	assert_false(rectangle_overlaps(rectangle_a, rectangle_c));

	free(low_a);
	free(high_a);
	free(rectangle_a);

	free(low_b);
	free(high_b);
	free(rectangle_b);

	free(low_c);
	free(high_c);
	free(rectangle_c);
}

void _test_rectangle_compare() {
	// Create rectangle A
	float coords_low_a[] = { 1, 2 };
	float coords_high_a[] = { 3, 4 };
	struct Rectangle *rectangle_a = create_2d_rectangle(coords_low_a, coords_high_a);

	// Create rectangle B	
	float coords_low_b[] = { 2, 1 };
	float coords_high_b[] = { 4, 3 };
	struct Rectangle *rectangle_b = create_2d_rectangle(coords_low_b, coords_high_b);
	
	// Test rectangle_compare
	uint8_t dim[] = { 0, 1 };

	assert_true(rectangle_compare(rectangle_a, rectangle_b, &dim[0]) < 0);
	assert_true(rectangle_compare(rectangle_a, rectangle_b, &dim[1]) > 0);
	assert_true(rectangle_compare(rectangle_b, rectangle_a, &dim[0]) > 0);
	assert_true(rectangle_compare(rectangle_b, rectangle_a, &dim[1]) < 0);
	assert_true(rectangle_compare(rectangle_b, rectangle_b, &dim[0]) == 0);
	assert_true(rectangle_compare(rectangle_b, rectangle_b, &dim[1]) == 0);

	destroy_rectangle(rectangle_a);
	destroy_rectangle(rectangle_b);	
}

void _test_rectangle_intersection_area() {
	int dim = 2;
	// Create rectangle A
	struct Rectangle *rectangle_a = malloc(sizeof(struct Rectangle));
	struct Point *low_a = malloc(sizeof(struct Point));
	struct Point *high_a = malloc(sizeof(struct Point));
	float coords_low_a[] = { 0, 0 };
	float coords_high_a[] = { 2, 2 };

	point_create(low_a, dim, coords_low_a);
	point_create(high_a, dim, coords_high_a);

	rectangle_create(rectangle_a, low_a, high_a);

	// Create rectangle B	
	struct Rectangle *rectangle_b = malloc(sizeof(struct Rectangle));
	struct Point *low_b = malloc(sizeof(struct Point));
	struct Point *high_b = malloc(sizeof(struct Point));
	float coords_low_b[2] = { 1, 1 };
	float coords_high_b[2] = { 3, 3 };

	point_create(low_b, dim, coords_low_b);
	point_create(high_b, dim, coords_high_b);

	rectangle_create(rectangle_b, low_b, high_b);

	// Test rectangle_intersection_area	
	assert_int_equal(rectangle_intersection_area(rectangle_a, rectangle_b), 1);
	assert_int_equal(rectangle_intersection_area(rectangle_b, rectangle_a), 1);

	free(low_a);
	free(high_a);
	free(rectangle_a);

	free(low_b);
	free(high_b);
	free(rectangle_b);
}

void _test_rectangle_margin() {
	struct Rectangle *rectangle = malloc(sizeof(struct Rectangle));

	int dim = 2;
	struct Point *low = malloc(sizeof(struct Point));
	struct Point *high = malloc(sizeof(struct Point));
	float coords_low[] = { 0, 0 };
	float coords_high[] = { 2, 3 };

	point_create(low, dim, coords_low);
	point_create(high, dim, coords_high);


	rectangle_create(rectangle, low, high);
	assert_int_equal(rectangle_margin(rectangle), 10);

	free(low);
	free(high);
	free(rectangle);
}

void _test_rectangle_margin_value() {
	int dim = 2;
	// Create rectangle A
	struct Rectangle *rectangle_a = malloc(sizeof(struct Rectangle));
	struct Point *low_a = malloc(sizeof(struct Point));
	struct Point *high_a = malloc(sizeof(struct Point));
	float coords_low_a[] = { 0, 0 };
	float coords_high_a[] = { 2, 2 };

	point_create(low_a, dim, coords_low_a);
	point_create(high_a, dim, coords_high_a);

	rectangle_create(rectangle_a, low_a, high_a);

	// Create rectangle B	
	struct Rectangle *rectangle_b = malloc(sizeof(struct Rectangle));
	struct Point *low_b = malloc(sizeof(struct Point));
	struct Point *high_b = malloc(sizeof(struct Point));
	float coords_low_b[2] = { 1, 1 };
	float coords_high_b[2] = { 5, 3 };

	point_create(low_b, dim, coords_low_b);
	point_create(high_b, dim, coords_high_b);

	rectangle_create(rectangle_b, low_b, high_b);
	
	// Test rectangle_margin_value
	assert_int_equal(rectangle_margin_value(rectangle_a, rectangle_b), 20);
	assert_int_equal(rectangle_margin_value(rectangle_b, rectangle_a), 20);

	free(low_a);
	free(high_a);
	free(rectangle_a);

	free(low_b);
	free(high_b);
	free(rectangle_b);
}

void _test_point_compare() {
	float coords_1[] = { 0, 1 };
	float coords_2[] = { 1, 0 };
	struct Point *point_1 = create_2d_point(coords_1);
	struct Point *point_2 = create_2d_point(coords_2);
	uint8_t dim[] = { 0, 1 };

	assert_true(point_compare(point_1, point_2, &dim[0]) < 0);
	assert_true(point_compare(point_1, point_2, &dim[1]) > 0);
	assert_true(point_compare(point_2, point_2, &dim[0]) == 0);
	assert_true(point_compare(point_2, point_2, &dim[1]) == 0);

	free(point_1);
	free(point_2);
}

int test_rectangle(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(_test_point_create),
		cmocka_unit_test(_test_rectangle_create),
		cmocka_unit_test(_test_rectangle_area),
		cmocka_unit_test(_test_rectangle_combine_1),
		cmocka_unit_test(_test_rectangle_combine_2),
		cmocka_unit_test(_test_rectangle_extend_infinitely),
		cmocka_unit_test(_test_rectangle_overlaps),
		cmocka_unit_test(_test_rectangle_min_distance),
		cmocka_unit_test(_test_point_compare),
		cmocka_unit_test(_test_rectangle_intersection_area),
		cmocka_unit_test(_test_rectangle_margin),
		cmocka_unit_test(_test_rectangle_margin_value),
		cmocka_unit_test(_test_rectangle_compare)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}