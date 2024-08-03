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

#include <libgen.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#include "sstable.h"

int
SSTableRecord_read(struct SSTable* table,
                   struct SSTableRecord* record,
                   uint64_t offset)
{
  int seek_res = fseeko(table->file, (long long)offset, SEEK_SET);
  if (seek_res == -1) {
    perror("fseeko");
    return -1;
  }

  uint64_t key_len;
  size_t file_res = fread(&key_len, sizeof(uint64_t), 1, table->file);
  if (file_res != 1) {
    perror("fread");
    return -1;
  }

  int64_t val_loc;
  file_res = fread(&val_loc, sizeof(int64_t), 1, table->file);
  if (file_res != 1) {
    perror("fread");
    return -1;
  }

  char* table_key = malloc(key_len);
  file_res = fread(table_key, sizeof(char), key_len, table->file);
  if (file_res != key_len) {
    perror("fread");
    return -1;
  }

  record->key_len = key_len;
  record->value_loc = val_loc;
  record->key = table_key;

  return 0;
}

unsigned long
SSTable_parse_timestamp(char* filename)
{
  int i = 0;
  while (1) {
    if (filename[i] == '-') {
      break;
    }
    i += 1;
  }
  char t[i + 1];
  memcpy(t, filename, i);
  t[i] = '\0';

  return strtoul(t, NULL, 10);
}

unsigned long
SSTable_parse_level(char* filename)
{
  int i = 0;
  while (1) {
    if (filename[i] == '-') {
      break;
    }
    i += 1;
  }

  int j = i + 1;
  while (1) {
    if (filename[j] == '.') {
      break;
    }
    j += 1;
  }
  char l[j - i];
  memcpy(l, filename + i + 1, j - (i + 1));
  l[j - (i + 1)] = '\0';

  return strtoul(l, NULL, 10);
}

void
SSTable_append_offset(struct SSTable* table, uint64_t offset)
{
  if (table->size < table->capacity) {
    table->records[table->size] = offset;
    table->size += 1;
    return;
  }

  // Grow the array
  uint64_t* new_records = malloc(table->capacity * 2 * sizeof(uint64_t));
  memcpy(new_records, table->records, table->capacity * sizeof(uint64_t));
  free(table->records);
  table->records = new_records;
  table->capacity = table->capacity * 2;

  // Add the new offset to the array
  table->records[table->size] = offset;
  table->size += 1;
}

struct SSTable*
SSTable_new(char* path)
{
  FILE* file = fopen(path, "r");
  if (file == NULL) {
    perror("fopen");
    return NULL;
  }

  char* filename = basename(path);
  if (filename == NULL) {
    perror("basename");
    return NULL;
  }

  struct SSTable* table = malloc(sizeof(struct SSTable));
  table->path = path;
  table->file = file;

  table->timestamp = SSTable_parse_timestamp(filename);
  table->level = SSTable_parse_level(filename);

  table->records = malloc(SSTABLE_MIN_SIZE * sizeof(uint64_t));
  table->capacity = SSTABLE_MIN_SIZE;
  table->size = 0;

  uint64_t curr_offset = 0;
  while (1) {
    int peek = fgetc(table->file);
    ungetc(peek, table->file);
    if (peek == EOF) {
      break;
    }

    uint64_t key_len;
    size_t file_res = fread(&key_len, sizeof(uint64_t), 1, table->file);
    if (file_res != 1) {
      perror("fread");
      return NULL;
    }

    uint64_t next = key_len + sizeof(int64_t);

    file_res = fseeko(table->file, (long long)next, SEEK_CUR);
    if (file_res != 0) {
      perror("fread");
      return NULL;
    }

    SSTable_append_offset(table, curr_offset);

    curr_offset += next + sizeof(uint64_t);
  }

  int seek_res = fseeko(table->file, (long long)table->records[0], SEEK_SET);
  if (seek_res == -1) {
    perror("fseeko");
    return NULL;
  }

  uint64_t low_key_len;
  size_t file_res = fread(&low_key_len, sizeof(uint64_t), 1, table->file);
  if (file_res != 1) {
    perror("fread");
    return NULL;
  }

  uint64_t val_loc;
  file_res = fread(&val_loc, sizeof(int64_t), 1, table->file);
  if (file_res != 1) {
    perror("fread");
    return NULL;
  }

  char* low_key = malloc(low_key_len);
  file_res = fread(low_key, sizeof(char), low_key_len, table->file);
  if (file_res != low_key_len) {
    perror("fread");
    return NULL;
  }

  table->low_key = low_key;
  table->low_key_len = low_key_len;

  seek_res =
    fseeko(table->file, (long long)table->records[table->size - 1], SEEK_SET);
  if (seek_res == -1) {
    perror("fseeko");
    return NULL;
  }

  uint64_t high_key_len;
  file_res = fread(&high_key_len, sizeof(uint64_t), 1, table->file);
  if (file_res != 1) {
    perror("fread");
    return NULL;
  }

  file_res = fread(&val_loc, sizeof(int64_t), 1, table->file);
  if (file_res != 1) {
    perror("fread");
    return NULL;
  }

  char* high_key = malloc(high_key_len);
  file_res = fread(high_key, sizeof(char), high_key_len, table->file);
  if (file_res != high_key_len) {
    perror("fread");
    return NULL;
  }

  table->high_key = high_key;
  table->high_key_len = high_key_len;

  return table;
}

struct SSTable*
SSTable_new_from_memtable(char* path, struct MemTable* memtable)
{
  FILE* file = fopen(path, "w+");
  if (file == NULL) {
    perror("fopen");
    return NULL;
  }

  for (size_t i = 0; i < memtable->size; i++) {
    uint64_t key_len = memtable->records[i]->key_len;
    int64_t value_loc = memtable->records[i]->value_loc;

    size_t res = fwrite(&key_len, sizeof(uint64_t), 1, file);
    if (res != 1) {
      perror("fwrite");
      return NULL;
    }

    res = fwrite(&value_loc, sizeof(int64_t), 1, file);
    if (res != 1) {
      perror("fwrite");
      return NULL;
    }

    res = fwrite(memtable->records[i]->key, sizeof(char), key_len, file);
    if (res != key_len) {
      perror("fwrite");
      return NULL;
    }
  }

  int res = fclose(file);
  if (res == -1) {
    perror("fclose");
    return NULL;
  }

  return SSTable_new(path);
}

int
SSTable_key_cmp(struct SSTableRecord* r, const char* key, size_t key_len)
{
  size_t len = r->key_len < key_len ? r->key_len : key_len;

  int cmp = memcmp(key, r->key, len);
  if (cmp != 0 || r->key_len == key_len) {
    return cmp;
  }

  return key_len < r->key_len ? -1 : 1;
}

int64_t
SSTable_get_value_loc(struct SSTable* table, char* key, size_t key_len)
{
  if (table->size == 0) {
    return SSTABLE_KEY_NOT_FOUND;
  }

  int a = 0;
  int b = (int)table->size - 1;

  while (a < b) {
    int m = a + (b - a) / 2;

    struct SSTableRecord record;
    SSTableRecord_read(table, &record, table->records[m]);

    int cmp = SSTable_key_cmp(&record, key, key_len);
    free(record.key);
    if (cmp == 0) {
      return record.value_loc;
    } else if (cmp < 0) {
      b = m - 1;
    } else {
      a = m + 1;
    }
  }

  struct SSTableRecord record;
  int res = SSTableRecord_read(table, &record, table->records[a]);
  if (res == -1) {
    perror("Error reading record from SSTable");
    return -1;
  }

  int cmp = SSTable_key_cmp(&record, key, key_len);
  free(record.key);
  if (cmp == 0) {
    return record.value_loc;
  }
  return SSTABLE_KEY_NOT_FOUND;
}

int
SSTable_in_key_range(struct SSTable* table, char* key, size_t key_len)
{
  int low_key_cmp;
  int high_key_cmp;

  size_t low_len = table->low_key_len < key_len ? table->low_key_len : key_len;
  int cmp = memcmp(key, table->low_key, low_len);
  if (cmp != 0 || table->low_key_len == key_len) {
    low_key_cmp = cmp;
  } else {
    low_key_cmp = key_len < table->low_key_len ? -1 : 1;
  }

  size_t high_len =
    table->high_key_len < key_len ? table->high_key_len : key_len;
  cmp = memcmp(key, table->high_key, high_len);
  if (cmp != 0 || table->high_key_len == key_len) {
    high_key_cmp = cmp;
  } else {
    high_key_cmp = key_len < table->high_key_len ? -1 : 1;
  }

  if (low_key_cmp >= 0 && high_key_cmp <= 0) {
    return 1;
  }
  return 0;
}

void
SSTable_free(struct SSTable* table)
{
  int res = fclose(table->file);
  if (res == -1) {
    perror("fclose");
  }

  free(table->high_key);
  free(table->low_key);
  free(table->records);

  free(table);
}
