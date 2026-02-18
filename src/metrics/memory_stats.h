#ifndef MEMORY_STATS_H
#define MEMORY_STATS_H

#include <stddef.h>

void memory_stats_init(void);
void memory_stats_reset(void);
void memory_stats_get(size_t* peak_rss_kb, size_t* current_rss_kb);

size_t get_current_rss_kb(void);
size_t get_peak_rss_kb(void);
size_t get_page_size(void);

typedef struct {
    size_t total_allocated;
    size_t total_freed;
    size_t current_usage;
    size_t peak_usage;
    size_t allocation_count;
    size_t free_count;
} allocation_tracker_t;

void allocation_tracker_init(allocation_tracker_t* tracker);
void allocation_tracker_record_alloc(allocation_tracker_t* tracker, size_t size);
void allocation_tracker_record_free(allocation_tracker_t* tracker, size_t size);
void allocation_tracker_reset(allocation_tracker_t* tracker);

size_t estimate_allocated_size(size_t requested_size);

#endif
