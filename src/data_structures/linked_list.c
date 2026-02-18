#include "linked_list.h"

int linked_list_init(linked_list_t* list, allocator_api_t* allocator) {
    if (!list || !allocator) return -1;

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
    list->allocator = allocator;
    return 0;
}

void linked_list_destroy(linked_list_t* list) {
    if (!list) return;

    linked_list_node_t* current = list->head;
    while (current) {
        linked_list_node_t* next = current->next;
        list->allocator->free(current);
        current = next;
    }

    list->head = NULL;
    list->tail = NULL;
    list->size = 0;
}

int linked_list_push_front(linked_list_t* list, void* data) {
    if (!list) return -1;

    linked_list_node_t* node = list->allocator->malloc(sizeof(linked_list_node_t));
    if (!node) return -1;

    node->data = data;
    node->prev = NULL;
    node->next = list->head;

    if (list->head) {
        list->head->prev = node;
    } else {
        list->tail = node;
    }

    list->head = node;
    list->size++;
    return 0;
}

int linked_list_push_back(linked_list_t* list, void* data) {
    if (!list) return -1;

    linked_list_node_t* node = list->allocator->malloc(sizeof(linked_list_node_t));
    if (!node) return -1;

    node->data = data;
    node->next = NULL;
    node->prev = list->tail;

    if (list->tail) {
        list->tail->next = node;
    } else {
        list->head = node;
    }

    list->tail = node;
    list->size++;
    return 0;
}

void* linked_list_pop_front(linked_list_t* list) {
    if (!list || !list->head) return NULL;

    linked_list_node_t* node = list->head;
    void* data = node->data;

    list->head = node->next;
    if (list->head) {
        list->head->prev = NULL;
    } else {
        list->tail = NULL;
    }

    list->allocator->free(node);
    list->size--;
    return data;
}

void* linked_list_pop_back(linked_list_t* list) {
    if (!list || !list->tail) return NULL;

    linked_list_node_t* node = list->tail;
    void* data = node->data;

    list->tail = node->prev;
    if (list->tail) {
        list->tail->next = NULL;
    } else {
        list->head = NULL;
    }

    list->allocator->free(node);
    list->size--;
    return data;
}

linked_list_node_t* linked_list_insert_after(linked_list_t* list, linked_list_node_t* node, void* data) {
    if (!list) return NULL;

    if (!node) {
        linked_list_push_front(list, data);
        return list->head;
    }

    linked_list_node_t* new_node = list->allocator->malloc(sizeof(linked_list_node_t));
    if (!new_node) return NULL;

    new_node->data = data;
    new_node->prev = node;
    new_node->next = node->next;

    if (node->next) {
        node->next->prev = new_node;
    } else {
        list->tail = new_node;
    }

    node->next = new_node;
    list->size++;
    return new_node;
}

linked_list_node_t* linked_list_insert_before(linked_list_t* list, linked_list_node_t* node, void* data) {
    if (!list) return NULL;

    if (!node) {
        linked_list_push_back(list, data);
        return list->tail;
    }

    linked_list_node_t* new_node = list->allocator->malloc(sizeof(linked_list_node_t));
    if (!new_node) return NULL;

    new_node->data = data;
    new_node->next = node;
    new_node->prev = node->prev;

    if (node->prev) {
        node->prev->next = new_node;
    } else {
        list->head = new_node;
    }

    node->prev = new_node;
    list->size++;
    return new_node;
}

void linked_list_remove(linked_list_t* list, linked_list_node_t* node) {
    if (!list || !node) return;

    if (node->prev) {
        node->prev->next = node->next;
    } else {
        list->head = node->next;
    }

    if (node->next) {
        node->next->prev = node->prev;
    } else {
        list->tail = node->prev;
    }

    list->allocator->free(node);
    list->size--;
}

linked_list_node_t* linked_list_find(linked_list_t* list, const void* data) {
    if (!list) return NULL;

    linked_list_node_t* current = list->head;
    while (current) {
        if (current->data == data) return current;
        current = current->next;
    }

    return NULL;
}

void* linked_list_get_at(linked_list_t* list, size_t index) {
    if (!list || index >= list->size) return NULL;

    linked_list_node_t* current = list->head;
    for (size_t i = 0; i < index && current; i++) {
        current = current->next;
    }

    return current ? current->data : NULL;
}

size_t linked_list_size(const linked_list_t* list) {
    return list ? list->size : 0;
}

int linked_list_is_empty(const linked_list_t* list) {
    return list ? list->size == 0 : 1;
}
