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

#ifndef WISCKEY_VALUE_LOG_H
#define WISCKEY_VALUE_LOG_H

#include <stdio.h>

/**
 * @file
 * @author Adam Comer <adambcomer@gmail.com>
 * @date May 15, 2020
 * @copyright Apache-2.0 License
 * @brief Log file of the key-value pairs.
 */

/**
 * @brief Value Log of the Database.
 *
 * The Value Log stores the values in the database in the order that they were written. New entries are written to the
 * head of the file. To remove values that have been overwritten or deleted, the database runs a garbage collection
 * process that hole-punches the file with `fallocate`. After the file has been hole-punched, the tail is updated to the
 * position of the last known write that is still live.
 *
 * The ValueLog entries also hold a copy of the key to speed up the garbage collection procces.
 */
struct ValueLog {
    FILE *file; ///< The file that the values are written to.
    size_t head; ///< The head of the ValueLog. This is where the next value will be written.
    size_t tail; ///< The tail of the ValueLog. This is the position of the oldest write that hasn't been overwritten or deleted.
};

/**
 * @brief Creates a new ValueLog or loads an existing one from disk.
 *
 * If the ValueLog file already exists, this function will only open the file without scanning it. This function
 * doesn't check if file has been corrupted.
 *
 * Note: Free this ValueLog with ValueLog_free.
 *
 * @param path Path to the ValueLog. The path must be be null-terminated.
 * @param head The head of the ValueLog. Set to 0 for a new ValueLog.
 * @param tail The tail of the ValueLog. Set to 0 for a new ValueLog.
 * @return A pointer to a ValueLog.
 */
struct ValueLog *ValueLog_new(const char *path, size_t head, size_t tail);

/**
 * @brief Appends a new key-value pair to the ValueLog.
 *
 * In the event that the write was unsuccessful, the ValueLog won't shift the head forward. Therefore, writes can be
 * retried without the risk of polluting the ValueLog or corrupting past entries.
 *
 * @param log The ValueLog to write to.
 * @param pos A pointer that is assigned to the location on the where the key-value pair was written to.
 * @param key The key being written.
 * @param key_len The length of the key.
 * @param value The value that is being written.
 * @param value_len The length of the value.
 * @return This function returns 0 if the entry was written successfully and -1 if there was an error.
 */
int ValueLog_append(struct ValueLog *log, size_t *pos, const char *key, size_t key_len, const char *value,
                    size_t value_len);

/**
 * @brief Fetches a value from the ValueLog at a given position.
 *
 * Note: The value pointer will allocate memory to hold the value that is being requested. The caller is responsible for
 * freeing the memory.
 *
 * @param log The ValueLog to read from.
 * @param value A double-pointer for the value. This pointer will set with a newly allocated block of memory with the value that the caller can access.
 * @param value_len A pointer that is assigned to the the length of the value pointer.
 * @param value_loc The location on the ValueLog that this value resides in.
 * @return This function returns 0 if the value was retrieved successfully and -1 if there was an error.
 */
int ValueLog_get(const struct ValueLog *log, char **value, size_t *value_len, size_t value_loc);

/**
 * @brief Syncs the ValueLog to the disk.
 *
 * This function forcefully flushes the changes to the ValueLog to disk. Use this function sparingly as is will heavily
 * reduce the write throughput of the ValueLog.
 *
 * @param log The ValueLog to flush to disk.
 * @return This function returns 0 if the ValueLog successfully synced to the disk and -1 if there was an error.
 */
int ValueLog_sync(const struct ValueLog *log);

/**
 * @brief Frees the ValueLog.
 *
 * Note: This function won't delete the file or the underlying data in the ValueLog. This function only frees the memory
 * allocated to a ValueLog.
 *
 * @param log The ValueLog to free.
 */
void ValueLog_free(struct ValueLog *log);

#endif /* WISCKEY_VALUE_LOG_H */