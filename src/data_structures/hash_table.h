#ifndef HASH_TABLE_H
#define HASH_TABLE_H

#include <stddef.h>
#include "../allocator_api.h"

#define HASH_TABLE_DEFAULT_CAPACITY 64
#define HASH_TABLE_LOAD_FACTOR 0.75

typedef struct hash_table_entry {
    void* key;
    void* value;
    struct hash_table_entry* next;
} hash_table_entry_t;

typedef unsigned long (*hash_func_t)(const void* key);
typedef int (*key_equal_func_t)(const void* a, const void* b);

typedef struct {
    hash_table_entry_t** buckets;
    size_t capacity;
    size_t size;
    hash_func_t hash;
    key_equal_func_t key_equal;
    allocator_api_t* allocator;
} hash_table_t;

int hash_table_init(hash_table_t* table, hash_func_t hash, key_equal_func_t key_equal,
                    allocator_api_t* allocator);
int hash_table_init_with_capacity(hash_table_t* table, hash_func_t hash, 
                                   key_equal_func_t key_equal, size_t capacity,
                                   allocator_api_t* allocator);
void hash_table_destroy(hash_table_t* table);

int hash_table_insert(hash_table_t* table, void* key, void* value);
void* hash_table_remove(hash_table_t* table, const void* key);
void* hash_table_get(hash_table_t* table, const void* key);
int hash_table_contains(hash_table_t* table, const void* key);

size_t hash_table_size(const hash_table_t* table);
int hash_table_is_empty(const hash_table_t* table);
void hash_table_clear(hash_table_t* table);

unsigned long hash_string(const void* key);
unsigned long hash_int(const void* key);
int key_equal_string(const void* a, const void* b);
int key_equal_int(const void* a, const void* b);

#endif
