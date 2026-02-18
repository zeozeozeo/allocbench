#include "results.h"
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

void results_init(results_context_t* ctx, const char* output_dir) {
    memset(ctx, 0, sizeof(results_context_t));
    strncpy(ctx->output_dir, output_dir ? output_dir : "results", sizeof(ctx->output_dir) - 1);

    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    strftime(ctx->timestamp, sizeof(ctx->timestamp), "%Y%m%d_%H%M%S", tm_info);
}

void results_add_entry(results_context_t* ctx, const char* benchmark_name,
                      const char* allocator_name, const benchmark_result_t* result) {
    if (ctx->count >= MAX_RESULTS) return;

    result_entry_t* entry = &ctx->entries[ctx->count];
    strncpy(entry->benchmark_name, benchmark_name, MAX_BENCHMARK_NAME - 1);
    strncpy(entry->allocator_name, allocator_name, MAX_ALLOCATOR_NAME - 1);
    memcpy(&entry->result, result, sizeof(benchmark_result_t));
    ctx->count++;
}

static void write_json_string(FILE* fp, const char* str) {
    fputc('"', fp);
    while (*str) {
        switch (*str) {
            case '"': fputs("\\\"", fp); break;
            case '\\': fputs("\\\\", fp); break;
            case '\n': fputs("\\n", fp); break;
            case '\r': fputs("\\r", fp); break;
            case '\t': fputs("\\t", fp); break;
            default: fputc(*str, fp);
        }
        str++;
    }
    fputc('"', fp);
}

int results_write_json(results_context_t* ctx, char* filepath, size_t filepath_size) {
    snprintf(filepath, filepath_size, "%s/benchmark_%s.json",
             ctx->output_dir, ctx->timestamp);

    mkdir(ctx->output_dir, 0755);

    FILE* fp = fopen(filepath, "w");
    if (!fp) return -1;

    fprintf(fp, "{\n");
    fprintf(fp, "  \"timestamp\": \"%s\",\n", ctx->timestamp);
    fprintf(fp, "  \"results\": [\n");

    for (int i = 0; i < ctx->count; i++) {
        result_entry_t* entry = &ctx->entries[i];
        benchmark_result_t* r = &entry->result;

        fprintf(fp, "    {\n");
        fprintf(fp, "      \"benchmark\": ");
        write_json_string(fp, entry->benchmark_name);
        fprintf(fp, ",\n");
        fprintf(fp, "      \"allocator\": ");
        write_json_string(fp, entry->allocator_name);
        fprintf(fp, ",\n");
        fprintf(fp, "      \"metrics\": {\n");
        fprintf(fp, "        \"total_time_ms\": %.6f,\n", r->total_time_ms);
        fprintf(fp, "        \"operations\": %zu,\n", r->operations_count);
        fprintf(fp, "        \"alloc_ops_per_sec\": %.2f,\n", r->alloc_ops_per_sec);
        fprintf(fp, "        \"free_ops_per_sec\": %.2f,\n", r->free_ops_per_sec);
        fprintf(fp, "        \"total_ops_per_sec\": %.2f,\n", r->total_ops_per_sec);
        fprintf(fp, "        \"avg_alloc_time_ns\": %.2f,\n", r->avg_alloc_time_ns);

        if (r->min_alloc_time_ns == BENCHMARK_METRIC_NA)
            fprintf(fp, "        \"min_alloc_time_ns\": null,\n");
        else
            fprintf(fp, "        \"min_alloc_time_ns\": %.2f,\n", r->min_alloc_time_ns);

        if (r->max_alloc_time_ns == BENCHMARK_METRIC_NA)
            fprintf(fp, "        \"max_alloc_time_ns\": null,\n");
        else
            fprintf(fp, "        \"max_alloc_time_ns\": %.2f,\n", r->max_alloc_time_ns);

        if (r->p50_alloc_time_ns == BENCHMARK_METRIC_NA)
            fprintf(fp, "        \"p50_alloc_time_ns\": null,\n");
        else
            fprintf(fp, "        \"p50_alloc_time_ns\": %.2f,\n", r->p50_alloc_time_ns);

        if (r->p99_alloc_time_ns == BENCHMARK_METRIC_NA)
            fprintf(fp, "        \"p99_alloc_time_ns\": null,\n");
        else
            fprintf(fp, "        \"p99_alloc_time_ns\": %.2f,\n", r->p99_alloc_time_ns);

        fprintf(fp, "        \"peak_rss_kb\": %zu,\n", r->peak_rss_kb);
        fprintf(fp, "        \"current_rss_kb\": %zu,\n", r->current_rss_kb);

        if (r->fragmentation_ratio == BENCHMARK_METRIC_NA)
            fprintf(fp, "        \"fragmentation_ratio\": null,\n");
        else
            fprintf(fp, "        \"fragmentation_ratio\": %.6f,\n", r->fragmentation_ratio);

        fprintf(fp, "        \"total_allocated_bytes\": %zu,\n", r->total_allocated_bytes);
        fprintf(fp, "        \"total_requested_bytes\": %zu,\n", r->total_requested_bytes);
        fprintf(fp, "        \"thread_count\": %d\n", r->thread_count);
        fprintf(fp, "      }\n");
        fprintf(fp, "    }%s\n", (i < ctx->count - 1) ? "," : "");
    }

    fprintf(fp, "  ]\n");
    fprintf(fp, "}\n");

    fclose(fp);
    return 0;
}

int results_write_csv(results_context_t* ctx, const char* filepath) {
    FILE* fp = fopen(filepath, "w");
    if (!fp) return -1;

    fprintf(fp, "benchmark,allocator,total_time_ms,operations,alloc_ops_per_sec,"
                "free_ops_per_sec,total_ops_per_sec,avg_alloc_time_ns,"
                "p99_alloc_time_ns,peak_rss_kb,fragmentation_ratio,thread_count\n");

    for (int i = 0; i < ctx->count; i++) {
        result_entry_t* e = &ctx->entries[i];
        benchmark_result_t* r = &e->result;

        fprintf(fp, "%s,%s,%.6f,%zu,%.2f,%.2f,%.2f,%.2f,%.2f,%zu,%.6f,%d\n",
                e->benchmark_name, e->allocator_name,
                r->total_time_ms, r->operations_count,
                r->alloc_ops_per_sec, r->free_ops_per_sec, r->total_ops_per_sec,
                r->avg_alloc_time_ns, r->p99_alloc_time_ns,
                r->peak_rss_kb, r->fragmentation_ratio, r->thread_count);
    }

    fclose(fp);
    return 0;
}

void results_print_summary(results_context_t* ctx) {
    printf("\n-- allocbench: results summary\n");
    printf("Total benchmarks run: %d\n", ctx->count);

    const char* current_bench = "";
    for (int i = 0; i < ctx->count; i++) {
        if (strcmp(current_bench, ctx->entries[i].benchmark_name) != 0) {
            current_bench = ctx->entries[i].benchmark_name;
            printf("\n[%s]\n", current_bench);
        }

        benchmark_result_t* r = &ctx->entries[i].result;
        printf("  %s: %.3f ms, %.2f M ops/sec, frag=%.3f\n",
               ctx->entries[i].allocator_name,
               r->total_time_ms,
               r->total_ops_per_sec / 1e6,
               r->fragmentation_ratio);
    }
}
