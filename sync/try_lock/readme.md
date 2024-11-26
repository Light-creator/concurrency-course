# TryLock для TicketLock

Реализуйте метод `TryLock` для [`TicketLock`](ticket_lock.hpp).

Семантика `TryLock`:

* Если спинлок свободен, то захватить его **без ожидания** и вернуть `true`
* Если спинлок захвачен другим потоком, и текущему потоку нужно ждать освобождения блокировки, то **без ожидания** вернуть `false`

Если вызов `TryLock()` вернул `true`, то поток захватил спинлок и находится в критической секции.

Каждый поток, прошедший через `Lock` или успешный `TryLock`, должен получать от спинлока уникальный порядковый номер.

Вызов метода `TryLock` должен завершаться за конечное число шагов, которое не зависит от числа потоков, разрядности машинного слова и т.п. В частности, в реализации `TryLock` нельзя вызывать метод `Lock`: число итераций в цикле ожидания в `Lock` зависит от числа потоков в очереди перед нами.

Неудачные попытки `TryLock` не должны приводить к вечным блокировкам вызовов `Lock`.

Изучите также гарантии [`try_lock` у `std::mutex`](https://en.cppreference.com/w/cpp/thread/mutex/try_lock).

---

В решении вы можете использовать любые атомарные RMW-операции, которые есть у [std::atomic](https://en.cppreference.com/w/cpp/atomic/atomic).

Реализацию методов `Lock` и `Unlock` менять нельзя.

## Формализация

Попробуйте описать семантику `TryLock` более формально. 

Что значит «Если спинлок свободен»? Про какой именно момент идет речь? 

## Weak MM

[После формализации семантики `TryLock`]

Может ли `TryLock` соврать пользователю, что спинлок захвачен, хотя на самом деле он свободен?

Какие гарантии дает [`try_lock`](https://en.cppreference.com/w/cpp/thread/mutex/try_lock) у `std::mutex`?

Почему так? Ответ – сложный: [Foundations of the C++ Concurrency Memory Model](https://www.hpl.hp.com/techreports/2008/HPL-2008-56.pdf)