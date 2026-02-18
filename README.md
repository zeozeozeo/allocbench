# allocbench

Benchmarks of various memory allocation libraries: [mimalloc](https://github.com/microsoft/mimalloc), [rpmalloc](https://github.com/mjansson/rpmalloc), [jemalloc](https://github.com/jemalloc/jemalloc) (not on Windows currently), [tcmalloc](https://github.com/google/tcmalloc) (not supported on Windows)

## Results

<details>
<summary>Windows (Xeon 10c/20t, DDR4-2133 quad)</summary>
# todo
</details>

<details>
<summary>Linux (Core i5-13420H, LPDDR4x 4267MT/s)</summary>

## Benchmark results (linux)

## Tested allocators

jemalloc, mimalloc, rpmalloc, system

## Results

### aligned_alloc

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 11.38M | 11.38M | 0.00 | 87.90ns | 56.00ns | 408.00ns | 8.00 KB |
| system | 3.22M | 3.22M | 0.00 | 310.81ns | 264.00ns | 1.13Kns | 0 |
| rpmalloc | 2.58M | 2.58M | 0.00 | 387.57ns | 56.00ns | 3.24Kns | 8.97 MB |
| jemalloc | 2.32M | 2.32M | 0.00 | 430.85ns | 22.00ns | 9.46Kns | 0 |

### alloc_free_immediate

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| jemalloc **(Winner)** | 75.42M | 0.00 | 0.00 | 13.26ns | N/A | N/A | 0 |
| mimalloc | 46.92M | 0.00 | 0.00 | 21.31ns | N/A | N/A | 0 |
| rpmalloc | 46.75M | 0.00 | 0.00 | 21.39ns | N/A | N/A | 0 |
| system | 17.87M | 0.00 | 0.00 | 55.95ns | N/A | N/A | 0 |

### binary_tree_ops

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 900.29K | 802.22K | 1.03M | 1.25Kns | N/A | N/A | 0 |
| jemalloc | 882.36K | 773.14K | 1.03M | 1.29Kns | N/A | N/A | 0 |
| system | 815.71K | 770.85K | 866.11K | 1.30Kns | N/A | N/A | 0 |
| rpmalloc | 748.02K | 691.27K | 814.92K | 1.45Kns | N/A | N/A | 72.00 KB |

### fragmentation_pattern

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 43.08M | 0.00 | 0.00 | 23.21ns | N/A | N/A | 0 |
| jemalloc | 14.21M | 0.00 | 0.00 | 70.36ns | N/A | N/A | 0 |
| system | 8.95M | 0.00 | 0.00 | 111.69ns | N/A | N/A | 0 |
| rpmalloc | 6.78M | 0.00 | 0.00 | 147.46ns | N/A | N/A | 136.00 KB |

### hash_table_ops

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 6.02M | 5.03M | 0.00 | 198.86ns | N/A | N/A | 0 |
| jemalloc | 5.41M | 4.30M | 0.00 | 232.57ns | N/A | N/A | 0 |
| system | 5.07M | 4.03M | 0.00 | 248.16ns | N/A | N/A | 0 |
| rpmalloc | 5.04M | 3.92M | 0.00 | 255.28ns | N/A | N/A | 3.93 MB |

### larson

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 32.35M | 16.19M | 16.17M | 30.91ns | N/A | N/A | 0 |
| rpmalloc | 24.47M | 12.24M | 12.23M | 40.87ns | N/A | N/A | 2.35 MB |
| jemalloc | 21.91M | 10.96M | 10.95M | 45.64ns | N/A | N/A | 0 |
| system | 8.29M | 4.15M | 4.14M | 120.61ns | N/A | N/A | 0 |

### linked_list_ops

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 34.93M | 25.99M | 53.21M | 38.47ns | N/A | N/A | 28.58 MB |
| rpmalloc | 32.47M | 23.56M | 52.23M | 42.45ns | N/A | N/A | 824.00 KB |
| system | 28.12M | 28.57M | 27.69M | 35.01ns | N/A | N/A | 0 |
| jemalloc | 23.91M | 18.88M | 32.57M | 52.96ns | N/A | N/A | 25.71 MB |

### random_alloc

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 12.48M | 11.18M | 14.77M | 89.45ns | 30.00ns | 340.00ns | 0 |
| jemalloc | 11.72M | 10.62M | 13.57M | 94.13ns | 18.00ns | 1.60Kns | 40.00 KB |
| rpmalloc | 3.08M | 7.20M | 1.75M | 138.97ns | 28.00ns | 3.03Kns | 0 |
| system | 1.76M | 1.76M | 1.76M | 567.25ns | 144.00ns | 4.16Kns | 491.99 MB |

### realloc

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| rpmalloc **(Winner)** | 51.36M | 51.36M | 51.36M | 19.47ns | 20.00ns | 24.00ns | 0 |
| system | 19.56M | 19.56M | 19.56M | 51.12ns | 44.00ns | 70.00ns | 0 |
| mimalloc | 12.74M | 12.74M | 12.74M | 78.47ns | 76.00ns | 162.00ns | 0 |
| jemalloc | 7.56M | 7.56M | 7.56M | 132.24ns | 100.00ns | 202.00ns | 55.99 MB |

### sequential_alloc

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| jemalloc **(Winner)** | 5.45M | 4.09M | 8.16M | 244.73ns | 20.00ns | 6.30Kns | 33.47 MB |
| rpmalloc | 2.96M | 2.03M | 5.49M | 493.73ns | 26.00ns | 212.00ns | 141.05 MB |
| mimalloc | 1.20M | 619.03K | 20.12M | 1.62Kns | 26.00ns | 304.00ns | 2.12 GB |
| system | 1.14M | 696.89K | 3.15M | 1.43Kns | 2.21Kns | 6.37Kns | 17.46 MB |

### stack_ops

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| rpmalloc **(Winner)** | 52.89M | 53.32M | 52.47M | 18.76ns | N/A | N/A | 0 |
| mimalloc | 50.19M | 47.08M | 53.74M | 21.24ns | N/A | N/A | 128.00 KB |
| jemalloc | 30.70M | 28.46M | 33.32M | 35.13ns | N/A | N/A | 0 |
| system | 28.19M | 28.28M | 28.09M | 35.36ns | N/A | N/A | 0 |

### threaded_alloc_1

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 10.62M | 5.31M | 5.31M | 57.18ns | N/A | N/A | 12.00 KB |
| jemalloc | 5.82M | 2.91M | 2.91M | 148.79ns | N/A | N/A | 0 |
| rpmalloc | 1.31M | 654.16K | 654.16K | 724.40ns | N/A | N/A | 16.70 MB |
| system | 1.09M | 546.16K | 546.16K | 890.28ns | N/A | N/A | 336.00 KB |

### threaded_alloc_16

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 53.27M | 26.63M | 26.63M | 93.51ns | N/A | N/A | 724.00 KB |
| jemalloc | 22.71M | 11.36M | 11.36M | 424.98ns | N/A | N/A | 2.96 MB |
| system | 17.78M | 8.89M | 8.89M | 427.85ns | N/A | N/A | 100.12 MB |
| rpmalloc | 5.84M | 2.92M | 2.92M | 1.87Kns | N/A | N/A | 19.96 MB |

### threaded_alloc_2

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 25.58M | 12.79M | 12.79M | 46.28ns | N/A | N/A | 12.00 KB |
| jemalloc | 11.20M | 5.60M | 5.60M | 152.49ns | N/A | N/A | 0 |
| system | 4.08M | 2.04M | 2.04M | 408.04ns | N/A | N/A | 0 |
| rpmalloc | 2.28M | 1.14M | 1.14M | 828.66ns | N/A | N/A | 9.95 MB |

### threaded_alloc_4

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 38.01M | 19.01M | 19.01M | 51.16ns | N/A | N/A | 24.00 KB |
| jemalloc | 17.82M | 8.91M | 8.91M | 168.06ns | N/A | N/A | 0 |
| system | 4.67M | 2.33M | 2.33M | 428.73ns | N/A | N/A | 13.07 MB |
| rpmalloc | 3.74M | 1.87M | 1.87M | 946.61ns | N/A | N/A | 12.27 MB |

### threaded_alloc_8

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 45.97M | 22.98M | 22.98M | 88.60ns | N/A | N/A | 48.00 KB |
| jemalloc | 18.21M | 9.10M | 9.10M | 337.00ns | N/A | N/A | 0 |
| system | 14.87M | 7.44M | 7.44M | 326.77ns | N/A | N/A | 185.60 MB |
| rpmalloc | 4.62M | 2.31M | 2.31M | 1.25Kns | N/A | N/A | 18.98 MB |

### vector_ops

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 54.60M | 53.47M | 55.78M | 18.70ns | N/A | N/A | 0 |
| system | 54.44M | 53.17M | 55.77M | 18.81ns | N/A | N/A | 0 |
| jemalloc | 51.90M | 48.50M | 55.82M | 20.62ns | N/A | N/A | 6.00 MB |
| rpmalloc | 48.74M | 43.28M | 55.78M | 23.11ns | N/A | N/A | 3.92 MB |

### worst_case_frag

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 63.92M | 0.00 | 0.00 | 15.64ns | N/A | N/A | 0 |
| jemalloc | 12.52M | 0.00 | 0.00 | 79.87ns | N/A | N/A | 0 |
| system | 11.86M | 0.00 | 0.00 | 84.35ns | N/A | N/A | 0 |
| rpmalloc | 4.38M | 0.00 | 0.00 | 228.12ns | N/A | N/A | 492.00 KB |

### Overall summary

**Overall Winner: mimalloc** (won 14 benchmarks)

| Allocator | Wins |
|-----------|------|
| mimalloc | 14 |
| jemalloc | 2 |
| rpmalloc | 2 |

## Thread scaling

| Threads | jemalloc | mimalloc | rpmalloc | system | Best |
|---------|--------|--------|--------|--------|--------|
| 1 | 5.82M | 10.62M ** | 1.31M | 1.09M | mimalloc |
| 2 | 11.20M | 25.58M ** | 2.28M | 4.08M | mimalloc |
| 4 | 17.82M | 38.01M ** | 3.74M | 4.67M | mimalloc |
| 8 | 18.21M | 45.97M ** | 4.62M | 14.87M | mimalloc |
| 16 | 22.71M | 53.27M ** | 5.84M | 17.78M | mimalloc |

![Thread Scaling](plots/linux/thread_scaling.png)

## Performance plots

### aligned_alloc

![aligned_alloc](plots/linux/aligned_alloc.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### alloc_free_immediate

![alloc_free_immediate](plots/linux/alloc_free_immediate.png)

**Best performer:** total_ops_per_sec: jemalloc, alloc_ops_per_sec: rpmalloc

### binary_tree_ops

![binary_tree_ops](plots/linux/binary_tree_ops.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### fragmentation_pattern

![fragmentation_pattern](plots/linux/fragmentation_pattern.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### hash_table_ops

![hash_table_ops](plots/linux/hash_table_ops.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### larson

![larson](plots/linux/larson.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### linked_list_ops

![linked_list_ops](plots/linux/linked_list_ops.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### random_alloc

![random_alloc](plots/linux/random_alloc.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### realloc

![realloc](plots/linux/realloc.png)

**Best performer:** total_ops_per_sec: rpmalloc, alloc_ops_per_sec: rpmalloc

### sequential_alloc

![sequential_alloc](plots/linux/sequential_alloc.png)

**Best performer:** total_ops_per_sec: jemalloc, alloc_ops_per_sec: jemalloc

### stack_ops

![stack_ops](plots/linux/stack_ops.png)

**Best performer:** total_ops_per_sec: rpmalloc, alloc_ops_per_sec: rpmalloc

### threaded_alloc_1

![threaded_alloc_1](plots/linux/threaded_alloc_1.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### threaded_alloc_16

![threaded_alloc_16](plots/linux/threaded_alloc_16.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### threaded_alloc_2

![threaded_alloc_2](plots/linux/threaded_alloc_2.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### threaded_alloc_4

![threaded_alloc_4](plots/linux/threaded_alloc_4.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### threaded_alloc_8

![threaded_alloc_8](plots/linux/threaded_alloc_8.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### vector_ops

![vector_ops](plots/linux/vector_ops.png)

**Best performer:** total_ops_per_sec: system, alloc_ops_per_sec: system

### worst_case_frag

![worst_case_frag](plots/linux/worst_case_frag.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

</details>

In conclusion, mimalloc is the clear winner on both Linux and Windows.

Linux kernel used:
- 6.18.9-arch1-2
- SLUB allocator
- EEVDF sched
