#include "stack.h"

int stack_init(stack_t* stack, allocator_api_t* allocator) {
    if (!stack || !allocator) return -1;
    
    stack->top = NULL;
    stack->size = 0;
    stack->allocator = allocator;
    return 0;
}

void stack_destroy(stack_t* stack) {
    if (!stack) return;
    
    stack_clear(stack);
}

int stack_push(stack_t* stack, void* data) {
    if (!stack) return -1;
    
    stack_node_t* node = stack->allocator->malloc(sizeof(stack_node_t));
    if (!node) return -1;
    
    node->data = data;
    node->next = stack->top;
    stack->top = node;
    stack->size++;
    
    return 0;
}

void* stack_pop(stack_t* stack) {
    if (!stack || !stack->top) return NULL;
    
    stack_node_t* node = stack->top;
    void* data = node->data;
    
    stack->top = node->next;
    stack->allocator->free(node);
    stack->size--;
    
    return data;
}

void* stack_peek(const stack_t* stack) {
    if (!stack || !stack->top) return NULL;
    return stack->top->data;
}

size_t stack_size(const stack_t* stack) {
    return stack ? stack->size : 0;
}

int stack_is_empty(const stack_t* stack) {
    return stack ? stack->size == 0 : 1;
}

void stack_clear(stack_t* stack) {
    if (!stack) return;
    
    while (stack->top) {
        stack_node_t* next = stack->top->next;
        stack->allocator->free(stack->top);
        stack->top = next;
    }
    
    stack->size = 0;
}
