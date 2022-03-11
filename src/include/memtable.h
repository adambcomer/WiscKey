#include <stdlib.h>

#ifndef MEMTABLE_H
#define MEMTABLE_H

#define MEMTABLE_SIZE 1024

struct MemTableRecord
{
    char *key;
    size_t key_len;
    size_t value_loc;
    size_t value_len;
    unsigned long long timestamp;
};

struct MemTable
{
    struct MemTableRecord *records[MEMTABLE_SIZE];
    unsigned int size;
};

struct MemTable *MemTable_new();

struct MemTableRecord *MemTable_get(const struct MemTable *memtable, const char *key, const size_t key_len);

void MemTable_set(struct MemTable *memtable, char *key, size_t key_len, size_t value_loc, size_t value_len);

void MemTable_delete(struct MemTable *memtable, char *key, size_t key_len);

void MemTable_free(struct MemTable *memtable);

#endif /* MEMTABLE_H */