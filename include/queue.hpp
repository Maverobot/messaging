#pragma once

#include <condition_variable>
#include <mutex>
#include <queue>

namespace messaging {
struct MessageBase {
  virtual ~MessageBase() {}
};

template <typename Msg>
struct WrappedMessage : MessageBase {
  Msg content;
  explicit WrappedMessage(Msg const& msg) : content(msg) {}
};

class Queue {
  std::mutex m_;
  std::condition_variable c_;
  std::queue<std::shared_ptr<MessageBase>> q_;

 public:
  template <typename T>
  void push(T const& msg) {
    std::lock_guard<std::mutex> lk(m_);
    q_.push(std::make_shared<WrappedMessage<T>>(msg));
    c_.notify_all();
  }

  std::shared_ptr<MessageBase> waitAndPop() {
    std::unique_lock<std::mutex> lk(m_);
    c_.wait(lk, [this] { return !q_.empty(); });
    auto res = q_.front();
    q_.pop();
    return res;
  }
};
}  // namespace messaging
