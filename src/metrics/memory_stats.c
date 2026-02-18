#include "memory_stats.h"
#include <stdio.h>
#include <string.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <psapi.h>

static size_t page_size_cache = 0;

size_t get_page_size(void) {
    if (page_size_cache == 0) {
        SYSTEM_INFO si;
        GetSystemInfo(&si);
        page_size_cache = si.dwPageSize;
    }
    return page_size_cache;
}

size_t get_current_rss_kb(void) {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.WorkingSetSize / 1024;
    }
    return 0;
}

size_t get_peak_rss_kb(void) {
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc))) {
        return pmc.PeakWorkingSetSize / 1024;
    }
    return 0;
}

#else
#include <unistd.h>
#include <sys/sysinfo.h>

static size_t page_size_cache = 0;

size_t get_page_size(void) {
    if (page_size_cache == 0) {
        page_size_cache = sysconf(_SC_PAGESIZE);
    }
    return page_size_cache;
}

size_t get_current_rss_kb(void) {
    FILE* fp = fopen("/proc/self/statm", "r");
    if (!fp) return 0;

    unsigned long size, resident, share, text, lib, data, dt;
    if (fscanf(fp, "%lu %lu %lu %lu %lu %lu %lu",
               &size, &resident, &share, &text, &lib, &data, &dt) != 7) {
        fclose(fp);
        return 0;
    }
    fclose(fp);

    return (resident * get_page_size()) / 1024;
}

size_t get_peak_rss_kb(void) {
    FILE* fp = fopen("/proc/self/status", "r");
    if (!fp) return 0;

    char line[256];
    size_t peak_rss = 0;

    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "VmHWM:", 6) == 0) {
            sscanf(line + 6, "%zu", &peak_rss);
            break;
        }
    }

    fclose(fp);
    return peak_rss;
}
#endif

static size_t global_peak_rss = 0;
static size_t baseline_rss = 0;

void memory_stats_init(void) {
    baseline_rss = get_current_rss_kb();
    global_peak_rss = baseline_rss;
}

void memory_stats_reset(void) {
    baseline_rss = get_current_rss_kb();
    global_peak_rss = baseline_rss;
}

void memory_stats_get(size_t* peak_rss_kb, size_t* current_rss_kb) {
    size_t current = get_current_rss_kb();
    if (current > global_peak_rss) {
        global_peak_rss = current;
    }

    if (peak_rss_kb) *peak_rss_kb = global_peak_rss - baseline_rss;
    if (current_rss_kb) *current_rss_kb = current - baseline_rss;
}

void allocation_tracker_init(allocation_tracker_t* tracker) {
    memset(tracker, 0, sizeof(allocation_tracker_t));
}

void allocation_tracker_record_alloc(allocation_tracker_t* tracker, size_t size) {
    tracker->total_allocated += size;
    tracker->current_usage += size;
    tracker->allocation_count++;

    if (tracker->current_usage > tracker->peak_usage) {
        tracker->peak_usage = tracker->current_usage;
    }
}

void allocation_tracker_record_free(allocation_tracker_t* tracker, size_t size) {
    tracker->total_freed += size;
    if (tracker->current_usage >= size) {
        tracker->current_usage -= size;
    } else {
        tracker->current_usage = 0;
    }
    tracker->free_count++;
}

void allocation_tracker_reset(allocation_tracker_t* tracker) {
    memset(tracker, 0, sizeof(allocation_tracker_t));
}

size_t estimate_allocated_size(size_t requested_size) {
    size_t page_size = get_page_size();

    if (requested_size <= 16) return 16;
    if (requested_size <= 32) return 32;
    if (requested_size <= 48) return 48;
    if (requested_size <= 64) return 64;
    if (requested_size <= 80) return 80;
    if (requested_size <= 96) return 96;
    if (requested_size <= 112) return 112;
    if (requested_size <= 128) return 128;
    if (requested_size <= 160) return 160;
    if (requested_size <= 192) return 192;
    if (requested_size <= 224) return 224;
    if (requested_size <= 256) return 256;
    if (requested_size <= 320) return 320;
    if (requested_size <= 384) return 384;
    if (requested_size <= 448) return 448;
    if (requested_size <= 512) return 512;
    if (requested_size <= 768) return 768;
    if (requested_size <= 1024) return 1024;
    if (requested_size <= 1536) return 1536;
    if (requested_size <= 2048) return 2048;
    if (requested_size <= 3072) return 3072;
    if (requested_size <= 4096) return 4096;

    size_t pages_needed = (requested_size + page_size - 1) / page_size;
    return pages_needed * page_size;
}
