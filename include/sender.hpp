#pragma once
#include <queue.hpp>
namespace messaging {
class Sender {
 public:
  explicit Sender(Queue* q) : q_(q) {}

  template <typename T>
  void send(T&& msg) {
    if (!q_) {
      throw std::logic_error("Sender with no queue cannot send.");
    }
    q_->push(std::forward<T>(msg));
  }

 private:
  Queue* q_;
};
}  // namespace messaging
