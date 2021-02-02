#include <thread>
#include <iostream>

#include "lib.hpp"

int fact(int num){
  if(num == 1) return 1;
  return num * fact(num - 1);
}

int say_hello_in_thread(){
  std::thread t1([] {
    std::cout << "hello from thread 1\n";
  });
  std::thread t2([] {
    std::cout << "hello from thread 2\n";
  });

  t1.join();
  t2.join();

  return 0;
}