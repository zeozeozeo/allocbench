#include "timer.h"
#include <time.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
static LARGE_INTEGER timer_freq;
static LARGE_INTEGER timer_start_val;
static int timer_initialized = 0;

void timer_start(void) {
    if (!timer_initialized) {
        QueryPerformanceFrequency(&timer_freq);
        timer_initialized = 1;
    }
    QueryPerformanceCounter(&timer_start_val);
}

double timer_end_ms(void) {
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    return (double)(end.QuadPart - timer_start_val.QuadPart) * 1000.0 / timer_freq.QuadPart;
}

double timer_end_us(void) {
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    return (double)(end.QuadPart - timer_start_val.QuadPart) * 1000000.0 / timer_freq.QuadPart;
}

double timer_end_ns(void) {
    LARGE_INTEGER end;
    QueryPerformanceCounter(&end);
    return (double)(end.QuadPart - timer_start_val.QuadPart) * 1000000000.0 / timer_freq.QuadPart;
}

double timer_get_elapsed_ms(void) { return timer_end_ms(); }
double timer_get_elapsed_us(void) { return timer_end_us(); }
double timer_get_elapsed_ns(void) { return timer_end_ns(); }

uint64_t get_cycles(void) {
    return __rdtsc();
}

#else
#include <stdint.h>
static struct timespec timer_start_val;

void timer_start(void) {
    clock_gettime(CLOCK_MONOTONIC, &timer_start_val);
}

static double timespec_diff_ms(struct timespec* start, struct timespec* end) {
    return (end->tv_sec - start->tv_sec) * 1000.0 +
           (end->tv_nsec - start->tv_nsec) / 1000000.0;
}

static double timespec_diff_us(struct timespec* start, struct timespec* end) {
    return (end->tv_sec - start->tv_sec) * 1000000.0 +
           (end->tv_nsec - start->tv_nsec) / 1000.0;
}

static double timespec_diff_ns(struct timespec* start, struct timespec* end) {
    return (end->tv_sec - start->tv_sec) * 1000000000.0 +
           (end->tv_nsec - start->tv_nsec);
}

double timer_end_ms(void) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    return timespec_diff_ms(&timer_start_val, &end);
}

double timer_end_us(void) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    return timespec_diff_us(&timer_start_val, &end);
}

double timer_end_ns(void) {
    struct timespec end;
    clock_gettime(CLOCK_MONOTONIC, &end);
    return timespec_diff_ns(&timer_start_val, &end);
}

double timer_get_elapsed_ms(void) { return timer_end_ms(); }
double timer_get_elapsed_us(void) { return timer_end_us(); }
double timer_get_elapsed_ns(void) { return timer_end_ns(); }

uint64_t get_cycles(void) {
    uint32_t lo, hi;
    __asm__ __volatile__ ("rdtsc" : "=a" (lo), "=d" (hi));
    return ((uint64_t)hi << 32) | lo;
}
#endif

double cycles_to_ns(uint64_t cycles) {
    return (double)cycles;
}

double cycles_to_us(uint64_t cycles) {
    return (double)cycles / 1000.0;
}

double cycles_to_ms(uint64_t cycles) {
    return (double)cycles / 1000000.0;
}

void hr_timer_init(hr_timer_t* timer) {
    timer->start_cycles = 0;
    timer->end_cycles = 0;
    timer->running = 0;
}

void hr_timer_start(hr_timer_t* timer) {
    timer->start_cycles = get_cycles();
    timer->running = 1;
}

double hr_timer_end(hr_timer_t* timer) {
    timer->end_cycles = get_cycles();
    timer->running = 0;
    return cycles_to_ns(timer->end_cycles - timer->start_cycles);
}

double hr_timer_elapsed(hr_timer_t* timer) {
    if (!timer->running) return 0.0;
    uint64_t now = get_cycles();
    return cycles_to_ns(now - timer->start_cycles);
}
