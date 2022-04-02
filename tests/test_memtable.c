#include <assert.h>
#include <string.h>

#include "../src/include/memtable.h"


void TestMemTable_new() {
    struct MemTable *m = MemTable_new();

    assert(m->size == 0);

    MemTable_free(m);
}

void TestMemTable_set_start() {
    struct MemTable *m = MemTable_new();

    char *key1 = "lime";
    char *value1 = "Key Lime Pie";
    long long value1_offset = 0;

    MemTable_set(m, key1, strlen(key1) + 1, value1_offset);

    assert(m->size == 1);

    assert(m->records[0]->key_len == strlen(key1) + 1);
    assert(memcmp(m->records[0]->key, key1, strlen(key1) + 1) == 0);
    assert(m->records[0]->value_loc == value1_offset);

    char *key2 = "cherry";
    char *value2 = "Cherry Pie";
    long long value2_offset = (long long) strlen(value1) + 1;

    MemTable_set(m, key2, strlen(key2) + 1, (long long) value2_offset);

    assert(m->size == 2);

    assert(m->records[0]->key_len == strlen(key2) + 1);
    assert(memcmp(m->records[0]->key, key2, strlen(key2) + 1) == 0);
    assert(m->records[0]->value_loc == value2_offset);

    assert(m->records[1]->key_len == strlen(key1) + 1);
    assert(memcmp(m->records[1]->key, key1, strlen(key1) + 1) == 0);
    assert(m->records[1]->value_loc == value1_offset);

    char *key3 = "apple";
    long long value3_offset = (long long) strlen(value1) + 1 + (long long) strlen(value2) + 1;

    MemTable_set(m, key3, strlen(key3) + 1, (long long) value3_offset);

    assert(m->size == 3);

    assert(m->records[0]->key_len == strlen(key3) + 1);
    assert(memcmp(m->records[0]->key, key3, strlen(key3) + 1) == 0);
    assert(m->records[0]->value_loc == value3_offset);

    assert(m->records[1]->key_len == strlen(key2) + 1);
    assert(memcmp(m->records[1]->key, key2, strlen(key2) + 1) == 0);
    assert(m->records[1]->value_loc == value2_offset);

    assert(m->records[2]->key_len == strlen(key1) + 1);
    assert(memcmp(m->records[2]->key, key1, strlen(key1) + 1) == 0);
    assert(m->records[2]->value_loc == value1_offset);

    MemTable_free(m);
}

void TestMemTable_set_end() {
    struct MemTable *m = MemTable_new();

    char *key1 = "apple";
    char *value1 = "Apple Pie";
    long long value1_offset = 0;

    MemTable_set(m, key1, strlen(key1) + 1, (long long) value1_offset);

    assert(m->size == 1);

    assert(m->records[0]->key_len == strlen(key1) + 1);
    assert(memcmp(m->records[0]->key, key1, strlen(key1) + 1) == 0);
    assert(m->records[0]->value_loc == value1_offset);

    char *key2 = "cherry";
    char *value2 = "Cherry Pie";
    long long value2_offset = (long long) strlen(value1) + 1;

    MemTable_set(m, key2, strlen(key2) + 1, (long long) value2_offset);

    assert(m->size == 2);

    assert(m->records[0]->key_len == strlen(key1) + 1);
    assert(memcmp(m->records[0]->key, key1, strlen(key1) + 1) == 0);
    assert(m->records[0]->value_loc == value1_offset);

    assert(m->records[1]->key_len == strlen(key2) + 1);
    assert(memcmp(m->records[1]->key, key2, strlen(key2) + 1) == 0);
    assert(m->records[1]->value_loc == value2_offset);

    char *key3 = "lime";
    long long value3_offset = (long long) strlen(value1) + 1 + (long long) strlen(value2) + 1;

    MemTable_set(m, key3, strlen(key3) + 1, (long long) value3_offset);

    assert(m->size == 3);

    assert(m->records[0]->key_len == strlen(key1) + 1);
    assert(memcmp(m->records[0]->key, key1, strlen(key1) + 1) == 0);
    assert(m->records[0]->value_loc == value1_offset);

    assert(m->records[1]->key_len == strlen(key2) + 1);
    assert(memcmp(m->records[1]->key, key2, strlen(key2) + 1) == 0);
    assert(m->records[1]->value_loc == value2_offset);

    assert(m->records[2]->key_len == strlen(key3) + 1);
    assert(memcmp(m->records[2]->key, key3, strlen(key3) + 1) == 0);
    assert(m->records[2]->value_loc == value3_offset);

    MemTable_free(m);
}

void TestMemTable_set_overwrite() {
    struct MemTable *m = MemTable_new();

    char *key1 = "apple";
    char *value1 = "Apple Pie";
    long long value1_offset = 0;

    MemTable_set(m, key1, strlen(key1) + 1, value1_offset);

    assert(m->size == 1);

    assert(m->records[0]->key_len == strlen(key1) + 1);
    assert(memcmp(m->records[0]->key, key1, strlen(key1) + 1) == 0);
    assert(m->records[0]->value_loc == value1_offset);

    char *key2 = "apple";
    long long value2_offset = (long long) strlen(value1) + 1;

    MemTable_set(m, key2, strlen(key2) + 1, value2_offset);

    assert(m->size == 1);

    assert(m->records[0]->key_len == strlen(key1) + 1);
    assert(memcmp(m->records[0]->key, key1, strlen(key1) + 1) == 0);
    assert(m->records[0]->value_loc == value2_offset);

    MemTable_free(m);
}

void TestMemTable_delete_empty() {
    struct MemTable *m = MemTable_new();

    char *key1 = "apple";

    MemTable_delete(m, key1, strlen(key1) + 1);

    assert(m->records[0]->key_len == strlen(key1) + 1);
    assert(memcmp(m->records[0]->key, key1, strlen(key1) + 1) == 0);
    assert(m->records[0]->value_loc == -1);

    MemTable_free(m);
}

void TestMemTable_delete_remove() {
    struct MemTable *m = MemTable_new();

    char *key = "apple";
    long long value_offset = 0;

    MemTable_set(m, key, strlen(key) + 1, value_offset);

    assert(m->size == 1);

    assert(m->records[0]->key_len == strlen(key) + 1);
    assert(memcmp(m->records[0]->key, key, strlen(key) + 1) == 0);
    assert(m->records[0]->value_loc == value_offset);

    MemTable_delete(m, key, strlen(key) + 1);

    assert(m->records[0]->key_len == strlen(key) + 1);
    assert(memcmp(m->records[0]->key, key, strlen(key) + 1) == 0);
    assert(m->records[0]->value_loc == -1);

    MemTable_free(m);
}

void TestMemTable_get() {
    struct MemTable *m = MemTable_new();

    char *key = "apple";
    size_t value_offset = 0;

    struct MemTableRecord *r1 = MemTable_get(m, key, strlen(key) + 1);

    assert(r1 == NULL);

    MemTable_set(m, key, strlen(key) + 1, (long long) value_offset);

    struct MemTableRecord *r2 = MemTable_get(m, key, strlen(key) + 1);

    assert(r2 != NULL);

    assert(r2->key_len == strlen(key) + 1);
    assert(memcmp(r2->key, key, strlen(key) + 1) == 0);
    assert(r2->value_loc == 0);

    MemTable_free(m);
}

int main() {
    // New
    TestMemTable_new();

    // Set
    TestMemTable_set_start();
    TestMemTable_set_end();
    TestMemTable_set_overwrite();

    // Delete
    TestMemTable_delete_empty();
    TestMemTable_delete_remove();

    // Get
    TestMemTable_get();

    return 0;
}