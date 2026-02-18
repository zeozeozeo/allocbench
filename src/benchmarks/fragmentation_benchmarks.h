#ifndef FRAGMENTATION_BENCHMARKS_H
#define FRAGMENTATION_BENCHMARKS_H

#include "../benchmark.h"

void register_fragmentation_benchmarks(void);

int bench_fragmentation_pattern(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_worst_case_fragmentation(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_larson(allocator_api_t* api, benchmark_result_t* result, void* config);

#endif
