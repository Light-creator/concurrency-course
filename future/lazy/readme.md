# (Lazy) Future

## Пререквизиты

- [future/fun](/tasks/future/fun)
- [sched/intrusive](/tasks/sched/intrusive)
- [рекомендуется] [sched/ws_thread_pool](/tasks/sched/ws_thread_pool)

---

В этой задаче мы напишем _ленивую_ (_lazy_) реализацию функциональных фьюч.

Ленивая реализация – это оптимизация: она избавит фьючи от лишней синхронизации и динамических аллокаций памяти, которые возникают в _энергичной_ (_eager_, _бодрой_, _жадной_) реализации.

Как и всякая оптимизация, ленивая реализация [сохранит поведение](https://en.wikipedia.org/wiki/Church%E2%80%93Rosser_theorem), но только для хорошо структурированных завершающихся программ, написанных в функциональном стиле.

## Пример

```cpp
// Здесь f – thunk, представляющий запуск лямбды в пуле потоков
future::Future<int> auto f = future::Submit(pool, [] {
  return 7;
});

// <- Пока задача не была отправлена в пул потоков

// Терминатор `Get` форсирует вычисление thunk-а / задача с лямбдой пользователя посылается в пул потоков
auto v = future::Get(std::move(f));
fmt::println("v = {}", v);  // Печатает 7
```

## References

Сначала – полезная для понимания дизайна теория:

#### Haskell

- [A History of Haskell: being lazy with class](https://www.youtube.com/watch?v=06x8Wf2r2Mc) ([статья](https://www.microsoft.com/en-us/research/wp-content/uploads/2016/07/history.pdf)) by [Simon Peyton Jones](https://en.wikipedia.org/wiki/Simon_Peyton_Jones)

##### Laziness

- [Lazy evaluation](https://wiki.haskell.org/Lazy_evaluation), [Thunk](https://wiki.haskell.org/Thunk)
- [The Incomplete Guide to Lazy Evaluation (in Haskell)](https://apfelmus.nfshost.com/articles/lazy-eval.html)

##### Semantics

- [Non-strict semantics](https://wiki.haskell.org/Non-strict_semantics)
- [Non-Strict Semantics of Haskell](https://apfelmus.nfshost.com/articles/non-strict-semantics.html)

#### λ-calculus
- [A Brief and Informal Introduction to the Lambda Calculus](https://www.cs.yale.edu/homes/hudak/CS201S08/lambda.pdf)
- [Church–Rosser theorem](https://en.wikipedia.org/wiki/Church%E2%80%93Rosser_theorem)

## Ленивые фьючи

### Ментальная модель

Фьюча – это _thunk_ (далее – просто _санк_, "замысел"), т.е. объект, который представляет еще не стартовавшую асинхронную операцию / будущее значение.

Санк _форсируется_ / асинхронная операция стартует только когда возникла _потребность_ (_demand_) в ее результате.

Потребность возникает когда поток / файбер / корутина хочет "развернуть" фьючу и получить реальный результат.

Потребность представляется _продолжением_ (_continuation_). 
Продолжение – это объект, представляющий вычисление, которому нужно входное значение для возобновления.

Форсирование санка – это:
1) _материализация_ (операция _materialize_) "замысла" в виде _вычисления_ (_computation_) и 
2) _запуск_ (операция _start_) вычисления:

При материализации _computation_ получает фиксированный адрес в памяти (_pinned_).

С вычислением связано _мутабельное состояние_ (_mutable state_), в которое входит планировщик.

### Код

Все понятия модели явно [представлены](exe/future/model) в коде в виде [концептов](https://en.cppreference.com/w/cpp/language/constraints):

- [`Continuation<T>`](exe/future/model/cont.hpp)
- [`Thunk`](exe/future/model/thunk.hpp)
- [`Computation`](exe/future/model/comp.hpp)
- [`State`](exe/future/model/state.hpp)

### Фьючи

Теперь фьюча – не конкретный класс, а концепт [`SomeFuture`](exe/future/type/future.hpp) (синоним для [`Thunk`](exe/future/model/thunk.hpp)) + параметризованный `Future<T>`.

Планируя цепочку шагов с помощью комбинаторов, пользователь конструирует в compile time конкретную фьючу (санк):

```cpp
// В типе фьючи f закодированы все шаги вычисления
future::Future<int> auto f = future::Just()
    | future::Via(pool)
    | future::Map([](Unit) {
        return 1;
      })
    | future::Map([](int v) {
        return v + 2;
      });  
```

#### Fallible

Как и раньше, мы добавим `SomeTryFuture` + `TryFuture<T>` для сбойных операций.

### Терминаторы

В ленивой реализации раскрывается линейность фьюч / требование завершать каждую цепочку терминатором:

Терминатор – это шаг, на котором форсируется вычисление санка / запускается асинхронная операция.

Поэтому в ленивой реализации мы переименуем `exe/future/terminate` в `exe/future/run`.

### Оптимизации

#### Storage

**Интрузивность** в планировщиках позволяет пользователю (в данном контексте – фьючам) выбирать сторадж (стек или куча) для задачи (например, в `Submit` – сторадж для лямбды с кодом пользователя).

**Ленивость** позволяет реализации фьюч отложить выбор стораджа для задачи до того момента, когда этот выбор можно будет сделать оптимально, т.е. до применения терминатора!

##### `Get`

Для синхронного терминатора (`Get` для потоков, `Await` для файберов и `co_await` для stackless корутин) достаточно
положить материализованное вычисление на стековый фрейм `Get` / `Await` / корутины. 

Таким образом, из исполнения стираются динамические аллокации памяти (причем без подсказок со стороны пользователя):

```cpp
// В этом сниппете нет динамических аллокаций памяти!

{
  auto f = future::Submit(pool, [] {
    return 42;
  });

  auto v = future::Get(std::move(f));  // <- Разрушение состояния задачи упорядочено с ее завершением
}
```

##### `Detach`

Для `Detach` придется упаковать материализованное вычисление в самоуничтожающийся "контейнер" на куче: пути текущего исполнения и асинхронного расходятся, время жизни вычисления неизвестно.

#### Синхронизация

Запуск асинхронной операции выполняется после подписки на ее результат, 
а значит рандеву между продьюсером и консьюмером в ленивой реализации не требуется.

Таким образом, синхронизация в ленивой реализации фьюч остается **только** там, где есть недетерминизм на уровне логики пользователя:
- в параллельных комбинаторах,
- в асинхронных примитивах синхронизации.

#### Результат 

Мы получили **zero-cost** futures!

## Комбинаторы

Ленивым фьючам потребуются специальные комбинаторы:

---

### 👉  `Box`

Заголовок: [`combine/seq/box.hpp`](exe/future/combine/seq/box.hpp)

Стирает конкретный тип санка до типа `BoxedFuture<T>`.

```cpp
auto f = future::Submit(pool, [] {
  return 42;
}) | future::Box();
```

Или

```cpp
// Auto-boxing
future::BoxedFuture<int> f = future::Value(7);
```

#### Интерфейсы

Боксинг необходим асинхронным интерфейсам:

```cpp
struct IAsyncReader {
  virtual ~IAsyncReader() = default;
  
  // Не можем использовать концепт Future<size_t>, должны указать конкретный тип
  virtual exe::future::BoxedFuture<Result<size_t>> ReadSome(std::span<std::byte> buffer) = 0;
};
```

---

### 👉  `Start`

Заголовок: [`combine/seq/start.hpp`](exe/future/combine/seq/start.hpp)

Форсирует санк / трансформирует ленивую фьючу в энергичную / стартует асинхронную операцию.

```cpp
future::Future<int> auto f = future::Submit(pool, [] {
  return 7;
}) | future::Start();  // <- Отправляем задачу в пул потоков

// <- Задача уже запланирована в пул
```

Синоним: `future::Force()`

#### Bang (!)

Заголовок: [`syntax/bang.hpp`](exe/future/syntax/bang.hpp)

По аналогии с [bang patterns в Haskell](https://ghc.gitlab.haskell.org/ghc/doc/users_guide/exts/strict.html), поддержим для фьюч [оператор `!` (_bang_)](exe/future/syntax/bang.hpp):

```cpp
// !f читается как "bang f", означает f | Force()
auto f = !future::Submit(pool, [] {
  return 7;
});
```

## References

- [Zero-cost futures in Rust](https://aturon.github.io/tech/2016/08/11/futures/), [Designing futures for Rust](https://aturon.github.io/tech/2016/09/07/futures-design/)
- [Senders (`std::execution`)](https://www.open-std.org/jtc1/sc22/wg21/docs/papers/2024/p2300r9.html)

---

## Задание

- Реализуйте `Submit`
- Реализуйте `Get` и `Detach`
- Реализуйте `Just`, `Via`, `Map`, перепишите `Submit`
- Реализуйте `MapOk`, `AndThen` и `OrElse` для `TryFuture`  
- Реализуйте `Flatten`
- Реализуйте `Contract`
- Реализуйте `Both` и `First`
- Реализуйте `Box`, `Start`  
- (опционально) Реализуйте `FirstOk` и `BothOk`

### Дополнительно

#### `Materialize`

Избавьтесь от перемещений продолжений в `Materialize`.

#### Future-local / Mutable state 

Поддержите произвольный типизированный mutable state, который в том числе позволял бы
устанавливать планировщик для комбинатора `Map`.

#### Синхронизация

Реализуйте асинхронные примитивы синхронизации (`Event`, `Mutex`, `WaitGroup`),
которые можно использовать как со stackful файберами, так и со stackless корутинами.

#### Boxing

Не делайте дополнительных аллокаций при боксинге энергичной фьючи.

Поддержите small future optimization.

### Замечания по реализации

- Маркируйте санки [`[[nodiscard]]`](https://en.cppreference.com/w/cpp/language/attributes/nodiscard)
- Запретите санкам копироваться

#### Типы

Для печати типов санков / вычислений используйте:

```cpp
template <typename T>
void PrintTypeOf(T&) {
  fmt::println("{}", __PRETTY_FUNCTION__);
}
```