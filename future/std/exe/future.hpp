#pragma once

#include "error.hpp"

#include <memory>
#include <cassert>
#include <exception>
#include <stdexcept>

template <typename T>
class Future {
  template <typename U>
  friend class Promise;

 public:
  // Non-copyable
  Future(const Future&) = delete;
  Future& operator=(const Future&) = delete;

  // Movable
  Future(Future&&) = default;
  Future& operator=(Future&&) = default;

  // One-shot
  T Get() {
    throw std::runtime_error("Not implemented");
  }

  bool Valid() const {
    return false;  // Not implemented
  }

 private:
  Future(/*???*/) {
  }

 private:
  // ???
};
