#pragma once

#include <queue.hpp>

namespace messaging {
template <typename PreviousDispatcher, typename Msg, typename Func>
class TemplateDispatcher {
  Queue* q_;
  PreviousDispatcher* prev_;
  Func f_;
  bool chained_;

  TemplateDispatcher(TemplateDispatcher const&) = delete;
  TemplateDispatcher& operator=(TemplateDispatcher const&) = delete;

  template <typename, typename, typename>
  friend class TemplateDispatcher;

  void waitAndDispatch() {
    for (;;) {
      auto msg = q_->waitAndPop();
      if (dispatch(msg)) {
        break;
      }
    }
  }

  bool dispatch(std::shared_ptr<MessageBase> const& msg) {
    if (WrappedMessage<Msg>* wrapper = dynamic_cast<WrappedMessage<Msg>*>(msg.get())) {
      f_(wrapper->content);
      return true;
    } else {
      return prev_->dispatch(msg);
    }
  }

 public:
  TemplateDispatcher(TemplateDispatcher&& other)
      : q_(other.q_), prev_(other.prev_), f_(std::move(other.f_)), chained_(other.chained_) {
    other.chained_ = true;
  }

  TemplateDispatcher(Queue* q, PreviousDispatcher* prev, Func&& f)
      : q_(q), prev_(prev), f_(std::forward<Func>(f)), chained_(false) {
    prev_->chained_ = true;
  }

  template <typename OtherMsg, typename OtherFunc>
  auto handle(OtherFunc&& f) {
    return TemplateDispatcher<TemplateDispatcher, OtherMsg, OtherFunc>(q_, this,
                                                                       std::forward<OtherFunc>(f));
  }

  ~TemplateDispatcher() noexcept(false) {
    if (!chained_) {
      waitAndDispatch();
    }
  }
};
}  // namespace messaging
