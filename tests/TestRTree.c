#include <stdarg.h>
#include <stddef.h>
#include <stdlib.h>
#include <setjmp.h>
#include <cmocka.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include "../src/Context.h"
#include "../src/Entry.h"
#include "../src/Node.h"
#include "../src/Rectangle.h"
#include "../src/RTree.h"
#include "../include/wbdrtree/wbdrtreeapi.h"
#include "TestRTree.h"

static rt_rect_t * create_rectangle_2d(float low_x, float low_y, float high_x, float high_y);
static float * initialize_coordinates(float x, float y);

/* TODO: Reenable tests once decided: RTree currently does not expose its internal structure for use in tests. */

/*
void _test_rtree_create() {
	rt_rtree_t *rtree;
	rt_ctx_t *context;
	rt_rect_t *rectangle;
	float *low = initialize_coordinates(0.0f, 0.0f);
	float *high = initialize_coordinates(2.0f, 2.0f);

	rectangle = rtree_rectangle_create(low, high, 2);
	context = rtree_context_create(4, 12, 2, 35, 4, rectangle);
	rtree = rtree_create(context);

	assert_ptr_equal(rtree->context, context);	
	assert_true(node_is_leaf(rtree->root));
	assert_true(node_is_root(rtree->root));


}

void _test_rtree_insert() {
	rt_rtree_t rtree;
	rt_ctx_t context;
	rt_rect_t space, MBR_1, MBR_2, MBR_3, MBR_4, MBR_5;
	rt_entry_t entry_1, entry_2, entry_3, entry_4, entry_5;
	uint8_t dimension = 2;
	int index, tuples[] = { 1, 2, 3, 4, 5 };
	float low[] = { 1, 1 }, high[] = { 25, 25 }, low_1[] = { 6, 3 }, high_1[] = { 8, 5 },
		low_2[] = { 9, 6 }, high_2[] = { 11, 8 }, low_3[] = { 12, 9 }, high_3[] = { 14, 11 },
		low_4[] = { 15, 12 }, high_4[] = { 17, 14 }, low_5[] = { 18, 15 }, high_5[] = { 20, 17 };

	rtree_rectangle_create(&space, low, high, dimension);
	rtree_context_create(&context, 2, 4, dimension, 35, 4, &space);
	rtree_create(&rtree, &context);
	
	rtree_rectangle_create(&MBR_1, low_1, high_1, dimension);
	rtree_entry_create(&entry_1, &tuples[0], &MBR_1);
	
	rtree_insert(&rtree, &entry_1);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.root->MBR->low[index] == low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_1[index]);
	}
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));

	rtree_rectangle_create(&MBR_2, low_2, high_2, dimension);
	rtree_entry_create(&entry_2, &tuples[1], &MBR_2);

	rtree_insert(&rtree, &entry_2);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.root->MBR->low[index] == low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_2[index]);
	}
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));

	rtree_rectangle_create(&MBR_3, low_3, high_3, dimension);
	rtree_entry_create(&entry_3, &tuples[2], &MBR_3);

	rtree_insert(&rtree, &entry_3);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.root->MBR->low[index] == low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_3[index]);
	}
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));

	rtree_rectangle_create(&MBR_4, low_4, high_4, dimension);
	rtree_entry_create(&entry_4, &tuples[3], &MBR_4);

	rtree_insert(&rtree, &entry_4);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.root->MBR->low[index] == low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_4[index]);
	}
	assert_true(node_is_leaf(rtree.root));
	assert_true(node_is_root(rtree.root));

	rtree_rectangle_create(&MBR_5, low_5, high_5, dimension);
	rtree_entry_create(&entry_5, &tuples[4], &MBR_5);

	rtree_insert(&rtree, &entry_5);

	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree.context->space->low[index] == low[index]);
		assert_true(rtree.context->space->high[index] == high[index]);
		assert_true(rtree.root->MBR->low[index] = low_1[index]);
		assert_true(rtree.root->MBR->high[index] == high_5[index]);
	}
	assert_ptr_equal(((rt_node_t *)rtree.root->entries[0])->parent, rtree.root);
	assert_ptr_equal(((rt_node_t *)rtree.root->entries[1])->parent, rtree.root);
	assert_true(((rt_node_t *)rtree.root->entries[0])->level == 0);
	assert_true(((rt_node_t *)rtree.root->entries[1])->level == 0);
	assert_true(rtree.root->level == 1);
} */

/*void _test_rtree_delete() {

}

void _test_rtree_destroy() {

}

void _test_rtree_search() {

}*/

void _test_rtree_try_insert() {
    rt_rtree_t *rtree;
    rt_ctx_t *context;
    rt_entry_t * optimal_entry;
    rt_rect_t *space, *MBR_1, *MBR_2, *MBR_3, *MBR_4, *MBR_5, *MBR_6, *MBR_7;
    rt_entry_t *entry_1, *entry_2, *entry_3, *entry_4, *entry_5, *entry_6, *entry_7;
    uint8_t dimension = 2, m = 2, M = 4;
    int index, tuples[] = { 1, 2, 3, 4, 5, 6, 7 };
    size_t entry_size = 35;
    float alloc_factor = 4.0f;

   	space = create_rectangle_2d(0.0f, 0.0f, 20.0f, 11.0f);
	context = rtree_context_create(m, M, dimension, entry_size, alloc_factor, space);
	rtree = rtree_create(context);

	MBR_1 = create_rectangle_2d(0.0f, 8.0f, 3.0f, 11.0f);
	entry_1 = rtree_entry_create(&tuples[0], MBR_1);
	rtree_insert(rtree, entry_1);

	MBR_2 = create_rectangle_2d(6.0f, 8.0f, 9.0f, 11.0f);
	entry_2 = rtree_entry_create(&tuples[1], MBR_2);
	rtree_insert(rtree, entry_2);

	MBR_3 = create_rectangle_2d(0.0f, 4.0f, 3.0f, 7.0f);
	entry_3 = rtree_entry_create(&tuples[2], MBR_3);
	rtree_insert(rtree, entry_3);

	MBR_4 = create_rectangle_2d(6.0f, 4.0f, 9.0f, 7.0f);
	entry_4 = rtree_entry_create(&tuples[3], MBR_4);
	rtree_insert(rtree, entry_4);

	MBR_5 = create_rectangle_2d(0.0f, 0.0f, 3.0f, 3.0f);
	entry_5 = rtree_entry_create(&tuples[4], MBR_5);
	rtree_insert(rtree, entry_5);

	MBR_6 = create_rectangle_2d(6.0f, 0.0f, 9.0f, 3.0f);
	entry_6 = rtree_entry_create(&tuples[5], MBR_6);
	rtree_insert(rtree, entry_6);

	MBR_7 = create_rectangle_2d(12.0f, 8.0f, 15.0f, 11.0f);
	entry_7 = rtree_entry_create(&tuples[6], MBR_7);
	optimal_entry = rtree_try_insert(rtree, entry_7);

	for (index = 0; index < dimension; index++)
	{
	   assert_true(optimal_entry->MBR->low[index] == entry_2->MBR->low[index]);
	   assert_true(optimal_entry->MBR->high[index] == entry_2->MBR->high[index]);
	}

	rtree_destroy(rtree);
	rtree_entry_destroy(optimal_entry);
}

void _test_rtree_split() {
    rt_rtree_t *rtree, *other;
    rt_ctx_t *context;
 	rt_rect_t *space, *MBR_1, *MBR_2, *MBR_3, *MBR_4, *MBR_5;
    rt_entry_t *entry_1, *entry_2, *entry_3, *entry_4, *entry_5;
    uint8_t dimension = 2, m = 2, M = 4;
    int index, tuples[] = { 1, 2, 3, 4, 5};
    size_t entry_size = 35;
    float alloc_factor = 4.0f;

    space = create_rectangle_2d(0.0f, 0.0f, 20.0f, 11.0f);
	context = rtree_context_create(m, M, dimension, entry_size, alloc_factor, space);
	rtree = rtree_create(context);

	MBR_1 = create_rectangle_2d(0.0f, 8.0f, 3.0f, 11.0f);
	entry_1 = rtree_entry_create(&tuples[0], MBR_1);
	rtree_insert(rtree, entry_1);

	MBR_2 = create_rectangle_2d(6.0f, 8.0f, 9.0f, 11.0f);
	entry_2 = rtree_entry_create(&tuples[1], MBR_2);
	rtree_insert(rtree, entry_2);

	MBR_3 = create_rectangle_2d(0.0f, 4.0f, 3.0f, 7.0f);
	entry_3 = rtree_entry_create(&tuples[2], MBR_3);
	rtree_insert(rtree, entry_3);

	MBR_4 = create_rectangle_2d(6.0f, 4.0f, 9.0f, 7.0f);
	entry_4 = rtree_entry_create(&tuples[3], MBR_4);
	rtree_insert(rtree, entry_4);

	MBR_5 = create_rectangle_2d(0.0f, 0.0f, 3.0f, 3.0f);
	entry_5 = rtree_entry_create(&tuples[4], MBR_5);
	rtree_insert(rtree, entry_5);

	other = rtree_split(rtree);

    assert_ptr_equal(context, rtree->context);

    assert_true(other->context->dim == context->dim);
    assert_true(other->context->m == context->m);
    assert_true(other->context->M == context->M);
    assert_true(other->context->alloc_factor == context->alloc_factor);
    assert_true(other->context->entry_size == context->entry_size);

    for (index = 0; index < dimension; index++)
    {
        assert_true(other->context->space->low[index] == context->space->low[index]);
        assert_true(other->context->space->high[index] == context->space->high[index]);
    }

    assert_true(rtree->root->MBR->low[0] == 0);
    assert_true(rtree->root->MBR->low[1] == 0);
    assert_true(rtree->root->MBR->high[0] == 3);
    assert_true(rtree->root->MBR->high[1] == 11);

    assert_true(other->root->MBR->low[0] == 6);
    assert_true(other->root->MBR->low[1] == 4);
    assert_true(other->root->MBR->high[0] == 9);
    assert_true(other->root->MBR->high[1] == 11);

    rtree_destroy(rtree);
    rtree_destroy(other);
}

static rt_rect_t * create_rectangle_2d(float low_x, float low_y, float high_x, float high_y)
{
 float *low = initialize_coordinates(low_x, low_y);
 float *high = initialize_coordinates(high_x, high_y);

 return rtree_rectangle_create(low, high, 2);
}

static float * initialize_coordinates(float x, float y)
{
 float *coords = malloc(2 * sizeof(float));

 coords[0] = x;
 coords[1] = y;

 return coords;
}
int test_rtree(void) {
	const struct CMUnitTest tests[] = {
		/*cmocka_unit_test(_test_rtree_create),
		cmocka_unit_test(_test_rtree_insert)*/
		cmocka_unit_test(_test_rtree_try_insert),
		cmocka_unit_test(_test_rtree_split)
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}