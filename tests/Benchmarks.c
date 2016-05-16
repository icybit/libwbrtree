#include <assert.h>
#include <float.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "Benchmarks.h"
#include "../src/Rectangle.h"
#include "../src/Entry.h"
#include "../src/RTree.h"
#include "../src/Context.h"

static rt_rect_t * create_rectangle_2d(float low_x, float low_y, float high_x, float high_y);
static float * initialize_coordinates(float x, float y);
static size_t serializer(rt_entry_t *entry, uint8_t **buffer);

void read_data(rt_coords_t *coords, int dataset_size)
{
	int index = 0;
	char *file_path = "GDCE_TEST_DATASET.txt";
	FILE *file;
	char *line = NULL;
	size_t length = 0;

	file = fopen(file_path, "r");

	while (getline(&line, &length, file) != -1 && index < dataset_size)
	{
		/*coords[index] = malloc(sizeof(rt_coords_t *));*/
		coords[index].lat = atof(strsep(&line, ","));
		coords[index].lng = atof(strsep(&line, ","));
		index++;
	}

	fclose(file);
	free(line);
}

void benchmark_rtree_insert(rt_coords_t *coords, rt_bench_params_t *params)
{
	const float step = 0.0001;
	
	int index, *tuples, inserts_count = 0;	
	float sum_time = 0.0f;	
	uint8_t dim = 2;
	rt_rect_t *space, **rectangles;
	rt_entry_t **entries;
	rt_rtree_t *rtree;
	rt_ctx_t *context;

	space = create_rectangle_2d(0.0f, 0.0f, 0.0f, 0.0f);
	context = context_create(params->m, params->M, dim, serializer, params->alloc_factor, space);
	rtree = create(context);

	rectangles = malloc(params->dataset_size * sizeof(rt_rect_t *));
	tuples = malloc(params->dataset_size * sizeof(int));
	entries = malloc(params->dataset_size * sizeof(rt_entry_t *));

	printf("Bechmarking rtree_insert operation:\n\n");
	
	for (index = 0; index < params->dataset_size; index++)
	{	 		
		float start_time, 
			  end_time, 
			  delta_time,
			  low_lat = coords[index].lat, 
			  low_lng = coords[index].lng,
			  high_lat = low_lat + (((float)(rand() % 10)) / 10000) + step, 
			  high_lng = low_lng + (((float)(rand() % 10)) / 10000) + step;
					  
		rectangles[index] = create_rectangle_2d(low_lat, low_lng, high_lat, high_lng);
		tuples[index] = index + 1;
		entries[index] = entry_create(&tuples[index], rectangles[index]);

		if (index >= params->dataset_size * 0.9 - 1)
		{
			start_time = (float)clock() / CLOCKS_PER_SEC;
			insert(rtree, entries[index]);		
			end_time = (float)clock() / CLOCKS_PER_SEC;
			delta_time = end_time - start_time;
			sum_time += delta_time;
			inserts_count++;
			printf("Insert time: %fms\n", delta_time * 1000);
		} else {
			insert(rtree, entries[index]);		
		}
		
	}

	printf("Average insert time: %f ms\n", (sum_time * 1000) / inserts_count);
	printf("Average inserts per second: %d\n", (int)(1000 / ((sum_time * 1000) / inserts_count)));
	

	destroy(&rtree);
	free(entries);
	free(rectangles);
	free(tuples);	
}

void params_initialize(rt_bench_params_t *dest, uint8_t m, uint8_t M, float alloc_factor, int dataset_size)
{
	assert(m > 1 && M > 3 && ((M / 2) >= m));

	dest->m = m;
	dest->M = M;
	dest->alloc_factor = (alloc_factor > 1.0f ? alloc_factor : 2.0f);
	dest->dataset_size = dataset_size;
}

void benchmarks_run(rt_bench_params_t *params)
{
	/*int index;*/
	rt_coords_t *coords = malloc(params->dataset_size * sizeof(rt_coords_t));
	read_data(coords, params->dataset_size);

	/*for (index = 0; index < params->dataset_size; index++)
	{
		printf("%s %d %f %f\n", "Index:", index, coords[index].lat, coords[index].lng);
	}*/

	benchmark_rtree_insert(coords, params);

	free(coords);
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
