#include <future>
#include <queue>
#include <type_traits>
#include <vector>
/*
ImageProcessorService that has function void processImage(std::vector<float>
image) and that uses function int neuralNetwork(std::vector<float> image) to
process it. This runs a neural network on the CPU. Assume that the
neuralNetwork() function is slow. processImage() must be fast and never block
the caller for a long time. It can be called at random intervals. You don’t have
to compile and run your solution, but we expect you to write C++ code that would
compile with minor changes. You are allowed to google the stdlib API.

*/

struct ThreadPool {
public:
  ThreadPool(int threadCount)
      : shutdown(false), threads([](auto count) {
          auto threads = std::vector<std::thread>();
          for (int i = 0; i < count; ++i) {
            threads.push_back(std::thread(&ThreadPool::run, this));
          }
          return threads;
        }(threadCount)) {}

  void run() {
    while (true) {
      auto lock = std::unique_lock(guard);
      guardNotifier.wait(
          lock, [&shutdown, &tasks]() { return shutdown || !tasks.empty(); });
      auto task = tasks.front();
      tasks.pop();
      lock.unlock();
      task();
    }
  }

  template <typename F, typename... Args>
  decltype(auto) schedule(F &&f, Args &&...args) {
    using Return = std::invoke_result_t<F, Args...>;
    std::promise<Return> p;
    auto result = p.get_future();
    auto lock = std::lock_guard(guard);
    tasks.push_back(
        [p = std::move(p), f = std::move(f), args = std::move(args)...]() {
          try {
            auto returnValue = f(args...);
            p.set_value(returnValue);
          } catch (...) {
            p.set_exception(std::current_exception());
          }
        });
    return result;
  }

private:
  std::mutex guard;
  std::condition_variable guardNotifier;
  bool shutdown;
  std::vector<std::thread> threads;
  std::queue<std::function<void()>> tasks;
};

struct ImageProcessorService {

public:
  ImageProcessorService(ThreadPool &pool_) : pool(pool_), shutdown(false), responseProcessor(std::thread([this](
  ){
    while(!shutdown) {
        auto lock = std::lock_guard(responseGuard);
        
    }

  })) {}
  int neuralNetwork(std::vector<float> &&data);
  void processImage(std::vector<float> &&data) {
    responses.push(pool.schedule(&ImageProcessorService::neuralNetwork, data));
  }

private:
  std::mutex responseGuard;
  std::queue<std::future<int>> responses;
  std::unordered_map<int, uint32_t> labelCount;
  ThreadPool &pool;
  bool shutdown;
  std::thread responseProcessor;
};



std::vector<float> producer() { return {}; }
int main() {
  ThreadPool pool(4);
  ImageProcessorService service(pool);

  while (true) {
    service.processImage(producer());
  }
}
