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

#include "include/value_log.h"

struct ValueLog *ValueLog_new(const char *filename, size_t head, size_t tail) {
    char *mode = "w+";
    if (access(filename, F_OK) == 0) {
        mode = "r+";
    }

    FILE *file = fopen(filename, mode);
    if (file == NULL) {
        perror("fopen");
        return NULL;
    }

    struct ValueLog *log = malloc(sizeof(struct ValueLog));

    log->file = file;
    log->head = head;
    log->tail = tail;

    return log;
}

int ValueLog_append(struct ValueLog *log, size_t *pos, const char *key, size_t key_len, const char *value,
                    size_t value_len) {
    int res = fseek(log->file, (long) log->head, SEEK_SET);
    if (res == -1) {
        perror("fseek");
        return -1;
    }

    uint64_t key_len_64 = key_len;
    uint64_t value_len_64 = value_len;

    size_t b_written = fwrite(&key_len_64, sizeof(uint64_t), 1, log->file);
    if (b_written != 1) {
        perror("fwrite");
        return -1;
    }
    b_written = fwrite(&value_len_64, sizeof(uint64_t), 1, log->file);
    if (b_written != 1) {
        perror("fwrite");
        return -1;
    }
    b_written = fwrite(key, sizeof(char), key_len, log->file);
    if (b_written != key_len) {
        perror("fwrite");
        return -1;
    }
    b_written = fwrite(value, sizeof(char), value_len, log->file);
    if (b_written != value_len) {
        perror("fwrite");
        return -1;
    }

    *pos = log->head;
    log->head += sizeof(uint64_t) + sizeof(uint64_t) + key_len + value_len;

    return 0;
}

int ValueLog_get(const struct ValueLog *log, char **value, size_t *value_len, size_t loc) {
    int res = fseek(log->file, (long) loc, SEEK_SET);
    if (res == -1) {
        return -1;
    }

    uint64_t key_len_64;
    uint64_t value_len_64;

    size_t b_read = fread(&key_len_64, sizeof(uint64_t), 1, log->file);
    if (b_read != 1) {
        perror("fread");
        return -1;
    }

    b_read = fread(&value_len_64, sizeof(uint64_t), 1, log->file);
    if (b_read != 1) {
        perror("fread");
        return -1;
    }

    res = fseek(log->file, (long) key_len_64, SEEK_CUR);
    if (res == -1) {
        perror("fseek");
        return -1;
    }

    *value = malloc(value_len_64);
    *value_len = value_len_64;
    b_read = fread(*value, sizeof(char), value_len_64, log->file);
    if (b_read != value_len_64) {
        perror("fread");
        return -1;
    }

    return 0;
}

int ValueLog_sync(const struct ValueLog *log) {
    int res = fflush(log->file);
    if (res == EOF) {
        perror("fflush");
        return -1;
    }

    res = fsync(fileno(log->file));
    if (res == -1) {
        perror("fsync");
        return -1;
    }
    return 0;
}

void ValueLog_free(struct ValueLog *log) {
    int res = fclose(log->file);
    if (res == -1) {
        perror("fclose");
    }

    free(log);
}