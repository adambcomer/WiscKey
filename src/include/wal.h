#include <stdio.h>

#include "memtable.h"

#ifndef WAL_H
#define WAL_H

struct WAL {
    FILE *file;
    char *filename;
};

struct WAL *WAL_new(char *filename);

int WAL_load_memtable(struct WAL *wal, struct MemTable *memtable);

int WAL_append(struct WAL *wal, const char *key, size_t key_len, long long value_loc);

int WAL_sync(const struct WAL *wal);

void WAL_free(struct WAL *wal);

#endif /* WAL_H */