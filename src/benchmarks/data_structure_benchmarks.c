#include "data_structure_benchmarks.h"
#include "vector.h"
#include "linked_list.h"
#include "binary_tree.h"
#include "hash_table.h"
#include "stack.h"
#include "timer.h"
#include "memory_stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static benchmark_config_t default_config = BENCHMARK_DEFAULT_CONFIG;

static unsigned int xorshift32(unsigned int* state) {
    unsigned int x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

void register_data_structure_benchmarks(void) {
    static benchmark_t bench1 = {
        .name = "vector_ops",
        .description = "Vector push/pop operations",
        .run = bench_vector_operations,
        .default_config = &default_config
    };
    benchmark_register(&bench1);

    static benchmark_t bench2 = {
        .name = "linked_list_ops",
        .description = "Linked list insert/delete operations",
        .run = bench_linked_list_operations,
        .default_config = &default_config
    };
    benchmark_register(&bench2);

    static benchmark_t bench3 = {
        .name = "binary_tree_ops",
        .description = "Binary tree insert/delete operations",
        .run = bench_binary_tree_operations,
        .default_config = &default_config
    };
    benchmark_register(&bench3);

    static benchmark_t bench4 = {
        .name = "hash_table_ops",
        .description = "Hash table insert/lookup operations",
        .run = bench_hash_table_operations,
        .default_config = &default_config
    };
    benchmark_register(&bench4);

    static benchmark_t bench5 = {
        .name = "stack_ops",
        .description = "Stack push/pop operations",
        .run = bench_stack_operations,
        .default_config = &default_config
    };
    benchmark_register(&bench5);
}

int bench_vector_operations(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;

    size_t iterations = cfg->iterations;
    unsigned int seed = cfg->seed;

    vector_t vec;
    if (vector_init(&vec, sizeof(int), 16, api) != 0) {
        return -1;
    }

    hr_timer_t timer;
    double total_push_time = 0;
    double total_pop_time = 0;
    size_t total_requested = 0;

    int* values = malloc(iterations * sizeof(int));
    for (size_t i = 0; i < iterations; i++) {
        values[i] = (int)(xorshift32(&seed));
    }

    for (size_t i = 0; i < iterations; i++) {
        hr_timer_init(&timer);
        hr_timer_start(&timer);
        vector_push_back(&vec, &values[i]);
        total_push_time += hr_timer_end(&timer);
        total_requested += sizeof(int);
    }

    int dummy;
    for (size_t i = 0; i < iterations; i++) {
        hr_timer_init(&timer);
        hr_timer_start(&timer);
        vector_pop_back(&vec, &dummy);
        total_pop_time += hr_timer_end(&timer);
    }

    vector_destroy(&vec);
    free(values);

    result->operations_count = iterations * 2;
    result->thread_count = 1;
    result->alloc_ops_per_sec = (double)iterations / (total_push_time / 1e9);
    result->free_ops_per_sec = (double)iterations / (total_pop_time / 1e9);
    result->total_ops_per_sec = (double)(iterations * 2) / ((total_push_time + total_pop_time) / 1e9);
    result->avg_alloc_time_ns = total_push_time / iterations;
    result->min_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->max_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p50_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p99_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->total_requested_bytes = total_requested;
    result->total_allocated_bytes = total_requested;
    result->fragmentation_ratio = BENCHMARK_METRIC_NA;

    return 0;
}

int bench_linked_list_operations(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;

    size_t iterations = cfg->iterations;
    unsigned int seed = cfg->seed;

    linked_list_t list;
    if (linked_list_init(&list, api) != 0) {
        return -1;
    }

    hr_timer_t timer;
    double total_push_time = 0;
    double total_pop_time = 0;
    size_t total_requested = 0;

    for (size_t i = 0; i < iterations; i++) {
        int* val = api->malloc(sizeof(int));
        *val = (int)(xorshift32(&seed));

        hr_timer_init(&timer);
        hr_timer_start(&timer);
        linked_list_push_back(&list, val);
        total_push_time += hr_timer_end(&timer);
        total_requested += sizeof(int) + sizeof(linked_list_node_t);
    }

    for (size_t i = 0; i < iterations; i++) {
        hr_timer_init(&timer);
        hr_timer_start(&timer);
        int* val = linked_list_pop_back(&list);
        total_pop_time += hr_timer_end(&timer);
        if (val) api->free(val);
    }

    linked_list_destroy(&list);

    result->operations_count = iterations * 2;
    result->thread_count = 1;
    result->alloc_ops_per_sec = (double)iterations / (total_push_time / 1e9);
    result->free_ops_per_sec = (double)iterations / (total_pop_time / 1e9);
    result->total_ops_per_sec = (double)(iterations * 2) / ((total_push_time + total_pop_time) / 1e9);
    result->avg_alloc_time_ns = total_push_time / iterations;
    result->min_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->max_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p50_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p99_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->total_requested_bytes = total_requested;
    result->total_allocated_bytes = total_requested;
    result->fragmentation_ratio = BENCHMARK_METRIC_NA;

    return 0;
}

static int compare_ints(const void* a, const void* b) {
    return (int)(intptr_t)a - (int)(intptr_t)b;
}

int bench_binary_tree_operations(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;

    size_t iterations = cfg->iterations;
    unsigned int seed = cfg->seed;

    binary_tree_t tree;
    if (binary_tree_init(&tree, compare_ints, api) != 0) {
        return -1;
    }

    hr_timer_t timer;
    double total_insert_time = 0;
    double total_remove_time = 0;
    size_t total_requested = 0;

    int* keys = malloc(iterations * sizeof(int));
    for (size_t i = 0; i < iterations; i++) {
        keys[i] = (int)(xorshift32(&seed));
    }

    for (size_t i = 0; i < iterations; i++) {
        hr_timer_init(&timer);
        hr_timer_start(&timer);
        binary_tree_insert(&tree, (void*)(intptr_t)keys[i], (void*)(intptr_t)i);
        total_insert_time += hr_timer_end(&timer);
        total_requested += sizeof(binary_tree_node_t);
    }

    for (size_t i = 0; i < iterations; i++) {
        hr_timer_init(&timer);
        hr_timer_start(&timer);
        binary_tree_remove(&tree, (void*)(intptr_t)keys[i]);
        total_remove_time += hr_timer_end(&timer);
    }

    binary_tree_destroy(&tree);
    free(keys);

    result->operations_count = iterations * 2;
    result->thread_count = 1;
    result->alloc_ops_per_sec = (double)iterations / (total_insert_time / 1e9);
    result->free_ops_per_sec = (double)iterations / (total_remove_time / 1e9);
    result->total_ops_per_sec = (double)(iterations * 2) / ((total_insert_time + total_remove_time) / 1e9);
    result->avg_alloc_time_ns = total_insert_time / iterations;
    result->min_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->max_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p50_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p99_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->total_requested_bytes = total_requested;
    result->total_allocated_bytes = total_requested;
    result->fragmentation_ratio = BENCHMARK_METRIC_NA;

    return 0;
}

int bench_hash_table_operations(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;

    size_t iterations = cfg->iterations;
    unsigned int seed = cfg->seed;

    hash_table_t table;
    if (hash_table_init(&table, hash_int, key_equal_int, api) != 0) {
        return -1;
    }

    hr_timer_t timer;
    double total_insert_time = 0;
    double total_lookup_time = 0;
    size_t total_requested = 0;

    for (size_t i = 0; i < iterations; i++) {
        void* key = (void*)(intptr_t)(xorshift32(&seed));

        hr_timer_init(&timer);
        hr_timer_start(&timer);
        hash_table_insert(&table, key, (void*)(intptr_t)i);
        total_insert_time += hr_timer_end(&timer);
        total_requested += sizeof(hash_table_entry_t);
    }

    seed = cfg->seed;
    for (size_t i = 0; i < iterations; i++) {
        void* key = (void*)(intptr_t)(xorshift32(&seed));

        hr_timer_init(&timer);
        hr_timer_start(&timer);
        hash_table_get(&table, key);
        total_lookup_time += hr_timer_end(&timer);
    }

    hash_table_destroy(&table);

    result->operations_count = iterations * 2;
    result->thread_count = 1;
    result->alloc_ops_per_sec = (double)iterations / (total_insert_time / 1e9);
    result->total_ops_per_sec = (double)(iterations * 2) / ((total_insert_time + total_lookup_time) / 1e9);
    result->avg_alloc_time_ns = total_insert_time / iterations;
    result->min_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->max_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p50_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p99_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->total_requested_bytes = total_requested;
    result->total_allocated_bytes = total_requested;
    result->fragmentation_ratio = BENCHMARK_METRIC_NA;

    return 0;
}

int bench_stack_operations(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;

    size_t iterations = cfg->iterations;
    unsigned int seed = cfg->seed;

    stack_t stack;
    if (stack_init(&stack, api) != 0) {
        return -1;
    }

    hr_timer_t timer;
    double total_push_time = 0;
    double total_pop_time = 0;
    size_t total_requested = 0;

    for (size_t i = 0; i < iterations; i++) {
        int* val = api->malloc(sizeof(int));
        *val = (int)(xorshift32(&seed));

        hr_timer_init(&timer);
        hr_timer_start(&timer);
        stack_push(&stack, val);
        total_push_time += hr_timer_end(&timer);
        total_requested += sizeof(int) + sizeof(stack_node_t);
    }

    for (size_t i = 0; i < iterations; i++) {
        hr_timer_init(&timer);
        hr_timer_start(&timer);
        int* val = stack_pop(&stack);
        total_pop_time += hr_timer_end(&timer);
        if (val) api->free(val);
    }

    stack_destroy(&stack);

    result->operations_count = iterations * 2;
    result->thread_count = 1;
    result->alloc_ops_per_sec = (double)iterations / (total_push_time / 1e9);
    result->free_ops_per_sec = (double)iterations / (total_pop_time / 1e9);
    result->total_ops_per_sec = (double)(iterations * 2) / ((total_push_time + total_pop_time) / 1e9);
    result->avg_alloc_time_ns = total_push_time / iterations;
    result->min_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->max_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p50_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p99_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->total_requested_bytes = total_requested;
    result->total_allocated_bytes = total_requested;
    result->fragmentation_ratio = BENCHMARK_METRIC_NA;

    return 0;
}
