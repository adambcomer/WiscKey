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
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../src/include/value_log.h"

void TestValueLog_new() {
    char *filename = "value_log.data";

    struct ValueLog *log = ValueLog_new(filename, 0, 128);

    assert(log != NULL);
    assert(log->head == 0);
    assert(log->tail == 128);

    ValueLog_free(log);

    remove(filename);
}

void TestValueLog_append() {
    char *filename = "value_log.data";

    struct ValueLog *log = ValueLog_new(filename, 0, 0);

    char *key1 = "apple";
    char *value1 = "Apple Pie";

    size_t pos;

    int res = ValueLog_append(log, &pos, key1, strlen(key1) + 1, value1, strlen(value1) + 1);
    assert(res == 0);

    res = ValueLog_sync(log);

    assert(res == 0);
    assert(pos == 0);

    FILE *file = fopen(filename, "r");

    {
        uint64_t value_log_key_len;
        uint64_t value_log_value_len;
        char value_log_key[strlen(key1) + 1];
        char value_log_value[strlen(value1) + 1];

        size_t file_res = fread(&value_log_key_len, sizeof(uint64_t), 1, file);
        assert(file_res == 1);
        file_res = fread(&value_log_value_len, sizeof(uint64_t), 1, file);
        assert(file_res == 1);
        file_res = fread(&value_log_key, sizeof(char), strlen(key1) + 1, file);
        assert(file_res == strlen(key1) + 1);
        file_res = fread(&value_log_value, sizeof(char), strlen(value1) + 1, file);
        assert(file_res == strlen(value1) + 1);

        assert(value_log_key_len == strlen(key1) + 1);
        assert(value_log_value_len == strlen(value1) + 1);
        assert(memcmp(value_log_key, key1, strlen(key1) + 1) == 0);
        assert(memcmp(value_log_value, value1, strlen(value1) + 1) == 0);
    }

    char *key2 = "lime";
    char *value2 = "Key Lime Pie";

    res = ValueLog_append(log, &pos, key2, strlen(key2) + 1, value2, strlen(value2) + 1);
    assert(res == 0);

    res = ValueLog_sync(log);

    assert(res == 0);
    assert(pos == 32);

    fseek(file, 0, SEEK_SET);

    {
        uint64_t value_log_key_len;
        uint64_t value_log_value_len;
        char value_log_key[strlen(key1) + 1];
        char value_log_value[strlen(value1) + 1];

        size_t file_res = fread(&value_log_key_len, sizeof(uint64_t), 1, file);
        assert(file_res == 1);
        file_res = fread(&value_log_value_len, sizeof(uint64_t), 1, file);
        assert(file_res == 1);
        file_res = fread(&value_log_key, sizeof(char), strlen(key1) + 1, file);
        assert(file_res == strlen(key1) + 1);
        file_res = fread(&value_log_value, sizeof(char), strlen(value1) + 1, file);
        assert(file_res == strlen(value1) + 1);

        assert(value_log_key_len == strlen(key1) + 1);
        assert(value_log_value_len == strlen(value1) + 1);
        assert(memcmp(value_log_key, key1, strlen(key1) + 1) == 0);
        assert(memcmp(value_log_value, value1, strlen(value1) + 1) == 0);
    }

    {
        uint64_t value_log_key_len;
        uint64_t value_log_value_len;
        char value_log_key[strlen(key2) + 1];
        char value_log_value[strlen(value2) + 1];

        size_t file_res = fread(&value_log_key_len, sizeof(uint64_t), 1, file);
        assert(file_res == 1);
        file_res = fread(&value_log_value_len, sizeof(uint64_t), 1, file);
        assert(file_res == 1);
        file_res = fread(&value_log_key, sizeof(char), strlen(key2) + 1, file);
        assert(file_res == strlen(key2) + 1);
        file_res = fread(&value_log_value, sizeof(char), strlen(value2) + 1, file);
        assert(file_res == strlen(value2) + 1);

        assert(value_log_key_len == strlen(key2) + 1);
        assert(value_log_value_len == strlen(value2) + 1);
        assert(memcmp(value_log_key, key2, strlen(key2) + 1) == 0);
        assert(memcmp(value_log_value, value2, strlen(value2) + 1) == 0);
    }

    fclose(file);

    ValueLog_free(log);

    remove(filename);
}

void TestValueLog_get() {
    char *filename = "value_log.data";

    struct ValueLog *log = ValueLog_new(filename, 0, 0);

    size_t pos1, pos2, pos3;

    int res = ValueLog_append(log, &pos1, "apple", strlen("apple") + 1, "Apple Pie", strlen("Apple Pie") + 1);
    assert(res == 0);

    res = ValueLog_append(log, &pos2, "lime", strlen("lime") + 1, "Key Lime Pie", strlen("Key Lime Pie") + 1);
    assert(res == 0);

    res = ValueLog_append(log, &pos3, "cherry", strlen("cherry") + 1, "Cherry Pie", strlen("Cherry Pie") + 1);
    assert(res == 0);

    res = ValueLog_sync(log);
    assert(res == 0);

    char *value;
    size_t value_len;

    res = ValueLog_get(log, &value, &value_len, pos1);
    assert(res == 0);

    assert(value_len == strlen("Apple Pie") + 1);
    assert(memcmp(value, "Apple Pie", value_len) == 0);

    free(value);

    res = ValueLog_get(log, &value, &value_len, pos2);
    assert(res == 0);

    assert(value_len == strlen("Key Lime Pie") + 1);
    assert(memcmp(value, "Key Lime Pie", value_len) == 0);

    free(value);

    res = ValueLog_get(log, &value, &value_len, pos3);
    assert(res == 0);

    assert(value_len == strlen("Cherry Pie") + 1);
    assert(memcmp(value, "Cherry Pie", value_len) == 0);

    free(value);

    ValueLog_free(log);

    remove(filename);
}

void TestValueLog_reload() {
    char *filename = "value_log.data";

    struct ValueLog *log = ValueLog_new(filename, 0, 0);

    size_t pos1, pos2, pos3;

    int res = ValueLog_append(log, &pos1, "apple", strlen("apple") + 1, "Apple Pie", strlen("Apple Pie") + 1);
    assert(res == 0);

    res = ValueLog_append(log, &pos2, "lime", strlen("lime") + 1, "Key Lime Pie", strlen("Key Lime Pie") + 1);
    assert(res == 0);

    res = ValueLog_append(log, &pos3, "cherry", strlen("cherry") + 1, "Cherry Pie", strlen("Cherry Pie") + 1);
    assert(res == 0);

    res = ValueLog_sync(log);
    assert(res == 0);

    size_t tail = log->tail;

    ValueLog_free(log);

    log = ValueLog_new(filename, 0, tail);

    char *value;
    size_t value_len;

    res = ValueLog_get(log, &value, &value_len, pos1);
    assert(res == 0);

    assert(value_len == strlen("Apple Pie") + 1);
    assert(memcmp(value, "Apple Pie", value_len) == 0);

    free(value);

    res = ValueLog_get(log, &value, &value_len, pos2);
    assert(res == 0);

    assert(value_len == strlen("Key Lime Pie") + 1);
    assert(memcmp(value, "Key Lime Pie", value_len) == 0);

    free(value);

    res = ValueLog_get(log, &value, &value_len, pos3);
    assert(res == 0);

    assert(value_len == strlen("Cherry Pie") + 1);
    assert(memcmp(value, "Cherry Pie", value_len) == 0);

    free(value);

    ValueLog_free(log);

    remove(filename);
}

int main() {
    // New
    TestValueLog_new();

    // Append
    TestValueLog_append();

    // Get
    TestValueLog_get();

    // Reload
    TestValueLog_reload();

    return 0;
}