/*
 * Copyright 2025 Adam Bishop Comer
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

#ifndef WISCKEY_COMMON_H
#define WISCKEY_COMMON_H

#include <stdlib.h>

/**
 * @brief Lexigraphical comparison of two keys.
 *
 * @param lhs The left key value.
 * @param lhs_len The left key length.
 * @param rhs The right key value.
 * @param rhs_len The right key length.
 * @return The function returns a negative value if a < b, 0 if a = b, or a
 * positive value if a > b.
 */
int
WiscKey_key_cmp(const char* lhs,
                size_t lhs_len,
                const char* rhs,
                size_t rhs_len);

#endif /* WISKEY_COMMON_H */
