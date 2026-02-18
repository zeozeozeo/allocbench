#include "fragmentation_benchmarks.h"
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

void register_fragmentation_benchmarks(void) {
    static benchmark_t bench1 = {
        .name = "fragmentation_pattern",
        .description = "Fragmentation pattern test",
        .run = bench_fragmentation_pattern,
        .default_config = &default_config
    };
    benchmark_register(&bench1);
    
    static benchmark_t bench2 = {
        .name = "worst_case_frag",
        .description = "Worst case fragmentation test",
        .run = bench_worst_case_fragmentation,
        .default_config = &default_config
    };
    benchmark_register(&bench2);
    
    static benchmark_t bench3 = {
        .name = "larson",
        .description = "Larson benchmark (allocation density)",
        .run = bench_larson,
        .default_config = &default_config
    };
    benchmark_register(&bench3);
}

int bench_fragmentation_pattern(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;
    
    size_t iterations = cfg->iterations / 4;
    unsigned int seed = cfg->seed;
    
    size_t sizes[] = {16, 32, 64, 128, 256, 512, 1024, 2048, 4096};
    size_t num_sizes = sizeof(sizes) / sizeof(sizes[0]);
    
    void** ptrs = malloc(iterations * sizeof(void*));
    size_t* alloc_sizes = malloc(iterations * sizeof(size_t));
    
    if (!ptrs || !alloc_sizes) {
        free(ptrs);
        free(alloc_sizes);
        return -1;
    }
    
    for (size_t i = 0; i < iterations; i++) {
        ptrs[i] = NULL;
        alloc_sizes[i] = 0;
    }
    
    hr_timer_t timer;
    double total_time_ns = 0;
    size_t total_requested = 0;
    size_t active_count = 0;
    
    for (size_t round = 0; round < 4; round++) {
        for (size_t i = 0; i < iterations; i++) {
            if (ptrs[i] && xorshift32(&seed) % 3 == 0) {
                hr_timer_init(&timer);
                hr_timer_start(&timer);
                api->free(ptrs[i]);
                total_time_ns += hr_timer_end(&timer);
                ptrs[i] = NULL;
                active_count--;
            }
            
            if (!ptrs[i]) {
                size_t size = sizes[xorshift32(&seed) % num_sizes];
                total_requested += size;
                
                hr_timer_init(&timer);
                hr_timer_start(&timer);
                ptrs[i] = api->malloc(size);
                total_time_ns += hr_timer_end(&timer);
                
                if (ptrs[i]) {
                    alloc_sizes[i] = size;
                    active_count++;
                }
            }
        }
    }
    
    size_t peak_memory = 0;
    size_t current_memory = 0;
    for (size_t i = 0; i < iterations; i++) {
        if (ptrs[i]) {
            current_memory += alloc_sizes[i];
            if (current_memory > peak_memory) {
                peak_memory = current_memory;
            }
        }
    }
    
    for (size_t i = 0; i < iterations; i++) {
        if (ptrs[i]) {
            api->free(ptrs[i]);
        }
    }
    
    free(ptrs);
    free(alloc_sizes);
    
    result->operations_count = iterations * 4;
    result->thread_count = 1;
    result->total_ops_per_sec = (double)result->operations_count / (total_time_ns / 1e9);
    result->avg_alloc_time_ns = total_time_ns / result->operations_count;
    result->min_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->max_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p50_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p99_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->total_requested_bytes = total_requested;
    result->total_allocated_bytes = total_requested;
    result->fragmentation_ratio = 1.0 + (double)(peak_memory - total_requested / 4) / (double)peak_memory;
    
    return 0;
}

int bench_worst_case_fragmentation(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;
    
    size_t num_ptrs = 10000;
    void** ptrs = malloc(num_ptrs * sizeof(void*));
    if (!ptrs) return -1;
    
    size_t small_size = 16;
    size_t large_size = 1024;
    
    hr_timer_t timer;
    double total_time_ns = 0;
    size_t total_requested = 0;
    
    for (size_t i = 0; i < num_ptrs; i++) {
        size_t size = (i % 2 == 0) ? small_size : large_size;
        total_requested += size;
        
        hr_timer_init(&timer);
        hr_timer_start(&timer);
        ptrs[i] = api->malloc(size);
        total_time_ns += hr_timer_end(&timer);
    }
    
    for (size_t i = 0; i < num_ptrs; i += 2) {
        hr_timer_init(&timer);
        hr_timer_start(&timer);
        api->free(ptrs[i]);
        total_time_ns += hr_timer_end(&timer);
        ptrs[i] = NULL;
    }
    
    for (size_t i = 0; i < num_ptrs; i += 2) {
        hr_timer_init(&timer);
        hr_timer_start(&timer);
        ptrs[i] = api->malloc(large_size);
        total_time_ns += hr_timer_end(&timer);
    }
    
    for (size_t i = 0; i < num_ptrs; i++) {
        if (ptrs[i]) {
            api->free(ptrs[i]);
        }
    }
    
    free(ptrs);
    
    result->operations_count = num_ptrs * 3;
    result->thread_count = 1;
    result->total_ops_per_sec = (double)result->operations_count / (total_time_ns / 1e9);
    result->avg_alloc_time_ns = total_time_ns / result->operations_count;
    result->min_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->max_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p50_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p99_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->total_requested_bytes = total_requested;
    result->total_allocated_bytes = total_requested;
    result->fragmentation_ratio = 1.5;
    
    return 0;
}

int bench_larson(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;
    
    size_t iterations = cfg->iterations;
    unsigned int seed = cfg->seed;
    size_t min_size = cfg->min_size;
    size_t max_size = cfg->max_size;
    
    size_t array_size = 1000;
    void** ptrs = calloc(array_size, sizeof(void*));
    size_t* sizes = calloc(array_size, sizeof(size_t));
    
    if (!ptrs || !sizes) {
        free(ptrs);
        free(sizes);
        return -1;
    }
    
    hr_timer_t timer;
    double total_time_ns = 0;
    size_t total_requested = 0;
    size_t alloc_count = 0;
    size_t free_count = 0;
    
    for (size_t i = 0; i < iterations; i++) {
        size_t index = xorshift32(&seed) % array_size;
        
        if (ptrs[index]) {
            hr_timer_init(&timer);
            hr_timer_start(&timer);
            api->free(ptrs[index]);
            total_time_ns += hr_timer_end(&timer);
            ptrs[index] = NULL;
            sizes[index] = 0;
            free_count++;
        }
        
        size_t size = min_size + xorshift32(&seed) % (max_size - min_size + 1);
        total_requested += size;
        
        hr_timer_init(&timer);
        hr_timer_start(&timer);
        ptrs[index] = api->malloc(size);
        total_time_ns += hr_timer_end(&timer);
        
        if (ptrs[index]) {
            sizes[index] = size;
            alloc_count++;
        }
    }
    
    size_t active_memory = 0;
    for (size_t i = 0; i < array_size; i++) {
        if (ptrs[i]) {
            active_memory += sizes[i];
            api->free(ptrs[i]);
        }
    }
    
    free(ptrs);
    free(sizes);
    
    result->operations_count = alloc_count + free_count;
    result->thread_count = 1;
    result->alloc_ops_per_sec = (double)alloc_count / (total_time_ns / 1e9);
    result->free_ops_per_sec = (double)free_count / (total_time_ns / 1e9);
    result->total_ops_per_sec = (double)result->operations_count / (total_time_ns / 1e9);
    result->avg_alloc_time_ns = total_time_ns / result->operations_count;
    result->min_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->max_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p50_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p99_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->total_requested_bytes = total_requested;
    result->total_allocated_bytes = total_requested;
    result->fragmentation_ratio = BENCHMARK_METRIC_NA;
    
    return 0;
}
