#include "TestRTree.h"
#include <stdarg.h>
#include <stddef.h>
#ifdef DEBUG
#include <stdio.h>
#endif
#include <stdlib.h>
#include <setjmp.h>
#include <string.h>
#include <cmocka.h>
#include "../src/hashset.h"
#include "../src/Context.h"
#include "../src/Entry.h"
#include "../src/Node.h"
#include "../src/Rectangle.h"
#include "../src/RTree.h"
#include "../include/wbrtree/wbrtreeapi.h"

static rt_rect_t * create_rectangle_2d(float low_x, float low_y, float high_x, float high_y);
static float * initialize_coordinates(float x, float y);
static size_t serializer(rt_entry_t *entry, uint8_t **buffer);

void _test_rtree_create() {
	rt_rtree_t *rtree;
	rt_ctx_t *context;
	rt_rect_t *rectangle;
	uint8_t dim = 2, m = 4, M = 12;
	float alloc_factor = 4;
	float *low = initialize_coordinates(0.0f, 0.0f);
	float *high = initialize_coordinates(2.0f, 2.0f);

	rectangle = rtree_rectangle_create(low, high, 2);
	context = rtree_context_create(m, M, dim, serializer, alloc_factor, rectangle);
	rtree = rtree_create(context);

	assert_ptr_equal(rtree->context, context);	
	assert_true(node_is_leaf(rtree->root));
	assert_true(node_is_root(rtree->root));

	rtree_destroy(&rtree);
}

void _test_rtree_insert() {
	rt_rtree_t *rtree;
	rt_ctx_t *context;
	rt_rect_t *space, *MBR_1, *MBR_2, *MBR_3, *MBR_4, *MBR_5;
	rt_entry_t *entry_1, *entry_2, *entry_3, *entry_4, *entry_5;
	uint8_t dimension = 2, m = 2, M = 4;
	float alloc_factor = 4.0f;
	int index, tuples[] = { 1, 2, 3, 4, 5 };	

	space = create_rectangle_2d(1.0f, 1.0f, 25.0f, 25.0f);
	context = rtree_context_create(m, M, dimension, serializer, alloc_factor, space);
	rtree = rtree_create(context);
	
	MBR_1 = create_rectangle_2d(6.0f, 3.0f, 8.0f, 5.0f);
	entry_1 = rtree_entry_create(&tuples[0], MBR_1);
	
	rtree_insert(rtree, entry_1);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree->root->MBR->low[index] == MBR_1->low[index]);
		assert_true(rtree->root->MBR->high[index] == MBR_1->high[index]);
	}
	assert_true(node_is_leaf(rtree->root));
	assert_true(node_is_root(rtree->root));

	MBR_2 = create_rectangle_2d(9.0f, 6.0f, 11.0f, 8.0f);
	entry_2 = rtree_entry_create(&tuples[1], MBR_2);

	rtree_insert(rtree, entry_2);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree->root->MBR->low[index] == MBR_1->low[index]);
		assert_true(rtree->root->MBR->high[index] == MBR_2->high[index]);
	}
	assert_true(node_is_leaf(rtree->root));
	assert_true(node_is_root(rtree->root));

	MBR_3 = create_rectangle_2d(12.0f, 9.0f, 14.0f, 11.0f);
	entry_3 = rtree_entry_create(&tuples[2], MBR_3);

	rtree_insert(rtree, entry_3);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree->root->MBR->low[index] == MBR_1->low[index]);
		assert_true(rtree->root->MBR->high[index] == MBR_3->high[index]);
	}
	assert_true(node_is_leaf(rtree->root));
	assert_true(node_is_root(rtree->root));

	MBR_4 = create_rectangle_2d(15.0f, 12.0f, 17.0f, 14.0f);
	entry_4 = rtree_entry_create(&tuples[3], MBR_4);

	rtree_insert(rtree, entry_4);
	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree->root->MBR->low[index] == MBR_1->low[index]);
		assert_true(rtree->root->MBR->high[index] == MBR_4->high[index]);
	}
	assert_true(node_is_leaf(rtree->root));
	assert_true(node_is_root(rtree->root));

	MBR_5 = create_rectangle_2d(18.0f, 15.0f, 20.0f, 17.0f);
	entry_5 = rtree_entry_create(&tuples[4], MBR_5);

	rtree_insert(rtree, entry_5);

	for (index = 0; index < dimension; index++)
	{
		assert_true(rtree->context->space->low[index] == space->low[index]);
		assert_true(rtree->context->space->high[index] == space->high[index]);
		assert_true(rtree->root->MBR->low[index] = MBR_1->low[index]);
		assert_true(rtree->root->MBR->high[index] == MBR_5->high[index]);
	}
	assert_ptr_equal(((rt_node_t *)rtree->root->entries[0])->parent, rtree->root);
	assert_ptr_equal(((rt_node_t *)rtree->root->entries[1])->parent, rtree->root);
	assert_true(((rt_node_t *)rtree->root->entries[0])->level == 0);
	assert_true(((rt_node_t *)rtree->root->entries[1])->level == 0);
	assert_true(rtree->root->level == 1);

	rtree_destroy(&rtree);
}

void _test_rtree_delete() {
	rt_rtree_t *rtree;
    rt_ctx_t *context;
 	rt_rect_t *space, *MBR_1, *MBR_2, *MBR_3, *MBR_4, *MBR_5, *MBR_6;
    rt_entry_t *entry_1, *entry_2, *entry_3, *entry_4, *entry_5, *entry_6;
    uint8_t dimension = 2, m = 2, M = 4;
    int index, tuples[] = { 1, 2, 3, 4, 5, 6};
    float alloc_factor = 4.0f;

    space = create_rectangle_2d(0.0f, 0.0f, 20.0f, 11.0f);
	context = rtree_context_create(m, M, dimension, serializer, alloc_factor, space);
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

	for (index = 0; index < dimension; index++)
	{
	   assert_true(rtree->root->MBR->low[index] == MBR_5->low[index]);
	   assert_true(rtree->root->MBR->high[index] == MBR_2->high[index]);

   	   assert_true(((rt_node_t *)rtree->root->entries[0])->MBR->low[index] == MBR_5->low[index]);
	   assert_true(((rt_node_t *)rtree->root->entries[0])->MBR->high[index] == MBR_1->high[index]);

	   assert_true(((rt_node_t *)rtree->root->entries[1])->MBR->low[index] == MBR_6->low[index]);
	   assert_true(((rt_node_t *)rtree->root->entries[1])->MBR->high[index] == MBR_2->high[index]);
	}

	rtree_delete(rtree, entry_6);

	for (index = 0; index < dimension; index++)
	{
	   assert_true(rtree->root->MBR->low[index] == MBR_5->low[index]);
	   assert_true(rtree->root->MBR->high[index] == MBR_2->high[index]);

   	   assert_true(((rt_node_t *)rtree->root->entries[0])->MBR->low[index] == MBR_5->low[index]);
	   assert_true(((rt_node_t *)rtree->root->entries[0])->MBR->high[index] == MBR_1->high[index]);

	   assert_true(((rt_node_t *)rtree->root->entries[1])->MBR->low[index] == MBR_4->low[index]);
	   assert_true(((rt_node_t *)rtree->root->entries[1])->MBR->high[index] == MBR_2->high[index]);
	}

	rtree_delete(rtree, entry_5);

	for (index = 0; index < dimension; index++)
	{
	   assert_true(rtree->root->MBR->low[index] == MBR_3->low[index]);
	   assert_true(rtree->root->MBR->high[index] == MBR_2->high[index]);

   	   assert_true(((rt_node_t *)rtree->root->entries[0])->MBR->low[index] == MBR_3->low[index]);
	   assert_true(((rt_node_t *)rtree->root->entries[0])->MBR->high[index] == MBR_1->high[index]);

	   assert_true(((rt_node_t *)rtree->root->entries[1])->MBR->low[index] == MBR_4->low[index]);
	   assert_true(((rt_node_t *)rtree->root->entries[1])->MBR->high[index] == MBR_2->high[index]);
	}
	
	rtree_delete(rtree, entry_4);

	for (index = 0; index < dimension; index++)
	{
	   assert_true(rtree->root->MBR->low[index] == MBR_3->low[index]);
	   assert_true(rtree->root->MBR->high[index] == MBR_2->high[index]);

	   assert_true(node_is_leaf(rtree->root));   	   
	}

	rtree_destroy(&rtree);
}

void _test_rtree_destroy() {
	rt_rtree_t *rtree;
	rt_ctx_t *context;
	rt_rect_t *space;	
	uint8_t dimension = 2, m = 2, M = 4;
	float alloc_factor = 4.0f;	

	space = create_rectangle_2d(1.0f, 1.0f, 25.0f, 25.0f);
	context = rtree_context_create(m, M, dimension, serializer, alloc_factor, space);
	rtree = rtree_create(context);	

	rtree_destroy(&rtree);
	assert_null(rtree);
}

void _test_rtree_search() {
	rt_rtree_t *rtree;
    rt_ctx_t *context;
 	rt_rect_t *space, *MBRs[5], *search_MBRs[3];
    rt_entry_t *entries[5];
    uint8_t dimension = 2, m = 2, M = 4;
    int index, tuples[5];
    float alloc_factor = 4.0f;
    rt_hset_t *results[3];

    space = create_rectangle_2d(0.0f, 0.0f, 20.0f, 11.0f);
	context = rtree_context_create(m, M, dimension, serializer, alloc_factor, space);
	rtree = rtree_create(context);

	MBRs[0] = create_rectangle_2d(0.0f, 8.0f, 3.0f, 11.0f);
	MBRs[1] = create_rectangle_2d(6.0f, 8.0f, 9.0f, 11.0f);
	MBRs[2] = create_rectangle_2d(0.0f, 4.0f, 3.0f, 7.0f);
	MBRs[3] = create_rectangle_2d(6.0f, 4.0f, 9.0f, 7.0f);
	MBRs[4] = create_rectangle_2d(0.0f, 0.0f, 3.0f, 3.0f);

	search_MBRs[0] = create_rectangle_2d(2.0f, 5.0f, 7.0f, 6.0f);
	search_MBRs[1] = create_rectangle_2d(0.0f, 4.0f, 9.0f, 11.0f);
	search_MBRs[2] = create_rectangle_2d(0.0f, 2.0f, 3.0f, 5.0f);

	for (index = 0; index < 5; index++)
	{
		tuples[index] = index + 1;
		entries[index] = rtree_entry_create(&tuples[index], MBRs[index]);
		rtree_insert(rtree, entries[index]); 
	}

	for (index = 0; index < 3; index++)
	{
		results[index] = hashset_create();
	}

	rtree_search(rtree, search_MBRs[0], results[0]);
	assert_true(hashset_is_member(results[0], entries[2]));
	assert_true(hashset_is_member(results[0], entries[3]));

	rtree_search(rtree, search_MBRs[1], results[1]);
	assert_true(hashset_is_member(results[1], entries[0]));
	assert_true(hashset_is_member(results[1], entries[1]));
	assert_true(hashset_is_member(results[1], entries[2]));
	assert_true(hashset_is_member(results[1], entries[3]));
	assert_false(hashset_is_member(results[1], entries[4]));

	rtree_search(rtree, search_MBRs[2], results[2]);
	assert_true(hashset_is_member(results[2], entries[2]));
	assert_true(hashset_is_member(results[2], entries[4]));
	assert_false(hashset_is_member(results[2], entries[0]));
	assert_false(hashset_is_member(results[2], entries[1]));
	assert_false(hashset_is_member(results[2], entries[3]));

	for (index = 0; index < 3; index++)
	{
		rtree_rectangle_destroy(&(search_MBRs[index]));
		hashset_destroy(results[index]);
	}
	
	rtree_destroy(&rtree);
}

void _test_rtree_try_insert() {
    rt_rtree_t *rtree;
    rt_ctx_t *context;
    rt_entry_t * optimal_entry;
    rt_rect_t *space, *MBR_1, *MBR_2, *MBR_3, *MBR_4, *MBR_5, *MBR_6, *MBR_7;
    rt_entry_t *entry_1, *entry_2, *entry_3, *entry_4, *entry_5, *entry_6, *entry_7;
    uint8_t dimension = 2, m = 2, M = 4;
    int index, tuples[] = { 1, 2, 3, 4, 5, 6, 7 };
    float alloc_factor = 4.0f;

   	space = create_rectangle_2d(0.0f, 0.0f, 20.0f, 11.0f);
	context = rtree_context_create(m, M, dimension, serializer, alloc_factor, space);
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
	optimal_entry = try_insert(rtree, entry_7);

	for (index = 0; index < dimension; index++)
	{
	   assert_true(optimal_entry->MBR->low[index] == entry_2->MBR->low[index]);
	   assert_true(optimal_entry->MBR->high[index] == entry_2->MBR->high[index]);
	}

	rtree_destroy(&rtree);
}

void _test_rtree_split() {
    rt_rtree_t *rtree, *other;
    rt_ctx_t *context;
 	rt_rect_t *space, *MBR_1, *MBR_2, *MBR_3, *MBR_4, *MBR_5;
    rt_entry_t *entry_1, *entry_2, *entry_3, *entry_4, *entry_5;
    uint8_t dimension = 2, m = 2, M = 4;
    int index, tuples[] = { 1, 2, 3, 4, 5};
    float alloc_factor = 4.0f;

    space = create_rectangle_2d(0.0f, 0.0f, 20.0f, 11.0f);
	context = rtree_context_create(m, M, dimension, serializer, alloc_factor, space);
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
    assert_ptr_equal(other->context->serializer, context->serializer);

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

    rtree_destroy(&rtree);
    rtree_destroy(&other);
}

void _test_rtree_serialize() {
	rt_rtree_t *rtree;
    rt_ctx_t *context;
 	rt_rect_t *space, *MBRs[5];
    rt_entry_t *entries[5];
    uint8_t dimension = 2, m = 2, M = 4, *buffer;
    int index, tuples[5];
    float alloc_factor = 4.0f;    

    space = create_rectangle_2d(0.0f, 0.0f, 20.0f, 11.0f);
	context = rtree_context_create(m, M, dimension, serializer, alloc_factor, space);
	rtree = rtree_create(context);

	MBRs[0] = create_rectangle_2d(0.0f, 8.0f, 3.0f, 11.0f);
	MBRs[1] = create_rectangle_2d(6.0f, 8.0f, 9.0f, 11.0f);
	MBRs[2] = create_rectangle_2d(0.0f, 4.0f, 3.0f, 7.0f);
	MBRs[3] = create_rectangle_2d(6.0f, 4.0f, 9.0f, 7.0f);
	MBRs[4] = create_rectangle_2d(0.0f, 0.0f, 3.0f, 3.0f);	

	for (index = 0; index < 5; index++)
	{
		tuples[index] = index + 1;
		entries[index] = rtree_entry_create(&tuples[index], MBRs[index]);
		rtree_insert(rtree, entries[index]); 
	}

	rtree_serialize(rtree, &buffer);
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

static size_t serializer(rt_entry_t *entry, uint8_t **buffer)
{
	size_t index = 0;	

	*buffer = malloc(2 * sizeof(float) + sizeof(uint8_t));
	memmove(&(*buffer[index]), entry->MBR->low, 2 * sizeof(float));
	index += 2 * sizeof(float);	
	memmove(&(*buffer[index]), &entry->MBR->dim, sizeof(uint8_t));
	index += sizeof(uint8_t);

	return index;
}

int test_rtree(void) {
	const struct CMUnitTest tests[] = {
		cmocka_unit_test(_test_rtree_create),
		cmocka_unit_test(_test_rtree_insert),
		cmocka_unit_test(_test_rtree_try_insert),
		cmocka_unit_test(_test_rtree_split),
		cmocka_unit_test(_test_rtree_delete),
		cmocka_unit_test(_test_rtree_search),
		cmocka_unit_test(_test_rtree_destroy),
		/*cmocka_unit_test(_test_rtree_serialize)*/
	};

	return cmocka_run_group_tests(tests, NULL, NULL);
}