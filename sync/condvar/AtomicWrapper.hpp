#pragma once

#include <twist/ed/std/atomic.hpp>

template <typename T>
class AtomicWrapper {
public:
  AtomicWrapper(T val): val(val) {}
  AtomicWrapper(): val() {}
  
  AtomicWrapper(const AtomicWrapper& other): val(other.val.load()) {}
  
  AtomicWrapper& operator=(const AtomicWrapper& other) { 
    val.store(other.val.load());
    return *this; 
  }

  AtomicWrapper(const twist::ed::std::atomic<T>& other): val(other.load()) {}

public:
  twist::ed::std::atomic<T> val;
};

