#include <lib.hpp>
#include <iostream>

int main(int argc, char** argv)
{
    std::cout << "factorial(10): " << fact(10) << std::endl;
    say_hello_in_thread();
    return 0;
}
