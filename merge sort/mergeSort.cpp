#include <iostream>

void merge(int &arrLen, int *arrIndexes, int *sortArray, int originalLength); //merge sort prototype

int main(){
	int sortArray[10] = { 10,9,8,7,6,5,4,3,2,1 }; //the array to sort
	int arrLen = sizeof(sortArray)/sizeof(sortArray[0]); //the length of the array
	int originalLength = arrLen; //the original length
	int arrIndexes[arrLen]; //the indexes of the sortArray
	
	//basic output stuff
	std::cout << "Input array: " << std::endl;
	for(int i = 0;i < originalLength;i++){
		std::cout << sortArray[i] << " ";
	}
	std::cout << std::endl;
	std::cout << std::endl;

	//filling the indexes array with 1's
	for(int i = 0;i < arrLen;i++){
		arrIndexes[i] = 1;
	}

	//merge sort
	merge(arrLen, arrIndexes, sortArray, originalLength);
}

void merge(int &arrLen, int *arrIndexes, int *sortArray, int originalLength){
	int temp[(arrLen-arrLen%2)/2+(arrLen%2)]; //make a indexes array that is half the size of arrLen (rounded up if non integer)
	int temp1[originalLength]; //an array of size of sort array is made
	int j = 0; //for looping through the indexes array

	int outerPoint = 0; //this is the first index on each iteration of the sort
	int midPoint = 0; //the middle of the array

	int lenFirstHalf = 0; //the length of the first half of the array to be sorted
	int lenSecondHalf = 0; //the length of the second half

	//the data is found using the indexes arrat being merged

	int index1 = 0; //the index on the first half
	int index2 = 0; //the index on the second half

	for(int i = 0;i < arrLen;i+=2){
		if(i+1 < arrLen){ //if there is a consecutive number, the merging stuff can happen
			midPoint = arrIndexes[i];
			lenFirstHalf = arrIndexes[i];
			lenSecondHalf = arrIndexes[i+1];
			arrIndexes[i] += arrIndexes[i+1];

			midPoint += outerPoint;
			index2 = outerPoint + lenFirstHalf;
		}else{ //otherwise it cannot
			midPoint = 0;
			lenFirstHalf = 0;
			lenSecondHalf = 0;
			index1 = 0;
			index2 = 0;
		}
		
		index1 = outerPoint; //in all cases the index1 is the outerpoint
		
		temp[j] = arrIndexes[i]; //this holds the indexes once they have been merged (so an index of 1 and 1, are merged to 2, 7 and 8 to 15 etc)

		for(int k = 0;k < temp[j];k++){
			if(midPoint == 0){ //if the midpoint is zero merging doesn't take place, so just dump the variables as they are
				temp1[k+outerPoint] = sortArray[index1];
				index1++;
			}else{
				if(sortArray[index1] <= sortArray[index2] && index1 < midPoint){ //if the index1 number is <= the index2 number, and index1 is less than the midpoint
					temp1[k+outerPoint] = sortArray[index1]; //put the index1 number inside the temp1[] index
					index1++; //and increment it, so as to move on from it, as it's been dealt with
				}else if(sortArray[index1] > sortArray[index2] && index2 < midPoint+lenSecondHalf){ //if the index1 number is > the index 2 number and the index2 number isn't greater than the maximum index
					temp1[k+outerPoint] = sortArray[index2]; //put the index 2 number into the temp1[] index
					index2++; //and increment it as we're done with it
				}else{ //if neither of those are true
					if(index1 < midPoint){ //so long as the index1 number isn't past the midpoint
						temp1[k+outerPoint] = sortArray[index1]; //put the number corresponding to the index into the temp1[] index
						index1++; //and increment, as we're done with it
					}else if(index2 < midPoint+lenSecondHalf){ //so long as the index2 number isn't past the maximum index
						temp1[k+outerPoint] = sortArray[index2]; //put the corresponding number into the temp1[] index
						index2++; //and increment, as we're done with it
					}
				}
			}
			std::cout << temp1[k+outerPoint] << " "; //output each number added to the array, for visualisation
		}
		std::cout << std::endl; //end line after outputting for that loop, again, for visualisation

		outerPoint += temp[j]; //increment the outerPoint index to the beginning of the next part, by incrementing by the merged index array
		j++; //and increment j, for the merging of the index array on the next loop to occur
	}
	
	std::cout << std::endl; //again end line for visualisation purposes

	arrLen = sizeof(temp)/sizeof(temp[0]); //get the actual size of the temporary variable, and make arrLen equal to it
	arrIndexes = temp; //make the input indexes array equal to the output indexes array

	sortArray = temp1; //make the input sortArray equal to the output sortArray

	if(arrLen != 1){ //if everything hasn't been merged into one array...
		merge(arrLen, arrIndexes, sortArray, originalLength); //...run merge(...) on it again, until the arrLen is equal to 1
	}else{ //otherwise output the output array
		std::cout << "Output array: " << std::endl;
		for(int i = 0;i < originalLength;i++){
			std::cout << sortArray[i] << " ";
		}
		std::cout << std::endl;
	}
}