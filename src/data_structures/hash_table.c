#include "hash_table.h"
#include <string.h>
#include <stdint.h>

static int ensure_capacity(hash_table_t* table) {
    if ((double)table->size / (double)table->capacity < HASH_TABLE_LOAD_FACTOR) {
        return 0;
    }

    size_t new_capacity = table->capacity * 2;
    hash_table_entry_t** new_buckets = table->allocator->calloc(new_capacity, sizeof(hash_table_entry_t*));
    if (!new_buckets) return -1;

    for (size_t i = 0; i < table->capacity; i++) {
        hash_table_entry_t* entry = table->buckets[i];
        while (entry) {
            hash_table_entry_t* next = entry->next;

            unsigned long new_index = table->hash(entry->key) % new_capacity;
            entry->next = new_buckets[new_index];
            new_buckets[new_index] = entry;

            entry = next;
        }
    }

    table->allocator->free(table->buckets);
    table->buckets = new_buckets;
    table->capacity = new_capacity;

    return 0;
}

int hash_table_init(hash_table_t* table, hash_func_t hash, key_equal_func_t key_equal,
                    allocator_api_t* allocator) {
    return hash_table_init_with_capacity(table, hash, key_equal, HASH_TABLE_DEFAULT_CAPACITY, allocator);
}

int hash_table_init_with_capacity(hash_table_t* table, hash_func_t hash,
                                   key_equal_func_t key_equal, size_t capacity,
                                   allocator_api_t* allocator) {
    if (!table || !hash || !key_equal || !allocator) return -1;

    table->buckets = allocator->calloc(capacity, sizeof(hash_table_entry_t*));
    if (!table->buckets) return -1;

    table->capacity = capacity;
    table->size = 0;
    table->hash = hash;
    table->key_equal = key_equal;
    table->allocator = allocator;

    return 0;
}

void hash_table_destroy(hash_table_t* table) {
    if (!table) return;

    for (size_t i = 0; i < table->capacity; i++) {
        hash_table_entry_t* entry = table->buckets[i];
        while (entry) {
            hash_table_entry_t* next = entry->next;
            table->allocator->free(entry);
            entry = next;
        }
    }

    table->allocator->free(table->buckets);
    table->buckets = NULL;
    table->size = 0;
}

int hash_table_insert(hash_table_t* table, void* key, void* value) {
    if (!table) return -1;

    unsigned long index = table->hash(key) % table->capacity;

    hash_table_entry_t* entry = table->buckets[index];
    while (entry) {
        if (table->key_equal(entry->key, key)) {
            entry->value = value;
            return 0;
        }
        entry = entry->next;
    }

    if (ensure_capacity(table) != 0) return -1;

    index = table->hash(key) % table->capacity;

    hash_table_entry_t* new_entry = table->allocator->malloc(sizeof(hash_table_entry_t));
    if (!new_entry) return -1;

    new_entry->key = key;
    new_entry->value = value;
    new_entry->next = table->buckets[index];
    table->buckets[index] = new_entry;
    table->size++;

    return 0;
}

void* hash_table_remove(hash_table_t* table, const void* key) {
    if (!table) return NULL;

    unsigned long index = table->hash(key) % table->capacity;

    hash_table_entry_t* entry = table->buckets[index];
    hash_table_entry_t* prev = NULL;

    while (entry) {
        if (table->key_equal(entry->key, key)) {
            void* value = entry->value;

            if (prev) {
                prev->next = entry->next;
            } else {
                table->buckets[index] = entry->next;
            }

            table->allocator->free(entry);
            table->size--;
            return value;
        }

        prev = entry;
        entry = entry->next;
    }

    return NULL;
}

void* hash_table_get(hash_table_t* table, const void* key) {
    if (!table) return NULL;

    unsigned long index = table->hash(key) % table->capacity;
    hash_table_entry_t* entry = table->buckets[index];

    while (entry) {
        if (table->key_equal(entry->key, key)) {
            return entry->value;
        }
        entry = entry->next;
    }

    return NULL;
}

int hash_table_contains(hash_table_t* table, const void* key) {
    return hash_table_get(table, key) != NULL;
}

size_t hash_table_size(const hash_table_t* table) {
    return table ? table->size : 0;
}

int hash_table_is_empty(const hash_table_t* table) {
    return table ? table->size == 0 : 1;
}

void hash_table_clear(hash_table_t* table) {
    if (!table) return;

    for (size_t i = 0; i < table->capacity; i++) {
        hash_table_entry_t* entry = table->buckets[i];
        while (entry) {
            hash_table_entry_t* next = entry->next;
            table->allocator->free(entry);
            entry = next;
        }
        table->buckets[i] = NULL;
    }

    table->size = 0;
}

unsigned long hash_string(const void* key) {
    const char* str = (const char*)key;
    unsigned long hash = 5381;
    int c;

    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }

    return hash;
}

unsigned long hash_int(const void* key) {
    return (unsigned long)(uintptr_t)key;
}

int key_equal_string(const void* a, const void* b) {
    return strcmp((const char*)a, (const char*)b) == 0;
}

int key_equal_int(const void* a, const void* b) {
    return a == b;
}
