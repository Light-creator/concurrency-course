#pragma once

#include <cstddef>
#include <cstdlib>  // std::abort
#include <memory>

namespace exe::fiber {

namespace detail {

// State shared between producers & consumers

template <typename T>
class BufferedChannelState {
 public:
  explicit BufferedChannelState(size_t capacity)
      : capacity_(capacity) {
    // Not implemented
  }

  ~BufferedChannelState() {
    // Not implemented
  }

  void Send(T /*value*/) {
    std::abort();  // Not implemented
  }

  T Recv() {
    std::abort();  // Not implemented
  }

 private:
  const size_t capacity_;
  // ???
};

}  // namespace detail

// Buffered MPMC Channel
// https://tour.golang.org/concurrency/3

template <typename T>
class BufferedChannel {
  using State = detail::BufferedChannelState<T>;

 public:
  // Bounded channel, `capacity` > 0
  explicit BufferedChannel(size_t capacity)
      : state_(std::make_shared<State>(capacity)) {
  }

  void Send(T value) {
    state_->Send(std::move(value));
  }

  T Recv() {
    return state_->Recv();
  }

 private:
  std::shared_ptr<State> state_;
};

}  // namespace exe::fiber
