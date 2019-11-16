#pragma once
#include <queue.hpp>
namespace messaging {
class Sender {
  Queue* q_;

 public:
  explicit Sender(Queue* q) : q_(q) {}

  template <typename T>
  void send(T const& msg) {
    if (q_) {
      q_->push(msg);
    }
  }
};
}  // namespace messaging
