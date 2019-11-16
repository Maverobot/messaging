#pragma once

#include <dispatcher.hpp>
#include <queue.hpp>
#include <sender.hpp>

namespace messaging {

class Receiver {
  Queue q_;

 public:
  // Allow implicit conversion to a sender that references the queue
  operator Sender() { return Sender(&q_); }

  Dispatcher wait() { return Dispatcher(&q_); }
};
}  // namespace messaging
