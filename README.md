# No Title Cellular Automata Project

> **Vibe-coded** — этот проект целиком написан с помощью нейросети.

**Проверено человеком ✅**

Крайнюков Георгий <nrk63@yandex.ru>

26.03.2026

## Что такое клеточный автомат?

**Клеточный автомат** — это дискретная вычислительная модель, состоящая из регулярной сетки ячеек (клеток), каждая из которых может находиться в одном из конечного числа состояний. На каждом шаге по времени состояние каждой клетки обновляется одновременно в соответствии с набором локальных правил, зависящих от текущего состояния клетки и состояний её соседей.

Несмотря на простоту определения, клеточные автоматы способны порождать чрезвычайно сложное и непредсказуемое поведение — от самоорганизующихся структур до полноценной вычислительной универсальности (Тьюринг-полнота).

Классические примеры:

- **Conway's Game of Life** — двумерный автомат с правилами рождения и выживания (B3/S23)

## Сборка проекта

### Windows

Установить Qt + MinGW (потому что msvc не поддерживает coverage).

Добавить в Path:
- ...\Qt\6.x.x\mingw_64\bin (windeployqt.exe)
- ...\Qt\Tools\mingw<ver>_64\bin (g++ и т.п.)
- ...\Qt\Tools\CMake_64\bin (по желанию)

Далее конфигурируем и запускаем сборку:

```cmake
cmake -S . -B build -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH=D:/Qt/6.x.x/mingw_64
cmake --build build --parallel
windeployqt build/ntcap-gui.exe
```

## Настройка ИИ-агента

### Zoo Code

В целях экономии $$$ был создан отдельный режим `researcher` (см. `.roomodes`).

Перед работой необходимо настроить два профиля во вкладке `Providers`:
- flash (одна тупая)
- pro (одна умная)

И расставить для режимов значения поля `API Configuration` следующим образом:

| Mode           | API Configuration |
|----------------|-------------------|
| `architect`    | `pro`             |
| `code`         | `flash`           |
| `ask`          | `flash`           |
| `debug`        | `pro`             |
| `orchestrator` | `flash`           |
| `researcher`   | `flash`           |

## Contributing

*TBD*

## License

This project is licensed under the terms of the GNU General Public License v3.0. See the LICENSE file for details.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
