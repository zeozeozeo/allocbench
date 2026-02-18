#ifndef STACK_H
#define STACK_H

#include <stddef.h>
#include "../allocator_api.h"

typedef struct stack_node {
    void* data;
    struct stack_node* next;
} stack_node_t;

typedef struct {
    stack_node_t* top;
    size_t size;
    allocator_api_t* allocator;
} stack_t;

int stack_init(stack_t* stack, allocator_api_t* allocator);
void stack_destroy(stack_t* stack);

int stack_push(stack_t* stack, void* data);
void* stack_pop(stack_t* stack);
void* stack_peek(const stack_t* stack);

size_t stack_size(const stack_t* stack);
int stack_is_empty(const stack_t* stack);
void stack_clear(stack_t* stack);

#endif
