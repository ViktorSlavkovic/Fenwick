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

#include <cassert>
#include <cstdio>
#include <random>

#include "fenwick_rmq.h"

using fenwick::rmq::FenwickRMQ;

constexpr int kNumTestCases = 1000;
constexpr int kNumSessions = 100;
constexpr int kQueriesPerSession = 10;
constexpr int kUpdatesPerSession = 10;

constexpr int kMaxN = 100000;
constexpr int kMaxVal = 1000;

int main() {
  std::random_device rd;
  std::mt19937 prng(rd());
  std::uniform_int_distribution<int> ngen(1, kNumTestCases);
  std::uniform_int_distribution<int> vgen(0, kMaxVal);

  for (int tc = 1; tc <= kNumTestCases; tc++) {
    int n = ngen(prng);
    FenwickRMQ rmq(n);
    std::uniform_int_distribution<int> igen(1, n);
    // Prefil, since it's all lower than INT_MAX, this is a simple case.
    for (int i = 1; i <= n; i++) {
      rmq.update(i, vgen(prng));
    }
    // Do a bunch of queries, mixed.
    for (int s = 0; s < kNumSessions; s++) {
      for (int u = 0; u < kUpdatesPerSession; u++) {
        rmq.update(igen(prng), vgen(prng));
      }
      for (int q = 0; q < kQueriesPerSession; q++) {
        int l = igen(prng);
        int r = igen(prng);
        if (l > r) {
          std::swap(l, r);
        }
        int sol = *std::min_element(rmq.a.begin() + l, rmq.a.begin() + r + 1);
        int myres = rmq.query(l, r);
        assert(sol == myres);
      }
    }
  }
  printf("Success!\n");
  return 0;
}
