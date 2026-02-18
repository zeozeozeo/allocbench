#ifndef BENCHMARK_H
#define BENCHMARK_H

#include <stddef.h>
#include <stdint.h>
#include "allocator_api.h"

#define MAX_ALLOCATOR_NAME 32
#define MAX_BENCHMARK_NAME 64
#define MAX_THREADS 16

typedef struct {
    char name[MAX_ALLOCATOR_NAME];
    allocator_api_t api;
    int available;
} allocator_info_t;

typedef struct {
    double alloc_ops_per_sec;
    double free_ops_per_sec;
    double realloc_ops_per_sec;
    double total_ops_per_sec;
    double avg_alloc_time_ns;
    double min_alloc_time_ns;
    double max_alloc_time_ns;
    double p50_alloc_time_ns;
    double p99_alloc_time_ns;
    size_t peak_rss_kb;
    size_t current_rss_kb;
    double fragmentation_ratio;
    size_t total_allocated_bytes;
    size_t total_requested_bytes;
    double total_time_ms;
    size_t operations_count;
    int thread_count;
} benchmark_result_t;

typedef struct {
    char name[MAX_BENCHMARK_NAME];
    char description[256];
    int (*run)(allocator_api_t* api, benchmark_result_t* result, void* config);
    void* default_config;
} benchmark_t;

typedef struct {
    size_t iterations;
    size_t min_size;
    size_t max_size;
    int thread_count;
    unsigned int seed;
} benchmark_config_t;

void benchmark_init(void);
void benchmark_register_allocator(const char* name, allocator_api_t* api);
int benchmark_get_allocator_count(void);
allocator_info_t* benchmark_get_allocator(int index);

void benchmark_register(const benchmark_t* bench);
int benchmark_get_count(void);
benchmark_t* benchmark_get(int index);

int benchmark_run_all(const char* output_dir);
int benchmark_run_single(const char* allocator_name, const char* benchmark_name, 
                         benchmark_result_t* result);
void benchmark_print_result(const char* benchmark_name, const char* allocator_name,
                           const benchmark_result_t* result);

#define BENCHMARK_METRIC_NA (-1.0)

#define BENCHMARK_DEFAULT_CONFIG { \
    .iterations = 1000000, \
    .min_size = 8, \
    .max_size = 4096, \
    .thread_count = 1, \
    .seed = 42 \
}

#define BENCHMARK_REGISTER(name, desc, run_func, config) \
    static benchmark_t __bench_##name = { \
        .name = #name, \
        .description = desc, \
        .run = run_func, \
        .default_config = config \
    }

#endif
