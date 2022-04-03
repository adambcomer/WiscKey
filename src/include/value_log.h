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

#ifndef VALUE_LOG_H
#define VALUE_LOG_H

struct ValueLog {
    FILE *file;
    size_t head;
    size_t tail;
};

struct ValueLog *ValueLog_new(const char *filename, size_t head, size_t tail);

int ValueLog_append(struct ValueLog *log, size_t *pos, const char *key, size_t key_len, const char *value,
                    size_t value_len);

int ValueLog_get(const struct ValueLog *log, char **value, size_t *value_len, size_t value_loc);

int ValueLog_sync(const struct ValueLog *log);

void ValueLog_free(struct ValueLog *log);

#endif /* VALUE_LOG_H */