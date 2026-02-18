#ifndef ALLOCATOR_API_H
#define ALLOCATOR_API_H

#include <stddef.h>

typedef struct {
    void* (*malloc)(size_t size);
    void* (*calloc)(size_t num, size_t size);
    void* (*realloc)(void* ptr, size_t size);
    void (*free)(void* ptr);
    void* (*aligned_alloc)(size_t alignment, size_t size);
    void (*aligned_free)(void* ptr);
    int (*init)(void);
    void (*cleanup)(void);
    const char* name;
} allocator_api_t;

void system_allocator_init(allocator_api_t* api);
void rpmalloc_allocator_init(allocator_api_t* api);
void jemalloc_allocator_init(allocator_api_t* api);
void mimalloc_allocator_init(allocator_api_t* api);
void tcmalloc_allocator_init(allocator_api_t* api);

#endif
