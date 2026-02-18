#include "benchmark.h"
#include "metrics/timer.h"
#include "metrics/memory_stats.h"
#include "metrics/results.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_ALLOCATORS 16
#define MAX_BENCHMARKS 64

static allocator_info_t allocators[MAX_ALLOCATORS];
static int allocator_count = 0;

static benchmark_t benchmarks[MAX_BENCHMARKS];
static int benchmark_count = 0;

void benchmark_init(void) {
    allocator_count = 0;
    benchmark_count = 0;
    memset(allocators, 0, sizeof(allocators));
    memset(benchmarks, 0, sizeof(benchmarks));
}

void benchmark_register_allocator(const char* name, allocator_api_t* api) {
    if (allocator_count >= MAX_ALLOCATORS) return;

    strncpy(allocators[allocator_count].name, name, MAX_ALLOCATOR_NAME - 1);
    allocators[allocator_count].api = *api;
    allocators[allocator_count].available = 1;
    allocator_count++;
}

int benchmark_get_allocator_count(void) {
    return allocator_count;
}

allocator_info_t* benchmark_get_allocator(int index) {
    if (index < 0 || index >= allocator_count) return NULL;
    return &allocators[index];
}

void benchmark_register(const benchmark_t* bench) {
    if (benchmark_count >= MAX_BENCHMARKS) return;
    benchmarks[benchmark_count] = *bench;
    benchmark_count++;
}

int benchmark_get_count(void) {
    return benchmark_count;
}

benchmark_t* benchmark_get(int index) {
    if (index < 0 || index >= benchmark_count) return NULL;
    return &benchmarks[index];
}

static int compare_doubles(const void* a, const void* b) {
    double diff = *(const double*)a - *(const double*)b;
    return (diff > 0) - (diff < 0);
}

static void calculate_percentiles(double* times, size_t count,
                                  double* p50, double* p99) {
    if (count == 0) {
        *p50 = *p99 = 0.0;
        return;
    }

    qsort(times, count, sizeof(double), compare_doubles);

    *p50 = times[count / 2];
    *p99 = times[(size_t)(count * 0.99)];
}

int benchmark_run_single(const char* allocator_name, const char* benchmark_name,
                        benchmark_result_t* result) {
    allocator_info_t* alloc = NULL;
    benchmark_t* bench = NULL;

    for (int i = 0; i < allocator_count; i++) {
        if (strcmp(allocators[i].name, allocator_name) == 0) {
            alloc = &allocators[i];
            break;
        }
    }

    for (int i = 0; i < benchmark_count; i++) {
        if (strcmp(benchmarks[i].name, benchmark_name) == 0) {
            bench = &benchmarks[i];
            break;
        }
    }

    if (!alloc || !bench) return -1;

    memset(result, 0, sizeof(benchmark_result_t));

    memory_stats_reset();
    timer_start();

    int ret = bench->run(&alloc->api, result, bench->default_config);

    result->total_time_ms = timer_end_ms();

    memory_stats_get(&result->peak_rss_kb, &result->current_rss_kb);

    if (result->fragmentation_ratio == 0.0 && result->total_requested_bytes > 0) {
        result->fragmentation_ratio = (double)result->total_allocated_bytes /
                                       (double)result->total_requested_bytes;
    }

    return ret;
}

void benchmark_print_result(const char* benchmark_name, const char* allocator_name,
                           const benchmark_result_t* result) {
    printf("\n=== %s [%s] ===\n", benchmark_name, allocator_name);
    printf("  Total time:        %.3f ms\n", result->total_time_ms);
    printf("  Operations:        %zu\n", result->operations_count);
    printf("  Alloc ops/sec:     %.2f M\n", result->alloc_ops_per_sec / 1e6);
    printf("  Free ops/sec:      %.2f M\n", result->free_ops_per_sec / 1e6);
    printf("  Total ops/sec:     %.2f M\n", result->total_ops_per_sec / 1e6);
    printf("  Avg alloc time:    %.2f ns\n", result->avg_alloc_time_ns);
    if (result->p99_alloc_time_ns != BENCHMARK_METRIC_NA)
        printf("  P99 alloc time:    %.2f ns\n", result->p99_alloc_time_ns);
    printf("  Peak RSS:          %zu KB\n", result->peak_rss_kb);
    if (result->fragmentation_ratio != BENCHMARK_METRIC_NA)
        printf("  Fragmentation:     %.3f\n", result->fragmentation_ratio);
}

int benchmark_run_all(const char* output_dir) {
    printf("\n--- allocbench: benchmark mode\n\n");

    printf("Registered allocators (%d):\n", allocator_count);
    for (int i = 0; i < allocator_count; i++) {
        printf("  [%d] %s\n", i + 1, allocators[i].name);
    }

    printf("\nRegistered benchmarks (%d):\n", benchmark_count);
    for (int i = 0; i < benchmark_count; i++) {
        printf("  [%d] %s - %s\n", i + 1, benchmarks[i].name, benchmarks[i].description);
    }

    results_context_t results_ctx;
    results_init(&results_ctx, output_dir);

    printf("\nRunning benchmarks...\n");
    printf("----------------------------------------\n");

    for (int b = 0; b < benchmark_count; b++) {
        printf("\n[Benchmark: %s]\n", benchmarks[b].name);
        printf("%-12s %12s %12s %12s\n",
               "Allocator", "Time(ms)", "Ops/sec", "Peak RSS(KB)");
        printf("%-12s %12s %12s %12s\n",
               "----------", "--------", "-------", "------------");

        for (int a = 0; a < allocator_count; a++) {
            benchmark_result_t result;

            int ret = benchmark_run_single(allocators[a].name, benchmarks[b].name, &result);

            if (ret == 0) {
                printf("%-12s %12.3f %12.2fM %12zu\n",
                       allocators[a].name,
                       result.total_time_ms,
                       result.total_ops_per_sec / 1e6,
                       result.peak_rss_kb);

                results_add_entry(&results_ctx, benchmarks[b].name,
                                 allocators[a].name, &result);
            } else {
                printf("%-12s %12s %12s %12s\n",
                       allocators[a].name, "ERROR", "-", "-");
            }
        }
    }

    char filepath[512];
    results_write_json(&results_ctx, filepath, sizeof(filepath));
    printf("\n\nResults saved to: %s\n", filepath);

    return 0;
}
