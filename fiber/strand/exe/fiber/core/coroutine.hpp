#pragma once

#include "stack.hpp"

#include <sure/context.hpp>

#include <function2/function2.hpp>

namespace exe::fiber {

class Coroutine {
 public:
  class SuspendContext {
    friend class Coroutine;

   public:
    void Suspend() {
      coro_->Suspend();
    }

   private:
    explicit SuspendContext(Coroutine* coro)
        : coro_(coro) {
    }

   private:
    Coroutine* coro_;
  };

  using Body = fu2::unique_function<void(SuspendContext)>;

 public:
  explicit Coroutine(Body);

  void Resume();

  bool IsCompleted() const;

 private:
  void Suspend();

 private:
  //
};

}  // namespace exe::fiber
