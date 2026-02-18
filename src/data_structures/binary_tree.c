#include "binary_tree.h"

int binary_tree_init(binary_tree_t* tree, compare_func_t compare, allocator_api_t* allocator) {
    if (!tree || !compare || !allocator) return -1;
    
    tree->root = NULL;
    tree->size = 0;
    tree->compare = compare;
    tree->allocator = allocator;
    return 0;
}

static void destroy_subtree(binary_tree_t* tree, binary_tree_node_t* node) {
    if (!node) return;
    destroy_subtree(tree, node->left);
    destroy_subtree(tree, node->right);
    tree->allocator->free(node);
}

void binary_tree_destroy(binary_tree_t* tree) {
    if (!tree) return;
    destroy_subtree(tree, tree->root);
    tree->root = NULL;
    tree->size = 0;
}

binary_tree_node_t* binary_tree_insert(binary_tree_t* tree, void* key, void* value) {
    if (!tree) return NULL;
    
    binary_tree_node_t* node = tree->allocator->malloc(sizeof(binary_tree_node_t));
    if (!node) return NULL;
    
    node->key = key;
    node->value = value;
    node->left = NULL;
    node->right = NULL;
    node->parent = NULL;
    
    if (!tree->root) {
        tree->root = node;
        tree->size++;
        return node;
    }
    
    binary_tree_node_t* current = tree->root;
    binary_tree_node_t* parent = NULL;
    
    while (current) {
        parent = current;
        int cmp = tree->compare(key, current->key);
        
        if (cmp < 0) {
            current = current->left;
        } else if (cmp > 0) {
            current = current->right;
        } else {
            current->value = value;
            tree->allocator->free(node);
            return current;
        }
    }
    
    node->parent = parent;
    if (tree->compare(key, parent->key) < 0) {
        parent->left = node;
    } else {
        parent->right = node;
    }
    
    tree->size++;
    return node;
}

binary_tree_node_t* binary_tree_find(binary_tree_t* tree, const void* key) {
    if (!tree) return NULL;
    
    binary_tree_node_t* current = tree->root;
    while (current) {
        int cmp = tree->compare(key, current->key);
        
        if (cmp < 0) {
            current = current->left;
        } else if (cmp > 0) {
            current = current->right;
        } else {
            return current;
        }
    }
    
    return NULL;
}

void* binary_tree_get(binary_tree_t* tree, const void* key) {
    binary_tree_node_t* node = binary_tree_find(tree, key);
    return node ? node->value : NULL;
}

static binary_tree_node_t* find_min(binary_tree_node_t* node) {
    while (node && node->left) {
        node = node->left;
    }
    return node;
}

static binary_tree_node_t* find_max(binary_tree_node_t* node) {
    while (node && node->right) {
        node = node->right;
    }
    return node;
}

binary_tree_node_t* binary_tree_min(binary_tree_t* tree) {
    return tree ? find_min(tree->root) : NULL;
}

binary_tree_node_t* binary_tree_max(binary_tree_t* tree) {
    return tree ? find_max(tree->root) : NULL;
}

binary_tree_node_t* binary_tree_successor(binary_tree_node_t* node) {
    if (!node) return NULL;
    
    if (node->right) {
        return find_min(node->right);
    }
    
    binary_tree_node_t* parent = node->parent;
    while (parent && node == parent->right) {
        node = parent;
        parent = parent->parent;
    }
    
    return parent;
}

binary_tree_node_t* binary_tree_predecessor(binary_tree_node_t* node) {
    if (!node) return NULL;
    
    if (node->left) {
        return find_max(node->left);
    }
    
    binary_tree_node_t* parent = node->parent;
    while (parent && node == parent->left) {
        node = parent;
        parent = parent->parent;
    }
    
    return parent;
}

static void transplant(binary_tree_t* tree, binary_tree_node_t* u, binary_tree_node_t* v) {
    if (!u->parent) {
        tree->root = v;
    } else if (u == u->parent->left) {
        u->parent->left = v;
    } else {
        u->parent->right = v;
    }
    
    if (v) {
        v->parent = u->parent;
    }
}

void* binary_tree_remove(binary_tree_t* tree, void* key) {
    if (!tree) return NULL;
    
    binary_tree_node_t* node = binary_tree_find(tree, key);
    if (!node) return NULL;
    
    void* value = node->value;
    
    if (!node->left) {
        transplant(tree, node, node->right);
    } else if (!node->right) {
        transplant(tree, node, node->left);
    } else {
        binary_tree_node_t* successor = find_min(node->right);
        
        if (successor->parent != node) {
            transplant(tree, successor, successor->right);
            successor->right = node->right;
            successor->right->parent = successor;
        }
        
        transplant(tree, node, successor);
        successor->left = node->left;
        successor->left->parent = successor;
    }
    
    tree->allocator->free(node);
    tree->size--;
    return value;
}

size_t binary_tree_size(const binary_tree_t* tree) {
    return tree ? tree->size : 0;
}

static size_t node_height(const binary_tree_node_t* node) {
    if (!node) return 0;
    
    size_t left_height = node_height(node->left);
    size_t right_height = node_height(node->right);
    
    return 1 + (left_height > right_height ? left_height : right_height);
}

size_t binary_tree_height(const binary_tree_t* tree) {
    return tree ? node_height(tree->root) : 0;
}

int binary_tree_is_empty(const binary_tree_t* tree) {
    return tree ? tree->size == 0 : 1;
}

static void inorder_traverse(binary_tree_node_t* node, void (*visit)(binary_tree_node_t*, void*), void* ctx) {
    if (!node) return;
    inorder_traverse(node->left, visit, ctx);
    visit(node, ctx);
    inorder_traverse(node->right, visit, ctx);
}

static void preorder_traverse(binary_tree_node_t* node, void (*visit)(binary_tree_node_t*, void*), void* ctx) {
    if (!node) return;
    visit(node, ctx);
    preorder_traverse(node->left, visit, ctx);
    preorder_traverse(node->right, visit, ctx);
}

static void postorder_traverse(binary_tree_node_t* node, void (*visit)(binary_tree_node_t*, void*), void* ctx) {
    if (!node) return;
    postorder_traverse(node->left, visit, ctx);
    postorder_traverse(node->right, visit, ctx);
    visit(node, ctx);
}

void binary_tree_inorder_traverse(binary_tree_t* tree, void (*visit)(binary_tree_node_t*, void*), void* ctx) {
    if (tree && visit) {
        inorder_traverse(tree->root, visit, ctx);
    }
}

void binary_tree_preorder_traverse(binary_tree_t* tree, void (*visit)(binary_tree_node_t*, void*), void* ctx) {
    if (tree && visit) {
        preorder_traverse(tree->root, visit, ctx);
    }
}

void binary_tree_postorder_traverse(binary_tree_t* tree, void (*visit)(binary_tree_node_t*, void*), void* ctx) {
    if (tree && visit) {
        postorder_traverse(tree->root, visit, ctx);
    }
}
