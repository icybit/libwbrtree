#ifndef _RTREE_BENCHMARKS_H_
#define _RTREE_BENCHMARKS_H_

#include <stdint.h>
#include <stddef.h>

struct Coords
{
	float lat;
	float lng;
};

struct Params
{
	uint8_t m;
	uint8_t M;
	float alloc_factor;
	int dataset_size;
};

typedef struct Coords rt_coords_t;
typedef struct Params rt_bench_params_t;

/*void benchmark_rtree_delete();*/
void benchmark_rtree_insert(rt_coords_t *coords, rt_bench_params_t *params);
/*void benchmark_rtree_search();*/
void read_data(rt_coords_t *coords, int dataset_size);
void benchmarks_run(rt_bench_params_t *params);

void params_initialize(rt_bench_params_t *dest, uint8_t m, uint8_t M, float alloc_factor, int dataset_size);

#endif