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
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "../src/sstable.h"

void
TestSSTable_new_from_memtable()
{
  char* path = "./123456789-1.sstable";

  struct MemTable* memtable = MemTable_new();

  for (int i = 0; i < MEMTABLE_SIZE; i++) {
    unsigned char bytes[4];
    bytes[0] = (i >> 24) & 0xFF;
    bytes[1] = (i >> 16) & 0xFF;
    bytes[2] = (i >> 8) & 0xFF;
    bytes[3] = i & 0xFF;

    MemTable_set(memtable, (const char*)&bytes, 4, i * 128);
  }

  struct SSTable* table = SSTable_new_from_memtable(path, memtable);

  assert(table != NULL);
  assert(table->size == MEMTABLE_SIZE);
  assert(table->capacity == MEMTABLE_SIZE);
  for (int i = 0; i < table->size; i++) {
    assert(table->records[i] == i * 20);
  }

  assert(table->low_key_len == 4);
  assert(memcmp(table->low_key, "\x00\x00\x00\x00", table->low_key_len) == 0);

  assert(table->high_key_len == 4);
  assert(memcmp(table->high_key, "\x00\x00\x03\xff", table->high_key_len) == 0);

  MemTable_free(memtable);
  SSTable_free(table);

  remove(path);
}

void
TestSSTable_new()
{
  char* path = "./123456789-1.sstable";

  struct MemTable* memtable = MemTable_new();

  for (int i = 0; i < MEMTABLE_SIZE; i++) {
    unsigned char bytes[4];
    bytes[0] = (i >> 24) & 0xFF;
    bytes[1] = (i >> 16) & 0xFF;
    bytes[2] = (i >> 8) & 0xFF;
    bytes[3] = i & 0xFF;

    MemTable_set(memtable, (const char*)&bytes, 4, i * 128);
  }

  struct SSTable* table = SSTable_new_from_memtable(path, memtable);
  assert(table != NULL);
  SSTable_free(table);
  MemTable_free(memtable);

  struct SSTable* new_table = SSTable_new(path);

  assert(new_table != NULL);
  assert(new_table->timestamp == 123456789);
  assert(new_table->level == 1);
  assert(new_table->size == MEMTABLE_SIZE);
  assert(new_table->capacity == MEMTABLE_SIZE);
  for (int i = 0; i < new_table->size; i++) {
    assert(new_table->records[i] == i * 20);
  }

  assert(new_table->low_key_len == 4);
  assert(memcmp(new_table->low_key,
                "\x00\x00\x00\x00",
                new_table->low_key_len) == 0);

  assert(new_table->high_key_len == 4);
  assert(memcmp(new_table->high_key,
                "\x00\x00\x03\xff",
                new_table->high_key_len) == 0);

  SSTable_free(new_table);

  remove(path);
}

void
TestSSTable_get_value_loc()
{
  char* path = "./123456789-1.sstable";

  struct MemTable* memtable = MemTable_new();

  for (int i = 0; i < MEMTABLE_SIZE; i++) {
    unsigned char bytes[4];
    bytes[0] = (i >> 24) & 0xFF;
    bytes[1] = (i >> 16) & 0xFF;
    bytes[2] = (i >> 8) & 0xFF;
    bytes[3] = i & 0xFF;

    MemTable_set(memtable, (const char*)&bytes, 4, i * 128);
  }

  struct SSTable* table = SSTable_new_from_memtable(path, memtable);
  assert(table != NULL);
  MemTable_free(memtable);

  // Test reading the valid keys.
  for (int i = 0; i < table->size; i++) {
    unsigned char key[4];
    key[0] = (i >> 24) & 0xFF;
    key[1] = (i >> 16) & 0xFF;
    key[2] = (i >> 8) & 0xFF;
    key[3] = i & 0xFF;

    int64_t value_loc = SSTable_get_value_loc(table, (char*)&key, 4);

    assert(value_loc == i * 128);
  }

  // Test reading an invalid key.
  unsigned char key[4];
  key[0] = (table->size >> 24) & 0xFF;
  key[1] = (table->size >> 16) & 0xFF;
  key[2] = (table->size >> 8) & 0xFF;
  key[3] = table->size & 0xFF;
  int64_t value_loc = SSTable_get_value_loc(table, (char*)&key, 4);

  assert(value_loc == SSTABLE_KEY_NOT_FOUND);

  SSTable_free(table);

  remove(path);
}

void
TestSSTable_in_key_range()
{
  char* path = "./123456789-1.sstable";

  struct MemTable* memtable = MemTable_new();

  for (int i = 0; i < MEMTABLE_SIZE; i++) {
    unsigned char bytes[4];
    bytes[0] = (i >> 24) & 0xFF;
    bytes[1] = (i >> 16) & 0xFF;
    bytes[2] = (i >> 8) & 0xFF;
    bytes[3] = i & 0xFF;

    MemTable_set(memtable, (const char*)&bytes, 4, i * 128);
  }

  struct SSTable* table = SSTable_new_from_memtable(path, memtable);
  assert(table != NULL);
  MemTable_free(memtable);

  // Test in key range of valid keys
  for (int i = 0; i < table->size; i++) {
    unsigned char key[4];
    key[0] = (i >> 24) & 0xFF;
    key[1] = (i >> 16) & 0xFF;
    key[2] = (i >> 8) & 0xFF;
    key[3] = i & 0xFF;

    int in_key_range = SSTable_in_key_range(table, (char*)&key, 4);

    assert(in_key_range == 1);
  }

  // Test in key range of invalid keys
  unsigned char key[4];
  key[0] = (table->size >> 24) & 0xFF;
  key[1] = (table->size >> 16) & 0xFF;
  key[2] = (table->size >> 8) & 0xFF;
  key[3] = table->size & 0xFF;
  int in_key_range = SSTable_in_key_range(table, (char*)&key, 4);

  assert(in_key_range == 0);

  SSTable_free(table);

  remove(path);
}

int
main()
{
  // New
  TestSSTable_new_from_memtable();
  TestSSTable_new();

  // Get Value Loc
  TestSSTable_get_value_loc();

  // In Key Range
  TestSSTable_in_key_range();

  return 0;
}
