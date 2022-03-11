#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "include/memtable.h"

struct MemTableRecord *MemTableRecord_new(char *key, size_t key_len, size_t value_loc, size_t value_len)
{
    struct MemTableRecord *record = malloc(sizeof(struct MemTableRecord));

    record->key = malloc(key_len);
    memcpy(record->key, key, key_len);
    record->key_len = key_len;

    record->value_loc = value_loc;
    record->value_len = value_len;

    struct timespec timestamp;
    clock_gettime(CLOCK_MONOTONIC, &timestamp);
    record->timestamp = timestamp.tv_sec * 1000000 + (timestamp.tv_nsec / 1000);

    return record;
}

int MemTableRecord_key_cmp(const struct MemTableRecord *r, const char *key, const size_t key_len)
{
    size_t len = r->key_len < key_len ? r->key_len : key_len;

    int cmp = memcmp(key, r->key, len);
    if (cmp != 0 || r->key_len == key_len)
    {
        return cmp;
    }

    return key_len < r->key_len ? -1 : 1;
}

struct MemTable *MemTable_new()
{
    struct MemTable *memtable = malloc(sizeof(struct MemTable));
    memtable->size = 0;

    for (int i = 0; i < MEMTABLE_SIZE; i++)
    {
        memtable->records[i] = NULL;
    }

    return memtable;
}

int MemTable_binary_search(const struct MemTable *memtable, const char *key, const size_t key_len)
{
    if (memtable->size == 0)
    {
        return -1;
    }

    int a = 0;
    int b = memtable->size - 1;

    while (a < b)
    {
        int m = a + (b - a) / 2;

        int cmp = MemTableRecord_key_cmp(memtable->records[m], key, key_len);
        if (cmp == 0)
        {
            return m;
        }
        else if (cmp < 0)
        {
            b = m - 1;
        }
        else
        {
            a = m + 1;
        }
    }

    int cmp = MemTableRecord_key_cmp(memtable->records[a], key, key_len);
    if (cmp == 0)
    {
        return a;
    }
    return -1;
}

unsigned int MemTable_insertion_point(const struct MemTable *memtable, const char *key, const size_t key_len)
{
    int a = 0;
    int b = memtable->size;

    while (a < b)
    {
        int m = (a + b) / 2;

        int cmp = MemTableRecord_key_cmp(memtable->records[m], key, key_len);
        if (cmp < 0)
        {
            b = m - 1;
        }
        else
        {
            a = m + 1;
        }
    }

    return a;
}

struct MemTableRecord *MemTable_get(const struct MemTable *memtable, const char *key, const size_t key_len)
{
    int idx = MemTable_binary_search(memtable, key, key_len);
    if (idx == -1)
    {
        return NULL;
    }

    return memtable->records[idx];
}

void MemTable_set(struct MemTable *memtable, char *key, size_t key_len, size_t value_loc, size_t value_len)
{
    int idx = MemTable_binary_search(memtable, key, key_len);
    if (idx == -1)
    {
        struct MemTableRecord *record = MemTableRecord_new(key, key_len, value_loc, value_len);

        unsigned int insert_idx = MemTable_insertion_point(memtable, key, key_len);

        if (insert_idx < memtable->size)
        {
            // Shift the array of records by one pointer;
            memcpy(&(memtable->records[insert_idx + 1]), &memtable->records[insert_idx], sizeof(struct MemTableRecord *) * (memtable->size - insert_idx));
        }

        memtable->records[insert_idx] = record;
        memtable->size++;
        return;
    }

    memtable->records[idx]->value_loc = value_loc;
    memtable->records[idx]->value_len = value_len;
}

void MemTable_delete(struct MemTable *memtable, char *key, size_t key_len)
{
    int idx = MemTable_binary_search(memtable, key, key_len);

    if (idx == -1)
    {
        struct MemTableRecord *record = MemTableRecord_new(key, key_len, 0, 0);

        unsigned int insert_idx = MemTable_insertion_point(memtable, key, key_len);

        if (insert_idx < memtable->size)
        {
            // Shift the array of records by one pointer;
            memcpy(&(memtable->records[insert_idx + 1]), &memtable->records[insert_idx], sizeof(struct MemTableRecord *) * (memtable->size - insert_idx));
        }

        memtable->records[insert_idx] = record;
        memtable->size++;
        return;
    }

    memtable->records[idx]->value_loc = 0;
    memtable->records[idx]->value_len = 0;
}

void MemTable_free(struct MemTable *memtable)
{
    // Free the MemTableRecords and its char*
    for (int i = 0; i < memtable->size; i++)
    {
        free(memtable->records[i]->key);
        free(memtable->records[i]);
    }

    free(memtable);
}
