# allocbench

Benchmarks of various memory allocation libraries: [mimalloc](https://github.com/microsoft/mimalloc), [rpmalloc](https://github.com/mjansson/rpmalloc), [jemalloc](https://github.com/jemalloc/jemalloc) (not on Windows currently)

## Results

<details>
<summary>Windows (Xeon 10c/20t, DDR4-2133 quad)</summary>

## Benchmark results (windows)

## Tested allocators

mimalloc, rpmalloc, system

## Results

### aligned_alloc

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| rpmalloc **(Winner)** | 2.19M | 2.19M | 0.00 | 455.74ns | 78.00ns | 4.04Kns | 10.50 MB |
| system | 440.96K | 440.96K | 0.00 | 2.27Kns | 208.00ns | 73.99Kns | 68.00 KB |
| mimalloc | 388.90K | 388.90K | 0.00 | 2.57Kns | 100.00ns | 19.87Kns | 110.50 MB |

### alloc_free_immediate

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| rpmalloc **(Winner)** | 26.49M | 0.00 | 0.00 | 37.75ns | N/A | N/A | 0 |
| mimalloc | 12.48M | 0.00 | 0.00 | 80.16ns | N/A | N/A | 0 |
| system | 5.17M | 0.00 | 0.00 | 193.49ns | N/A | N/A | 0 |

### binary_tree_ops

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 783.37K | 823.15K | 747.26K | 1.21Kns | N/A | N/A | 7.71 MB |
| rpmalloc | 747.31K | 790.40K | 708.68K | 1.27Kns | N/A | N/A | 72.00 KB |
| system | 669.72K | 695.17K | 646.06K | 1.44Kns | N/A | N/A | 116.00 KB |

### fragmentation_pattern

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 5.12M | 0.00 | 0.00 | 195.19ns | N/A | N/A | 27.05 MB |
| rpmalloc | 5.08M | 0.00 | 0.00 | 196.82ns | N/A | N/A | 136.00 KB |
| system | 3.06M | 0.00 | 0.00 | 326.78ns | N/A | N/A | 219.32 MB |

### hash_table_ops

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 4.23M | 2.58M | 0.00 | 387.13ns | N/A | N/A | 17.99 MB |
| rpmalloc | 3.43M | 2.15M | 0.00 | 465.02ns | N/A | N/A | 3.88 MB |
| system | 2.79M | 1.65M | 0.00 | 607.29ns | N/A | N/A | 30.89 MB |

### larson

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 21.49M | 10.75M | 10.74M | 46.54ns | N/A | N/A | 0 |
| rpmalloc | 20.79M | 10.40M | 10.39M | 48.11ns | N/A | N/A | 2.35 MB |
| system | 6.10M | 3.05M | 3.05M | 164.04ns | N/A | N/A | 680.00 KB |

### linked_list_ops

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 24.76M | 19.57M | 33.69M | 51.10ns | N/A | N/A | 0 |
| rpmalloc | 20.98M | 14.33M | 39.20M | 69.79ns | N/A | N/A | 824.00 KB |
| system | 8.01M | 6.73M | 9.89M | 148.60ns | N/A | N/A | 0 |

### random_alloc

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 3.34M | 2.57M | 5.56M | 389.39ns | 86.00ns | 9.71Kns | 66.77 MB |
| rpmalloc | 2.69M | 5.46M | 1.61M | 183.27ns | 52.00ns | 4.16Kns | 820.00 KB |
| system | 1.63M | 1.36M | 2.23M | 737.48ns | 218.00ns | 732.00ns | 0 |

### realloc

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| rpmalloc **(Winner)** | 28.19M | 28.19M | 28.19M | 35.47ns | 24.00ns | 60.00ns | 0 |
| mimalloc | 4.01M | 4.01M | 4.01M | 249.19ns | 130.00ns | 412.00ns | 0 |
| system | 1.39M | 1.39M | 1.39M | 720.83ns | 562.00ns | 1.80Kns | 864.00 KB |

### sequential_alloc

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 723.41K | 476.42K | 1.50M | 2.10Kns | 62.00ns | 18.46Kns | 346.36 MB |
| system | 700.41K | 596.03K | 849.10K | 1.68Kns | 160.00ns | 6.60Kns | 7.87 MB |
| rpmalloc | 671.32K | 1.43M | 439.04K | 701.47ns | 40.00ns | 6.84Kns | 138.18 MB |

### stack_ops

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| rpmalloc **(Winner)** | 37.24M | 34.68M | 40.20M | 28.83ns | N/A | N/A | 0 |
| mimalloc | 32.42M | 29.34M | 36.23M | 34.08ns | N/A | N/A | 0 |
| system | 8.35M | 8.14M | 8.57M | 122.91ns | N/A | N/A | 0 |

### threaded_alloc_1

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 1.26M | 628.89K | 628.89K | 757.16ns | N/A | N/A | 120.43 MB |
| system | 966.72K | 483.36K | 483.36K | 960.62ns | N/A | N/A | 0 |
| rpmalloc | 690.86K | 345.43K | 345.43K | 1.39Kns | N/A | N/A | 16.82 MB |

### threaded_alloc_16

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 4.47M | 2.24M | 2.24M | 3.43Kns | N/A | N/A | 233.43 MB |
| system | 2.82M | 1.41M | 1.41M | 3.70Kns | N/A | N/A | 0 |
| rpmalloc | 2.75M | 1.37M | 1.37M | 5.37Kns | N/A | N/A | 19.91 MB |

### threaded_alloc_2

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| system **(Winner)** | 4.74M | 2.37M | 2.37M | 360.83ns | N/A | N/A | 206.45 MB |
| mimalloc | 3.39M | 1.70M | 1.70M | 513.38ns | N/A | N/A | 255.02 MB |
| rpmalloc | 1.04M | 518.97K | 518.97K | 1.86Kns | N/A | N/A | 8.98 MB |

### threaded_alloc_4

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 7.10M | 3.55M | 3.55M | 515.21ns | N/A | N/A | 255.03 MB |
| system | 6.29M | 3.14M | 3.14M | 505.81ns | N/A | N/A | 5.87 MB |
| rpmalloc | 1.59M | 793.83K | 793.83K | 2.43Kns | N/A | N/A | 12.12 MB |

### threaded_alloc_8

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| mimalloc **(Winner)** | 5.66M | 2.83M | 2.83M | 1.33Kns | N/A | N/A | 253.79 MB |
| system | 3.85M | 1.93M | 1.93M | 1.63Kns | N/A | N/A | 768.00 KB |
| rpmalloc | 2.29M | 1.15M | 1.15M | 3.36Kns | N/A | N/A | 18.80 MB |

### vector_ops

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| rpmalloc **(Winner)** | 38.37M | 35.01M | 42.45M | 28.57ns | N/A | N/A | 3.98 MB |
| mimalloc | 38.35M | 34.94M | 42.51M | 28.62ns | N/A | N/A | 7.93 MB |
| system | 37.67M | 33.94M | 42.31M | 29.47ns | N/A | N/A | 952.00 KB |

### worst_case_frag

| Allocator | Total ops/s | Alloc ops/s | Free ops/s | Avg time (ns) | P50 (ns) | P99 (ns) | Peak RSS |
|-----------|-------------|-------------|------------|---------------|----------|----------|----------|
| system **(Winner)** | 8.78M | 0.00 | 0.00 | 113.92ns | N/A | N/A | 64.00 KB |
| mimalloc | 3.06M | 0.00 | 0.00 | 326.27ns | N/A | N/A | 0 |
| rpmalloc | 3.00M | 0.00 | 0.00 | 333.40ns | N/A | N/A | 492.00 KB |

### Overall Summary

**Overall Winner: mimalloc** (won 11 benchmarks)

| Allocator | Wins |
|-----------|------|
| mimalloc | 11 |
| rpmalloc | 5 |
| system | 2 |

## Thread scaling

| Threads | mimalloc | rpmalloc | system | Best |
|---------|--------|--------|--------|--------|
| 1 | 1.26M ** | 690.86K | 966.72K | mimalloc |
| 2 | 3.39M | 1.04M | 4.74M ** | system |
| 4 | 7.10M ** | 1.59M | 6.29M | mimalloc |
| 8 | 5.66M ** | 2.29M | 3.85M | mimalloc |
| 16 | 4.47M ** | 2.75M | 2.82M | mimalloc |

![Thread Scaling](plots/windows/thread_scaling.png)

## Performance plots

### aligned_alloc

![aligned_alloc](plots/windows/aligned_alloc.png)

**Best performer:** total_ops_per_sec: rpmalloc, alloc_ops_per_sec: rpmalloc

### alloc_free_immediate

![alloc_free_immediate](plots/windows/alloc_free_immediate.png)

**Best performer:** total_ops_per_sec: rpmalloc, alloc_ops_per_sec: system

### binary_tree_ops

![binary_tree_ops](plots/windows/binary_tree_ops.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### fragmentation_pattern

![fragmentation_pattern](plots/windows/fragmentation_pattern.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### hash_table_ops

![hash_table_ops](plots/windows/hash_table_ops.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### larson

![larson](plots/windows/larson.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### linked_list_ops

![linked_list_ops](plots/windows/linked_list_ops.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### random_alloc

![random_alloc](plots/windows/random_alloc.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: rpmalloc

### realloc

![realloc](plots/windows/realloc.png)

**Best performer:** total_ops_per_sec: rpmalloc, alloc_ops_per_sec: rpmalloc

### sequential_alloc

![sequential_alloc](plots/windows/sequential_alloc.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: rpmalloc

### stack_ops

![stack_ops](plots/windows/stack_ops.png)

**Best performer:** total_ops_per_sec: rpmalloc, alloc_ops_per_sec: rpmalloc

### threaded_alloc_1

![threaded_alloc_1](plots/windows/threaded_alloc_1.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### threaded_alloc_16

![threaded_alloc_16](plots/windows/threaded_alloc_16.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### threaded_alloc_2

![threaded_alloc_2](plots/windows/threaded_alloc_2.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### threaded_alloc_4

![threaded_alloc_4](plots/windows/threaded_alloc_4.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: mimalloc

### threaded_alloc_8

![threaded_alloc_8](plots/windows/threaded_alloc_8.png)

**Best performer:** total_ops_per_sec: system, alloc_ops_per_sec: system

### vector_ops

![vector_ops](plots/windows/vector_ops.png)

**Best performer:** total_ops_per_sec: system, alloc_ops_per_sec: rpmalloc

### worst_case_frag

![worst_case_frag](plots/windows/worst_case_frag.png)

**Best performer:** total_ops_per_sec: mimalloc, alloc_ops_per_sec: system

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

In conclusion, mimalloc is the clear winner on both Linux and Windows. All allocators were compiled with clang 21 -O3.

Linux kernel used:
- 6.18.9-arch1-2
- SLUB allocator
- EEVDF sched
