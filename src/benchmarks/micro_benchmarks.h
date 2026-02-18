#ifndef MICRO_BENCHMARKS_H
#define MICRO_BENCHMARKS_H

#include "../benchmark.h"

void register_micro_benchmarks(void);

int bench_sequential_alloc(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_random_alloc(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_realloc_benchmark(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_aligned_alloc_benchmark(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_alloc_free_immediate(allocator_api_t* api, benchmark_result_t* result, void* config);

#endif
