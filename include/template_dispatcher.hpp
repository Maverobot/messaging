#pragma once

#include <queue.hpp>

namespace messaging {

// The dispatcher comparing to messaging::Dispatcher
template <typename PreviousDispatcher, typename Msg, typename Func>
class TemplateDispatcher {
 public:
  TemplateDispatcher(TemplateDispatcher&& other)
      : q_(other.q_), prev_(other.prev_), f_(std::move(other.f_)), chained_(other.chained_) {
    other.chained_ = true;
  }

  TemplateDispatcher(Queue* q, PreviousDispatcher* prev, Func&& f)
      : q_(q), prev_(prev), f_(std::forward<Func>(f)), chained_(false) {
    prev_->chained_ = true;
  }

  // Dispatching happens in destructor
  ~TemplateDispatcher() noexcept(false) {
    if (!chained_) {
      waitAndDispatch();
    }
  }

  /**
   * Example:
   * messaging::Receiver = rec;
   * rec.wait().handle<MessageTypeA>(func_on_message_type_A)
   *           .handle<MessageTypeB>(func_on_message_type_B)
   *           .handle<MessageTypeC>(func_on_message_type_C);
   *
   * When a message pops up, it will first be tried to match MessageTypeC, then MessageTypeB, then
   * MessageTypeC. If any of them matches, the corresponding function will be called and proceeds.
   * Otherwise, it will keep waiting.
   *
   */
  template <typename NextMsg, typename NextFunc>
  auto handle(NextFunc&& f) {
    return TemplateDispatcher<TemplateDispatcher, NextMsg, NextFunc>(q_, this,
                                                                     std::forward<NextFunc>(f));
  }

 private:
  template <typename, typename, typename>
  friend class TemplateDispatcher;

  TemplateDispatcher(TemplateDispatcher const&) = delete;
  TemplateDispatcher& operator=(TemplateDispatcher const&) = delete;

  void waitAndDispatch() {
    for (;;) {
      auto msg = q_->waitAndPop();
      if (dispatch(msg)) {
        // After dispatching the first WrappedMessage<Msg>
        break;
      }
    }
  }

  bool dispatch(std::shared_ptr<MessageBase> const& msg) {
    if (WrappedMessage<Msg>* wrapper = dynamic_cast<WrappedMessage<Msg>*>(msg.get())) {
      f_(wrapper->content);
      return true;
    }
    // If the current TemplateDispatcher does not deal with this msg type, try with the previous
    // TemplateDispatcher
    return prev_->dispatch(msg);
  }

  Queue* q_;
  PreviousDispatcher* prev_;
  Func f_;
  bool chained_;
};
}  // namespace messaging
