#include "manager.hpp"

#include <twist/ed/static/var.hpp>

namespace hazard {

Manager& Manager::Get() {
  TWISTED_STATIC(Manager, instance);
  return *instance;
}

Mutator Manager::MakeMutator() {
  std::abort();  // Not implemented
}

Manager::~Manager() {
  // Not implemented
}

}  // namespace hazard
