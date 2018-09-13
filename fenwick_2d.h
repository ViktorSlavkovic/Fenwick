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

#ifndef FENWICK_2D_H_
#define FENWICK_2D_H_

#include <algorithm>
#include <cstdio>
#include <cstdlib>

namespace fenwick {

// Implements 2D Fenwick (Binary-Indexed) Tree data strucutre with it's basic
// operations. Consider that all the operations are performed on a fictive array
// a[1..n][1..n].
struct Fenwick2D {
  // Allocates the structure of size n x n, n = 2^m - 1 and sets all the
  // elements to 0. This is the one true way of creating this struct.
  // Delete with free(3).
  static Fenwick2D* allocate(int m) {
    int n = (1 << m) - 1;
    Fenwick2D* instance = reinterpret_cast<Fenwick2D*>(
        malloc(sizeof(Fenwick2D) + (n + 1) * (n + 1) * sizeof(int64_t)));
    instance->n = n;
    std::fill_n(instance->T, (n + 1) * (n + 1), 0LL);
    return instance;
  }

  // Disable other creation, copying ans assigning.
  Fenwick2D() = delete;
  Fenwick2D(const Fenwick2D&) = delete;
  void operator=(const Fenwick2D&) = delete;

  // Sets all array elements to 0.
  void clear() { std::fill_n(T, (n + 1) * (n + 1), 0LL); }

  // Calculates the prefix sum: a[1:x][1:y].
  // Complexity: O(log^2 n)
  // Assumes that: 1 <= x, y <= n.
  int64_t prefix_sum(int x, int y) const {
    int64_t sum = 0;
    for (; x >= 1; x -= x & -x) {
      int yy = y;
      for (; yy >= 1; yy -= yy & -yy) {
        sum += T[(n + 1) * yy + x];
      }
    }
    return sum;
  }

  // Adds delta to all a[x][y].
  // Complexity: O(log^2 n)
  // Assumes that: 1 <= x, y <= n.
  void update(int x, int y, int64_t delta) {
    for (; x <= n; x += x & -x) {
      int yy = y;
      for (; yy <= n; yy += yy & -yy) {
        T[(n + 1) * yy + x] += delta;
      }
    }
  }

  // Calculates the range sum: a[x1:x2][y1:y2].
  // Complexity: O(log^2 n)
  // Assumes that: 1 <= x1 <= x2 <= n and 1 <= y1 <= y2 <= n.
  int64_t range_sum(int x1, int y1, int x2, int y2) const {
    int64_t sum = prefix_sum(x2, y2);
    if (x1 > 1) {
      sum -= prefix_sum(x1 - 1, y2);
    }
    if (y1 > 1) {
      sum -= prefix_sum(x2, y1 - 1);
    }
    if (x1 > 1 && y1 > 1) {
      sum += prefix_sum(x1 - 1, y1 - 1);
    }
    return sum;
  }

  // Size of the tree and the fictive array a are n x n.
  int n;
  // The tree storage array.
  int64_t T[];
};

}  // namespace fenwick

#endif  // FENWICK_2D_H_
