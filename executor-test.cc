#include "executor.h"
#include <iostream>
#include <chrono>

int main() {
  using DurationType = std::chrono::milliseconds;
  using ClockType = std::chrono::system_clock;
  EventLoop<ClockType, DurationType> eventLoop(5);

  const auto task = [] { std::cout << "task1" << std::endl; };

  eventLoop.schedule(task, DurationType(0), DurationType(10'000));

  std::this_thread::sleep_for(DurationType(60'000));
}