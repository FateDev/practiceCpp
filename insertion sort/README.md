# Insertion Sort
So this has 2 nested loops, just like Bubble Sort, however the inner one starts from the end of the array, and loops until the counter for the inner loop is no longer greater than the counter of the inner loop.
(What I'll describe will be the sorting in ascending order, lowest to highest).
As the outer loop is incrementing, and the inner loop is decrementing, and we are tying to sort in ascending order, whenever array\[outer loop counter\] is greater than array\[inner loop counter\] a swap needs to occur.
And as this will mean the outer loop index will be matched against every value of a higher index, we can consider indexes which are less than the outer loop counter to be sorted.
And so a maximum of n^2 swaps will occur, so the worst case scenario time complexity of this sorting algorithm is O(n^2).
However, Bubble sort generally does about twice as many operations for a list to be sorted, so Insertion sort is preferable.
