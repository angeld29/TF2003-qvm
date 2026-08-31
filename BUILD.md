# Сборка TF2003-qvm

Мод собирается в два артефакта:

- **Native PR2 shared-либа** `qwprogs` — `qwprogs.so` (Linux) / `qwprogs.dll` (Windows), `sv_progtype 1/3`;
- **QVM-байткод** `qwprogs.qvm` — платформонезависимый, `sv_progtype 2`.

Запуск на серверах mvdsv / qwsv262 — см. `README`.

## Требования

- `make`, `cmake >= 3.4`, `ninja-build`.
- Компиляторы под нужные платформы (см. таблицу).

| Платформа | Компилятор | Пакет (Ubuntu/Debian) |
|---|---|---|
| linux-amd64 | `gcc` | `build-essential` (хост) |
| linux-i686 | `x86_64-linux-gnu-gcc` (`-m32`) | `gcc-multilib g++-multilib` + `linux-libc-dev:i386` |
| linux-armhf | `arm-linux-gnueabihf-gcc` | `gcc-arm-linux-gnueabihf` |
| windows-x64 | `x86_64-w64-mingw32-gcc` | `gcc-mingw-w64-x86-64` |
| windows-x86 | `i686-w64-mingw32-gcc` | `gcc-mingw-w64-i686` |

Кросс-g++ не нужен — компилируется только C. Тулзы `q3lcc`/`q3asm` собираются автоматически из дерева (`tools/`).

## Сборка через make

Всё собирается в каталог `_cmake/` (игнорируется git).

| Команда | Что собирает | Результат |
|---|---|---|
| `make` / `make qvm` | QVM (по умолчанию) | `_cmake/qvm/qwprogs.qvm` |
| `make current` | native под текущий хост, без кросс-тулчейна | `_cmake/current/qwprogs.so` |
| `make <платформа>` | одна платформа | `_cmake/<platform>/qwprogs.so` / `qwprogs.dll` |
| `make all` | все 5 платформ + QVM (релиз) | все артефакты |
| `make clean` | удалить `_cmake/` | — |
| `make help` | список целей | — |

`<платформа>` — `linux-amd64 | linux-armhf | linux-i686 | windows-x64 | windows-x86`.

Примеры:

```bash
make            # только QVM
make current    # нативный билд под текущую машину
make windows-x64
make all        # полная сборка (релиз)
```

После изменения списка исходников в `CMakeLists.txt` `make qvm` сам переконфигурирует проект (генерирует список модулей `game.q3asm`).

## Ручная сборка через cmake (без make)

```bash
# QVM
cmake -S . -B _cmake/qvm -G Ninja
cmake --build _cmake/qvm --target qvm

# native под текущий хост
cmake -S . -B _cmake/current
cmake --build _cmake/current

# одна платформа
cmake -S . -B _cmake/<platform> -G Ninja -DCMAKE_TOOLCHAIN_FILE=tools/cross-cmake/<platform>.cmake
cmake --build _cmake/<platform> --config Release
```

## Артефакты и установка

- `qwprogs.qvm` → каталог протегов сервера, `sv_progtype 2`;
- `qwprogs.so` / `qwprogs.dll` → каталог протегов сервера, `sv_progtype 1` (или 3).

## Замечания

- Новый исходник добавляется в `SOURCE_LIB` в `CMakeLists.txt` — этого достаточно и для native, и для QVM (список модулей генерируется автоматически).
- QVM-ограничения компилятора (нет `double`, особенности `switch`, типы и т.п.) — см. `README`.
