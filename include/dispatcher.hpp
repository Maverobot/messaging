#pragma once

#include <queue.hpp>
#include <template_dispatcher.hpp>

namespace messaging {
class CloseQueue {};

class Dispatcher {
  Queue* q_;
  bool chained_;

  // Delete copy constructor and operator
  Dispatcher(Dispatcher const&) = delete;
  Dispatcher& operator=(Dispatcher const&) = delete;

  // Allow TemplateDispatcher instances to access the internals
  template <typename Dispatcher, typename Msg, typename Func>
  friend class TemplateDispatcher;

  // Infinite loop to dispatch messages
  void wait_and_dispatch() {
    for (;;) {
      auto msg = q_->waitAndPop();
      dispatch(msg);
    }
  }

  // Checks for a CloseQueue message and throws
  bool dispatch(std::shared_ptr<MessageBase> const& msg) {
    if (dynamic_cast<WrappedMessage<CloseQueue>*>(msg.get())) {
      throw CloseQueue();
    }
    return false;
  }

 public:
  Dispatcher(Dispatcher&& other) : q_(other.q_), chained_(other.chained_) {
    // why chained needs to be set to true
    other.chained_ = true;
  }

  explicit Dispatcher(Queue* q) : q_(q), chained_(false) {}

  // It might throw
  ~Dispatcher() noexcept(false) {
    if (!chained_) {
      wait_and_dispatch();
    }
  }

  template <typename Msg, typename Func>
  auto handle(Func&& f) {
    return TemplateDispatcher<messaging::Dispatcher, Msg, Func>(q_, this, std::forward<Func>(f));
  }
};
}  // namespace messaging
