#  A+B

Реализуйте метод `ComputeAnswer` класса `DeepThought` в файле [`deep_thought.hpp`](deep_thought.hpp).

Метод должен вычислять [ответ на главный вопрос жизни, Вселенной и всего остального](https://en.wikipedia.org/wiki/42_(number)#The_Hitchhiker's_Guide_to_the_Galaxy). 

## 📎 Clippy 

[Справочник команд / работа с задачей](https://gitlab.com/Lipovsky/clippy/-/blob/master/docs/commands.md#%D1%83%D1%80%D0%BE%D0%B2%D0%B5%D0%BD%D1%8C-%D0%B7%D0%B0%D0%B4%D0%B0%D1%87%D0%B8)

Для запуска тестов выполните в директории с задачей команду `clippy test`.

По умолчанию команда `test` выполняет команду `censor` перед запуском тестов.
Шаг `censor` можно отключить с помощью флага `--no-censor`.

Для запуска конкретной тестовой цели CMake выполните в директории с задачей команду `clippy target {target-name} {build-profile-name}`.

Для автоматического форматирования кода задачи с помощью [`clang-format`](https://clang.llvm.org/docs/ClangFormat.html) выполните команду `clippy format`. Конфиг для `clang-format` находится в корне репозитория в файле [`.clang-format`](/.clang-format).

Для проверки кода с помощью линтера [`clang-tidy`](https://clang.llvm.org/extra/clang-tidy/) выполните команду `clippy tidy`. Конфиг для `clang-tidy` находится в корне репозитория в файле [`.clang-tidy`](/.clang-tidy).

Разрешается менять только файлы / директории, перечисленные в поле `submit_files` конфига задачи [`task.json`](task.json). Только эти файлы, как следует из названия поля, будут отправлены на проверку.

В CI на решении запускается команда `clippy validate` для проверки качества кода и запрещенных паттернов.
