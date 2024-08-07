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

#ifndef WISCKEY_H
#define WISCKEY_H

#include <stdlib.h>

struct WiscKeyDB;

struct WiscKeyDB*
WiscKeyDB_new(char* dir);

size_t
WiscKeyDB_get(struct WiscKeyDB* db, char* ptr, char* key, size_t key_length);

int
WiscKeyDB_set(struct WiscKeyDB* db,
              char* key,
              char* value,
              size_t key_length,
              size_t value_length);

int
WiscKeyDB_delete(struct WiscKeyDB* db, char* key, size_t key_length);

#endif /* WISCKEY_H */
