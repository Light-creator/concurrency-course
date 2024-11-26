#pragma once

#include <exe/future/syntax/pipe.hpp>
#include <exe/future/thunk/stub.hpp>
#include <exe/future/trait/value_of.hpp>

namespace exe::future {

namespace pipe {

struct [[nodiscard]] Start {
  Start() = default;

  // Non-copyable
  Start(const Start&) = delete;

  template <SomeFuture InputFuture>
  Future<trait::ValueOf<InputFuture>> auto Pipe(InputFuture) {
    return thunk::Stub<trait::ValueOf<InputFuture>>{};
  }
};

}  // namespace pipe

/*
 * Turn lazy future into eager, force thunk evaluation
 *
 */

inline auto Start() {
  return pipe::Start{};
}

}  // namespace exe::future
