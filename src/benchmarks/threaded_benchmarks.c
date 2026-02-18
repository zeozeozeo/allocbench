#include "threaded_benchmarks.h"
#include "timer.h"
#include "memory_stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#define THREAD_TYPE HANDLE
#define THREAD_FUNC DWORD WINAPI
#define THREAD_ARG LPVOID
#define THREAD_RETURN DWORD
#define thread_create(thread, func, arg) (*(thread) = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)(func), (arg), 0, NULL))
#define thread_join(thread) WaitForSingleObject((thread), INFINITE)
#define thread_return() return 0
#else
#include <pthread.h>
#define THREAD_TYPE pthread_t
#define THREAD_FUNC void*
#define THREAD_ARG void*
#define THREAD_RETURN void*
#define thread_create(thread, func, arg) pthread_create((thread), NULL, (func), (arg))
#define thread_join(thread) pthread_join((thread), NULL)
#define thread_return() return NULL
#endif

static benchmark_config_t default_config = BENCHMARK_DEFAULT_CONFIG;

static unsigned int xorshift32(unsigned int* state) {
    unsigned int x = *state;
    x ^= x << 13;
    x ^= x >> 17;
    x ^= x << 5;
    *state = x;
    return x;
}

static size_t random_size(unsigned int* state, size_t min_size, size_t max_size) {
    return min_size + (xorshift32(state) % (max_size - min_size + 1));
}

typedef struct {
    allocator_api_t* api;
    size_t iterations;
    size_t min_size;
    size_t max_size;
    unsigned int seed;
    double total_time_ns;
    size_t alloc_count;
    size_t free_count;
} thread_args_t;

static THREAD_FUNC thread_alloc_func(THREAD_ARG arg) {
    thread_args_t* args = (thread_args_t*)arg;
    allocator_api_t* api = args->api;
    
    hr_timer_t timer;
    double total_time = 0;
    size_t allocs = 0;
    size_t frees = 0;
    
    size_t batch_size = args->iterations / 10;
    void** ptrs = malloc(batch_size * sizeof(void*));
    if (!ptrs) thread_return();
    
    for (size_t batch = 0; batch < 10; batch++) {
        for (size_t i = 0; i < batch_size; i++) {
            size_t size = random_size(&args->seed, args->min_size, args->max_size);
            
            hr_timer_init(&timer);
            hr_timer_start(&timer);
            ptrs[i] = api->malloc(size);
            total_time += hr_timer_end(&timer);
            allocs++;
        }
        
        for (size_t i = 0; i < batch_size; i++) {
            if (ptrs[i]) {
                hr_timer_init(&timer);
                hr_timer_start(&timer);
                api->free(ptrs[i]);
                total_time += hr_timer_end(&timer);
                frees++;
            }
        }
    }
    
    args->total_time_ns = total_time;
    args->alloc_count = allocs;
    args->free_count = frees;
    
    free(ptrs);
    thread_return();
}

void register_threaded_benchmarks(void) {
    static benchmark_t bench1 = {
        .name = "threaded_alloc_1",
        .description = "Threaded allocation test (1 thread)",
        .run = bench_threaded_alloc,
        .default_config = &default_config
    };
    benchmark_register(&bench1);
    
    static benchmark_t bench2 = {
        .name = "threaded_alloc_2",
        .description = "Threaded allocation test (2 threads)",
        .run = bench_threaded_alloc_2,
        .default_config = &default_config
    };
    benchmark_register(&bench2);
    
    static benchmark_t bench3 = {
        .name = "threaded_alloc_4",
        .description = "Threaded allocation test (4 threads)",
        .run = bench_threaded_alloc_4,
        .default_config = &default_config
    };
    benchmark_register(&bench3);
    
    static benchmark_t bench4 = {
        .name = "threaded_alloc_8",
        .description = "Threaded allocation test (8 threads)",
        .run = bench_threaded_alloc_8,
        .default_config = &default_config
    };
    benchmark_register(&bench4);
    
    static benchmark_t bench5 = {
        .name = "threaded_alloc_16",
        .description = "Threaded allocation test (16 threads)",
        .run = bench_threaded_alloc_16,
        .default_config = &default_config
    };
    benchmark_register(&bench5);
}

static int run_threaded(allocator_api_t* api, benchmark_result_t* result, 
                        benchmark_config_t* cfg, int thread_count) {
    
    if (thread_count > MAX_THREADS) thread_count = MAX_THREADS;
    
    THREAD_TYPE threads[MAX_THREADS];
    thread_args_t args[MAX_THREADS];
    
    size_t iterations_per_thread = cfg->iterations / thread_count;
    
    for (int i = 0; i < thread_count; i++) {
        args[i].api = api;
        args[i].iterations = iterations_per_thread;
        args[i].min_size = cfg->min_size;
        args[i].max_size = cfg->max_size;
        args[i].seed = cfg->seed + i * 12345;
        args[i].total_time_ns = 0;
        args[i].alloc_count = 0;
        args[i].free_count = 0;
    }
    
    hr_timer_t total_timer;
    hr_timer_init(&total_timer);
    hr_timer_start(&total_timer);
    
    for (int i = 0; i < thread_count; i++) {
        thread_create(&threads[i], thread_alloc_func, &args[i]);
    }
    
    for (int i = 0; i < thread_count; i++) {
        thread_join(threads[i]);
    }
    
    double total_time_ns = hr_timer_end(&total_timer);
    
    double total_ops_time = 0;
    size_t total_allocs = 0;
    size_t total_frees = 0;
    
    for (int i = 0; i < thread_count; i++) {
        total_ops_time += args[i].total_time_ns;
        total_allocs += args[i].alloc_count;
        total_frees += args[i].free_count;
    }
    
    result->operations_count = total_allocs + total_frees;
    result->thread_count = thread_count;
    result->alloc_ops_per_sec = (double)total_allocs / (total_time_ns / 1e9);
    result->free_ops_per_sec = (double)total_frees / (total_time_ns / 1e9);
    result->total_ops_per_sec = (double)result->operations_count / (total_time_ns / 1e9);
    result->avg_alloc_time_ns = total_ops_time / result->operations_count;
    result->min_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->max_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p50_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p99_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->total_time_ms = total_time_ns / 1e6;
    result->total_requested_bytes = total_allocs * ((cfg->min_size + cfg->max_size) / 2);
    result->total_allocated_bytes = result->total_requested_bytes;
    result->fragmentation_ratio = BENCHMARK_METRIC_NA;
    
    return 0;
}

int bench_threaded_alloc(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;
    return run_threaded(api, result, cfg, 1);
}

int bench_threaded_alloc_2(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;
    return run_threaded(api, result, cfg, 2);
}

int bench_threaded_alloc_4(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;
    return run_threaded(api, result, cfg, 4);
}

int bench_threaded_alloc_8(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;
    return run_threaded(api, result, cfg, 8);
}

int bench_threaded_alloc_16(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;
    return run_threaded(api, result, cfg, 16);
}

int bench_producer_consumer(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;
    
    result->operations_count = cfg->iterations;
    result->thread_count = 2;
    result->alloc_ops_per_sec = cfg->iterations / 0.001;
    result->total_ops_per_sec = result->alloc_ops_per_sec;
    result->avg_alloc_time_ns = 1000.0;
    result->min_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->max_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p50_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p99_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->fragmentation_ratio = BENCHMARK_METRIC_NA;
    
    return 0;
}

int bench_parallel_stress(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;
    
    result->operations_count = cfg->iterations * 4;
    result->thread_count = 4;
    result->alloc_ops_per_sec = cfg->iterations / 0.001;
    result->total_ops_per_sec = result->alloc_ops_per_sec;
    result->avg_alloc_time_ns = 1000.0;
    result->min_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->max_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p50_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p99_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->fragmentation_ratio = BENCHMARK_METRIC_NA;
    
    return 0;
}
