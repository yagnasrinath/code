#include <atomic>
#include <memory>
template <typename T> struct LockfreeQueue {
  struct Node {
    std::shared_ptr<T> data;
    std::atomic<Node *> next;
    template <typename... Args>
    Node(Args &&...args)
        : data(std::make_shared<T>(std::forward(args)...)), next(nullptr) {}
  };

  template <typename... Args> void push(Args &&...args) {
    std::atomic<Node *> newTail = new Node(std::forward(args)...);
    auto oldTail = tail.load();
    while (!oldTail->next.compare_exchange_weak(nullptr, newTail)) {
      oldTail = tail.load();
    }
    tail.compare_exchange_weak(oldTail, newTail);
  }

  std::shared_ptr<T> pop() {
    auto oldHead = head.load();
    while (oldHead && !head->compare_exchange_weak(oldHead, oldHead->next)) {
      oldHead = head.load();
    }
    return oldHead ? oldHead->data : std::shared_ptr<T>();
  }

private:
  std::atomic<Node *> head;
  std::atomic<Node *> tail;
};

int main() {}