# RootFindingLab – Лабораторна робота №7 (варіант 2)

Модернізація статичної бібліотеки для розв’язання нелінійних рівнянь із повною підтримкою механізму винятків, гарантій безпеки та модульного тестування.

## Структура проєкту

```
RootFindingLab/
├── CMakeLists.txt
├── vcpkg.json
├── README.md
├── include/
│   ├── RootExceptions.h      # ієрархія винятків
│   ├── RootFindingTask.h     # основний клас
│   └── ScopeLogger.h         # RAII-логер
├── src/
│   └── RootFindingTask.cpp
├── demo/
│   └── main.cpp              # демонстрація
├── antipattern/
│   └── main.cpp              # антипатерни
└── tests/
    └── RootFindingTests.cpp  # Google Test
```

## Інваріанти класу `RootFindingTask`

Для кожного коректного об’єкта завжди виконується:

- `a_` і `b_` – скінченні числа (`std::isfinite`);
- `a_ < b_`;
- `eps_ > 0` і скінченне;
- `max_iterations_ > 0`.

Некоректний стан ніколи не доступний через публічний інтерфейс: усі перевірки виконуються до зміни членів класу.

## Ієрархія винятків

```
std::runtime_error
 └── RootException
      ├── InvalidIntervalException   (parameter, value)
      ├── NoSignChangeException
      ├── ZeroDerivativeException    (x)
      ├── ZeroDenominatorException
      └── NoConvergenceException     (max_iterations)
```

Додатково використовуються стандартні типи:

- `std::invalid_argument` – неправильний порядок меж, нульова точність, нульова кількість ітерацій;
- `std::overflow_error` – поява нескінченних значень у ітераціях.

## Сценарії помилок (не менше 5)

1. Недопустимий інтервал (a ≥ b або NaN/Inf) → `InvalidIntervalException` / `std::invalid_argument`
2. Відсутність зміни знака (бісекція) → `NoSignChangeException`
3. Нульова похідна (Ньютон) → `ZeroDerivativeException`
4. Нульовий знаменник (метод січних) → `ZeroDenominatorException`
5. Відсутність збіжності → `NoConvergenceException`

## Гарантії безпеки щодо винятків

| Метод                    | Гарантія      | Коментар |
|--------------------------|---------------|----------|
| конструктор              | strong        | об’єкт або повністю створений, або виняток |
| `setInterval`            | strong        | validate → assign |
| `setTolerance`           | strong        | validate → assign |
| `setMaxIterations`       | strong        | validate → assign |
| `bisection` / `newton` / `secant` | basic | не змінюють стан об’єкта |
| `computeAndStoreBoth`    | **strong**    | prepare-then-commit: спочатку обчислюємо обидва корені у тимчасові змінні, лише після успіху обох оновлюємо `last_*` |
| спостерігачі            | noexcept      | |

Деструктори усіх класів позначені `noexcept` (у т.ч. `ScopeLogger`).

## Дослідження розкручування стека (ScopeLogger)

Очікувана послідовність повідомлень при поширенні винятку через три рівні:

```
[ENTER] level1
[ENTER] level2
[ENTER] level3
Creating task with invalid interval...
[EXIT ] level3
[LOG] level2 caught: ...
[EXIT ] level2
[EXIT ] level1
[MAIN] Exception: ...
Program continues normally.
```

Фактичний результат після запуску `RootFindingDemo` повинен збігатися з очікуваним (порядок знищення локальних об’єктів – зворотний до порядку створення).

## Антипатерни

Файл `antipattern/main.cpp` демонструє:

- `throw 42` і `throw std::string(...)` **не** перехоплюються `catch (const std::exception&)`;
- `throw ex;` (замість `throw;`) призводить до object slicing.

**Висновок:** для бібліотек C++ слід використовувати ієрархію, похідну від `std::exception` (або `std::runtime_error`), щоб забезпечити єдиний інтерфейс `what()` і централізовану обробку.

## Збирання (Visual Studio + vcpkg)

1. Відкрити папку як CMake-проєкт.
2. У терміналі проєкту:
   ```
   vcpkg x-update-baseline --add-initial-baseline
   ```
3. Налаштувати `CMakePresets.json` з шляхом до `vcpkg.cmake`.
4. Зібрати цілі `RootFindingLib`, `RootFindingDemo`, `AntipatternDemo`, `RootFindingTests`.
5. Запустити тести через Test Explorer або `ctest`.

## Аналіз результатів

- Власна ієрархія дозволяє передавати структуровані діагностичні дані (параметр, значення, кількість ітерацій).
- Інваріанти захищені перевірками у конструкторі та мутаторах до будь-якої зміни стану.
- Сильна гарантія `computeAndStoreBoth` реалізована класичним prepare-then-commit.
- Розкручування стека коректно знищує локальні `ScopeLogger` у зворотному порядку.
- Модульні тести покривають успішні обчислення, усі сценарії помилок, граничні значення та сильну гарантію.
- Винятки + RAII + автоматизовані тести суттєво підвищують надійність чисельних бібліотек.
