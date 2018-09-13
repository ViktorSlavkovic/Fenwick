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

#include "fenwick.h"

using fenwick::Fenwick;

////////////////////////////////////////////////////////////////////////////////
// MEASUREMENT METHODS
////////////////////////////////////////////////////////////////////////////////

// #define VERIFY

absl::Duration measure_prefix_sum(Fenwick* ft, int ntc,
                                  std::function<int64_t()> idx_gen) {
  // Generate.
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

absl::Duration measure_update(Fenwick* ft, int ntc,
                              std::function<int()> idx_gen,
                              std::function<int64_t()> val_gen) {
  // Generate
  std::vector<int> idxs;
  std::vector<int64_t> vals;
  for (int i = 0; i < ntc; i++) {
    idxs.push_back(idx_gen());
    vals.push_back(val_gen());
  }
  // Do the updates.
  auto start = absl::Now();
  for (int tc = 0; tc < ntc; tc++) {
    ft->update(idxs[tc], vals[tc]);
  }
  auto duration = absl::Now() - start;
  return duration / ntc;
}

absl::Duration measure_construct(Fenwick* ft, int n, int ntc,
                                 std::function<int64_t()> val_gen) {
  // Generate.
  std::vector<int64_t*> arrays(ntc);
  for (int64_t*& a : arrays) {
    a = new int64_t[n + 1];
    for (int i = 1; i <= n; i++) {
      a[i] = val_gen();
    }
  }
  // Do the constructions.
  auto start = absl::Now();
  for (int tc = 0; tc < ntc; tc++) {
    ft->construct(arrays[tc]);
    #ifdef VERIFY
      for (int i = 1; i <= n; i++) {
        assert(arrays[tc][i] == ft->access(i));
      }
    #endif
  }
  auto duration = absl::Now() - start;
  // Cleanup after construction.
  for (int64_t*& a : arrays) {
    delete[] a;
  }
  return duration / ntc;
}

absl::Duration measure_fast_construct(Fenwick* ft, int n, int ntc,
                                      std::function<int64_t()> val_gen) {
  // Generate.
  std::vector<int64_t*> arrays(ntc);
  for (int64_t*& a : arrays) {
    a = new int64_t[n + 1];
    for (int i = 1; i <= n; i++) {
      a[i] = val_gen();
    }
  }
  // Do the constructions.
  auto start = absl::Now();
  for (int tc = 0; tc < ntc; tc++) {
    #ifdef VERIFY
      std::vector<int64_t> bak(arrays[tc], arrays[tc] + n + 1);
    #endif
    ft->fast_construct(arrays[tc]);
    #ifdef VERIFY
      for (int i = 1; i <= n; i++) {
        assert(bak[i] == ft->access(i));
      }
    #endif
  }
  auto duration = absl::Now() - start;
  // Cleanup after construction.
  for (int64_t*& a : arrays) {
    delete[] a;
  }
  return duration / ntc;
}

absl::Duration measure_access(Fenwick* ft, int ntc,
                              std::function<int()> idx_gen) {
  // Generate.
  std::vector<int> idxs;
  for (int i = 0; i < ntc; i++) {
    idxs.push_back(idx_gen());
  }
  // Do the queries.
  auto start = absl::Now();
  for (int tc = 0; tc < ntc; tc++) {
    #ifndef VERIFY
      ft->access(idxs[tc]);
    #else
      assert(ft->access(idxs[tc]) == ft->fast_access(idxs[tc]));
    #endif
  }
  auto duration = absl::Now() - start;
  return duration / ntc;
}

absl::Duration measure_fast_access(Fenwick* ft, int ntc,
                                   std::function<int()> idx_gen) {
  // Generate.
  std::vector<int> idxs;
  for (int i = 0; i < ntc; i++) {
    idxs.push_back(idx_gen());
  }
  // Do the queries.
  auto start = absl::Now();
  for (int tc = 0; tc < ntc; tc++) {
    ft->fast_access(idxs[tc]);
  }
  auto duration = absl::Now() - start;
  return duration / ntc;
}

absl::Duration measure_search(Fenwick* ft, int ntc,
                              std::function<int64_t()> search_gen) {
  // Generate ntc values to search for.
  std::vector<int64_t> vals;
  for (int i = 0; i < ntc; i++) {
    vals.push_back(search_gen());
  }
  // Do the queries.
  auto start = absl::Now();
  for (int tc = 0; tc < ntc; tc++) {
    #ifndef VERIFY
      ft->search(vals[tc]);
    #else
      assert(ft->search(vals[tc]) == ft->fast_search(vals[tc]));
    #endif
  }
  auto duration = absl::Now() - start;
  return duration / ntc;
}

absl::Duration measure_fast_search(Fenwick* ft, int ntc,
                                   std::function<int64_t()> search_gen) {
  // Generate ntc values to search for.
  std::vector<int64_t> vals;
  for (int i = 0; i < ntc; i++) {
    vals.push_back(search_gen());
  }
  // Do the queries.
  auto start = absl::Now();
  for (int tc = 0; tc < ntc; tc++) {
    ft->fast_search(vals[tc]);
  }
  auto duration = absl::Now() - start;
  return duration / ntc;
}

absl::Duration measure_range_sum(Fenwick* ft, int ntc,
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
    #ifndef VERIFY
      ft->range_sum(ls[tc], rs[tc]);
    #else
      assert(ft->range_sum(ls[tc], rs[tc]) ==
             ft->fast_range_sum(ls[tc], rs[tc]));
    #endif
  }
  auto duration = absl::Now() - start;
  return duration / ntc;
}

absl::Duration measure_fast_range_sum(Fenwick* ft, int ntc,
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
    ft->fast_range_sum(ls[tc], rs[tc]);
  }
  auto duration = absl::Now() - start;
  return duration / ntc;
}

absl::Duration measure_rupq_update(Fenwick* ft, int ntc,
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
    ft->rupq_update(ls[tc], rs[tc], vals[tc]);
  }
  auto duration = absl::Now() - start;
  return duration / ntc;
}

////////////////////////////////////////////////////////////////////////////////

constexpr int64_t kMaxVal = 1000;
constexpr int kMinOrder = 7;
constexpr int kMaxOrder = 27;
const std::unordered_map<int, int> kNumEachPerOrder {
  { 7, 10000}, { 8, 10000}, { 9, 10000}, {10, 10000}, {11, 10000}, {12,  1000},
  {13,  1000}, {14,  1000}, {15,  1000}, {16,  1000}, {17,   800}, {18,   500},
  {19,   100}, {20,   100}, {21,   100}, {22,   100}, {23,   100}, {24,   100},
  {25,    50}, {26,    50}, {27,    50}, {28,    50}, {29,    50}, {30,    50},
};

#define PLOT_DUMP

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
    auto* ft = Fenwick::allocate(order);
    int n = (1 << order) - 1;
    std::uniform_int_distribution<int> idx_dist(1, n);
    auto idx_gen = [&]() { return idx_dist(prng); };
    const int kNumEach = kNumEachPerOrder.at(order);
    // 1) Prefix Sum
    {
      auto duration = measure_prefix_sum(ft, kNumEach, idx_gen);
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
      auto duration = measure_update(ft, kNumEach, idx_gen, val_gen);
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x update: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    // 3) Construction
    {
      ft->clear();
      auto duration = measure_construct(ft, n, kNumEach, val_gen);
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x construct: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    // 4) Fast Construction
    {
      auto duration = measure_fast_construct(ft, n, kNumEach, val_gen);
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x fast_construct: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    // 5) Access
    {
      auto duration = measure_access(ft, kNumEach, idx_gen);
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x access: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    // 6) Fast Access
    {
      auto duration = measure_fast_access(ft, kNumEach, idx_gen);
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x fast_access: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    // 7) Search
    {
      std::uniform_int_distribution<int64_t> search_dist(
          1, ft->prefix_sum(n) * 2);
      auto search_gen = [&]() { return search_dist(prng); };
      auto duration = measure_search(ft, kNumEach, search_gen);
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x search: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    // 8) Fast Search
    {
      std::uniform_int_distribution<int64_t> search_dist(
          1, ft->prefix_sum(n) * 2);
      auto search_gen = [&]() { return search_dist(prng); };
      auto duration = measure_fast_search(ft, kNumEach, search_gen);
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x fast_search: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    // 9) Range Sum
    {
      auto duration = measure_range_sum(ft, kNumEach, idx_gen);
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x range_sum: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    // 10) Fast Range Sum
    {
      auto duration = measure_fast_range_sum(ft, kNumEach, idx_gen);
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x fast_range_sum: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    // 11) RUPQ Update
    {
      ft->clear();
      auto duration = measure_rupq_update(ft, kNumEach, idx_gen, val_gen);
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x rupq_update: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    free(ft);
    printf("\n");
  }
  return 0;
}
