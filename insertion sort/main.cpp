#include <iostream>

int main() {

	int intArray[] = { 5,4,3,2,1,10,12,14,44,21,22,32,2 };

	const int arraySize = sizeof(intArray) / sizeof(intArray[0]);

	int carry = 0;

	for (int i = 0; i < arraySize; i++) {
		for (int j = arraySize -1; j > i; j--) {
			if (intArray[i] > intArray[j]) {
				carry = intArray[j];
				intArray[j] = intArray[i];
				intArray[i] = carry;
			}
		}
	}

	std::cout << std::endl;

	for (int i = 0; i < arraySize; i++) {
		std::cout << intArray[i] << std::endl;
	}

	std::cin.get();

	return 0;
}