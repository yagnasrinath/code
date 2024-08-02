#pragma once
#include <functional>
#include <future>
#include <queue>
#include <thread>
#include <vector>

struct Threadpool {
private:
  std::mutex guard;
  std::condition_variable guardNotifier;
  bool isShutdown;
  std::queue<std::function<void()>> tasks;

  std::vector<std::thread> threads;
  void run() {
    auto lock = std::unique_lock(guard, std::defer_lock);
    while (true) {
      lock.lock();
      guardNotifier.wait(lock,
                         [this]() { return !tasks.empty() || isShutdown; });
      if (isShutdown) {
        return;
      }
      auto task = std::move(tasks.front());
      tasks.pop();
      lock.unlock();
      task();
    }
  }

public:
  Threadpool(const uint8_t count)
      : isShutdown(false), threads([](const auto count, auto Self) {
          std::vector<std::thread> threads;
          for (auto i = 0; i < count; ++i) {
            threads.emplace_back(std::thread(&Threadpool::run, Self));
          }
          return std::move(threads);
        }(count, this)) {}
  ~Threadpool() {
    {
      auto lock = std::lock_guard(guard);
      isShutdown = true;
    }
    guardNotifier.notify_all();
    for (auto &t : threads) {
      t.join();
    }
  }
  template <typename F, typename... Args>
  decltype(auto) schedule(F &&f, Args &&...args) {
    using Return = std::invoke_result_t<std::decay_t<F>, std::decay_t<Args>...>;
    auto fn = std::make_shared<std::packaged_task<Return()>>(
        std::bind(std::forward<F>(f), std::forward<Args>(args)...));
    auto ret = fn->get_future();
    {
      auto lock = std::lock_guard(guard);
      tasks.push([fn = std::move(fn)]() mutable { (*fn)(); });
    }
    guardNotifier.notify_one();
    return ret;
  }
};