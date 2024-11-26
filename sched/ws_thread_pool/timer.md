# Таймеры

Поддержите [планирование таймеров](exe/sched/timer).

Для парковки воркера в пуле используйте [`futex::WaitTimed`](https://gitlab.com/Lipovsky/twist/-/blob/master/docs/ru/twist/ed/wait/futex.md), в качестве таймаута выбирайте
время до выполнения очередного таймера.

## Применения

### Файберы

#### `SleepFor`

```cpp
fiber::Go(pool, [] {
  for (int i = 10; i > 0; --i) {
    fmt::println("{}", i);
    // fiber/sched/sleep_for.hpp
    fiber::SleepFor(1s);
  }
});
```

#### `Ticker`

[Go by Example: Tickers](https://gobyexample.com/tickers)

```cpp
fiber::Go(pool, [] {
  // fiber/sync/channel/ticker.hpp
  fiber::Ticker ticker{1s};
  
  for (int i = 10; i > 0; --i) {
    fmt::println("{}", i);
    ticker.Recv();  // Получаем unit раз в секунду
  }
});
```

### Фьючи

#### `After`

Фьюча, вычисляющая `unit` через заданное время.

Типичное применение – таймауты:

```cpp
auto timeout = future::After(pool, 1s)) 
    | future::Map([](Unit) -> Result<int> {
        return result::Err(TimeoutError());
      });

auto compute = future::Submit(pool, [] -> Result<int> {
  return 7;  // ~ Тяжелое вычисление
});

// Вычисление с таймаутом
auto f = std::move(compute) or std::move(timeout);

auto r = future::Get(std::move(f));
```

### Stackless Coroutines

```cpp
auto sleep = [&pool] -> coro::Task<Unit> {
  co_await coro::JumpTo(pool);
  
  for (int i = 10; i > 0; --i) {
    fmt::println("{}", i);
    co_await 1s;
  }
};

coro::RunSync(sleep());
```