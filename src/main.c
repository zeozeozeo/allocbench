#include "benchmark.h"
#include "allocator_api.h"
#include "memory_stats.h"
#include "micro_benchmarks.h"
#include "data_structure_benchmarks.h"
#include "threaded_benchmarks.h"
#include "fragmentation_benchmarks.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#ifdef _WIN32
#include <direct.h>
#define mkdir(path, mode) _mkdir(path)
#endif

static const size_t GRAPH_ITERATIONS[] = {
    1000, 5000, 10000, 25000, 50000, 100000, 250000, 500000, 1000000
};
static const int NUM_GRAPH_ITERATIONS = sizeof(GRAPH_ITERATIONS) / sizeof(GRAPH_ITERATIONS[0]);

static void register_all_benchmarks(void) {
    register_micro_benchmarks();
    register_data_structure_benchmarks();
    register_threaded_benchmarks();
    register_fragmentation_benchmarks();
}

static void register_all_allocators(void) {
    allocator_api_t api;

#ifdef USE_SYSTEM_ALLOC
    system_allocator_init(&api);
    benchmark_register_allocator("system", &api);
#else
    system_allocator_init(&api);
    benchmark_register_allocator("system", &api);

#ifdef HAVE_RPMALLOC
    rpmalloc_allocator_init(&api);
    if (api.init) api.init();
    benchmark_register_allocator("rpmalloc", &api);
#endif

#ifdef HAVE_MIMALLOC
    mimalloc_allocator_init(&api);
    benchmark_register_allocator("mimalloc", &api);
#endif

#ifdef HAVE_JEMALLOC
    jemalloc_allocator_init(&api);
    benchmark_register_allocator("jemalloc", &api);
#endif

#ifdef HAVE_TCMALLOC
    tcmalloc_allocator_init(&api);
    benchmark_register_allocator("tcmalloc", &api);
#endif
#endif
}

static void print_usage(const char* program) {
    printf("allocbench\n\n");
    printf("Usage: %s [options]\n\n", program);
    printf("Options:\n");
    printf("  -h, --help              Show this help message\n");
    printf("  -l, --list              List available benchmarks and allocators\n");
    printf("  -a <name>               Run benchmarks for specific allocator\n");
    printf("  -b <name>               Run specific benchmark\n");
    printf("  -o <dir>                Output directory for results (default: results)\n");
    printf("  -i <count>              Number of iterations (default: 1000000)\n");
    printf("  --graph                 Benchmark across multiple iteration counts\n");
    printf("\n");
}

static void list_benchmarks_and_allocators(void) {
    printf("Available allocators:\n");
    for (int i = 0; i < benchmark_get_allocator_count(); i++) {
        allocator_info_t* info = benchmark_get_allocator(i);
        printf("  %s\n", info->name);
    }

    printf("\nAvailable benchmarks:\n");
    for (int i = 0; i < benchmark_get_count(); i++) {
        benchmark_t* bench = benchmark_get(i);
        printf("  %-25s - %s\n", bench->name, bench->description);
    }
}

static void write_graph_json(const char* filepath, const char* benchmark_name,
                             const char** allocator_names, int num_allocators,
                             const size_t* iteration_counts, int num_iterations,
                             benchmark_result_t** results) {
    FILE* fp = fopen(filepath, "w");
    if (!fp) return;

    fprintf(fp, "{\n");
    fprintf(fp, "  \"mode\": \"graph\",\n");
    fprintf(fp, "  \"benchmark\": \"%s\",\n", benchmark_name);
    fprintf(fp, "  \"iterations\": [");
    for (int i = 0; i < num_iterations; i++) {
        fprintf(fp, "%zu%s", iteration_counts[i], (i < num_iterations - 1) ? ", " : "");
    }
    fprintf(fp, "],\n");

    fprintf(fp, "  \"allocators\": [");
    for (int i = 0; i < num_allocators; i++) {
        fprintf(fp, "\"%s\"%s", allocator_names[i], (i < num_allocators - 1) ? ", " : "");
    }
    fprintf(fp, "],\n");

    fprintf(fp, "  \"metrics\": {\n");

    const char* metric_names[] = {
        "total_ops_per_sec", "alloc_ops_per_sec", "free_ops_per_sec",
        "avg_alloc_time_ns", "peak_rss_kb"
    };
    int num_metrics = 5;

    for (int m = 0; m < num_metrics; m++) {
        fprintf(fp, "    \"%s\": {\n", metric_names[m]);

        for (int a = 0; a < num_allocators; a++) {
            fprintf(fp, "      \"%s\": [", allocator_names[a]);

            for (int i = 0; i < num_iterations; i++) {
                benchmark_result_t* r = &results[a][i];
                double value = 0;

                if (strcmp(metric_names[m], "total_ops_per_sec") == 0)
                    value = r->total_ops_per_sec;
                else if (strcmp(metric_names[m], "alloc_ops_per_sec") == 0)
                    value = r->alloc_ops_per_sec;
                else if (strcmp(metric_names[m], "free_ops_per_sec") == 0)
                    value = r->free_ops_per_sec;
                else if (strcmp(metric_names[m], "avg_alloc_time_ns") == 0)
                    value = r->avg_alloc_time_ns;
                else if (strcmp(metric_names[m], "peak_rss_kb") == 0)
                    value = (double)r->peak_rss_kb;

                fprintf(fp, "%.2f%s", value, (i < num_iterations - 1) ? ", " : "");
            }

            fprintf(fp, "]%s\n", (a < num_allocators - 1) ? "," : "");
        }

        fprintf(fp, "    }%s\n", (m < num_metrics - 1) ? "," : "");
    }

    fprintf(fp, "  }\n");
    fprintf(fp, "}\n");

    fclose(fp);
}

static int run_graph_mode(const char* output_dir, const char* specific_benchmark,
                          const char* specific_allocator) {
    mkdir(output_dir, 0755);

    int start_bench = 0;
    int end_bench = benchmark_get_count();

    if (specific_benchmark) {
        for (int i = 0; i < end_bench; i++) {
            benchmark_t* b = benchmark_get(i);
            if (strcmp(b->name, specific_benchmark) == 0) {
                start_bench = i;
                end_bench = i + 1;
                break;
            }
        }
    }

    printf("\n--- allocbench: graph mode\n\n");
    printf("Iteration counts: ");
    for (int i = 0; i < NUM_GRAPH_ITERATIONS; i++) {
        printf("%zu%s", GRAPH_ITERATIONS[i], (i < NUM_GRAPH_ITERATIONS - 1) ? ", " : "\n");
    }
    printf("\n");

    for (int b = start_bench; b < end_bench; b++) {
        benchmark_t* bench = benchmark_get(b);

        printf("\n[Benchmark: %s]\n", bench->name);

        int num_allocators = benchmark_get_allocator_count();
        const char* allocator_names[16];
        benchmark_result_t* results[16];

        for (int a = 0; a < num_allocators; a++) {
            allocator_names[a] = benchmark_get_allocator(a)->name;
            results[a] = malloc(NUM_GRAPH_ITERATIONS * sizeof(benchmark_result_t));
        }

        printf("%-12s", "Iterations");
        for (int a = 0; a < num_allocators; a++) {
            printf("%15s", allocator_names[a]);
        }
        printf("\n");
        printf("%-12s", "----------");
        for (int a = 0; a < num_allocators; a++) {
            printf("%15s", "---------------");
        }
        printf("\n");

        for (int i = 0; i < NUM_GRAPH_ITERATIONS; i++) {
            benchmark_config_t config = {
                .iterations = GRAPH_ITERATIONS[i],
                .min_size = 8,
                .max_size = 4096,
                .thread_count = 1,
                .seed = 42
            };

            printf("%-12zu", GRAPH_ITERATIONS[i]);

            for (int a = 0; a < num_allocators; a++) {
                allocator_info_t* alloc = benchmark_get_allocator(a);

                if (specific_allocator && strcmp(alloc->name, specific_allocator) != 0) {
                    memset(&results[a][i], 0, sizeof(benchmark_result_t));
                    printf("%15s", "-");
                    continue;
                }

                benchmark_config_t* old_config = bench->default_config;
                bench->default_config = &config;

                memory_stats_reset();
                int ret = bench->run(&alloc->api, &results[a][i], &config);
                memory_stats_get(&results[a][i].peak_rss_kb, &results[a][i].current_rss_kb);

                bench->default_config = old_config;

                if (ret == 0) {
                    printf("%14.2fM", results[a][i].total_ops_per_sec / 1e6);
                } else {
                    memset(&results[a][i], 0, sizeof(benchmark_result_t));
                    printf("%15s", "ERROR");
                }
            }
            printf("\n");
        }

        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s/graph_%s.json", output_dir, bench->name);
        write_graph_json(filepath, bench->name, allocator_names, num_allocators,
                        GRAPH_ITERATIONS, NUM_GRAPH_ITERATIONS, results);
        printf("\nSaved: %s\n", filepath);

        for (int a = 0; a < num_allocators; a++) {
            free(results[a]);
        }
    }

    return 0;
}

int main(int argc, char** argv) {
    const char* output_dir = "results";
    const char* specific_allocator = NULL;
    const char* specific_benchmark = NULL;
    size_t iterations = 1000000;
    int graph_mode = 0;

    for (int i = 1; i < argc; i++) {
        if (strcmp(argv[i], "-h") == 0 || strcmp(argv[i], "--help") == 0) {
            print_usage(argv[0]);
            return 0;
        }
        else if (strcmp(argv[i], "-l") == 0 || strcmp(argv[i], "--list") == 0) {
            benchmark_init();
            register_all_benchmarks();
            register_all_allocators();
            list_benchmarks_and_allocators();
            return 0;
        }
        else if (strcmp(argv[i], "--graph") == 0) {
            graph_mode = 1;
        }
        else if (strcmp(argv[i], "-a") == 0 && i + 1 < argc) {
            specific_allocator = argv[++i];
        }
        else if (strcmp(argv[i], "-b") == 0 && i + 1 < argc) {
            specific_benchmark = argv[++i];
        }
        else if (strcmp(argv[i], "-o") == 0 && i + 1 < argc) {
            output_dir = argv[++i];
        }
        else if (strcmp(argv[i], "-i") == 0 && i + 1 < argc) {
            iterations = (size_t)atoll(argv[++i]);
        }
    }

    benchmark_init();
    register_all_benchmarks();
    register_all_allocators();

    memory_stats_init();

    if (graph_mode) {
        return run_graph_mode(output_dir, specific_benchmark, specific_allocator);
    }
    else if (specific_allocator && specific_benchmark) {
        benchmark_result_t result;
        int ret = benchmark_run_single(specific_allocator, specific_benchmark, &result);

        if (ret == 0) {
            benchmark_print_result(specific_benchmark, specific_allocator, &result);
        } else {
            fprintf(stderr, "Error running benchmark\n");
            return 1;
        }
    } else {
        benchmark_run_all(output_dir);
    }

    return 0;
}
