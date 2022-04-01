#include <stdlib.h>

#ifndef MEMTABLE_H
#define MEMTABLE_H

#define MEMTABLE_SIZE 1024

struct MemTableRecord {
    char *key;
    size_t key_len;
    long long value_loc;
};

struct MemTable {
    struct MemTableRecord *records[MEMTABLE_SIZE];
    unsigned int size;
};

struct MemTable *MemTable_new();

struct MemTableRecord *MemTable_get(const struct MemTable *memtable, const char *key, size_t key_len);

void MemTable_set(struct MemTable *memtable, const char *key, size_t key_len, long long value_loc);

void MemTable_delete(struct MemTable *memtable, const char *key, size_t key_len);

void MemTable_free(struct MemTable *memtable);

#endif /* MEMTABLE_H */