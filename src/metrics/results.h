#ifndef RESULTS_H
#define RESULTS_H

#include "../benchmark.h"
#include <stdio.h>

#define MAX_RESULTS 1024

typedef struct {
    char benchmark_name[MAX_BENCHMARK_NAME];
    char allocator_name[MAX_ALLOCATOR_NAME];
    benchmark_result_t result;
} result_entry_t;

typedef struct {
    result_entry_t entries[MAX_RESULTS];
    int count;
    char output_dir[256];
    char timestamp[32];
} results_context_t;

void results_init(results_context_t* ctx, const char* output_dir);
void results_add_entry(results_context_t* ctx, const char* benchmark_name,
                      const char* allocator_name, const benchmark_result_t* result);
int results_write_json(results_context_t* ctx, char* filepath, size_t filepath_size);
int results_write_csv(results_context_t* ctx, const char* filepath);

void results_print_summary(results_context_t* ctx);

#endif
