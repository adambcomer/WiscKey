#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>

#include "../src/include/value_log.h"

#define ValueLogEntry_key_value_assert(filename, pos, k, k_len, v, v_len) \
    {                                                                     \
        FILE *file = fopen(filename, "r");                                \
        fseek(file, pos, SEEK_SET);                                       \
                                                                          \
        uint64_t value_log_key_len;                                       \
        uint64_t value_log_value_len;                                     \
        char value_log_key[k_len];                                        \
        char value_log_value[v_len];                                      \
                                                                          \
        int res = fread(&value_log_key_len, sizeof(uint64_t), 1, file);   \
        assert(res == 1);                                                 \
        res = fread(&value_log_value_len, sizeof(uint64_t), 1, file);     \
        assert(res == 1);                                                 \
        res = fread(&value_log_key, sizeof(char), k_len, file);           \
        assert(res == k_len);                                             \
        res = fread(&value_log_value, sizeof(char), v_len, file);         \
        assert(res == v_len);                                             \
                                                                          \
        assert(value_log_key_len == k_len);                               \
        assert(value_log_value_len == v_len);                             \
        assert(memcmp(value_log_key, k, k_len) == 0);                     \
        assert(memcmp(value_log_value, v, v_len) == 0);                   \
                                                                          \
        fclose(file);                                                     \
    }

void TestValueLog_new()
{
    char *filename = "value_log.data";

    struct ValueLog *log = ValueLog_new(filename, 0, 128);

    assert(log != NULL);
    assert(log->head == 0);
    assert(log->tail == 128);

    ValueLog_free(log);

    remove(filename);
}

void TestValueLog_append()
{
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
    ValueLogEntry_key_value_assert(filename, 0, key1, strlen(key1) + 1, value1, strlen(value1) + 1);

    char *key2 = "lime";
    char *value2 = "Key Lime Pie";

    res = ValueLog_append(log, &pos, key2, strlen(key2) + 1, value2, strlen(value2) + 1);
    assert(res == 0);

    res = ValueLog_sync(log);

    assert(res == 0);
    assert(pos == 32);
    ValueLogEntry_key_value_assert(filename, 0, key1, strlen(key1) + 1, value1, strlen(value1) + 1);
    ValueLogEntry_key_value_assert(filename, 32, key2, strlen(key2) + 1, value2, strlen(value2) + 1);

    ValueLog_free(log);

    remove(filename);
}

void TestValueLog_get()
{
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

void TestValueLog_reload()
{
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

int main()
{
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