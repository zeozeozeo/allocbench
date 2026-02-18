#include "micro_benchmarks.h"
#include "timer.h"
#include "memory_stats.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALLOC_TIMES 1000000

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

static int compare_doubles(const void* a, const void* b) {
    double diff = *(const double*)a - *(const double*)b;
    return (diff > 0) - (diff < 0);
}

void register_micro_benchmarks(void) {
    static benchmark_t bench1 = {
        .name = "sequential_alloc",
        .description = "Sequential allocation and deallocation",
        .run = bench_sequential_alloc,
        .default_config = &default_config
    };
    benchmark_register(&bench1);

    static benchmark_t bench2 = {
        .name = "random_alloc",
        .description = "Random size allocations",
        .run = bench_random_alloc,
        .default_config = &default_config
    };
    benchmark_register(&bench2);

    static benchmark_t bench3 = {
        .name = "realloc",
        .description = "Reallocation stress test",
        .run = bench_realloc_benchmark,
        .default_config = &default_config
    };
    benchmark_register(&bench3);

    static benchmark_t bench4 = {
        .name = "aligned_alloc",
        .description = "Aligned allocation benchmark",
        .run = bench_aligned_alloc_benchmark,
        .default_config = &default_config
    };
    benchmark_register(&bench4);

    static benchmark_t bench5 = {
        .name = "alloc_free_immediate",
        .description = "Allocate and immediately free",
        .run = bench_alloc_free_immediate,
        .default_config = &default_config
    };
    benchmark_register(&bench5);
}

int bench_sequential_alloc(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;

    size_t iterations = cfg->iterations;
    size_t min_size = cfg->min_size;
    size_t max_size = cfg->max_size;
    unsigned int seed = cfg->seed;

    void** ptrs = malloc(iterations * sizeof(void*));
    size_t* sizes = malloc(iterations * sizeof(size_t));
    double* alloc_times = malloc(iterations * sizeof(double));

    if (!ptrs || !sizes || !alloc_times) {
        free(ptrs);
        free(sizes);
        free(alloc_times);
        return -1;
    }

    result->operations_count = iterations * 2;
    result->thread_count = 1;

    hr_timer_t timer;
    size_t total_requested = 0;
    double total_alloc_time_ns = 0;
    double min_time = 1e30;
    double max_time = 0;

    for (size_t i = 0; i < iterations; i++) {
        sizes[i] = random_size(&seed, min_size, max_size);
        total_requested += sizes[i];

        hr_timer_init(&timer);
        hr_timer_start(&timer);
        ptrs[i] = api->malloc(sizes[i]);
        alloc_times[i] = hr_timer_end(&timer);

        if (!ptrs[i]) {
            for (size_t j = 0; j < i; j++) {
                api->free(ptrs[j]);
            }
            free(ptrs);
            free(sizes);
            free(alloc_times);
            return -1;
        }

        total_alloc_time_ns += alloc_times[i];
        if (alloc_times[i] < min_time) min_time = alloc_times[i];
        if (alloc_times[i] > max_time) max_time = alloc_times[i];
    }

    qsort(alloc_times, iterations, sizeof(double), compare_doubles);

    result->avg_alloc_time_ns = total_alloc_time_ns / iterations;
    result->min_alloc_time_ns = min_time;
    result->max_alloc_time_ns = max_time;
    result->p50_alloc_time_ns = alloc_times[iterations / 2];
    result->p99_alloc_time_ns = alloc_times[(size_t)(iterations * 0.99)];

    free(alloc_times);

    result->alloc_ops_per_sec = (double)iterations / (total_alloc_time_ns / 1e9);
    result->total_requested_bytes = total_requested;
    result->total_allocated_bytes = total_requested;

    double free_time_ns = 0;
    hr_timer_start(&timer);
    for (size_t i = 0; i < iterations; i++) {
        api->free(ptrs[i]);
    }
    free_time_ns = hr_timer_end(&timer);

    result->free_ops_per_sec = (double)iterations / (free_time_ns / 1e9);
    result->total_ops_per_sec = (double)(iterations * 2) / ((total_alloc_time_ns + free_time_ns) / 1e9);

    free(ptrs);
    free(sizes);

    return 0;
}

int bench_random_alloc(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;

    size_t iterations = cfg->iterations;
    size_t min_size = cfg->min_size;
    size_t max_size = cfg->max_size;
    unsigned int seed = cfg->seed;

    size_t active_count = iterations / 4;
    void** active_ptrs = malloc(active_count * sizeof(void*));
    size_t* active_sizes = malloc(active_count * sizeof(size_t));

    if (!active_ptrs || !active_sizes) {
        free(active_ptrs);
        free(active_sizes);
        return -1;
    }

    for (size_t i = 0; i < active_count; i++) {
        active_ptrs[i] = NULL;
        active_sizes[i] = 0;
    }

    result->operations_count = iterations;
    result->thread_count = 1;

    hr_timer_t timer;
    double total_alloc_time_ns = 0;
    double total_free_time_ns = 0;
    size_t alloc_count = 0;
    size_t free_count = 0;
    size_t total_requested = 0;

    double* alloc_times = malloc(iterations * sizeof(double));
    size_t alloc_time_idx = 0;

    for (size_t i = 0; i < iterations; i++) {
        size_t slot = xorshift32(&seed) % active_count;

        if (active_ptrs[slot]) {
            hr_timer_init(&timer);
            hr_timer_start(&timer);
            api->free(active_ptrs[slot]);
            total_free_time_ns += hr_timer_end(&timer);
            active_ptrs[slot] = NULL;
            free_count++;
        }

        size_t size = random_size(&seed, min_size, max_size);
        total_requested += size;

        hr_timer_init(&timer);
        hr_timer_start(&timer);
        void* ptr = api->malloc(size);
        double time = hr_timer_end(&timer);

        if (ptr) {
            active_ptrs[slot] = ptr;
            active_sizes[slot] = size;
            total_alloc_time_ns += time;
            alloc_count++;
            if (alloc_time_idx < iterations) {
                alloc_times[alloc_time_idx++] = time;
            }
        }
    }

    for (size_t i = 0; i < active_count; i++) {
        if (active_ptrs[i]) {
            api->free(active_ptrs[i]);
        }
    }

    if (alloc_time_idx > 0) {
        qsort(alloc_times, alloc_time_idx, sizeof(double), compare_doubles);

        result->avg_alloc_time_ns = total_alloc_time_ns / alloc_time_idx;
        result->p50_alloc_time_ns = alloc_times[alloc_time_idx / 2];
        result->p99_alloc_time_ns = alloc_times[(size_t)(alloc_time_idx * 0.99)];
    }

    result->alloc_ops_per_sec = (double)alloc_count / (total_alloc_time_ns / 1e9);
    result->free_ops_per_sec = (double)free_count / (total_free_time_ns / 1e9);
    result->total_ops_per_sec = (double)(alloc_count + free_count) / ((total_alloc_time_ns + total_free_time_ns) / 1e9);
    result->total_requested_bytes = total_requested;
    result->total_allocated_bytes = total_requested;

    free(active_ptrs);
    free(active_sizes);
    free(alloc_times);

    return 0;
}

int bench_realloc_benchmark(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;

    size_t iterations = cfg->iterations;
    size_t min_size = cfg->min_size;
    size_t max_size = cfg->max_size;
    unsigned int seed = cfg->seed;

    void* ptr = api->malloc(min_size);
    if (!ptr) return -1;

    size_t current_size = min_size;
    double total_time_ns = 0;
    size_t total_requested = min_size;

    hr_timer_t timer;
    double* times = malloc(iterations * sizeof(double));
    double min_time = 1e30, max_time = 0;

    for (size_t i = 0; i < iterations; i++) {
        size_t new_size = random_size(&seed, min_size, max_size);
        total_requested += new_size > current_size ? new_size - current_size : 0;

        hr_timer_init(&timer);
        hr_timer_start(&timer);
        void* new_ptr = api->realloc(ptr, new_size);
        double time = hr_timer_end(&timer);

        if (new_ptr) {
            ptr = new_ptr;
            current_size = new_size;
            total_time_ns += time;
            times[i] = time;
            if (time < min_time) min_time = time;
            if (time > max_time) max_time = time;
        } else {
            times[i] = 0;
        }
    }

    api->free(ptr);

    qsort(times, iterations, sizeof(double), compare_doubles);

    result->operations_count = iterations + 2;
    result->thread_count = 1;
    result->realloc_ops_per_sec = (double)iterations / (total_time_ns / 1e9);
    result->total_ops_per_sec = result->realloc_ops_per_sec;
    result->alloc_ops_per_sec = result->realloc_ops_per_sec;
    result->free_ops_per_sec = result->realloc_ops_per_sec;
    result->avg_alloc_time_ns = total_time_ns / iterations;
    result->min_alloc_time_ns = min_time;
    result->max_alloc_time_ns = max_time;
    result->p50_alloc_time_ns = times[iterations / 2];
    result->p99_alloc_time_ns = times[(size_t)(iterations * 0.99)];
    result->total_requested_bytes = total_requested;
    result->total_allocated_bytes = total_requested;
    result->fragmentation_ratio = BENCHMARK_METRIC_NA;

    free(times);

    return 0;
}

int bench_aligned_alloc_benchmark(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;

    size_t iterations = cfg->iterations / 10;
    if (iterations < 100) iterations = 100;

    size_t min_size = cfg->min_size;
    size_t max_size = cfg->max_size;
    unsigned int seed = cfg->seed;

    void** ptrs = malloc(iterations * sizeof(void*));
    double* times = malloc(iterations * sizeof(double));
    if (!ptrs || !times) {
        free(ptrs);
        free(times);
        return -1;
    }

    size_t alignments[] = {16, 32, 64, 128, 256, 512, 1024, 4096};
    size_t num_alignments = sizeof(alignments) / sizeof(alignments[0]);

    hr_timer_t timer;
    double total_time_ns = 0;
    size_t total_requested = 0;
    double min_time = 1e30, max_time = 0;

    for (size_t i = 0; i < iterations; i++) {
        size_t size = random_size(&seed, min_size, max_size);
        size_t align = alignments[xorshift32(&seed) % num_alignments];
        total_requested += size;

        hr_timer_init(&timer);
        hr_timer_start(&timer);
        ptrs[i] = api->aligned_alloc(align, size);
        times[i] = hr_timer_end(&timer);
        total_time_ns += times[i];

        if (times[i] < min_time) min_time = times[i];
        if (times[i] > max_time) max_time = times[i];

        if (!ptrs[i]) {
            for (size_t j = 0; j < i; j++) {
                api->aligned_free(ptrs[j]);
            }
            free(ptrs);
            free(times);
            return -1;
        }
    }

    for (size_t i = 0; i < iterations; i++) {
        api->aligned_free(ptrs[i]);
    }

    free(ptrs);

    qsort(times, iterations, sizeof(double), compare_doubles);

    result->operations_count = iterations * 2;
    result->thread_count = 1;
    result->alloc_ops_per_sec = (double)iterations / (total_time_ns / 1e9);
    result->total_ops_per_sec = result->alloc_ops_per_sec;
    result->avg_alloc_time_ns = total_time_ns / iterations;
    result->min_alloc_time_ns = min_time;
    result->max_alloc_time_ns = max_time;
    result->p50_alloc_time_ns = times[iterations / 2];
    result->p99_alloc_time_ns = times[(size_t)(iterations * 0.99)];
    result->total_requested_bytes = total_requested;
    result->total_allocated_bytes = total_requested;
    result->fragmentation_ratio = BENCHMARK_METRIC_NA;

    free(times);

    return 0;
}

int bench_alloc_free_immediate(allocator_api_t* api, benchmark_result_t* result, void* config) {
    benchmark_config_t* cfg = config ? (benchmark_config_t*)config : &default_config;

    size_t iterations = cfg->iterations;
    size_t min_size = cfg->min_size;
    size_t max_size = cfg->max_size;
    unsigned int seed = cfg->seed;

    hr_timer_t timer;
    double total_time_ns = 0;
    size_t total_requested = 0;

    for (size_t i = 0; i < iterations; i++) {
        size_t size = random_size(&seed, min_size, max_size);
        total_requested += size;

        hr_timer_init(&timer);
        hr_timer_start(&timer);
        void* ptr = api->malloc(size);
        if (ptr) {
            api->free(ptr);
        }
        total_time_ns += hr_timer_end(&timer);
    }

    result->operations_count = iterations * 2;
    result->thread_count = 1;
    result->total_ops_per_sec = (double)(iterations * 2) / (total_time_ns / 1e9);
    result->avg_alloc_time_ns = total_time_ns / (iterations * 2);
    result->min_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->max_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p50_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->p99_alloc_time_ns = BENCHMARK_METRIC_NA;
    result->total_requested_bytes = total_requested;
    result->total_allocated_bytes = total_requested;
    result->fragmentation_ratio = BENCHMARK_METRIC_NA;

    return 0;
}
