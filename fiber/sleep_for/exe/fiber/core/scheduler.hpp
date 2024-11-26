#pragma once

#include <asio/io_context.hpp>

namespace exe::fiber {

using Scheduler = asio::io_context;

}  // namespace exe::fiber
