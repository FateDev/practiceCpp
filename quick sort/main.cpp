#include <iostream>

bool quickSort(int *arr, int begin, int end); //function prototype

int main(){
    int array[10] = {1540,659,8974,725,61,55,4233,357,211,1002}; //the array

    quickSort(array, 0, sizeof(array)/sizeof(int)); //passes the array, the end point (size of the array), and the beginning point (0)

    for(int i = 0; i < 10; i++){ //prints the sorted array
        std::cout << array[i] << std::endl;
    }
}

void swap(int *arr, int index1, int index2, int index3){ //swaps 3 index of an array
    int temp1 = arr[index1];
    int temp2 = arr[index2];
    arr[index2] = arr[index3];
    arr[index1] = temp2;
    arr[index3] = temp1;
}

void swap(int *arr, int index1, int index2){ //swaps 2 indexes of an array
    int temp1 = arr[index1];
    arr[index1] = arr[index2];
    arr[index2] = temp1;
}

bool quickSort(int *arr, int begin, int end){ //quicksort
    int gap = end - begin; //the gap betwen the beginning and end
    int midPoint = gap/2 + begin; //the mid point of th

    if(gap > 3){ //if the gap is more than 3, proceed, otherwise do not, as with a gap of smaller than 3, 4 beginning and end points cannot be extracted
        int i = midPoint-1; //for iterating to the bottom of the array from the midpoint
        while(i > begin-1){ //so long as the lowest value i can be is the beginning, loop
            if(arr[i] > arr[midPoint] && i != midPoint-1){ //so long as the i index is greater than the midpoint index, and i isn't conseuctive to midpoint
                swap(arr, midPoint, i, midPoint-1); //swap the value next to midpoint, the midpoint value, and the value i, so that it's on the other side of the midpoint
                midPoint--; //decrement the midpoint, as it has shifted 1 to the left
            }else if(arr[i] > arr[midPoint]){ //basically catches if i is in fact consecutive to midpoint
                swap(arr, midPoint, i); //swap i and midpoint
                midPoint--; //decrement midpoint, as it has shifted down
            }
            i--; //decrement i towards the beginning of the array
        }

        i = midPoint+1; //iterate up to the end of the array, to find values smaller than the midpoint
        while(i < end){ //so long as it is not the end value (which is past the last index of course, as sizeof returns the size of array in normal terms, not index terms)
            if(arr[i] < arr[midPoint] && i != midPoint+1){ //so long as the i index is smaller than the midpoint index, and i isn't consecutive to midpoint
                swap(arr, midPoint, i, midPoint+1); //swap the 3 values
                midPoint++; //increment the midpoint index as it has shifted to the right
            }else if(arr[i] < arr[midPoint]){ //catches if i is consecutive to index
                swap(arr, midPoint, i); //swaps the midpoint and i index, shifting midpoint to the right
                midPoint++; //increments midpoint
            }
            i++; //increments i towards the end of the array
        }
    }

    int begin_1 = begin; //sets the first index of the first half of the array to the beginning
    int end_1 = midPoint; //sets the last index of the first half of the array to the midpoint

    int begin_2 = midPoint+1; //sets the second half's beginning to 1 greater than the end of the first half's, so midpoint+1
    int end_2 = end; //sets the end of the final value to end, which is not looped on in the swapping, so it doesn't matter that the index is larger than the array

    if(end_1 - begin_1 > 3){ //if the gap between the begin and end of the arrays is more than 3, proceed to more sorting, otherwise don't as it wouldn't have much of a useful effect, as there would only be 4 values or so to shift
        quickSort(arr, begin_1, end_1); //recursively sort using the values above
    }

    if(end_2 - begin_2 > 3){ //if the gap between the begin and end of the arrays is more than 3, proceed to more sorting, otherwise don't as it wouldn't have much of a useful effect, as there would only be 4 values or so to shift
        quickSort(arr, begin_2, end_2); //recursively sort using the values above
    }
}