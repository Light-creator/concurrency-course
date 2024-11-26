#include <exe/future/contract.hpp>
#include <exe/future/get.hpp>

#include <fmt/core.h>

#include <string>
#include <thread>

using namespace exe;  // NOLINT

int main() {
  // Contract
  auto [f, p] = future::Contract<std::string>();

  // Producer
  std::thread producer([p = std::move(p)] mutable {
    std::move(p).Set("Hello");
  });

  // Consumer
  auto message = future::Get(std::move(f));
  fmt::println("message = {}", message);

  producer.join();

  return 0;
}
