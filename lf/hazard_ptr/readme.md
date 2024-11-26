# Hazard Pointers

Реализуйте MPMC лок-фри очередь Майкла-Скотта:

- [Simple, Fast, and Practical Non-Blocking and Blocking Concurrent Queue Algorithms](https://www.cs.rochester.edu/~scott/papers/1996_PODC_queues.pdf)
- [Lock-free структуры данных. Очередной трактат](https://habr.com/ru/articles/219201/)

Для управления памятью используйте [Hazard Pointers](https://www.cs.otago.ac.nz/cosc440/readings/hazard-pointers.pdf).

## Hazard Pointers

### References

- [Hazard Pointers: Safe Memory Reclamation for Lock-Free Objects](https://www.cs.otago.ac.nz/cosc440/readings/hazard-pointers.pdf)
- [Схемы управления памятью для лок-фри структур данных](https://habr.com/ru/articles/202190/)

### Промышленные реализации

- [Folly](https://github.com/facebook/folly/blob/main/folly/synchronization/Hazptr.h)
- [YTsaurus](https://github.com/ytsaurus/ytsaurus/blob/main/yt/yt/core/misc/hazard_ptr.h)
- [YDB](https://github.com/ydb-platform/ydb/blob/main/ydb/core/util/hazard.h)


### API

(вы можете адаптировать его)

#### `Manager`

– сборщик мусора.

Методы:
- `static Manager* Get()` – получить доступ к глобальному экземпляру (синглтону) `Manager`
- `Mutator MakeMutator()` – построить _мутатор_ для выполнения операции над лок-фри контейнером 

#### `Mutator`

_Мутатором_ (_mutator_) в алгоритмах [garbage collection](https://gchandbook.org/) называют поток программы, который меняет ссылки в графе объектов.

С помощью `Mutator` поток в операции над лок-фри контейнером
- защищает объекты в памяти от удаления и
- планирует удаление объектов.

`Mutator` создается для каждого **вызова операции** над лок-фри контейнером. 

Число мутаторов в программе, таким образом, не превосходит число потоков; для каждого потока может существовать не более одного мутатора.

Методы:
- `PtrGuard GetHazardPtr(size_t index)` – получить тред-локальный `HazardPtr` с заданным индексом.
- `void Retire(T* ptr)` – добавить объект в локальную очередь на удаление (_retire list_)

#### `PtrGuard`

`PtrGuard` – RAII guard для тред-локального `HazardPtr`.

Методы:
- `T* Protect(atomic<T*>& atomic_ptr)` – прочитать указатель на объект из `atomic_ptr` пользователя и защитить этот объект от удаления через данный `HazardPtr`.
- `void Announce(T* ptr)` – анонсировать обращение к объекту для других потоков через данный `HazardPtr`
- `void Reset()` – вручную сбросить `HazardPtr`

Объект, защищенный через `Protect`, защищен от удаления до разрушения `PtrGuard` или до вызова на нем `Protect` / `Announce` / `Reset`.


#### Пример использования 

[`LockFreeStack<T>`](lock_free_stack.hpp)

```cpp
template <typename T>
std::optional<T> LockFreeStack<T>::TryPop() {
  // Получаем Mutator для работы с hazard pointers текущего потока
  auto mutator = hazard::Manager::Get().MakeMutator();

  // Получаем тред-локальный hazard pointer
  // в виде hazard::PtrGuard
  auto top_guard = mutator.GetHazardPtr(0);

  while (true) {
    // _Атомарно_ 1) читаем указатель на вершину стека и
    // 2) защищаем ее (вершину) от удаления с помощью hazard pointer
    Node* curr_top = top_guard.Protect(top_);

    if (curr_top == nullptr) {
      return std::nullopt;
    }

    if (top_.compare_exchange_weak(curr_top, curr_top->next)) {
      T value = std::move(curr_top->value);
      // Сбрасываем hazard pointer,
      // защищающий извлеченную вершину от удаления
      top_guard.Reset();
      // Планируем удаление извлеченного узла
      mutator.Retire(curr_top);
      return value;
    }
  }
}
```

### Требования к реализации HP

- Ограниченный расход памяти
- Гарантия прогресса – lock-free
- На быстром пути – wait-free и атомарные операции `load` / `store`

## Задание

1) Реализуйте [лок-фри очередь Michael-Scott](lock_free_queue.hpp) с неограниченным потреблением памяти (тесты [queue/lf.json](pipelines/queue/lf.json))
2) Реализуйте [hazard pointers](memory/hazard/) (тесты [stack.json](pipelines/stack.json))
3) Добавьте hazard pointers в очередь MS (тесты [queue/lf_mm.json](pipelines/queue/lf_mm.json))
