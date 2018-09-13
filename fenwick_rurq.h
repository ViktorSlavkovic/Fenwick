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

#ifndef FENWICK_RURQ_H_
#define FENWICK_RURQ_H_

#include "fenwick.h"

namespace fenwick {

// Implements a structure consisting of two Fenwick trees to solve the
// range-update range-query varioation of the Dynamic Partial Sums problem.
struct FenwickRURQ {
  // This structure can be normally constructed.
  explicit FenwickRURQ(int m) {
    T1 = Fenwick::allocate(m);
    T2 = Fenwick::allocate(m);
    n = T1->n;
  }

  // Disable other copying ans assigning.
  FenwickRURQ(const Fenwick&) = delete;
  void operator=(const Fenwick&) = delete;

  ~FenwickRURQ() {
    free(T1);
    free(T2);
  }

  // Adds delta to all a[x] where l <= x <= r.
  // Complexity: O(log n)
  // Assumes that: 1 <= l <= r <= n.
  void update(int l, int r, int64_t delta) {
    T1->update(l, delta);
    T2->update(l, delta * (l - 1));
    if (r < n) {
      T1->update(r + 1, -delta);
      T2->update(r + 1, delta * r);
    }
  }
  // Returns a[1] + ... + a[idx].
  // Complexity: O(log n)
  // Assumes that: 1 <= idx <= n.
  int64_t prefix_sum(int idx) const {
    return T1->prefix_sum(idx) * idx - T2->prefix_sum(idx);
  }
  // Returns a[l] + ... + a[r].
  // Complexity: O(log n)
  // Assumes that: 1 <= l <= r <= n.
  int64_t range_sum(int l, int r) const {
    int64_t sum = prefix_sum(r);
    if (l > 1) {
      sum -= prefix_sum(l - 1);
    }
    return sum;
  }

  // Size of the trees and the fictive array.
  int n;
  // The two fenwick trees.
  Fenwick* T1;
  Fenwick* T2;
};

}  // namespace fenwick

#endif  // FENWICK_RURQ_H_
