#include <atomic>
template <typename T> class queue {
private:
  uint32_t head;
  std::atomic<uint32_t> tail;
  const uint32_t size;
  std::vector<T *> entries;

public:
  queue(uint32_t capacity) : size(capacity), entries(capacity, nullptr) {}

  void put(T *entry) {
    auto tailLocal = tail.load(std::memory_order_relaxed);
    auto i = tailLocal % size;
    entries[i] = entry;
    tail.fetch_add(1, std::memory_order_release);
  }

  entry *poll() {
    auto tailLocal = tail.load(std::memory_order_acquire);
    if (head < tailLocal) {
      auto entry = entries[head % size];
      ++head;
      return entry;
    }
    return nullptr;
  }
};