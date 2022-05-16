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

#include <assert.h>
#include <string.h>
#include <stdint.h>

#include "../src/include/wal.h"
#include "../src/include/memtable.h"

void TestWAL_new() {
    char *filename = "wal.data";

    struct WAL *wal = WAL_new(filename);

    assert(wal != NULL);

    WAL_free(wal);

    remove(filename);
}

void TestWAL_append() {
    char *filename = "wal.data";

    struct WAL *wal = WAL_new(filename);

    char *key1 = "apple";
    char *value1 = "Apple Pie";

    int res = WAL_append(wal, key1, strlen(key1) + 1, 0);
    assert(res == 0);

    res = WAL_sync(wal);
    assert(res == 0);

    FILE *file = fopen(filename, "r");

    {
        uint64_t wal_key_len;
        int64_t wal_value_loc;
        char wal_key[strlen(key1) + 1];

        size_t file_res = fread(&wal_key_len, sizeof(uint64_t), 1, file);
        assert(file_res == 1);
        file_res = fread(&wal_value_loc, sizeof(int64_t), 1, file);
        assert(file_res == 1);
        file_res = fread(&wal_key, sizeof(char), strlen(key1) + 1, file);
        assert(file_res == strlen(key1) + 1);

        assert(wal_key_len == strlen(key1) + 1);
        assert(wal_value_loc == 0);
        assert(memcmp(wal_key, key1, strlen(key1) + 1) == 0);
    }


    char *key2 = "lime";
    long long value2_offset = (long long) strlen(value1) + 1;

    res = WAL_append(wal, key2, strlen(key2) + 1, value2_offset);
    assert(res == 0);

    res = WAL_sync(wal);
    assert(res == 0);


    fseek(file, 0, SEEK_SET);

    {
        uint64_t wal_key_len;
        int64_t wal_value_loc;
        char wal_key[strlen(key1) + 1];

        size_t file_res = fread(&wal_key_len, sizeof(uint64_t), 1, file);
        assert(file_res == 1);
        file_res = fread(&wal_value_loc, sizeof(int64_t), 1, file);
        assert(file_res == 1);
        file_res = fread(&wal_key, sizeof(char), strlen(key1) + 1, file);
        assert(file_res == strlen(key1) + 1);

        assert(wal_key_len == strlen(key1) + 1);
        assert(wal_value_loc == 0);
        assert(memcmp(wal_key, key1, strlen(key1) + 1) == 0);
    }

    {
        uint64_t wal_key_len;
        int64_t wal_value_loc;
        char wal_key[strlen(key2) + 1];

        size_t file_res = fread(&wal_key_len, sizeof(uint64_t), 1, file);
        assert(file_res == 1);
        file_res = fread(&wal_value_loc, sizeof(int64_t), 1, file);
        assert(file_res == 1);
        file_res = fread(&wal_key, sizeof(char), strlen(key2) + 1, file);
        assert(file_res == strlen(key2) + 1);

        assert(wal_key_len == strlen(key2) + 1);
        assert(wal_value_loc == value2_offset);
        assert(memcmp(wal_key, key2, strlen(key2) + 1) == 0);
    }

    fclose(file);

    WAL_free(wal);

    remove(filename);
}

void TestWAL_load_memtable() {
    char *filename = "wal.data";

    struct WAL *wal = WAL_new(filename);

    char *key1 = "apple";
    char *value1 = "Apple Pie";

    int res = WAL_append(wal, key1, strlen(key1) + 1, 0);
    assert(res == 0);

    char *key2 = "lime";
    long long value2_offset = (long long) strlen(value1) + 1;

    res = WAL_append(wal, key2, strlen(key2) + 1, value2_offset);
    assert(res == 0);

    char *key3 = "orange";

    res = WAL_append(wal, key3, strlen(key3) + 1, 100);
    assert(res == 0);

    res = WAL_append(wal, key3, strlen(key3) + 1, -1);
    assert(res == 0);

    // Simulate shutting down the database.
    WAL_free(wal);

    struct MemTable *m = MemTable_new();

    wal = WAL_new(filename);

    res = WAL_load_memtable(wal, m);
    assert(res == 0);

    assert(m->size == 3);

    assert(m->records[0]->key_len == strlen(key1) + 1);
    assert(memcmp(m->records[0]->key, key1, strlen(key1) + 1) == 0);
    assert(m->records[0]->value_loc == 0);

    assert(m->records[1]->key_len == strlen(key2) + 1);
    assert(memcmp(m->records[1]->key, key2, strlen(key2) + 1) == 0);
    assert(m->records[1]->value_loc == value2_offset);

    assert(m->records[2]->key_len == strlen(key3) + 1);
    assert(memcmp(m->records[2]->key, key3, strlen(key3) + 1) == 0);
    assert(m->records[2]->value_loc == -1);

    WAL_free(wal);
    MemTable_free(m);

    remove(filename);
}

int main() {

    // New
    TestWAL_new();

    // Append
    TestWAL_append();

    // Load MemTable
    TestWAL_load_memtable();

    return 0;
}