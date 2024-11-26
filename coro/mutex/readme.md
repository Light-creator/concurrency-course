# Gor-routines

## Пререквизиты

- [fiber/mutex](/tasks/fiber/mutex)
- [sched/intrusive](/tasks/sched/intrusive)  
- [рекомендуется] [sched/ws_thread_pool](/tasks/sched/ws_thread_pool)

----

## Корутины

В задаче [fiber/mutex](/tasks/fiber/mutex) мы реализовали кооперативную многозадачность в виде _файберов_, которые были построены на _stackful_ корутинах.

Теперь мы реализуем кооперативную многозадачность с помощью [_stackless_](https://en.cppreference.com/w/cpp/language/coroutines) корутин.

Назовем их (в шутку!) _гор-рутинами_ (_gor-routines_) в честь одного из дизайнеров корутин в С++20 – [Гора Нишанова](https://www.youtube.com/watch?v=_fu0gx-xseY).

### Императивность

В этом задании корутины будут императивно синхронизироваться друг с другом с помощью привычных примитивов: `Event`, `Mutex` и `WaitGroup`.

### Декларативность

Наравне с поддержкой императивного стиля, корутины допускают комбинирование в функциональном стиле с помощью `Task<T>`.

### Пример

```cpp
void GorRoutinesExample() {
  using namespace exe;
  
  // В качестве планировщика возьмем 
  // уже готовый (желательно – шардированный) пул потоков
  sched::ThreadPool scheduler{4};
  scheduler.Start();
  
  thread::WaitGroup example;
  
  auto coordinator = [&] -> coro::Go {
    co_await coro::JumpTo(scheduler);

    // Корутинам нужны собственные примитивы синхронизации
    coro::Mutex mutex;
    size_t cs = 0;
    
    coro::WaitGroup wg;

    // Лямбда-корутина - это функциональный объект, 
    // у которого operator() является корутиной
    auto contender = [&] -> coro::Go {
      // Корутина исполняется в точке вызова до первой остановки
      // В данном случае остановка происходит при перепланировании в пул потоков
      co_await coro::JumpTo(scheduler);
      
      // <-- Теперь корутина бежит в виде отдельной задачи в пуле потоков

      for (size_t i = 0; i < 100'00; ++i) {
        // Захватываем асинхронный мьютекс
        // Возможно, останавливаем текущую корутину на время ожидания
        // co_await возвращает экземпляр LockGuard
        auto guard = co_await mutex.ScopedLock();
        ++cs;  // <-- В критической секции
      }  // <-- Синхронно отпускаем мьютекс в деструкторе LockGuard
      
      wg.Done();
    };
    
    for (size_t i = 0; i < 17; ++i) {
      wg.Add(1);
      contender();  // Стартуем корутину
                    // Управление возвращается после остановки в coro::JumpTo
    }
    
    co_await wg.Wait();
    
    example.Done();
  };

  example.Add(1);
  coordinator();  // Стартуем корутину
  example.Wait();
  
  scheduler.Stop();
}
```

## Планирование

Для исполнения корутин мы будем использовать уже готовые планировщики – `ThreadPool` и `RunLoop`.

### `JumpTo`

Останавливает текущую корутину и возобновляет ее исполнение в виде задачи в заданном планировщике:

```cpp
auto co = [&pool] -> coro::Go {
  co_await coro::JumpTo(pool);  // Останавливаем корутину и 
                                // возобновляем в задаче пула потоков

  // <- В пуле потоков
}
```

### `Yield`

Перепланирует корутину в текущий планировщик

```cpp
auto co = [&pool] -> coro::Go {
  co_await coro::JumpTo(pool);
  
  for (size_t i = 0; i < 5; ++i) {
    co_await coro::Yield();  // Текущий планировщик был установлен в JumpTo
  }
}
```

## Примитивы синхронизации

Как и файберы, корутины могут координировать свои действия через примитивы синхронизации. 

В этой задаче мы реализуем для корутин
- [`Event`](exe/coro/sync/event.hpp)
- [`Mutex`](exe/coro/sync/mutex.hpp) 
- [`WaitGroup`](exe/coro/sync/wait_group.hpp)

### `Mutex`

В задаче [fiber/mutex](/tasks/fiber/mutex) вы могли увидеть, что `Mutex::Unlock` в хорошей реализации требует остановки текущей корутины.

В комбинации с RAII (`lock_guard` и т.п.) это означает остановку в деструкторе. 

Но С++ не поддерживает [красные](https://journal.stuffwithstuff.com/2015/02/01/what-color-is-your-function/) (асинхронные) деструкторы... 

Тогда какое API выбрать для корутинного мьютекса? В этой задаче вам предлагается написать несколько вариаций:

#### `ScopedLock`

```cpp
// exe/coro/sync/mutex_flavor/scoped_lock.hpp

{
  auto guard = co_await mutex.ScopedLock();  // Асинхронный захват
  // <- Критическая секция
}  // Синхронное освобождение в деструкторе LockGuard
```

За `coro::Mutex` возьмем эту вариацию, она будет тестироваться по умолчанию.

#### `Run`

```cpp
// exe/coro/sync/mutex_flavor/run.hpp

co_await mutex.Run([] {
  // Критическая секция
  // co_await запрещены
});
// Критическая секция выполнена
```

Мы отказываемся от привычного для пользователя API, но получаем бОльшую свободу в реализации ⇒ бОльшую производительность.

## Запуск 

### `Go`

В этой задаче мы ограничимся очень простым протоколом запуска, который задает (через свой promise type)
возвращаемый тип `coro::Go`: 

Гор-рутина
- энергично стартует и
- не возвращает значения.

```cpp
auto co = [](sched::ThreadPool& pool) -> coro::Go {
  co_await coro::JumpTo(pool);
  
  // Выполняемся в пуле потоков
}

co();  // Вызов возвращает управление при
       // перепланировании корутины в пул потоков
```

## Реализация

### Планирование

#### Интрузивность

От планировщиков нам потребуется поддержка [интрузивных задач](exe/sched/task/task.hpp).

Интрузивность позволит планировать корутины без дополнительных аллокаций памяти.

Пусть корутина при старте "прыгает" в планировщик:
```cpp
auto Coro(sched::ThreadPool& pool) -> coro::Go {
  co_await coro::JumpTo(pool);

  // ...
}
```

Компилятор [перепишет](https://lewissbaker.github.io/2022/08/27/understanding-the-compiler-transform) тело `Coro` в класс-автомат, который аллоцирует на куче в вызове `Coro()`, а awaiter для `JumpTo` поместит в поля этого класса.

Если теперь awaiter наследовать от `sched::task::TaskBase` и в `await_suspend` поместить его как задачу в интрузивную очередь планировщика (например, пула потоков),
то мы тем самым неявно свяжем запланированные на исполнение корутины в список.

#### Текущий планировщик

Храните текущий планировщик в promise корутины.

Для доступа к promise используйте шаблонный (по promise type) вариант `await_suspend`:

```cpp
template <typename Promise>
void await_suspend(std::coroutine_handle<Promise>) {
  //
}
```

### Примитивы синхронизации

#### Интрузивность

В реализации примитивов синхронизации не должно быть динамических аллокаций памяти. 

Очереди ожидания сделайте интрузивными, узлами в них будут awaiter-ы асинхронных операций.

#### Awaiter

Обратите внимание: протокол `co_await` предусматривает не более одной остановки корутины.

##### `[[nodiscard]]`

Помечайте awaiter-ы как ``[[nodiscard]]``, тогда пользователю будет сложнее пропустить `co_await`:

```cpp
// В теле корутины
coro::JumpTo(pool);  // Не компилируется c -Werror
```

Хотя возможность допустить ошибку все равно останется:

```cpp
{
  auto guard = mutex.ScopedLock();
  // Больше не критическая секция 
}
```

##### `await_ready`

В `await_ready` опишите оптимистичный сценарий обращения к примитиву синхронизации:
- захват без ожидания в `Mutex`
- проверку готовности в `Event` и `WaitGroup`

Приносит ли `await_ready` пользу (или вред) в лок-фри реализации `Mutex`?

##### `await_suspend`

Для хорошей (лок-фри) реализации вам потребуется вариация `await_suspend`, возвращающая `bool`:
- `true` – awaiter запланировал возобновление корутины, сейчас корутина должна остановиться
- `false` означает, что корутина передумала останавливаться, она сразу выполнит `await_resume` и продолжит исполнение

#### Пробуждение

В `Event::Fire`, `Mutex::Unlock` и `WaitGroup::Done` возобновляйте ждущие корутины в планировщике.

Не возобновляйте их прямо в текущем вызове (например, `Event::Fire`), это лишит реализацию естественной параллельности.

## Задание

1) Реализуйте [`JumpTo`](exe/coro/sched/jump.hpp) и [`Yield`](exe/coro/sched/yield.hpp)
2) Реализуйте [`Event`](exe/coro/sync/event.hpp), [`Mutex`](exe/coro/sync/mutex.hpp) и [`WaitGroup`](exe/coro/sync/wait_group.hpp).
3) (опционально, рекомендуется) Напишите лок-фри реализации примитивов синхронизации

## References

### Механика

- 🔥 [Asymmetric Transfer](https://lewissbaker.github.io/)
- [cppreference](https://en.cppreference.com/w/cpp/language/coroutines)
- [dcl.fct.def.coroutine](https://eel.is/c++draft/dcl.fct.def.coroutine), [expr.await](https://eel.is/c++draft/expr.await#:co_await)

### Talks

- [Gor Nishanov – C++ Coroutines – a negative overhead abstraction](https://www.youtube.com/watch?v=Ts-1mWBmTNE)
- [Gor Nishanov – C++ Coroutines: Under the covers](https://www.youtube.com/watch?v=8C8NnE1Dg4A)
