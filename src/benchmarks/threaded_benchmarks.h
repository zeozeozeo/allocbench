#ifndef THREADED_BENCHMARKS_H
#define THREADED_BENCHMARKS_H

#include "../benchmark.h"

void register_threaded_benchmarks(void);

int bench_threaded_alloc(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_threaded_alloc_2(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_threaded_alloc_4(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_threaded_alloc_8(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_threaded_alloc_16(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_producer_consumer(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_parallel_stress(allocator_api_t* api, benchmark_result_t* result, void* config);

#endif
