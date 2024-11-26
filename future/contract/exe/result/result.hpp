#pragma once

#include "error.hpp"

#include <expected>

namespace exe {

template <typename T>
using Result = std::expected<T, Error>;

}  // namespace exe
