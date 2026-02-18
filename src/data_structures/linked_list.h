#ifndef LINKED_LIST_H
#define LINKED_LIST_H

#include <stddef.h>
#include "../allocator_api.h"

typedef struct linked_list_node {
    void* data;
    struct linked_list_node* next;
    struct linked_list_node* prev;
} linked_list_node_t;

typedef struct {
    linked_list_node_t* head;
    linked_list_node_t* tail;
    size_t size;
    allocator_api_t* allocator;
} linked_list_t;

int linked_list_init(linked_list_t* list, allocator_api_t* allocator);
void linked_list_destroy(linked_list_t* list);

int linked_list_push_front(linked_list_t* list, void* data);
int linked_list_push_back(linked_list_t* list, void* data);
void* linked_list_pop_front(linked_list_t* list);
void* linked_list_pop_back(linked_list_t* list);

linked_list_node_t* linked_list_insert_after(linked_list_t* list, linked_list_node_t* node, void* data);
linked_list_node_t* linked_list_insert_before(linked_list_t* list, linked_list_node_t* node, void* data);
void linked_list_remove(linked_list_t* list, linked_list_node_t* node);

linked_list_node_t* linked_list_find(linked_list_t* list, const void* data);
void* linked_list_get_at(linked_list_t* list, size_t index);
size_t linked_list_size(const linked_list_t* list);
int linked_list_is_empty(const linked_list_t* list);

#endif
