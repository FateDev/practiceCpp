#include <iostream>
#include <string>

void binSort(int *arr, unsigned int &begin, unsigned int &end, unsigned int search);

int main()
{
  int list[16] = {1,2,3,4,5,6,7,8,9,10,11,12,13,14,15,16};
  
  unsigned int begin = 0; //index of the first element of the list
  unsigned int end = sizeof(list)/sizeof(int)-1; //size of the list, to be used as index
  unsigned int search = 9; //the value to search for
  
  binSort(list, begin, end, search); //the binary search function
}

void binSort(int *arr, unsigned int &begin, unsigned int &end, unsigned int search){
    int gap = end-begin;
    int newEnd = end - (gap/2);
    int newBegin = begin + (gap/2);

    if((newEnd == end && newBegin == begin) || arr[end] < search || arr[begin] > search){
      std::cout << "Not found." << std::endl;
    }else{
      if(arr[end] == search || arr[begin] == search || arr[newBegin] == search || arr[newEnd] == search){
          if(arr[end] == search){
            std::cout << "Found, at index: " << end << ", or position: " << end+1 << std::endl;
          }else if(arr[begin] == search){
            std::cout << "Found, at index: " << begin << ", or position: " << begin+1  << std::endl;
          }else if(arr[newBegin] == search){
            std::cout << "Found, at index: " << newBegin << ", or position: " << newBegin+1  << std::endl;
          }else if(arr[newEnd] == search){
            std::cout << "Found, at index: " << newEnd << ", or position: " << newEnd+1  << std::endl;
          }
      }else if(arr[newEnd] > search && arr[newBegin] < search){
        end = newEnd;
        begin = newBegin;
        binSort(arr, begin, end, search);
      }else if(arr[newEnd] > search && arr[begin] < search){
        end = newEnd;
        binSort(arr, begin, end, search);
      }else if(arr[end] > search && arr[newBegin] < search){
        begin = newBegin;
        binSort(arr, begin, end, search);
      }
    }
}
