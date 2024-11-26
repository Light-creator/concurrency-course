# Интрузивные задачи

## Пререквизиты

- [sched/run_loop](/tasks/sched/run_loop)
- [tutorial/function](/tasks/tutorial/function)

---

Пока планирование задачи в `ThreadPool` или `RunLoop` – это (в общем случае) динамическая аллокация контейнера для лямбды (назовем его _состоянием задачи_) для type erasure внутри `fu2::unique_function`.

![Non-intrusive tasks](https://gitlab.com/Lipovsky/concurrency-course-media/-/raw/main/tasks/sched/intrusive/non-intrusive-tasks.png)

При этом главным "пользователям" планировщиков – stackful файберам, stackless корутинам, фьючам – динамические аллокации при планировании задач не требуются, они могут управлять состоянием своих задач эффективнее. 

## Интрузивные задачи

Мы избавимся от динамических аллокаций памяти в планировщиках, сделав задачи _интрузивными_.

Во-первых, спрячем конкретный тип задачи от планировщика за интерфейсом `ITask`:

```cpp
// exe/sched/task/task.hpp

namespace exe::sched::task {

struct ITask {
  virtual void Run() noexcept = 0;

 protected:
  ~ITask() = default;
};

// ...

}  // namespace exe::sched::task
```

За этим интерфейсом может находиться
- `Fiber`
- awaiter для stackless корутины
- `Future`

Задачам нужно будет укладываться в очереди, так что встроим в них указатель:

```cpp
// exe/sched/task/task.hpp

namespace exe::sched::task {

// ...

struct TaskBase 
    : ITask,
      wheels::IntrusiveForwardListNode<TaskBase> {
  //
};

}  // namespace exe::sched::task
```

Метод `Submit` у `IScheduler` будет принимать `TaskBase*`:

```cpp
// exe/sched/task/scheduler.hpp

namespace exe::sched::task {

struct IScheduler {
  virtual ~IScheduler() = default;
  
  virtual void Submit(TaskBase* task) = 0;
};

}  // namespace exe::sched::task
```

Пользователь, планирующий задачу в `IScheduler`, должен гарантировать,
что состояние задачи не разрушится до завершения ее исполнения.

## Пример

### Raw

В этом примере нет ни одной динамической аллокации памяти:

```cpp
void IntrusiveTask() {
  using namespace exe;
  
  struct HelloTask : task::TaskBase {
    void Run() noexcept override {
      fmt::println("Hello, world!");
    }
  };
  
  sched::RunLoop loop;
  
  {
    HelloTask hello{};
  
    loop.Submit(&hello);
    loop.Run();
  }
}
```

### Файберы

```cpp
void YieldExample() {
  using namespace exe;
  
  // Единственные динамические аллокации в этом примере
  // - аллокации самих файберов в функции fiber::Go
  
  sched::RunLoop scheduler;
  
  for (size_t i = 0; i < 0; i < 4; ++i) {
    fiber::Go(scheduler, [] {
      for (size_t j = 0; j < 128; ++j) {
        fiber::Yield();  // Файберы перепланируются без аллокаций памяти
      }
    });
  }
  
  scheduler.Run();
}
```

В памяти выстроится следующая конструкция:

![Intrusive fibers](https://gitlab.com/Lipovsky/concurrency-course-media/-/raw/main/tasks/sched/intrusive/intrusive-fibers.png)

## Задание

1) Перепишите `RunLoop` и `ThreadPool` для работы с интрузивными задачами
2) Сделайте файберы интрузивными задачами и избавьтесь от динамических аллокаций памяти при их планировании
