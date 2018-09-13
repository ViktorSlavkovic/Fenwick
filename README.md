# Analysis, Implementation and Applications of Fenwick Trees

Here you can find implementation of Fenwick tree data structure and it's
applications to Dynamic RMQ and Dynamic Partial Sums (and it's variations)
problems. What's covered:

- Standard operations with Fenwick trees (solves 1D Dynamic Partial Sums problem)
  - Prefix Sum
  - Point Update
  - Range Sum
  - Optimized Range Sum
  - Read Single
  - Optimized Read Single (O(1) on average!)
  - Search (for an index with specified prefix sum)
  - Optimized Search
  - Construction from an array in O(n) and in O(n log n)
- Point-Update Range-Query (covered above)
- Range-Update Point-Query
- Range-Update Range-Query (with 2 trees)
- 2D Fenwick Tree
  - 2D Prefix Sum
  - 2D Range Sum
  - Point Update
- Dynamic RMQ Structure with a regular and a counter Fenwick tree
  - Indented item
  - Indented item

There are some random tests and benchmarks included as well. However, if all you
care about are the implementations, .h files are all you need.

What's coming:

- A paper with explanation of the structures and operations and detailed
complexity analysis (with all the proofs!) in English and Serbian
- Benchmark plots
