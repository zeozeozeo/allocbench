#ifndef BINARY_TREE_H
#define BINARY_TREE_H

#include <stddef.h>
#include "../allocator_api.h"

typedef struct binary_tree_node {
    void* key;
    void* value;
    struct binary_tree_node* left;
    struct binary_tree_node* right;
    struct binary_tree_node* parent;
} binary_tree_node_t;

typedef int (*compare_func_t)(const void* a, const void* b);

typedef struct {
    binary_tree_node_t* root;
    size_t size;
    compare_func_t compare;
    allocator_api_t* allocator;
} binary_tree_t;

int binary_tree_init(binary_tree_t* tree, compare_func_t compare, allocator_api_t* allocator);
void binary_tree_destroy(binary_tree_t* tree);

binary_tree_node_t* binary_tree_insert(binary_tree_t* tree, void* key, void* value);
void* binary_tree_remove(binary_tree_t* tree, void* key);
binary_tree_node_t* binary_tree_find(binary_tree_t* tree, const void* key);
void* binary_tree_get(binary_tree_t* tree, const void* key);

binary_tree_node_t* binary_tree_min(binary_tree_t* tree);
binary_tree_node_t* binary_tree_max(binary_tree_t* tree);
binary_tree_node_t* binary_tree_successor(binary_tree_node_t* node);
binary_tree_node_t* binary_tree_predecessor(binary_tree_node_t* node);

size_t binary_tree_size(const binary_tree_t* tree);
size_t binary_tree_height(const binary_tree_t* tree);
int binary_tree_is_empty(const binary_tree_t* tree);

void binary_tree_inorder_traverse(binary_tree_t* tree, void (*visit)(binary_tree_node_t* node, void* ctx), void* ctx);
void binary_tree_preorder_traverse(binary_tree_t* tree, void (*visit)(binary_tree_node_t* node, void* ctx), void* ctx);
void binary_tree_postorder_traverse(binary_tree_t* tree, void (*visit)(binary_tree_node_t* node, void* ctx), void* ctx);

#endif
