#include <atomic>
template <typename T> class SharedPtr {

public:
  SharedPtr() : data(nullptr), refCount(nullptr) {}
  SharedPtr(T &&t)
      : data(new T(std::forward(t))), refCount(new std::atomic<uint32_t>(1)) {}

  SharedPtr(const SharedPtr &other)
      : data(other.data), refCount([](auto &refCount) {
          if (refCount) {
            refCount->fetch_add(1);
          }
          return refCount;
        }(other.refCount)) {}

  SharedPtr(SharedPtr &&other) {
    data = std::move(other.data);
    other.data = nullptr;
    refCount = std::move(other.data);
    other.refCount = nullptr;
  }

  SharedPtr &operator=(const SharedPtr &other) {
    if (this != &other) {
      if (refCount) {
        refCount->fetch_sub(1);
        if (refCount == 0) {
          delete data;
          delete refCount;
        }
      }
      if (other.refCount) {
        data = other.data;
        refCount = other.refCount;
        refCount->fetch_add(1);
      }
    }
  }
  SharedPtr &operator=(SharedPtr &&other) {
    if (this != *other) {
      std::swap(data, other.data);
      std::swap(refCount, other.refCount);
    }
  }
  ~SharedPtr() {
    if (refCount) {
      refCount->fetch_sub(1);
      if (refCount == 0) {
        delete data;
        delete refCount;
      }
    }
  }

  T *operator->() const { return data; }

  T &operator*() const { return *data; }

private:
  T *data;
  std::atomic<uint32_t> *refCount;
};

int main() { SharedPtr<int> s; }