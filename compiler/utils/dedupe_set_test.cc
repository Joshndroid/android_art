/*
 * Copyright (C) 2013 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include "dedupe_set.h"

#include <algorithm>
#include <cstdio>

#include "gtest/gtest.h"
#include "thread-inl.h"

namespace art {

class DedupeHashFunc {
 public:
  size_t operator()(const std::vector<uint8_t>& array) const {
    size_t hash = 0;
    for (uint8_t c : array) {
      hash += c;
      hash += hash << 10;
      hash += hash >> 6;
    }
    return hash;
  }
};
TEST(DedupeSetTest, Test) {
  Thread* self = Thread::Current();
  typedef std::vector<uint8_t> ByteArray;
  SwapAllocator<void> swap(nullptr);
  DedupeSet<ByteArray, SwapVector<uint8_t>, size_t, DedupeHashFunc> deduplicator("test", swap);
  SwapVector<uint8_t>* array1;
  {
    ByteArray test1;
    test1.push_back(10);
    test1.push_back(20);
    test1.push_back(30);
    test1.push_back(45);

    array1 = deduplicator.Add(self, test1);
    ASSERT_NE(array1, nullptr);
    ASSERT_TRUE(std::equal(test1.begin(), test1.end(), array1->begin()));
  }

  SwapVector<uint8_t>* array2;
  {
    ByteArray test1;
    test1.push_back(10);
    test1.push_back(20);
    test1.push_back(30);
    test1.push_back(45);
    array2 = deduplicator.Add(self, test1);
    ASSERT_EQ(array2, array1);
    ASSERT_TRUE(std::equal(test1.begin(), test1.end(), array2->begin()));
  }

  SwapVector<uint8_t>* array3;
  {
    ByteArray test1;
    test1.push_back(10);
    test1.push_back(22);
    test1.push_back(30);
    test1.push_back(47);
    array3 = deduplicator.Add(self, test1);
    ASSERT_NE(array3, nullptr);
    ASSERT_TRUE(std::equal(test1.begin(), test1.end(), array3->begin()));
  }
}

}  // namespace art
