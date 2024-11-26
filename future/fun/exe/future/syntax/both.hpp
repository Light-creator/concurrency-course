#pragma once

#include <exe/future/combine/concur/all.hpp>

/*
 * Syntactic sugar for future::Both combinator
 *
 * Usage:
 *
 * auto both = std::move(lhs) + std::move(rhs);
 *
 */

template <typename A, typename B>
auto operator+(exe::future::Future<A> lhs, exe::future::Future<B> rhs) {
  return exe::future::Both(std::move(lhs), std::move(rhs));
}
