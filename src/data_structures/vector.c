#include "vector.h"
#include <string.h>

int vector_init(vector_t* vec, size_t element_size, size_t initial_capacity,
                allocator_api_t* allocator) {
    if (!vec || !allocator || element_size == 0) return -1;
    
    vec->element_size = element_size;
    vec->size = 0;
    vec->capacity = initial_capacity > 0 ? initial_capacity : 8;
    vec->allocator = allocator;
    
    vec->data = allocator->malloc(vec->capacity * vec->element_size);
    if (!vec->data) {
        vec->capacity = 0;
        return -1;
    }
    
    return 0;
}

void vector_destroy(vector_t* vec) {
    if (!vec) return;
    if (vec->data && vec->allocator) {
        vec->allocator->free(vec->data);
    }
    vec->data = NULL;
    vec->size = 0;
    vec->capacity = 0;
}

int vector_reserve(vector_t* vec, size_t new_capacity) {
    if (!vec || new_capacity <= vec->capacity) return -1;
    
    void* new_data = vec->allocator->realloc(vec->data, new_capacity * vec->element_size);
    if (!new_data) return -1;
    
    vec->data = new_data;
    vec->capacity = new_capacity;
    return 0;
}

int vector_resize(vector_t* vec, size_t new_size) {
    if (!vec) return -1;
    
    if (new_size > vec->capacity) {
        size_t new_capacity = vec->capacity * 2;
        while (new_capacity < new_size) new_capacity *= 2;
        
        if (vector_reserve(vec, new_capacity) != 0) return -1;
    }
    
    vec->size = new_size;
    return 0;
}

int vector_push_back(vector_t* vec, const void* element) {
    if (!vec || !element) return -1;
    
    if (vec->size >= vec->capacity) {
        size_t new_capacity = vec->capacity * 2;
        if (vector_reserve(vec, new_capacity) != 0) return -1;
    }
    
    memcpy((char*)vec->data + vec->size * vec->element_size, element, vec->element_size);
    vec->size++;
    return 0;
}

int vector_pop_back(vector_t* vec, void* out_element) {
    if (!vec || vec->size == 0) return -1;
    
    vec->size--;
    if (out_element) {
        memcpy(out_element, (char*)vec->data + vec->size * vec->element_size, vec->element_size);
    }
    return 0;
}

int vector_get(const vector_t* vec, size_t index, void* out_element) {
    if (!vec || index >= vec->size || !out_element) return -1;
    
    memcpy(out_element, (char*)vec->data + index * vec->element_size, vec->element_size);
    return 0;
}

int vector_set(vector_t* vec, size_t index, const void* element) {
    if (!vec || index >= vec->size || !element) return -1;
    
    memcpy((char*)vec->data + index * vec->element_size, element, vec->element_size);
    return 0;
}

void vector_clear(vector_t* vec) {
    if (vec) vec->size = 0;
}
