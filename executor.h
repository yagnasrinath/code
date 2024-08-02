#pragma once
#include <atomic>
#include <chrono>
#include <condition_variable>
#include <functional>
#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <thread>

struct TaskHandle final {
  TaskHandle(uint64_t id) : id(id) {}
  bool operator==(const TaskHandle &other) const { return id == other.id; }

private:
  const uint64_t id;
  friend struct std::hash<TaskHandle>;
};

template <> struct std::hash<TaskHandle> {
  std::size_t operator()(const TaskHandle &taskHandle) const {
    return std::hash<uint64_t>{}(taskHandle.id);
  }
};
template <typename ClockType = std::chrono::system_clock,
          typename DurationType = std::chrono::milliseconds>
class EventLoop {
  using TimePointType = std::chrono::time_point<ClockType, DurationType>;
  class Task final {
  private:
    bool cancelled;
    std::function<void()> fn;
    std::optional<DurationType> interval;

  public:
    Task(std::function<void()> &&fn, std::optional<DurationType> interval)
        : cancelled(false), fn(fn), interval(interval) {}
    Task(Task &&other) {
      cancelled = other.cancelled;
      fn = std::move(other.fn);
      interval = other.interval;
      other.cancelled = true;
    }
    ~Task() {
      if (!cancelled) {
        cancelled = true;
      }
    }
    void run() const {
      if (!cancelled) {
        fn();
      }
    }
    void cancel() { cancelled = true; }
    bool isCancelled() const { return cancelled; }
    std::optional<DurationType> getInterval() const { return interval; }
  };

public:
  EventLoop(uint8_t threadCount) : isRunning(true), pool() {
    for (auto i = 0; i < threadCount; ++i) {
      pool.push_back(std::thread(std::bind(&EventLoop::run, this)));
    }
  }
  TaskHandle schedule(std::function<void()> fn,
                      std::optional<DurationType> delay,
                      std::optional<DurationType> interval) {
    static std::atomic<uint64_t> id = 0;
    TaskHandle taskHandle{id.fetch_add(1)};
    Task task(std::move(fn), interval);
    auto lock = std::lock_guard(tasksGuard);
    taskHandleToTasks.emplace(taskHandle, std::move(task));
    const TimePointType delayTime =
        std::chrono::time_point_cast<DurationType>(ClockType::now()) +
        delay.value_or(DurationType{0});
    timeTotaskHandles.emplace(delayTime, taskHandle);
    tasksCond.notify_one();
    return taskHandle;
  }
  void cancelBlocking(TaskHandle task_id) {
    auto lock = std::unique_lock(tasksGuard);
    tasksCond.wait(lock, [&task_id, this] {
      return taskHandleToTasks.find(task_id) != taskHandleToTasks.end();
    });
    taskHandleToTasks.at(task_id).cancel();
  }

  ~EventLoop() {
    {
      auto guard = std::lock_guard(tasksGuard);
      isRunning = false;
      for (auto &[_, task] : taskHandleToTasks) {
        task.cancel();
      }
    }
    tasksCond.notify_all();
    for (auto &loop : pool) {
      loop.join();
    }
  }
  void run() {
    auto lock = std::unique_lock(tasksGuard, std::defer_lock);
    while (true) {
      lock.lock();
      auto waitTime = DurationType{60'000};
      if (!timeTotaskHandles.empty()) {
        const TimePointType now =
            std::chrono::time_point_cast<DurationType>(ClockType::now());
        if (timeTotaskHandles.begin()->first < now) {
          waitTime = DurationType{0};
        } else {
          waitTime = std::chrono::duration_cast<DurationType>(
              timeTotaskHandles.begin()->first - now);
        }
      }
      tasksCond.wait_for(lock, waitTime, [&] { return !isRunning; });

      if (!isRunning) {
        lock.unlock();
        return;
      } else if (timeTotaskHandles.empty()) {
        lock.unlock();
        continue;
      } else {
        const auto timeToTaskIt = timeTotaskHandles.begin();
        const auto taskHandle = timeToTaskIt->second;
        timeTotaskHandles.erase(timeToTaskIt);
        const auto taskHandleToTaskIt = taskHandleToTasks.find(taskHandle);
        auto task = std::move(taskHandleToTaskIt->second);
        taskHandleToTasks.erase(taskHandleToTaskIt);
        lock.unlock();
        try {
          task.run();
        } catch (...) {
          std::cerr << "Task is not supposed to throw" << std::endl;
        }
        if (const auto interval = task.getInterval(); interval) {
          lock.lock();
          const TimePointType next =
              std::chrono::time_point_cast<DurationType>(ClockType::now()) +
              *interval;
          timeTotaskHandles.emplace(next, taskHandle);
          taskHandleToTasks.emplace(taskHandle, std::move(task));
          lock.unlock();
          tasksCond.notify_one();
        }
      }
    }
  }

private:
  std::mutex tasksGuard;
  std::condition_variable tasksCond;
  std::map<TimePointType, TaskHandle> timeTotaskHandles;
  std::unordered_map<TaskHandle, Task> taskHandleToTasks;
  bool isRunning;
  std::vector<std::thread> pool;
};