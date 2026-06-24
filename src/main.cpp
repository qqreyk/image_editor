#include <iostream>
#include <limits>
#include <string>

#include "filters.h"
#include "image.h"

static void printMenu() {
    std::cout << "\n=== Операции ===\n"
              << "1. Преобразовать в чёрно-белое\n"
              << "2. Изменить яркость\n"
              << "3. Наложить цветовой фильтр\n"
              << "4. Размыть изображение\n"
              << "5. Изменить разрешение\n"
              << "6. Инвертировать цвета\n"
              << "7. Повысить резкость\n"
              << "8. Повернуть на 90° по часовой\n"
              << "0. Сохранить и выйти\n"
              << "Выберите операцию: ";
}

int main(int argc, char* argv[]) {
    std::string inputPath;
    std::string outputPath;

    if (argc >= 3) {
        inputPath = argv[1];
        outputPath = argv[2];
    } else {
        std::cout << "=== Консольный редактор изображений ===\n";
        std::cout << "Путь к входному файлу (PNG): ";
        std::getline(std::cin, inputPath);
        std::cout << "Путь к выходному файлу (PNG): ";
        std::getline(std::cin, outputPath);
    }

    try {
        Image img(inputPath);
        std::cout << "Изображение загружено: " << img.width() << "x" << img.height() << ", "
                  << img.channels() << " канал(а)\n";

        bool running = true;
        while (running) {
            printMenu();

            int choice = 0;
            if (!(std::cin >> choice)) {
                std::cin.clear();
                std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
                std::cout << "Некорректный ввод, попробуйте снова.\n";
                continue;
            }

            try {
                switch (choice) {
                    case 0:
                        running = false;
                        break;
                    case 1:
                        filters::toGrayscale(img);
                        std::cout << "Выполнено: преобразование в ЧБ.\n";
                        break;
                    case 2: {
                        int delta = 0;
                        std::cout << "Введите значение яркости (например, 30 или -30): ";
                        std::cin >> delta;
                        filters::adjustBrightness(img, delta);
                        std::cout << "Выполнено: яркость изменена на " << delta << ".\n";
                        break;
                    }
                    case 3: {
                        int r = 0, g = 0, b = 0;
                        std::cout << "Введите R G B фильтра (0-255), например: 255 100 100\n> ";
                        std::cin >> r >> g >> b;
                        filters::applyColorFilter(img, r, g, b);
                        std::cout << "Выполнено: цветовой фильтр применён.\n";
                        break;
                    }
                    case 4:
                        filters::blur(img);
                        std::cout << "Выполнено: размытие применено.\n";
                        break;
                    case 5: {
                        std::cout << "Введите кратность (>0 — увеличить, <0 — уменьшить).\n"
                                  << "Например: 2 (вдвое больше), -2 (вдвое меньше): ";
                        int factor = 0;
                        std::cin >> factor;
                        img = filters::resizeImage(img, factor);
                        std::cout << "Выполнено: новый размер " << img.width() << "x"
                                  << img.height() << ".\n";
                        break;
                    }
                    case 6:
                        filters::invertColors(img);
                        std::cout << "Выполнено: цвета инвертированы.\n";
                        break;
                    case 7:
                        filters::sharpen(img);
                        std::cout << "Выполнено: резкость повышена.\n";
                        break;
                    case 8:
                        img = filters::rotate90(img);
                        std::cout << "Выполнено: поворот на 90°. Новый размер: " << img.width()
                                  << "x" << img.height() << ".\n";
                        break;
                    default:
                        std::cout << "Неизвестная операция.\n";
                        break;
                }
            } catch (const std::exception& e) {
                std::cout << "Ошибка операции: " << e.what() << "\n";
            }
        }

        img.save(outputPath);
        std::cout << "Изображение сохранено: " << outputPath << "\n";

    } catch (const std::exception& e) {
        std::cerr << "Ошибка: " << e.what() << "\n";
        return 1;
    }

    return 0;
}
