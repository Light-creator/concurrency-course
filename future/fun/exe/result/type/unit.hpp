#pragma once

#include <variant>

namespace exe {

using Unit = std::monostate;

static constexpr Unit unit = Unit{};  // NOLINT

}  // namespace exe
