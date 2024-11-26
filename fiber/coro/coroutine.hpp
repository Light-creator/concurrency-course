#pragma once

#include <sure/context.hpp>
#include <sure/stack.hpp>

#include <function2/function2.hpp>

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
