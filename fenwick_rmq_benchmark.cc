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

#include <cstdio>
#include <random>
#include <unordered_map>

#include "absl/strings/substitute.h"
#include "absl/time/clock.h"
#include "absl/time/time.h"

#include "fenwick_rmq.h"

using fenwick::rmq::FenwickRMQ;

constexpr int kMaxVal = 1 << 30;
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
  std::uniform_int_distribution<int> vgen(0, kMaxVal);

  for (int order = kMinOrder; order <= kMaxOrder; order++) {
    #ifndef PLOT_DUMP
      printf("Order: %d\n", order);
    #else
      printf("%d\t", order);
    #endif
    int n = (1 << order) - 1;
    FenwickRMQ rmq(n);
    std::uniform_int_distribution<int> igen(1, n);
    const int kNumEach = kNumEachPerOrder.at(order);
    // Prefil, since it's all lower than INT_MAX, this is a simple case.
    for (int i = 1; i <= n; i++) {
      rmq.update(i, vgen(prng));
    }
    // Measure updates.
    {
      // Generate.
      std::vector<int> is;
      std::vector<int> vs;
      for (int i = 0; i < kNumEach; i++) {
        is.push_back(igen(prng));
        vs.push_back(vgen(prng));
      }
      // Run.
      auto start = absl::Now();
      for (int i = 0; i < kNumEach; i++) {
        rmq.update(is[i], vs[i]);
      }
      auto duration = absl::Now() - start;
      // Print.
      duration /= kNumEach;
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x update: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    // Measure queries.
    {
      // Generate.
      std::vector<int> ls;
      std::vector<int> rs;
      for (int i = 0; i < kNumEach; i++) {
        int l = igen(prng);
        int r = igen(prng);
        if (l > r) {
          std::swap(l, r);
        }
        ls.push_back(l);
        rs.push_back(r);
      }
      // Run.
      auto start = absl::Now();
      for (int i = 0; i < kNumEach; i++) {
        rmq.query(ls[i], rs[i]);
      }
      auto duration = absl::Now() - start;
      // Print.
      duration /= kNumEach;
      #ifndef PLOT_DUMP
        auto msg = absl::Substitute("$0 x query: $1: ", kNumEach,
                                    absl::FormatDuration(duration));
        printf("  %s\n", msg.c_str());
      #else
        printf("%15.3lf\t", ToDoubleNanoseconds(duration));
      #endif
    }
    printf("\n");
  }

}
