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

#ifndef FENWICK_H_
#define FENWICK_H_

#include <algorithm>
#include <cstdio>
#include <cstdlib>

namespace fenwick {

// Implements Fenwick (Binary-Indexed) Tree data strucutre with it's basic
// operations, as well as the additional operations used to solve the Dynamic
// Partial Sums problem and it's variations:
//   * range-update point-query
//   * point-update range-query
// Consider that all the operations are performed on a fictive array a[1..n].
struct Fenwick {
  // Allocates the structure of size n = 2^m - 1 and sets all the elements to 0.
  // This is the one true way of creating this struct.
  // Delete with free(3).
  static Fenwick* allocate(int m) {
    int n = (1 << m) - 1;
    int nmask = 1 << (m - 1);
    Fenwick* instance = reinterpret_cast<Fenwick*>(
        malloc(sizeof(Fenwick) + (n + 1) * sizeof(int64_t)));
    instance->nmask = nmask;
    instance->n = n;
    std::fill_n(instance->T, n + 1, 0LL);
    return instance;
  }

  // Disable other creation, copying ans assigning.
  Fenwick() = delete;
  Fenwick(const Fenwick&) = delete;
  void operator=(const Fenwick&) = delete;

  // Sets all array elements to 0.
  void clear() { std::fill_n(T, n + 1, 0LL); }

  // Calculates the prefix sum: a[1] + ... + a[idx].
  // Complexity: O(log n)
  // Assumes that: 1 <= idx <= n.
  int64_t prefix_sum(int idx) const {
    int64_t sum = 0;
    for (; idx >= 1; idx -= idx & -idx) {
      sum += T[idx];
    }
    return sum;
  }
  // Adds delta to a[idx].
  // Complexity: O(log n)
  // Assumes that: 1 <= idx <= n.
  void update(int idx, int64_t delta) {
    for (; idx <= n; idx += idx & -idx) {
      T[idx] += delta;
    }
  }
  // Constructs the tree from an array of the same size, n.
  // Complexity: O(n log n)
  void construct(const int64_t* a) {
    for (int i = n; i >= 1; i--) {
      T[i] = 0LL;
      update(i, a[i]);
    }
  }
  // Constucts the tree from an array  of the same size, n. Array gets modified
  // into it's cumulative sums array!
  // Complexity: O(n)
  void fast_construct(int64_t* a) {
    a[0] = 0LL;
    for (int i = 1; i <= n; i++) {
      a[i] += a[i - 1];
    }
    for (int i = 1; i <= n; i++) {
      int ii = i - (i & -i);
      T[i] = (ii == 0) ? a[i] : a[i] - a[ii];
    }
  }
  // Returns a[idx].
  // Complexity O(log n)
  // Assumes that: 1 <= idx <= n.
  int64_t access(int idx) const {
    return (idx == 1) ? T[1] : prefix_sum(idx) - prefix_sum(idx - 1);
  }
  // Returns a[idx].
  // Complexity: O(1) on average, O(log n) worst.
  // Assumes that: 1 <= idx <= n.
  int64_t fast_access(int idx) const {
    int64_t sum = T[idx];
    int i = idx - (idx & -idx);
    int j = idx - 1;
    while (i != j) {
      if (i > j) {
        sum += T[i];
        i -= i & -i;
      } else {
        sum -= T[j];
        j -= j & -j;
      }
    }
    return sum;
  }
  // Returns the smallest k, such that a[1] + ... + a[k] is GEQ than val on
  // success. Returns n + 1 if the total sum is smaller.
  // Complexity: O(n log n).
  int search(int64_t val) const {
    for (int i = 1; i <= n; i++) {
      if (prefix_sum(i) >= val) {
        return i;
      }
    }
    return n + 1;
  }
  // Returns the smallest k, such that a[1] + ... + a[k] is GEQ than val on
  // success. Returns n + 1 if the total sum is smaller.
  // Complexity: O(log n)
  // Note: This only works if the cumulative sums are nondecreasing!
  int fast_search(int64_t val) const {
    val--;
    int i = 0;
    int mask = nmask;
    while (mask != 0) {
      int ii = i + mask;
      mask >>= 1;
      if (ii > n) {
        continue;
      }
      if (T[ii] <= val) {
        val -= T[ii];
        i = ii;
      }
    }
    return i + 1;
  }
  // Returns a[l] + ... + a[r]. This solves the point-update range-query variant
  // of the Dynamic Partial Sums problem and is comaptible with all of the above
  // update methods.
  // Complexity: O(log n)
  // Assumes that: 1 <= l <= r <= n.
  int64_t range_sum(int l, int r) const {
    int64_t sum = prefix_sum(r);
    if (l > 1) {
      sum -= prefix_sum(l - 1);
    }
    return sum;
  }
  // Returns a[l] + ... + a[r] in the point-update range-query variant of the
  // Dynamic Partial Sums problem and is comaptible with all of the above
  // update methods.
  // Complexity: optimized, but sitll O(log n)
  // Assumes that: 1 <= l <= r <= n.
  int64_t fast_range_sum(int l, int r) const {
    int64_t sum = T[r];
    int i = r - (r & -r);
    int j = l - 1;
    while (i != j) {
      if (i > j) {
        sum += T[i];
        i -= i & -i;
      } else {
        sum -= T[j];
        j -= j & -j;
      }
    }
    return sum;
  }

  // The following methods solve range-update point-query problem and are not
  // allowed to be used with any of the above methods, except, of course,
  // allocate.                                                             (!)
  //
  // Adds delta to all a[x] where l <= x <= r.
  // Complexity: O(log n)
  // Assumes that: 1 <= l <= r <= n.
  void rupq_update(int l, int r, int64_t delta) {
    update(l, delta);
    if (r < n) {
      update(r + 1, -delta);
    }
  }
  // Returns a[idx].
  // Complexity: O(log n)
  // Assumes that: 1 <= idx <= n.
  int64_t rupq_access(int idx) const { return prefix_sum(idx); }

  // Size of the tree and the fictive array a.
  int n;
  // Highest set bit in n isolated.
  int nmask;
  // The tree storage array.
  int64_t T[];
};

}  // namespace fenwick

#endif  // FENWICK_H_
