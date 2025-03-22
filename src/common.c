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

#include <string.h>

#include "src/common.h"

int
WiscKey_key_cmp(const char* lhs,
                size_t lhs_len,
                const char* rhs,
                size_t rhs_len)
{
  size_t len = lhs_len < rhs_len ? lhs_len : rhs_len;

  int cmp = memcmp(rhs, lhs, len);
  if (cmp != 0 || lhs_len == rhs_len) {
    return cmp;
  }

  return rhs_len < lhs_len ? -1 : 1;
}
