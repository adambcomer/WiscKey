#include <assert.h>
#include <string.h>

#include "../src/include/memtable.h"

#define MemTableRecord_key_value_assert(r, k, k_len, v_loc) \
    {                                                              \
        assert(r->key_len == k_len);                               \
        assert(memcmp(r->key, k, k_len) == 0);                     \
        assert(r->value_loc == v_loc);                             \
    }

void TestMemTable_new()
{
    struct MemTable *m = MemTable_new();

    assert(m->size == 0);

    MemTable_free(m);
}

void TestMemTable_set_start()
{
    struct MemTable *m = MemTable_new();

    char *key1 = "lime";
    char *value1 = "Key Lime Pie";
    size_t value1_offset = 0;

    MemTable_set(m, key1, strlen(key1) + 1, value1_offset);

    assert(m->size == 1);

    MemTableRecord_key_value_assert(m->records[0], key1, strlen(key1) + 1, value1_offset);

    char *key2 = "cherry";
    char *value2 = "Cherry Pie";
    size_t value2_offset = strlen(value1) + 1;

    MemTable_set(m, key2, strlen(key2) + 1, value2_offset);

    assert(m->size == 2);

    MemTableRecord_key_value_assert(m->records[0], key2, strlen(key2) + 1, value2_offset);
    MemTableRecord_key_value_assert(m->records[1], key1, strlen(key1) + 1, value1_offset);

    char *key3 = "apple";
    char *value3 = "Apple Pie";
    size_t value3_offset = strlen(value1) + 1 + strlen(value2) + 1;

    MemTable_set(m, key3, strlen(key3) + 1, value3_offset);

    assert(m->size == 3);

    MemTableRecord_key_value_assert(m->records[0], key3, strlen(key3) + 1, value3_offset);
    MemTableRecord_key_value_assert(m->records[1], key2, strlen(key2) + 1, value2_offset);
    MemTableRecord_key_value_assert(m->records[2], key1, strlen(key1) + 1, value1_offset);

    MemTable_free(m);
}

void TestMemTable_set_end()
{
    struct MemTable *m = MemTable_new();

    char *key1 = strdup("apple");
    char *value1 = strdup("Apple Pie");
    size_t value1_offset = 0;

    MemTable_set(m, key1, strlen(key1) + 1, value1_offset);

    assert(m->size == 1);

    MemTableRecord_key_value_assert(m->records[0], key1, strlen(key1) + 1, value1_offset);

    char *key2 = strdup("cherry");
    char *value2 = strdup("Cherry Pie");
    size_t value2_offset = strlen(value1) + 1;

    MemTable_set(m, key2, strlen(key2) + 1, value2_offset);

    assert(m->size == 2);

    MemTableRecord_key_value_assert(m->records[0], key1, strlen(key1) + 1, value1_offset);
    MemTableRecord_key_value_assert(m->records[1], key2, strlen(key2) + 1, value2_offset);

    char *key3 = strdup("lime");
    char *value3 = strdup("Key Lime Pie");
    size_t value3_offset = strlen(value1) + 1 + strlen(value2) + 1;

    MemTable_set(m, key3, strlen(key3) + 1, value3_offset);

    assert(m->size == 3);

    MemTableRecord_key_value_assert(m->records[0], key1, strlen(key1) + 1, value1_offset);
    MemTableRecord_key_value_assert(m->records[1], key2, strlen(key2) + 1, value2_offset);
    MemTableRecord_key_value_assert(m->records[2], key3, strlen(key3) + 1, value3_offset);

    MemTable_free(m);
}

void TestMemTable_set_overwrite()
{
    struct MemTable *m = MemTable_new();

    char *key1 = "apple";
    char *value1 = "Apple Pie";
    size_t value1_offset = 0;

    MemTable_set(m, key1, strlen(key1) + 1, value1_offset);

    assert(m->size == 1);

    MemTableRecord_key_value_assert(m->records[0], key1, strlen(key1) + 1, value1_offset);

    char *key2 = "apple";
    char *value2 = "Apple Crisp";
    size_t value2_offset = strlen(value1) + 1;

    MemTable_set(m, key2, strlen(key2) + 1, value2_offset);

    assert(m->size == 1);

    MemTableRecord_key_value_assert(m->records[0], key1, strlen(key1) + 1, value2_offset);

    MemTable_free(m);
}

void TestMemTable_delete_empty()
{
    struct MemTable *m = MemTable_new();

    char *key1 = "apple";

    MemTable_delete(m, key1, strlen(key1) + 1);

    MemTableRecord_key_value_assert(m->records[0], key1, strlen(key1) + 1, -1);

    MemTable_free(m);
}

void TestMemTable_delete_remove()
{
    struct MemTable *m = MemTable_new();

    char *key = "apple";
    char *value = "Apple Pie";
    size_t value_offset = 0;

    MemTable_set(m, key, strlen(key) + 1, value_offset);

    assert(m->size == 1);

    MemTableRecord_key_value_assert(m->records[0], key, strlen(key) + 1, value_offset);

    MemTable_delete(m, key, strlen(key) + 1);

    MemTableRecord_key_value_assert(m->records[0], key, strlen(key) + 1, -1);

    MemTable_free(m);
}

void TestMemTable_get()
{
    struct MemTable *m = MemTable_new();

    char *key = "apple";
    char *value = "Apple Pie";
    size_t value_offset = 0;

    struct MemTableRecord *r1 = MemTable_get(m, key, strlen(key) + 1);

    assert(r1 == NULL);

    MemTable_set(m, key, strlen(key) + 1, value_offset);

    struct MemTableRecord *r2 = MemTable_get(m, key, strlen(key) + 1);

    MemTableRecord_key_value_assert(r2, key, strlen(key) + 1, 0);

    MemTable_free(m);
}

int main()
{
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