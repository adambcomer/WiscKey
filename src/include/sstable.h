/*
 * Copyright 2022 Adam Bishop Comer
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <stdlib.h>
#include <stdio.h>

#include "memtable.h"

#ifndef WISCKEY_SSTABLE_H
#define WISCKEY_SSTABLE_H

#define SSTABLE_MIN_SIZE 1024
#define SSTABLE_KEY_NOT_FOUND (-2)

struct SSTableRecord {
    char *key;
    size_t key_len;
    int64_t value_loc;
};

struct SSTable {
    char *filename;
    FILE *file;
    uint64_t *records;
    size_t capacity;
    size_t size;

    char *low_key;
    size_t low_key_len;
    char *high_key;
    size_t high_key_len;
};

struct SSTable *SSTable_new(char *filename);

struct SSTable *SSTable_new_from_memtable(char *filename, struct MemTable *memtable);

int64_t SSTable_get_value_loc(struct SSTable *table, char *key, size_t key_len);

int SSTable_in_key_range(struct SSTable *table, char *key, size_t key_len);

void SSTable_free(struct SSTable *table);

#endif /* WISCKEY_SSTABLE_H */