#include <iostream>

int main() {

	int inputVals[50] = { 0 };

	std::cout << "Input 50 values, line by line: " << std::endl;

	int highest = 0;
	int lowest = 0;
	int average = 0;

	for (int i = 0; i < 50; i++) {
		std::cout << "Value " << i + 1 << ": ";
		std::cin >> inputVals[i];

		if (inputVals[i] > highest) {
			highest = inputVals[i];
		}

		if (inputVals[i] < lowest || i == 0) {
			lowest = inputVals[i];
		}

		average += inputVals[i];
	}

	std::cout << std::endl;
	
	std::cout << "Lowest: " << lowest << std::endl;
	std::cout << "Average: " << average / 50 << std::endl;
	std::cout << "Highest: " << highest << std::endl;

	std::cout << std::endl;

	return 0;
}
