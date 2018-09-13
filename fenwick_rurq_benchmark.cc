// Copyright (c) 2018 Viktor Slavkovic
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <functional>
#include <random>
#include <unordered_map>
#include <vector>

#include "absl/strings/substitute.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"

#include "fenwick_rurq.h"

using fenwick::FenwickRURQ;

////////////////////////////////////////////////////////////////////////////////
// MEASUREMENT METHODS
////////////////////////////////////////////////////////////////////////////////

absl::Duration measure_prefix_sum(FenwickRURQ* ft, int ntc,
                                  std::function<int64_t()> idx_gen) {
  // Generate ntc indexes to query.
  std::vector<int> idxs;
  for (int i = 0; i < ntc; i++) {
    idxs.push_back(idx_gen());
  }
  // Do the queries.
  auto start = absl::Now();
  for (int tc = 0; tc < ntc; tc++) {
    ft->prefix_sum(idxs[tc]);
  }
  auto duration = absl::Now() - start;
  return duration / ntc;
}

absl::Duration measure_range_sum(FenwickRURQ* ft, int ntc,
                                 std::function<int()> idx_gen) {
  // Generate.
  std::vector<int> ls;
  std::vector<int> rs;
  for (int i = 0; i < ntc; i++) {
    int l = idx_gen();
    int r = idx_gen();
    if (l > r) {
      std::swap(l, r);
    }
    ls.push_back(l);
    rs.push_back(r);
  }
  // Do the queries.
  auto start = absl::Now();
  for (int tc = 0; tc < ntc; tc++) {
    ft->range_sum(ls[tc], rs[tc]);
  }
  auto duration = absl::Now() - start;
  return duration / ntc;
}

absl::Duration measure_update(FenwickRURQ* ft, int ntc,
                              std::function<int()> idx_gen,
                              std::function<int()> val_gen) {
  // Generate.
  std::vector<int> ls;
  std::vector<int> rs;
  std::vector<int64_t> vals;
  for (int i = 0; i < ntc; i++) {
    int l = idx_gen();
    int r = idx_gen();
    if (l > r) {
      std::swap(l, r);
    }
    ls.push_back(l);
    rs.push_back(r);
    vals.push_back(val_gen());
  }
  // Do the updates.
  auto start = absl::Now();
  for (int tc = 0; tc < ntc; tc++) {
    ft->update(ls[tc], rs[tc], vals[tc]);
  }
  auto duration = absl::Now() - start;
  return duration / ntc;
}

////////////////////////////////////////////////////////////////////////////////

constexpr int64_t kMaxVal = 1000;
constexpr int kMinOrder = 7;
constexpr int kMaxOrder = 28;
const std::unordered_map<int, int> kNumEachPerOrder {
  { 7, 10000}, { 8, 10000}, { 9, 10000}, {10, 10000}, {11, 10000}, {12,  1000},
  {13,  1000}, {14,  1000}, {15,  1000}, {16,  1000}, {17,   800}, {18,   500},
  {19,   100}, {20,   100}, {21,   100}, {22,   100}, {23,   100}, {24,   100},
  {25,    50}, {26,    50}, {27,    50}, {28,    50}, {29,    50}, {30,    50},
};

// #define PLOT_DUMP

int main() {
  std::random_device rd;
  std::mt19937 prng(rd());
  std::uniform_int_distribution<int64_t> elem_dist(0, kMaxVal);
  auto val_gen = [&]() { return elem_dist(prng); };

  for (int order = kMinOrder; order <= kMaxOrder; order++) {
    #ifndef PLOT_DUMP
      printf("Order: %d\n", order);
    #else
      printf("%d\t", order);
    #endif
    FenwickRURQ ft(order);
    int n = (1 << order) - 1;
    std::uniform_int_distribution<int> idx_dist(1, n);
    auto idx_gen = [&]() { return idx_dist(prng); };
    const int kNumEach = kNumEachPerOrder.at(order);
    // 1) Prefix Sum
    {
      auto duration = measure_prefix_sum(&ft, kNumEach, idx_gen);
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x prefix_sum: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    // 2) Update
    {
      auto duration = measure_update(&ft, kNumEach, idx_gen, val_gen);
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x update: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    // 3) Range Sum
    {
      auto duration = measure_range_sum(&ft, kNumEach, idx_gen);
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x range_sum: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    printf("\n");
  }
  return 0;
}
