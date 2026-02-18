#ifndef DATA_STRUCTURE_BENCHMARKS_H
#define DATA_STRUCTURE_BENCHMARKS_H

#include "../benchmark.h"

void register_data_structure_benchmarks(void);

int bench_vector_operations(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_linked_list_operations(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_binary_tree_operations(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_hash_table_operations(allocator_api_t* api, benchmark_result_t* result, void* config);
int bench_stack_operations(allocator_api_t* api, benchmark_result_t* result, void* config);

#endif
