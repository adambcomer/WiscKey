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

#include <stdio.h>
#include <stdint.h>

#include "memtable.h"

#ifndef WISCKEY_WAL_H
#define WISCKEY_WAL_H

struct WAL {
    FILE *file;
    char *filename;
};

struct WAL *WAL_new(char *filename);

int WAL_load_memtable(struct WAL *wal, struct MemTable *memtable);

int WAL_append(struct WAL *wal, const char *key, size_t key_len, int64_t value_loc);

int WAL_sync(const struct WAL *wal);

void WAL_free(struct WAL *wal);

#endif /* WISCKEY_WAL_H */