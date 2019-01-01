# Bubble Sort
So this has 2 nested loops, both start from index 0 and increment to the maximum value.
(What I'll describe will be the sorting in ascending order, lowest to highest).
As the outer loop is incrementing, and the inner loop is incrementing full through each iteration of the outer loop, the biggest values will 'bubble' to the top of the array through the comparisons and the swaps, and the smaller values will 'fall' to the bottom, and as there will be n^2 comparisons the list is guaranteed to be sorted so the time complexity of this sorting algorithm is O(n^2).
