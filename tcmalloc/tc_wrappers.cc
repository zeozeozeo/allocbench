#include <stddef.h>

extern "C" {
void* TCMallocInternalMalloc(size_t size);
void* TCMallocInternalCalloc(size_t n, size_t size);
void* TCMallocInternalRealloc(void* ptr, size_t size);
void TCMallocInternalFree(void* ptr);

void* tc_malloc(size_t size) {
    return TCMallocInternalMalloc(size);
}

void* tc_calloc(size_t n, size_t size) {
    return TCMallocInternalCalloc(n, size);
}

void* tc_realloc(void* ptr, size_t size) {
    return TCMallocInternalRealloc(ptr, size);
}

void tc_free(void* ptr) {
    TCMallocInternalFree(ptr);
}
}
