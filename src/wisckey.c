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

#include "include/wisckey.h"
#include "memtable.h"

struct WiscKeyDB
{
  char* dir;
  struct MemTable* memtable;
};

struct WiscKeyDB*
WiscKeyDB_new(char* dir)
{
  struct WiscKeyDB* db = malloc(sizeof(struct WiscKeyDB));
  db->dir = dir;
  db->memtable = MemTable_new();

  return db;
}

size_t
WiscKeyDB_get(struct WiscKeyDB* db,
              __attribute__((unused)) char* ptr,
              char* key,
              size_t key_length)
{
  struct MemTableRecord* m_record = MemTable_get(db->memtable, key, key_length);
  if (m_record != NULL) {
    ptr = m_record->key;
    return m_record->key_len;
  }

  return 0;
}

int
WiscKeyDB_set(__attribute__((unused)) struct WiscKeyDB* db,
              __attribute__((unused)) char* key,
              __attribute__((unused)) char* value,
              __attribute__((unused)) size_t key_length,
              __attribute__((unused)) size_t value_length)
{
  return 0;
}

int
WiscKeyDB_delete(__attribute__((unused)) struct WiscKeyDB* db,
                 __attribute__((unused)) char* key,
                 __attribute__((unused)) size_t key_length)
{
  return 0;
}
