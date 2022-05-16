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

#ifndef WISCKEY_MEMTABLE_H
#define WISCKEY_MEMTABLE_H

#include <stdlib.h>
#include <stdint.h>

#define MEMTABLE_SIZE 1024 ///< Max number of MemTableRecords in a MemTable

/**
 * @file
 * @author Adam Comer <adambcomer@gmail.com>
 * @date May 15, 2020
 * @copyright Apache-2.0 License
 * @brief In-memory table of the records that have been modified most recently.
 */

/**
 * @brief Single Record in the MemTable.
 *
 * Each MemTableRecord holds the key and the position of the record in the ValueLog.
 */
struct MemTableRecord {
    char *key; ///< The key of the record.
    size_t key_len; ///< The length of the key.
    int64_t value_loc; ///< The location of the value in the ValueLog.
};

/**
 * @brief MemTable of the Database.
 *
 * In-memory table of the records that have been modified most recently. At any given time, there is only one active
 * MemTable in the database engine. The MemTable is always the first store to be searched when a key-value pair is
 * requested.
 */
struct MemTable {
    struct MemTableRecord *records[MEMTABLE_SIZE]; ///< Array of records sorted by key.
    size_t size; ///< The number of records filled in `records`.
};

/**
 * @brief Creates a new empty MemTable.
 *
 * Note: Free this MemTable with MemTable_free.
 *
 * @return A new empty MemTable.
 */
struct MemTable *MemTable_new();

/**
 * @brief Gets a MemTableRecord from a MemTable by key.
 *
 * This function will return NULL if none of the records in the MemTable.
 *
 * This function uses binary search for a runtime of `O(log(n))`.
 *
 * @param memtable The MemTable to search.
 * @param key The key to search the MemTable with.
 * @param key_len The length of the key.
 * @return A MemTableRecord if the record exists or NULL if it doesn't not exist.
 */
struct MemTableRecord *MemTable_get(const struct MemTable *memtable, const char *key, size_t key_len);

/**
 * @brief Sets a key-value pair in a MemTable.
 *
 * This function uses binary search for a runtime of `O(log(n))`.
 *
 * @param memtable The MemTable to set a value to.
 * @param key The key to set this value for.
 * @param key_len The length of the key.
 * @param value_loc The seek location to the value in the ValueLog.
 */
void MemTable_set(struct MemTable *memtable, const char *key, size_t key_len, int64_t value_loc);

/**
 * @brief Deletes a record from a MemTable.
 *
 * This function uses binary search for a runtime of `O(log(n))`.
 *
 * Note: This function will set a tombstone to propagate the delete into the SSTables.
 *
 * @param memtable The MemTable to delete a record from.
 * @param key The key to delete.
 * @param key_len The length of the key.
 */
void MemTable_delete(struct MemTable *memtable, const char *key, size_t key_len);

/**
 * @brief Frees a MemTable and its records.
 *
 * Note: This function will free all the MemTableRecords plus their keys in this MemTable.
 *
 * @param memtable The MemTable to free.
 */
void MemTable_free(struct MemTable *memtable);

#endif /* WISCKEY_MEMTABLE_H */