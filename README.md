# Image Editor

Консольный редактор изображений на C++17. Загружает PNG, применяет одну или несколько операций обработки через текстовое меню и сохраняет результат.

## Возможности

- Преобразование в чёрно-белое (взвешенная яркость по формуле Y = 0.299R + 0.587G + 0.114B)
- Изменение яркости
- Цветовой фильтр (тинт) по отдельным коэффициентам R/G/B
- Размытие (box blur 3×3)
- Изменение разрешения — увеличение или уменьшение в целое число раз
- Инверсия цветов
- Повышение резкости (unsharp mask, ядро 3×3)
- Поворот на 90° по часовой стрелке

## Стек

- C++17, CMake (FetchContent)
- [stb_image](https://github.com/nothings/stb) — загрузка/сохранение PNG
- [doctest](https://github.com/doctest/doctest) — модульные тесты

## Сборка и запуск

```bash
git clone https://github.com/qqreyk/image_editor.git
cd image_editor
mkdir build && cd build
cmake ..
cmake --build .
```

CMake автоматически скачает stb и doctest при первой конфигурации.

Запуск:

```bash
./image_editor
# или сразу с путями:
./image_editor input.png output.png
```

## Тесты

```bash
cd build
ctest
# или напрямую:
./tests
```

## Структура проекта

```
.
├── CMakeLists.txt        — система сборки
├── .clang-format         — стиль форматирования кода
├── Doxyfile              — конфигурация Doxygen
├── src/
│   ├── image.h / .cpp    — класс Image (загрузка, сохранение, доступ к пикселям)
│   ├── filters.h / .cpp  — операции обработки изображений
│   └── main.cpp          — консольный интерфейс
└── tests/
    └── test_filters.cpp  — модульные тесты (doctest)
```

## Документация

Doxygen-документация по коду:

```bash
doxygen Doxyfile
open docs/html/index.html
```

## Требования

- Компилятор с поддержкой C++17 (GCC/Clang/MSVC)
- CMake ≥ 3.15
- Подключение к интернету при первой сборке (для FetchContent)
