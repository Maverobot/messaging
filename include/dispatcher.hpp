#pragma once

#include <queue.hpp>
#include <template_dispatcher.hpp>

namespace messaging {
class CloseQueue {};

// This is the root/parent dispatcher
class Dispatcher {
 public:
  Dispatcher(Dispatcher&& other) : q_(other.q_), chained_(other.chained_) {
    // why chained needs to be set to true
    other.chained_ = true;
  }

  explicit Dispatcher(Queue* q) : q_(q), chained_(false) {}

  // It might throw
  ~Dispatcher() noexcept(false) {
    if (!chained_) {
      // If this dispatcher has no chained dispatchers, it means it does not have any handling
      // functions. What it can do in the destructor is to wait for CloseQueue event to come.
      wait_and_dispatch();
    }
  }

  template <typename Msg, typename Func>
  auto handle(Func&& f) {
    return TemplateDispatcher<messaging::Dispatcher, Msg, Func>(q_, this, std::forward<Func>(f));
  }

 private:
  // Delete copy constructor and operator
  Dispatcher(Dispatcher const&) = delete;
  Dispatcher& operator=(Dispatcher const&) = delete;

  // Allow TemplateDispatcher instances to access the internals
  template <typename Dispatcher, typename Msg, typename Func>
  friend class TemplateDispatcher;

  // Infinite loop until CloseQueue event occurs
  void wait_and_dispatch() {
    for (;;) {
      std::cout << "infinite\n";
      auto msg = q_->waitAndPop();
      dispatch(msg);
    }
  }

  // Checks for a CloseQueue message and throws
  bool dispatch(std::shared_ptr<MessageBase> const& msg) {
    if (dynamic_cast<WrappedMessage<CloseQueue>*>(msg.get())) {
      throw CloseQueue();
    }
    std::cout << "No chained message handlers match this message\n";
    return false;
  }

  Queue* q_;
  bool chained_;
};
}  // namespace messaging
