#pragma once

class UniqueFunction {
 private:
  // For type erasure
  struct IRunnable {
    virtual ~IRunnable() = default;

    virtual void Run() = 0;
  };

 public:
  template <typename F>
  explicit UniqueFunction(F&&) {
    // Not implemented
  }

  // Movable

  UniqueFunction(UniqueFunction&&) {
    // Not implemented
  }

  UniqueFunction& operator=(UniqueFunction&&) {
    return *this;  // Not implemented
  }

  // Non-copyable
  UniqueFunction(const UniqueFunction&&) = delete;
  UniqueFunction& operator=(const UniqueFunction&&) = delete;

  void operator()() {
    // Not implemented
  }

 private:
  //
};
