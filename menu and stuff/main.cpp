#include <iostream>
#include <string>

void menuCall(int option);
void beerBottles();
double calculator(char calcOperator, double x, double y);
void goAgain();

int main() {

	menuCall(0);
	
	return 0;
}

void menuCall(int option) {
	if (option == NULL) {
		option = 0;
	}

	if (option < 1) {
		system("clear");
		std::cout << "|**********************************||" << std::endl;
		std::cout << "||---------------------------------||" << std::endl;
		std::cout << "||---------------MENU--------------||" << std::endl;
		std::cout << "||---------------------------------||" << std::endl;
		std::cout << "||---------------------------------||" << std::endl;
		std::cout << "|| Option 1:    Calculator         ||" << std::endl;
		std::cout << "||---------------------------------||" << std::endl;
		std::cout << "|| Option 2:    99 Cans of Beer    ||" << std::endl;
		std::cout << "||---------------------------------||" << std::endl;
		std::cout << "|| Option 3:    Exit               ||" << std::endl;
		std::cout << "||_________________________________||" << std::endl;
		
		std::cout << std::endl << "Your choice (number): ";
		std::cin >> option;

		menuCall(option);

	}else if(option == 1){
		system("clear");

		double num1, num2;
		char numOperator;
		std::string response;

		std::cout << "|**********************************||" << std::endl;
		std::cout << "||------------CALCULATOR-----------||" << std::endl;
		std::cout << "|| Operators:    *, /, +, -        ||" << std::endl;
		std::cout << "||_________________________________||" << std::endl << std::endl;

		std::cout << "Enter the operator to be used: ";
		std::cin >> numOperator;

		std::cout << std::endl << "Enter the first number: ";
		std::cin >> num1;

		std::cout << std::endl << "Enter the second number: ";
		std::cin >> num2;

		std::cout << std::endl << "Output: " << calculator(numOperator, num1, num2) << std::endl << std::endl;

		goAgain();
	}
	else if (option == 2) {
		system("clear");
		beerBottles();

		goAgain();
	}
	else if (option == 3) {
		system("clear");
		exit;
	}
	else {
		std::cout << "The input was incorrect, please try again." << std::endl << std::endl;
		menuCall(0);
	}
}

void beerBottles() {
	system("clear");

	for (int i = 99; i >= 0; i--) {
		if (i == 1) {
			std::cout << "1 bottle of beer on the wall, 1 bottle of beer." << std::endl;
			std::cout << "Take one down and pass it around, no more bottles of beer on the wall." << std::endl << std::endl;
		}
		else if (i == 0) {
			std::cout << "No more bottles of beer on the wall, no more bottles of beer. " << std::endl;
			std::cout << "Go to the store and buy some more, 99 bottles of beer on the wall." << std::endl << std::endl;
		}
		else {
			std::cout << i << " bottles of beer on the wall, " << i << " bottles of beer." << std::endl;
			std::cout << "Take one down and pass it around, " << i - 1 << " bottles of beer on the wall." << std::endl << std::endl;
		}
	}
}

double calculator(char calcOperator, double x, double y) {
	switch (calcOperator) {
		case '+':
			return x + y;
		case '/':
			return x / y;
		case '*':
			return x * y;
		case '-':
			return x - y;
		default:
			return 0.0;
	}
}

void goAgain() {
	int response;

	std::cout << "Would you like to go back to the menu (1) or exit (0)? ";
	std::cin >> response;

	if (response == 1) {
		std::cout << std::endl;
		menuCall(0);
	}
	else {
		menuCall(3);
	}
}
