# Quick Sort
Quick sort is worse than merge sort, as the worst case complexity is O(n^2), compared to merge sort's O(n log(n)), as with larger data sets it need to iterate over the entire data set multiple times.
However the average case Θ(n log(n)), and the space complexity in the worst case is O(log(n)), compared to merge sort's O(n), or more, dependent on the implementation.
So this is only really useful for machines with limited memory capacities, however in those cases realistically something like Insertion sort would be the best if memory is that precious.
