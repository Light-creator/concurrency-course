#pragma once

#include "mutator.hpp"

namespace hazard {

class Manager {
  friend class Mutator;

 public:
  static Manager& Get();

  Mutator MakeMutator();

  ~Manager();

 private:
  // ???
};

}  // namespace hazard
