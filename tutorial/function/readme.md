# Function

Реализуйте [`UniqueFunction`](function.hpp) – контейнер для функции, стирающий ее тип.

Для простоты `UniqueFunction` будет поддерживать только функции без аргументов и возвращаемого значения.

```cpp
int x = 1;
int y = 2;

UniqueFunction f([x, y] {
  fmt::println("{}", x + y);
});

UniqueFunction g([x] {
  fmt::println("{}", x);
});

f();  // Печатает 3
g();  // Печатает 1
```
