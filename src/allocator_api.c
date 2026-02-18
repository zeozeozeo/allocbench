#include "allocator_api.h"
#include <stdlib.h>
#include <string.h>

static void* system_malloc(size_t size) { return malloc(size); }
static void* system_calloc(size_t num, size_t size) { return calloc(num, size); }
static void* system_realloc(void* ptr, size_t size) { return realloc(ptr, size); }
static void system_free(void* ptr) { free(ptr); }

#if defined(_WIN32) || defined(_WIN64)
#include <malloc.h>
static void* system_aligned_alloc(size_t alignment, size_t size) {
    return _aligned_malloc(size, alignment);
}
static void system_aligned_free(void* ptr) {
    _aligned_free(ptr);
}
#else
static void* system_aligned_alloc(size_t alignment, size_t size) {
    void* ptr = NULL;
    posix_memalign(&ptr, alignment, size);
    return ptr;
}
static void system_aligned_free(void* ptr) {
    free(ptr);
}
#endif

void system_allocator_init(allocator_api_t* api) {
    memset(api, 0, sizeof(allocator_api_t));
    api->malloc = system_malloc;
    api->calloc = system_calloc;
    api->realloc = system_realloc;
    api->free = system_free;
    api->aligned_alloc = system_aligned_alloc;
    api->aligned_free = system_aligned_free;
    api->name = "system";
}

#ifdef HAVE_RPMALLOC
#include <rpmalloc.h>

static void* rp_malloc(size_t size) { return rpmalloc(size); }
static void* rp_calloc(size_t num, size_t size) { return rpcalloc(num, size); }
static void* rp_realloc(void* ptr, size_t size) { return rprealloc(ptr, size); }
static void rp_free(void* ptr) { rpfree(ptr); }
static void* rp_aligned_alloc(size_t alignment, size_t size) {
    return rpaligned_alloc(alignment, size);
}
static int rp_init(void) {
    rpmalloc_initialize(NULL);
    return 0;
}
static void rp_cleanup(void) {
    rpmalloc_finalize();
}

void rpmalloc_allocator_init(allocator_api_t* api) {
    memset(api, 0, sizeof(allocator_api_t));
    api->malloc = rp_malloc;
    api->calloc = rp_calloc;
    api->realloc = rp_realloc;
    api->free = rp_free;
    api->aligned_alloc = rp_aligned_alloc;
    api->aligned_free = rp_free;
    api->init = rp_init;
    api->cleanup = rp_cleanup;
    api->name = "rpmalloc";
}
#else
void rpmalloc_allocator_init(allocator_api_t* api) {
    memset(api, 0, sizeof(allocator_api_t));
    api->name = "rpmalloc (unavailable)";
}
#endif

#ifdef HAVE_JEMALLOC
#define JEMALLOC_NO_DEMANGLE
#include <jemalloc/jemalloc.h>

static void* je_malloc_wrapper(size_t size) { return je_malloc(size); }
static void* je_calloc_wrapper(size_t num, size_t size) { return je_calloc(num, size); }
static void* je_realloc_wrapper(void* ptr, size_t size) { return je_realloc(ptr, size); }
static void je_free_wrapper(void* ptr) { je_free(ptr); }
static void* je_aligned_alloc_wrapper(size_t alignment, size_t size) {
    return je_aligned_alloc(alignment, size);
}

void jemalloc_allocator_init(allocator_api_t* api) {
    memset(api, 0, sizeof(allocator_api_t));
    api->malloc = je_malloc_wrapper;
    api->calloc = je_calloc_wrapper;
    api->realloc = je_realloc_wrapper;
    api->free = je_free_wrapper;
    api->aligned_alloc = je_aligned_alloc_wrapper;
    api->aligned_free = je_free_wrapper;
    api->name = "jemalloc";
}
#else
void jemalloc_allocator_init(allocator_api_t* api) {
    memset(api, 0, sizeof(allocator_api_t));
    api->name = "jemalloc (unavailable)";
}
#endif

#ifdef HAVE_MIMALLOC
#include <mimalloc.h>

static void* mi_malloc_wrapper(size_t size) { return mi_malloc(size); }
static void* mi_calloc_wrapper(size_t num, size_t size) { return mi_calloc(num, size); }
static void* mi_realloc_wrapper(void* ptr, size_t size) { return mi_realloc(ptr, size); }
static void mi_free_wrapper(void* ptr) { mi_free(ptr); }
static void* mi_aligned_alloc_wrapper(size_t alignment, size_t size) {
    return mi_aligned_alloc(alignment, size);
}

void mimalloc_allocator_init(allocator_api_t* api) {
    memset(api, 0, sizeof(allocator_api_t));
    api->malloc = mi_malloc_wrapper;
    api->calloc = mi_calloc_wrapper;
    api->realloc = mi_realloc_wrapper;
    api->free = mi_free_wrapper;
    api->aligned_alloc = mi_aligned_alloc_wrapper;
    api->aligned_free = mi_free_wrapper;
    api->name = "mimalloc";
}
#else
void mimalloc_allocator_init(allocator_api_t* api) {
    memset(api, 0, sizeof(allocator_api_t));
    api->name = "mimalloc (unavailable)";
}
#endif

#ifdef HAVE_TCMALLOC
#include <gperftools/tcmalloc.h>

static void* tc_malloc_wrapper(size_t size) { return tc_malloc(size); }
static void* tc_calloc_wrapper(size_t num, size_t size) { return tc_calloc(num, size); }
static void* tc_realloc_wrapper(void* ptr, size_t size) { return tc_realloc(ptr, size); }
static void tc_free_wrapper(void* ptr) { tc_free(ptr); }

void tcmalloc_allocator_init(allocator_api_t* api) {
    memset(api, 0, sizeof(allocator_api_t));
    api->malloc = tc_malloc_wrapper;
    api->calloc = tc_calloc_wrapper;
    api->realloc = tc_realloc_wrapper;
    api->free = tc_free_wrapper;
    api->name = "tcmalloc";
}
#else
void tcmalloc_allocator_init(allocator_api_t* api) {
    memset(api, 0, sizeof(allocator_api_t));
    api->name = "tcmalloc (unavailable)";
}
#endif
