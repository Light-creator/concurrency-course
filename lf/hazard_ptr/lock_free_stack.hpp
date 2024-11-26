#pragma once

#include "memory/hazard/manager.hpp"

#include <twist/ed/std/atomic.hpp>

#include <twist/assist/memory.hpp>
#include <twist/trace/scope.hpp>

#include <optional>

// Treiber unbounded MPMC lock-free stack

template <typename T>
class LockFreeStack {
  struct Node {
    T value;
    Node* next{nullptr};
  };

 public:
  LockFreeStack() = default;

  void Push(T value) {
    twist::trace::Scope push{stack_};

    Node* new_node = twist::assist::New<Node>(std::move(value));
    new_node->next = top_.load();

    while (!top_.compare_exchange_weak(new_node->next, new_node)) {
      ;
    }
  }

  std::optional<T> TryPop() {
    twist::trace::Scope try_pop{stack_};

    auto mutator = hazard::Manager::Get().MakeMutator();

    auto top_guard = mutator.GetHazardPtr(0);

    while (true) {
      Node* curr_top = top_guard.Protect(top_);

      if (curr_top == nullptr) {
        return std::nullopt;
      }

      if (top_.compare_exchange_weak(curr_top,
                                     twist::assist::Ptr(curr_top)->next)) {
        T value = std::move(curr_top->value);
        top_guard.Reset();
        mutator.Retire(curr_top);
        return value;
      }
    }
  }

  ~LockFreeStack() {
    Node* curr = top_.load();
    while (curr != nullptr) {
      Node* next = curr->next;
      delete curr;
      curr = next;
    }
  }

 private:
  twist::ed::std::atomic<Node*> top_{nullptr};
  twist::trace::Domain stack_{"Stack"};
};
