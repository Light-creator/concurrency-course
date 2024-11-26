# (Functional) Future

## Пререквизиты

- [sync/wait_group](/tasks/sync/wait_group)
- [sched/run_loop](/tasks/sched/run_loop)
- [future/contract](/tasks/future/contract)
- [рекомендуется] [fiber/mutex](/tasks/fiber/mutex)

---

До этого момента мы писали (и думали о) concurrency в императивном стиле, представляя конкурентные активности как серии мутаций разделяемого состояния и называя их файберами. 

В этой задаче предлагается [функциональная](https://www.haskell.org/) модель (и язык) для concurrency, в которой конкурентные активности
представлены как графы трансформаций иммутабельных значений и именуются _фьючами_ (_futures_).

## `Future`

В основе модели лежит понятие _future_ (далее – просто _фьюча_) и шаблон `Future<T>`.

`Future<T>` представляет будущий, еще не готовый, результат асинхронной операции.

За фьючей может стоять
- [вычисление в пуле потоков](exe/future/make/submit.hpp),
- [RPC](https://capnproto.org/cxxrpc.html),  
- [IO](https://tokio.rs/tokio/tutorial/io),
- [ожидание на семафоре](https://github.com/twitter/util/blob/143adbdf5bc6dc55eddb9248c597eeb11a799094/util-core/src/main/scala/com/twitter/concurrent/AsyncSemaphore.scala#L141),
- [таймаут](https://gitlab.com/Lipovsky/await/-/blob/3ff428941d6ccf414e0e4c9fd3831f883c6379f5/demo/main.cpp#L44) и т.д.

## `Result`

Фьюча может представлять вызов метода на удаленном сервере или чтение с диска,
и нужно учитывать, что подобная операция может завершиться ошибкой.

Результат для таких (_fallible_, допускающих сбой) операций мы будем представлять в виде `Result<T>`, который содержит либо значение типа `T`, либо ошибку.

Положим [`TryFuture<T>`](exe/future/type/result.hpp) = `Future<Result<T>>` – _сбойная_ фьюча.

## Комбинаторы

Фьючи, т.е. стоящие за ними асинхронные операции, мы будем комбинировать в функциональном стиле:

```cpp
future::TryFuture<Response> Hedge(Request request) {
  // primary – сбойная фьюча, представляющая удаленный вызов к некоторому сервису
  auto primary = Rpc(request);

  // "Запасной" запрос к другой реплике сервиса, выполняемый по истечению таймаута
  auto backup = future::After(99ms) | future::FlatMap([request](Unit) {
    return Rpc(request);
  });
  
  // Возвращаем первый успешный ответ (а что делать с опоздавшим запросом?)
  return future::FirstOk(std::move(primary), std::move(backup));
}
``` 

На этом небольшом примере с _хэджированием_ (_hedging_) запросов (см. статью [Tail at Scale](https://www.barroso.org/publications/TheTailAtScale.pdf)) хорошо видны преимущества функционального подхода в приложении к concurrency:

Разработчик описывает **что** и **когда** он хочет сделать, а за реализацию этого плана (и за всю сопряженную с ним синхронизацию) отвечают комбинаторы.

**Меняется ментальная модель** concurrency: разработчик думает не про чередование обращений к разделяемому состоянию, т.е. не про control flow, а про **трансформацию иммутабельных значений**, которые "текут" от продьюсеров к консьюмерам, т.е. про **data flow**.

### 🔥 References

Обязательны к прочтению!

- [Your Server as a Function](https://monkey.org/~marius/funsrv.pdf) by Marius Eriksen
- [Futures aren't Ersatz Threads](https://monkey.org/~marius/futures-arent-ersatz-threads.html) by Marius Eriksen

## Модель

Фьючи – значения, которые представляют цепочки / графы задач или (в общем случае) – произвольные конкурентные активности.

### Операции

- _конструкторы_ (`Contract`, `Submit`, `Value`, ...) или _сервисы_ (см. [Your Server as a Function](https://monkey.org/~marius/funsrv.pdf)) строят фьючи / открывают цепочки,
- _комбинаторы_ (`Map`, `FlatMap`, `AndThen`, `OrElse`, ...) получают фьючи на вход, поглощают их и строят новые фьючи / продолжают цепочки,
- _терминаторы_ (`Get`, `Detach`) поглощают фьючи / завершают цепочки.

_Конструкторы_ моделируют продьюсеров, _терминаторы_ – консьюмеров.

Вместе они описывают графы операций / вычислений / задач.

### Линейность

Фьючи – _линейные_ (_linear_): каждая фьюча должна быть поглощена (комбинатором или терминатором) **ровно один раз** (_exactly-once_).

#### C++

К сожалению, линейность нельзя выразить в системе типов С++ ☹️.

В качестве приближения потребуем: фьюча не может быть скопирована (_non-copyable_), только перемещена (_move-constructible_, _non-move-assignable_).

Будем считать, что непотребленная фьюча – это [implementation-defined behavior](https://eel.is/c++draft/defns.impl.defined).

## Пример

```cpp
void FuturesExample() {
  using namespace exe;
  
  sched::ThreadPool pool{4};
  pool.Start();
  
  auto pipeline = future::Just()  // <- конструктор, начинает цепочку
      | future::Via(pool) 
      | future::Map([](Unit) {  // <- комбинатор
          fmt::println("Running on thread pool");
          return 42;
        }) 
      | future::FlatMap([&pool](int) {
        // Асинхронный шаг
        return future::Submit(pool, [] -> Result<int> {
          return result::Err(TimeoutError());  // Который завершается ошибкой
        };
      })
      | future::OrElse([](Error) {
          return future::Ok(7);  // fallback
        })
      | future::MapOk([](int v) {
          return v + 1;
        });
      
  auto r = future::Get(std::move(pipeline));  // <- терминатор
  fmt::println("{}", *r);  // Печатает 8
  
  pool.Stop();
}
```

Больше примеров – в [play/main.cpp](play/main.cpp)


## Структура `exe/future`

- `type` – типы
- `make` – конструкторы
- `combine` – комбинаторы
  - `seq` – последовательная композиция
  - `concur` – конкурентная композиция
- `terminate` – терминаторы
- `syntax` – перегрузки операторов

## Конструкторы

_Конструкторы_ – функции, конструирующие новые фьючи.

Каталог базовых конструкторов: [`future/make`](exe/future/make)

Фреймворк RPC расширит этот базовый набор конструктором удаленного вызова, библиотека IO – операциями асинхронного чтения / записи и т.д.

---

### 👉 `Contract`

Заголовок: [`make/contract.hpp`](exe/future/make/contract.hpp)

Асинхронный контракт между продьюсером и консьюмером:

```cpp
auto [f, p] = future::Contract<int>();

// Producer
std::thread producer([p = std::move(p)] mutable {
  std::move(p).Set(7);
});

// Consumer
auto v = future::Get(std::move(f));  // 7

producer.join();
```

Обязанности сторон:

- продьюсер обязан выполнить фьючу с помощью `Set`,
- консьюмер обязан потребить фьючу с помощью комбинатора или терминатора.

---

### 👉 `Submit`

Заголовок: [`make/submit.hpp`](exe/future/make/submit.hpp)

Фьюча, представляющая вычисление в планировщике задач.

```cpp
auto f = future::Submit(pool, [] {
  return 7;
});
```

---

### 👉 `Ready`

Заголовок: [`make/ready.hpp`](exe/future/make/ready.hpp)

Фьюча, представляющая готовое значение.

```cpp
auto f = future::Ready(result::Ok(2));
```

---

### 👉 `Value`

Заголовок: [`make/value.hpp`](exe/future/make/value.hpp)

Аналогично `Ready`, но только для простых значений: `Result<T>` в качестве значения не допускается.

Существует для повышения читаемости пользовательского кода.

```cpp
auto f = future::Value(7);
```

---

### 👉 `Return`

Заголовок: [`make/return.hpp`](exe/future/make/return.hpp)

Синоним для `Ready`, отсылающий читателя к [монадам](https://wiki.haskell.org/Typeclassopedia).

```cpp
auto f = future::Return(7);
```

---

### 👉 `Just`

Заголовок: [`make/just.hpp`](exe/future/make/just.hpp)

Фьюча, представляющая готовое значение типа [`Unit`](exe/result/type/unit.hpp).

```cpp
auto f = future::Just()
    | future::Via(pool) 
    | future::Map([](Unit) {
        return 42;
      });
```

#### Unit

Асинхронные операции, не возвращающие значения, моделируются с помощью `Future<Unit>`: 

https://en.wikipedia.org/wiki/Unit_type

Фьючи – это будущие значения, но значений типа `void` не бывает, значит не должно быть и будущих `void`-ов.

## Комбинаторы

_Комбинаторы_ – функции, которые получают фьючи на вход (потребляют их) и строят новые фьючи.

### Последовательная композиция

Каталог: [`future/combine/seq`](exe/future/combine/seq)

#### Pipeline operator

Последовательная композиция фьюч на уровне синтаксиса выражается через оператор `|` (_pipeline operator_). 

Оператор не имеет прямого отношения к фьючам, он решает более общую задачу: упростить описание цепочек вызовов функций, где последующий вызов получает на вход выход предыдущего. 

Выражение `f(a) | h(x, y)` переписывается оператором `|` в `h(f(a), x, y)`

Поддержка `|` для фьюч: [future/syntax/pipe.hpp](exe/future/syntax/pipe.hpp)

##### References

С помощью перегрузки оператора `|` мы эмулируем "настоящий" (и не существующий в языке С++) оператор `|>`:

- [OCaml] [Pipelining](https://cs3110.github.io/textbook/chapters/hop/pipelining.html)
- [JavaScript] [Pipe Operator (|>) for JavaScript](https://github.com/tc39/proposal-pipeline-operator/)
- [C++] [Exploring the Design Space for a Pipeline Operator](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2022/p2672r0.html)

---

#### 👉 `Map` 

Заголовок: [`combine/seq/map.hpp`](exe/future/combine/seq/map.hpp)

Сигнатура: `Future<T>` → (`T` → `U`) → `Future<U>` ([нотация](https://www.haskell.org/tutorial/functions.html) заимствована из Haskell)

Комбинатор `Map` применяет (в будущем) функцию пользователя (_маппер_) к значению входной фьючи:

```cpp
auto f = future::Value(1) | future::Map([](int v) {
  return v + 1;
});
```

Как и любой другой комбинатор, `Map` применяется к фьюче **без ожидания** будущего результата, он лишь планирует вызов продолжения через подписку на результат входной фьючи и строит новую (выходную) фьючу.

---

#### 👉 `Via`

Заголовок: [`combine/seq/via.hpp`](exe/future/combine/seq/via.hpp)

##### Гонка

Рассмотрим пример:

```cpp
// Гонка между продьюсером и консьюмером
auto f = future::Submit(pool, [] { return 7; })
    | future::Map([](int v) { return v + 1; });
```

Поток, в котором запустится маппер, зависит от того, как упорядочатся при рандеву
- выполнение задачи в пуле потоков и 
- подписка на результат в комбинаторе `Map`.

Пользователь, планирующий с помощью фьюч цепочку асинхронных действий, 
должен иметь контроль на тем, в каком потоке (вернее, в каком планировщике) выполнится каждый ее шаг.

Такой контроль дает комбинатор `Via`: 

```cpp
auto f = future::Submit(pool, [] { return 7; })
    | future::Via(pool)  // <- Устанавливаем планировщик для запуска кода пользователя
    | future::Map([](int value) {
        // Гарантированно выполняется в пуле потоков `pool`
        fmt::println("Running on thread pool");
        return result::Ok()
      });
```

Установленный через `Via` планировщик **наследуется** по цепочке (пока не встретит параллельный комбинатор или новый `Via`).

##### `Inline`

Планировщик по умолчанию – [`Inline`](exe/sched/inline.hpp).

##### State

Можно заметить, что `Via` – это мутация состояния, которая "просочилась" в _чистую_ (_pure_) функциональную модель.

---

#### 👉 `Flatten`

Заголовок: [`combine/seq/flatten.hpp`](exe/future/combine/seq/flatten.hpp)

Сигнатура: `Future<Future<T>>` → `Future<T>`

"Уплощает" вложенную асинхронность:

```cpp
future::Future<int> f = future::Submit(pool, [&pool] {
  return future::Submit(pool, [] {
    return 7;
  })
}) | future::Flatten();
```

---

#### 👉 `FlatMap`

Заголовок: [`combine/seq/flat_map.hpp`](exe/future/combine/seq/flat_map.hpp)

Сигнатура: `Future<T>` → (`T` → `Future<U>`) → `Future<U>`

Планирует асинхронное продолжение цепочки задач.

```cpp
future::Future<int> f = future::Submit(pool, [] { return 1; }) 
    | future::FlatMap([&pool](int v) {
        return future::Submit(pool, [v] {
          return v + 1;
        });
      });   
```

##### Монады

Фьючи с конструктором `Return` (`return`) и комбинатором `FlatMap` (`>>=`) образуют [монаду](https://wiki.haskell.org/All_About_Monads#The_Monad_class).

---

#### `TryFuture<T>`

Комбинаторы для `TryFuture<T>`, которая представляет сбойные операции.

##### 👉 `AndThen` / `OrElse`

Заголовки:
- [`combine/seq/result/and_then.hpp`](exe/future/combine/seq/result/and_then.hpp)
- [`combine/seq/result/or_else.hpp`](exe/future/combine/seq/result/or_else.hpp)

Сигнатура:
- `AndThen`: `TryFuture<T>` → (`T` → `TryFuture<U>`) → `TryFuture<U>`
- `OrElse`: `TryFuture<T>` → (`Error` → `TryFuture<T>`) → `TryFuture<T>`

Комбинаторы `AndThen` / `OrElse` разделяют успешный путь / обработку ошибки в цепочке сбойных асинхронных шагов:
- `AndThen` вызывается только на значениях,
- `OrElse` – только на ошибках.

```cpp
auto f = future::Just() 
    | future::Map([] -> Result<int> {  // Fallible
        return result::Err(IoError());
      })
    | future::AndThen([](int v) {
        return future::Ok(v + 1);  // Аналогично result::Ok
      })
    | future::OrElse([](Error) {
        return future::Ok(7);  // Fallback
      });
```

Нетрудно заметить, что `AndThen` / `OrElse` – это асинхронный аналог для блока `try` / `catch`.

##### 👉 `MapOk`

Заголовок: [`combine/seq/result/map_ok.hpp`](exe/future/combine/seq/result/map_ok.hpp)

Сигнатура: `TryFuture<T>` → (`T` → `U`) → `TryFuture<U>`

Вариация `Map` для успешного пути в сбойной фьюче:

```cpp
auto f = future::Ready(result::Ok(1))
     | future::MapOk([](int v) {
         return v + 2;
       });
```

### Concurrent композиция

Каталог: [`combine/concur`](exe/future/combine/concur)

---

#### 👉 `First`

Заголовок: [`combine/concur/first.hpp`](exe/future/combine/concur/first.hpp)

Фьюча, представляющая первое значение фьюч, поданных на вход.

```cpp
auto timeout = future::After(1s) 
    | future::Map([](Unit) -> Result<int> {
        return result::Err(TimeoutError());
      });

auto compute = future::Submit(pool, [] -> Result<int> {
  return 7;  // ~ Тяжелое вычисление
});

// Вычисление с таймаутом
auto compute_with_timeout = future::First(std::move(compute), std::move(timeout));

// Первый готовый Result
auto result = future::Get(std::move(compute_with_timeout));
```

Тип значения входных фьюч не интерпретируется.

#### 👉 `FirstOk`

Заголовок: [`combine/concur/result/first.hpp`](exe/future/combine/concur/result/first.hpp)

Вариация `First` для `TryFuture`.

Фьюча, представляющая первый успех / последнюю ошибку двух сбойных фьюч, поданных на вход.

См. пример с хеджированием RPC.

---

#### 👉 `Both` / `All`

Заголовок: [`combine/concur/all.hpp`](exe/future/combine/concur/all.hpp)

Фьюча, представляющая пару (кортеж) значений фьюч, поданных на вход:

```cpp
auto f = future::Submit(pool, [] { return 1; });
auto g = future::Submit(pool, [] { return 2; });

// Без ожидания
auto both = future::Both(std::move(f), std::move(g));

// Синхронно ожидаем двух значений
auto [x, y] = future::Get(std::move(both));
```

Тип значения входных фьюч не интерпретируется.

##### 👉 `BothOk` / `AllOk` 

Вариация `Both` для `TryFuture`.

Фьюча, представляющая оба (все) значения / первую ошибку сбойных фьюч, поданных на вход.

##### Short-circuit

Синхронная распаковка фьючи `BothOk` **не эквивалентна** последовательной синхронной распаковке
двух фьюч `f` и `g`: если вторая фьюча завершилась ошибкой, то ожидание первой будет прервано.

---

#### Контекст

Параллельные комбинаторы **сбрасывают** планировщик до `Inline`, так что пользователь должен явно установить его после объединения цепочек.

## Терминаторы

_Терминаторы_ завершают асинхронные цепочки / графы.

Каталог: [`future/terminate`](exe/future/terminate)

---

### 👉 `Get`

Заголовок: [`terminate/get.hpp`](exe/future/terminate/get.hpp)

Терминатор `Get` блокирует текущий поток до готовности значения фьючи:

```cpp
// Планируем задачу в пул потоков
auto f = future::Submit(pool, [] {
  return result::Ok(7);
});

// Блокируем поток до готовности результата
auto r = future::Get(std::move(f));
```

Иначе говоря, `Get` синхронно "распаковывает" `Future` в её `ValueType`.

#### Chaining

Терминатор `Get` можно использовать с оператором `|`:

```cpp
auto r = future::Submit(pool, [] { return result::Ok(7); })
          | future::Get();
```

---

### 👉 `Detach`

Заголовок: [`terminate/detach.hpp`](exe/future/terminate/detach.hpp)

`Future` аннотирована как [`[[nodiscard]]`](https://en.cppreference.com/w/cpp/language/attributes/nodiscard) и должна быть явно поглощена консьюмером. 

Терминатор `Detach` поглощает фьючу и игнорирует ее результат.

```cpp
// Завершение задачи в пуле нас не интересует
future::Submit(pool, [] { /* ... */ }) 
    | future::Detach();
```

## Операторы

В [`future/syntax`](exe/future/syntax) собраны перегрузки операторов для фьюч:

- `f | c` означает `c.Pipe(f)`
- `f or g` означает `First(f, g)`
- `f + g` означает `Both(f, g)`

## Лямбды

Эргономика фьюч сильно зависит от лаконичности синтаксиса анонимных функций.

Сравните:
- `[](int v) { return v + 1; }` (C++)
- `|v| v + 1` ([Rust](https://doc.rust-lang.org/rust-by-example/fn/closures.html))
- `_ + 1` ([Scala](https://docs.scala-lang.org/scala3/book/fun-anonymous-functions.html))

## Значения

Как вы можете видеть, пользователь работает с фьючами как с **иммутабельными значениями**, т.е. передает (точнее, перемещает)
их в комбинаторы и терминаторы.

## `Result`

[`Result<T>`](exe/result/type/result.hpp) – это alias для `std::expected<T, E>`, где `E` = [`Error`](exe/result/type/error.hpp) = `std::error_code`.

### `Error`

Мы фиксируем тип ошибки для простоты, в хорошем фреймворке у пользователя должна быть свобода в выборе типа `E`.

Хороший тип ошибки подразумевает возможность:
- добавлять к ошибке контекст (source location, correlation id и т.д.)
- комбинировать ошибки

### Конструкторы

#### `Ok`

Позволяет не указывать в сигнатуре лямбды возвращаемый тип:

```cpp
auto f = future::Value(7) | future::Map([](int v) {
  return result::Ok(v + 1);
);
```

Альтернативный вариант:

```cpp
auto f = future::Value(7) | future::Map([](int v) -> Result<int> {
  return v + 1;   // Автоматическая упаковка
);
```

#### `Err`

```cpp
// Status – alias для Result<Unit>
auto f = future::Submit(pool, []() -> Result<int> {
  return result::Err(TimeoutError());  // Тип значения выводится автоматически
});
```

### Комбинаторы

```cpp
auto r = result::Ok(2)
      | result::Map([](int v) { return v + 1; })
      | result::AndThen([](int v) {
          return result::Ok(v + 2);  
        })
      | result::OrElse([](Error) {
        return result::Ok(42);  // Fallback
      });
```

## Задание

1) Реализуйте `future::Contract` + `future::Get` с помощью wait-free рандеву и подписки с коллбэком
2) Прочтите статью [Your Server as a Function](https://monkey.org/~marius/funsrv.pdf)
3) Реализуйте функциональные фьючи
   1) Конструкторы
   2) Последовательные комбинаторы
   3) Конкурентные комбинаторы
4) [Познакомьтесь с языком Haskell](https://www.haskell.org/tutorial/), мы к нему еще вернемся
5) Поразмышляйте над эффективной реализацией фьюч
6) Изучите каталоги комбинаторов в разных языках / фреймворках, попробуйте составить свой собственный каталог

### Дополнительно

#### Файберы

Реализуйте функцию `fiber::Await` для ожидания произвольной фьючи в файбере:

```cpp
fiber::Go(pool, [&pool] {
  auto f = future::Submit(pool, [] {
    return 1;
  };
  
  auto g = future::Submit(pool, [] {
    return 2;
  });
  
  // Без ожидания
  auto first = std::move(f) or std::move(g);
  
  // Останавливаем текущий файбер до готовности одного из входов First
  // fiber/sched/await.hpp
  auto v = fiber::Await(std::move(first));
  
  fmt::println("first -> {}", v);
});
```

[Тесты](pipelines/fiber/sched/await.json)

#### `First` & `All`

(Продвинутый уровень)

Напишите variadic версии комбинаторов `First` и `All` (обобщение `Both`).

#### `Result<T, E>`

(Продвинутый уровень)

Поддержите произвольный тип `E` для `Result`.

#### `Unit`

Скройте от пользователя `Unit`-ы:

```cpp
auto f = future::Just() | future::Via(pool) | future::Map([] {
  // Маппер принимает значение типа Unit
  return 7;  
});
```

## Реализация

Задача не навязывает конкретную реализацию фьюч и допускает большую степень свободы в ее выборе. 

Но будет разумно начать с самой наивной и неэффективной, и только потом задуматься о том, что можно сделать лучше.

Наша главная цель на данном этапе – познакомиться с функциональным подходом к concurrency и освоить
 довольно обширный язык комбинаторов.

### Коллбэки

За исключением `Get`, в реализации фьюч нигде нет блокирующего ожидания!

Внутренняя механика всех комбинаторов и терминаторов – подписка на результат с помощью коллбэка.

Например, 
- в `Map` коллбэк планирует запуск маппера в установленном планировщике задач,
- в `Get` коллбэк устанавливает результат и будит ждущий поток,
- в `First` – "гоняется" с коллбэком другого "входа" за право выполнить выходную фьючу.

### Типы коллбэков

Разделим коллбэки на два класса:

1) В коллбэке исполняется **произвольный код пользователя** (например, в комбинаторах `Map` и `AndThen`)
2) В коллбэке исполняется **служебный код** комбинатора (например, `Flatten` или `First`) или терминатора (`Get` для потока или `Await` для файбера)

#### Пользователь

Потребуем, чтобы код пользователя (маппер) всегда исполнялся в виде отдельной задачи
в явно установленном планировщике.

#### Служебные коллбэки

Потребуем, чтобы служебные коллбэки всегда инлайнились (см. `Inline`).

### Синхронизация

Фьючам не требуется взаимное исключение, все терминаторы и комбинаторы в
задаче могут быть реализованы с гарантией прогресса wait-free.

## References

### Futures

- [Your Server as a Function](https://monkey.org/~marius/funsrv.pdf)
- [Futures aren't Ersatz Threads](https://monkey.org/~marius/futures-arent-ersatz-threads.html)
- [Асинхронность в программировании](https://habr.com/ru/companies/jugru/articles/446562/)

#### In the Wild

- [com.twitter.util.Future](https://twitter.github.io/util/docs/com/twitter/util/Future.html), [Twitter Futures](https://twitter.github.io/finagle/guide/Futures.html)
- [Rust Futures](https://docs.rs/futures/latest/futures/)
- [cats.effect.IO](https://typelevel.org/cats-effect/api/3.x/cats/effect/IO.html)
- [Futures for C++11 at Facebook](https://engineering.fb.com/2015/06/19/developer-tools/futures-for-c-11-at-facebook/), [Folly Futures](https://github.com/facebook/folly/blob/main/folly/docs/Futures.md)
- [YTsaurus Futures](https://github.com/ytsaurus/ytsaurus/tree/main/yt/yt/core/actions)    

### Errors

- [The Error Model](https://joeduffyblog.com/2016/02/07/the-error-model/)

### Haskell

- https://www.haskell.org/
- [A Gentle Introduction to Haskell](https://www.haskell.org/tutorial/)

### Linear types

- [Substructural type system](https://en.wikipedia.org/wiki/Substructural_type_system)
- [Linear Haskell](https://arxiv.org/pdf/1710.09756.pdf)

### Monads

#### Introduction

- [About monads](https://www.haskell.org/tutorial/monads.html)
- [All About Monads](https://wiki.haskell.org/All_About_Monads)

#### Deep Dive

- [Monads for functional programming](https://homepages.inf.ed.ac.uk/wadler/papers/marktoberdorf/baastad.pdf)
- [Imperative functional programming](https://www.microsoft.com/en-us/research/wp-content/uploads/1993/01/imperative.pdf)
- [Typeclassopedia](https://wiki.haskell.org/Typeclassopedia)
- [A monad is just a monoid in the category of endofunctors, what's the problem?](https://stackoverflow.com/questions/3870088/a-monad-is-just-a-monoid-in-the-category-of-endofunctors-whats-the-problem)
- [Functors, Applicatives, And Monads In Pictures](https://www.adit.io/posts/2013-04-17-functors,_applicatives,_and_monads_in_pictures.html)
- [Abstraction, intuition, and the “monad tutorial fallacy”](https://byorgey.wordpress.com/2009/01/12/abstraction-intuition-and-the-monad-tutorial-fallacy/)

