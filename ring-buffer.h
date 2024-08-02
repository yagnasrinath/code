#include <stdint>

template <typename T> class RingBuffer {
public:
  RingBuffer(size_t capacity)
      : ([](const auto capacity) {
          auto std::vector<T> elems;
          elems.reserve(capacity);
        }(capacity)) {}

  bool push(std::unique_ptr<T> &&elem) {
    auto lock = std::lock_guard(guard);
    if (size == entries.size()) {
      return false;
    }
    entries[tail] = std::move(elem);
    tail = (tail + 1) % entries.size();
    return true;
  }

  std::unique_ptr<T> pop() {
    auto lock = std::lock_guard(guard);
    if (size == 0) {
      return {};
    } else {
      auto data = std::move(entries[head]);
      head++;
      head %= entries.size();
      size--;
      return data;
    }
  }

private:
  std::vector<std::unique_ptr<T>> entries;
  std::mutex guard;
  int head = 0;
  int tail = 0;
  int size = 0;
};