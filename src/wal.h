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

#ifndef WISCKEY_WAL_H
#define WISCKEY_WAL_H

#include <stdint.h>
#include <stdio.h>

#include "memtable.h"

/**
 * @file
 * @author Adam Comer <adambcomer@gmail.com>
 * @date May 15, 2020
 * @copyright Apache-2.0 License
 * @brief Write-Ahead Log for the keys and the value positions.
 */

/**
 * @brief Write-Ahead Log(WAL) of the Database.
 *
 * The WAL holds a running log of the operations that were applied to the
 * MemTable. When the database restarts, the WAL is replayed to recover the
 * MemTable.
 */
struct WAL
{
  FILE* file; ///< The file that the WAL writes the keys to.
  char* path; ///< The path of the WAL file.
};

/**
 * @brief Creates a new emtpy WAL.
 *
 * Note: If a WAL already exists at this path, the file will not be overwritten.
 *
 * @param path The path of the new WAL file.
 * @return A pointer to a new WAL.
 */
struct WAL*
WAL_new(char* path);

/**
 * @brief Replays the WAL from the start and recreates the MemTable.
 *
 * This is the main recovery function for the WAL for when the Database
 * restarts.
 *
 * @param wal The WAL to replay the log from.
 * @param memtable A empty MemTable to replay the log into.
 * @return This function returns 0 if the WAL successfully replayed and -1 if
 * there was an error.
 */
int
WAL_load_memtable(struct WAL* wal, struct MemTable* memtable);

/**
 * @brief Appends a new MemTable operation to the WAL.
 *
 * For sets, the `value_loc` should be the position in the ValueLog that the
 * value entry was written to. For deletes, `value_loc` should be -1 to indicate
 * a tombstone.
 *
 * @param wal The WAL to append a MemTable operation to.
 * @param key The key to apply an operation to.
 * @param key_len The length of the key.
 * @param value_loc The location in the ValueLog of the value or -1 if it is
 * being deleted.
 * @return This function returns 0 if the operation was successfully writen to
 * the WAL and -1 if there was an error.
 */
int
WAL_append(struct WAL* wal, const char* key, size_t key_len, int64_t value_loc);

/**
 * @brief Syncs the WAl to the disk.
 *
 * This function forcefully flushes the changes to the WAL to disk. Use this
 * function sparingly as is will heavily reduce the write throughput of the WAL.
 *
 * @param wal The WAL to flush to disk.
 * @return This function returns 0 if the WAL successfully synced to the disk
 * and -1 if there was an error.
 */
int
WAL_sync(const struct WAL* wal);

/**
 * @brief Frees the WAL.
 *
 * Note: This function won't delete the file or the underlying data in the WAL.
 * This function only frees the memory allocated to a WAL.
 *
 * @param wal The WAL to free.
 */
void
WAL_free(struct WAL* wal);

#endif /* WISCKEY_WAL_H */