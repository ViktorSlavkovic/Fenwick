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

#ifndef FENWICK_RMQ_H_
#define FENWICK_RMQ_H_

#include <algorithm>
#include <climits>
#include <vector>

namespace fenwick {
namespace rmq {

// Implements a structure consisting of a Fenwick tree, a counter Fenwick tree
// and an up-to-date array to solve the Dymanic RMQ problem.
struct FenwickRMQ {
  explicit FenwickRMQ(int n)
      : n(n), a(n + 1, INT_MAX), lbit(n + 1, INT_MAX), rbit(n + 1, INT_MAX) {}
  // Returns minimum value among a[from], ... , a[to].
  // Complexity: O(log n)
  // Assumes that: 1 <= from <= to <= n.
  int query(int from, int to) {
    if (from < 1) return INT_MAX;
    if (to > n) return INT_MAX;
    if (from > to) return INT_MAX;

    int res = INT_MAX;

    // Climb rbit.
    int i = from;
    int ii = i + (i & -i);
    while (i <= n && ii - 1 <= to) {
      res = std::min(res, rbit[i]);
      i = ii;
      ii = i + (i & -i);
    }
    if (i <= to) res = std::min(res, a[i]);

    // Climb lbit.
    i = to;
    ii = i - (i & -i);
    while (i >= 1 && ii + 1 >= from) {
      res = std::min(res, lbit[i]);
      i = ii;
      ii = i - (i & -i);
    }
    if (i >= from) res = std::min(res, a[i]);

    return res;
  }
  // Sets a[idx] to val.
  // Complexity: O(log n)
  // Assumes that: 1 <= idx <= n.
  void update(int idx, int val) {
    if (a[idx] == val) return;

    int l, r; /* [l, r] - resp. range of the current node */
    int ll, lr, lmin; /* range (ll, lr] of the current node in the left side
                         (..., idx-1] climb and left side min built so far. */
    int rl, rr, rmin; /* range [rl, rr) of the current node in the left side
                         [idx+1, ...) climb and right side min built so far. */

    #define SIDES_INIT {                                                       \
      lr = idx - 1;                                                            \
      ll = lr - (lr & -lr);                                                    \
      lmin = INT_MAX;                                                          \
      rl = idx + 1;                                                            \
      rr = rl + (rl & -rl);                                                    \
      rmin = INT_MAX;                                                          \
    }

    #define SIDES_UPDATE {                                                     \
      while (lr >= 1 && ll + 1 >= l) {                                         \
        lmin = std::min(lmin, lbit[lr]);                                       \
        lr = ll;                                                               \
        ll = lr - (lr & -lr);                                                  \
      }                                                                        \
      if (lr >= l) {                                                           \
        lmin = std::min(lmin, a[lr]);                                          \
      }                                                                        \
      while (rl <= n && rr - 1 <= r) {                                         \
        rmin = std::min(rmin, rbit[rl]);                                       \
        rl = rr;                                                               \
        rr = rl + (rl & -rl);                                                  \
      }                                                                        \
      if (rl <= std::min(n, r)) {                                              \
        rmin = std::min(rmin, a[rl]);                                          \
      }                                                                        \
    }

    // Update lbit.
    r = idx;
    l = r - (r & -r) + 1;
    SIDES_INIT;
    while (r <= n) {
      if (val <= lbit[r]) {
        // If the new value is less or equeal to the previous node min, use the
        // new value.
        lbit[r] = val;
      } else if (lbit[r] == a[idx]) {
        SIDES_UPDATE;
        // Update lbit node.
        lbit[r] = std::min(val, std::min(lmin, rmin));
      }
      // Advance lbit.
      r = r + (r & -r);
      l = r - (r & -r) + 1;
    }
    // Update rbit.
    l = idx;
    r = l + (l & -l) - 1;
    SIDES_INIT;
    while (l >= 1) {
      if (val <= rbit[l]) {
        // If the new value is less or equeal to the previous node min, use the
        // new value.
        rbit[l] = val;
      } else if (rbit[l] == a[idx]) {
        SIDES_UPDATE;
        // Update rbit node.
        rbit[l] = std::min(val, std::min(lmin, rmin));
      }
      // Advance rbit.
      l = l - (l & -l);
      r = l + (l & -l) - 1;
    }
    // Update the array.
    a[idx] = val;
  }

  // Size of the trees and the array.
  int n;
  // The array in question. Kept up-to-date.
  std::vector<int> a;
  // Refular Fenwick tree.
  std::vector<int> lbit;
  // Counter Fenwick tree.
  std::vector<int> rbit;
};

}  // namespace rmq
}  // namespace fenwick

#endif  // FENWICK_RMQ_H_
