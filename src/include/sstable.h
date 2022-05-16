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

#ifndef WISCKEY_SSTABLE_H
#define WISCKEY_SSTABLE_H

#include <stdlib.h>
#include <stdio.h>

#include "memtable.h"

/**
 * @file
 * @author Adam Comer <adambcomer@gmail.com>
 * @date May 15, 2020
 * @copyright Apache-2.0 License
 * @brief Non-volatile storage of the keys in the Database.
 */

#define SSTABLE_MIN_SIZE 1024 ///< Minimum number of records in a SSTable index array.
#define SSTABLE_KEY_NOT_FOUND (-2) ///< Return value if the value is not found.

/**
 * @brief Single Record in a SSTable.
 *
 * Each SSTableRecord holds the key and the position of the record in the ValueLog.
 */
struct SSTableRecord {
    char *key; ///< Key of the record.
    size_t key_len; ///< Length of they key.
    int64_t value_loc; ///< Location of the value in the ValueLog.
};

/**
 * @brief On-disk String-Sorted Table(SSTable) of the keys.
 *
 * On-disk storage of the keys and their value locations. Records in a SSTable are sorted by their key as to support
 * binary search. If a record isn't found in the MemTable, then the database searches the SSTables starting with the
 * lowest one in the hierarchy.
 */
struct SSTable {
    char *path; ///< Path of the SSTable on-disk.
    FILE *file; ///< File that the keys reside on.
    uint64_t *records; ///< In-memory index of the location of the keys. Growable array with size and capacity. Uses 8 bytes per key.
    size_t capacity; ///< Capacity of the growable in-memory index.
    size_t size; ///< Size of the growable in-memory index.

    char *low_key; ///< Lowest key in the SSTable. Used to check if a key could possibly be in this SSTable.
    size_t low_key_len; ///< Length of the lowest key.
    char *high_key; ///< Highest key in the SSTable. Used to check if a key could possibly be in this SSTable.
    size_t high_key_len; ///< Length of the highest key.
};

/**
 * @brief Loads a SSTable at a path.
 *
 * Note: This operation scans the entire SSTable to build the index.
 *
 * @param path The Path of the SSTable on the filesystem.
 * @return A pointer to a new SSTable.
 */
struct SSTable *SSTable_new(char *path);

/**
 * @brief Creates a new SSTable from a full MemTable.
 *
 * This function will create a new SSTable at a path. If a file already exists at this path, then the file will be
 * overwritten.
 *
 * Note: The MemTable will not be freed after creating a new SSTable. The caller is responsible for freeing the
 * MemTable.
 *
 * @param path The path of the new SSTable.
 * @param memtable The MemTable to create a SSTable from.
 * @return A pointer to the newly created SSTable.
 */
struct SSTable *SSTable_new_from_memtable(char *path, struct MemTable *memtable);

/**
 * @brief Gets the location of a value on the ValueLog from a key.
 *
 * This function uses the in-memory index to seek each record on disk. This function uses binary search for a runtime
 * of `O(log(n))` seeks.
 *
 * @param table The SSTable to search.
 * @param key The key to search with.
 * @param key_len The length of the key.
 * @return This function returns the position in the ValueLog if the key is found. -2 if the key is not in the SSTable. -1 if there is an error reading the record.
 */
int64_t SSTable_get_value_loc(struct SSTable *table, char *key, size_t key_len);

/**
 * @brief Checks if the given key could be in this SSTable.
 *
 * This function runs in constant time without any operations on disk.
 *
 * @param table The SSTable to search.
 * @param key The key in question.
 * @param key_len The length of the key.
 * @return This function returns 1 if the key is in the range and 0 if it is not.
 */
int SSTable_in_key_range(struct SSTable *table, char *key, size_t key_len);

/**
 * @brief Frees the SSTable.
 *
 * Note: This function won't delete the file or the underlying data in the SSTable. This function only frees the memory
 * allocated to a SSTable.
 *
 * @param table The SSTable to free.
 */
void SSTable_free(struct SSTable *table);

#endif /* WISCKEY_SSTABLE_H */