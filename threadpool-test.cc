#include "threadpool.h"
#include <iostream>

int main() {
  auto t = Threadpool(1);
  auto f = t.schedule(
      [](int i) -> int {
        std::cout << "hello" << std::endl;
        return i;
      },
      1);
  f.get();
}