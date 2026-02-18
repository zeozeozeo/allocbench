#ifndef VECTOR_H
#define VECTOR_H

#include <stddef.h>
#include "../allocator_api.h"

typedef struct {
    void** data;
    size_t size;
    size_t capacity;
    size_t element_size;
    allocator_api_t* allocator;
} vector_t;

int vector_init(vector_t* vec, size_t element_size, size_t initial_capacity, 
                allocator_api_t* allocator);
void vector_destroy(vector_t* vec);
int vector_push_back(vector_t* vec, const void* element);
int vector_pop_back(vector_t* vec, void* out_element);
int vector_get(const vector_t* vec, size_t index, void* out_element);
int vector_set(vector_t* vec, size_t index, const void* element);
int vector_reserve(vector_t* vec, size_t new_capacity);
int vector_resize(vector_t* vec, size_t new_size);
void vector_clear(vector_t* vec);

#define VECTOR_FOREACH(vec, idx, elem_ptr) \
    for (size_t idx = 0; idx < (vec)->size && ((elem_ptr) = (vec)->data + idx * (vec)->element_size); idx++)

#endif
