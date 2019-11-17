#pragma once

#include <dispatcher.hpp>
#include <queue.hpp>
#include <sender.hpp>

namespace messaging {

class Receiver {
 public:
  // Allow implicit conversion to a sender that references the queue
  operator Sender() { return Sender(&q_); }

  Dispatcher wait() { return Dispatcher(&q_); }

 private:
  Queue q_;
};
}  // namespace messaging
