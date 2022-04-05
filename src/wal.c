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
#include <unistd.h>
#include <stdint.h>

#include "include/wal.h"
#include "include/memtable.h"

struct WAL *WAL_new(char *filename) {
    FILE *file = fopen(filename, "a+");
    if (file == NULL) {
        return NULL;
    }

    struct WAL *wal = malloc(sizeof(struct WAL));
    wal->file = file;
    wal->filename = filename;

    return wal;
}

int WAL_load_memtable(struct WAL *wal, struct MemTable *memtable) {
    int res = fseek(wal->file, 0, SEEK_SET);
    if (res != 0) {
        return -1;
    }

    while (1) {
        int peek = fgetc(wal->file);
        ungetc(peek, wal->file);
        if (peek == EOF) {
            break;
        }

        uint64_t wal_key_len;
        size_t file_res = fread(&wal_key_len, sizeof(uint64_t), 1, wal->file);
        if (file_res != 1) {
            return -1;
        }

        int64_t wal_value_loc;
        file_res = fread(&wal_value_loc, sizeof(int64_t), 1, wal->file);
        if (file_res != 1) {
            return -1;
        }

        char wal_key[wal_key_len];
        file_res = fread(&wal_key, sizeof(char), wal_key_len, wal->file);
        if (file_res != wal_key_len) {
            return -1;
        }

        if (wal_value_loc == -1) {
            MemTable_delete(memtable, wal_key, wal_key_len);
        } else {
            MemTable_set(memtable, wal_key, wal_key_len, wal_value_loc);
        }
    }

    return 0;
}

int WAL_append(struct WAL *wal, const char *key, size_t key_len, int64_t value_loc) {
    uint64_t key_len_64 = key_len;
    int64_t value_loc_64 = value_loc;

    size_t b_written = fwrite(&key_len_64, sizeof(uint64_t), 1, wal->file);
    if (b_written != 1) {
        return -1;
    }
    b_written = fwrite(&value_loc_64, sizeof(int64_t), 1, wal->file);
    if (b_written != 1) {
        return -1;
    }
    b_written = fwrite(key, sizeof(char), key_len, wal->file);
    if (b_written != key_len) {
        return -1;
    }

    return 0;
}

int WAL_sync(const struct WAL *wal) {
    int res = fflush(wal->file);
    if (res != 0) {
        return -1;
    }

    res = fsync(fileno(wal->file));
    if (res < 0) {
        return -1;
    }
    return 0;
}

void WAL_free(struct WAL *wal) {
    fclose(wal->file);

    free(wal);
}