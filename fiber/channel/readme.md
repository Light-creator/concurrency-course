# Каналы

## Пререквизиты

- [fiber/mutex](/tasks/fiber/mutex)
- [для лок-фри реализации] [lf/hazard_ptr](/tasks/lf/hazard_ptr)

## Go Proverb

> _Do not communicate by sharing memory; instead, share memory by communicating._

[Share Memory By Communicating](https://go.dev/blog/codelab-share)

## Канал

Канал – линия коммуникации, по которой файберы передают друг другу данные:
- отправляют значения (операция `Send`) и
- получают отправленные значения (операция `Recv`).


### Buffered

[A Tour of Go / Buffered Channels](https://go.dev/tour/concurrency/3)

[`BufferedChannel<T>`](exe/fiber/sync/channel/buffered.hpp) – MPMC FIFO канал с буфером фиксированного размера.

Метод `Send` останавливает файбер если буфер канала заполнен, метод `Recv` – если пуст.

Чтобы упростить сигнатуры методов, мы обойдемся без метода `Close`. 

#### Пример

```cpp
void ChannelExample() {
  using namespace exe;
  
  sched::ThreadPool pool{4};
  pool.Start();
  
  thread::WaitGroup wg;
  
  wg.Add(2);
  
  fiber::BufferedChannel<int> messages{4};

  // Захватываем канал по значению, владение каналом – разделяемое
  fiber::Go(pool, [&wg, messages] mutable {
    // Producer
    for (int i = 0; i < 128; ++i) {
      messages.Send(i);
    }
    
    // Отправляем poison pill
    messages.Send(-1);
    
    wg.Done();
  });  

  fiber::Go(pool, [&wg, messages] mutable {
    // Consumer
    while (true) {
      int v = messages.Recv();
      if (v == -1) {
        break;  // Poison pill
      }
      fmt::println("Recv -> {}", v);
    }
    
    wg.Done();
  });

  wg.Wait();
  
  pool.Stop();
}
```

### `Rendezvous`

[A Tour of Go / (Rendezvous) Channels ](https://go.dev/tour/concurrency/2)

В `RendezvousChannel` нет внутреннего буфера для данных, продьюсер завершает вызов `Send` / консьюмер завершает вызов `Recv` когда состоялось рандеву с симметричной операцией.

Паттерн доступа – MPMC, порядок доставки сообщений – FIFO.

## Реализация

### Синхронизация

Для синхронизации продьюсеров и консьюмеров используйте спинлок (так делают в Golang). Лок-фри реализация не требуется.

### Рандеву

Если в канале есть ждущие консьюмеры, то в `Send` передавайте сообщение консьюмеру напрямую, со стека на стек, минуя буфер канала.

### Аллокации

Методы `Send` / `Recv` не должны выполнять динамических аллокаций памяти:

- Для списка остановленных продьюсеров / консьюмеров используйте интрузивные списки
- (Опционально) Для буфера сообщений используйте циклический буфер

### Симметрия

В канале не могут одновременно ждать и продьюсеры, и консьюмеры.

Реализуйте канал так, чтобы и продьюсеры, и консьюмеры хранились в одной очереди _операций_.

Это пригодится и в лок-фри реализации, и для поддержки `Select`.

## Лок-фри

((очень) продвинутый уровень)

- Статья: [Fast and Scalable Channels in Kotlin Coroutines](https://arxiv.org/abs/2211.04986)
- Реализация: [Fast and scalable channels algorithm](https://github.com/Kotlin/kotlinx.coroutines/issues/3621)

Напишите лок-фри реализации `RendezvousChannel` и `BufferedChannel`.

## Задание

1) Напишите `BufferedChannel`
2) (опционально) Напишите `RendezvousChannel`
3) (опционально) Напишите лок-фри `RendezvousChannel`
4) (опционально) Напишите лок-фри `BufferedChannel`
