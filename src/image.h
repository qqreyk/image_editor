#pragma once

#include <stdexcept>
#include <string>
#include <vector>

/**
 * @brief Класс для хранения и работы с пиксельным изображением.
 *
 * Загружает PNG-файл с помощью библиотеки stb_image,
 * хранит пиксели в std::vector<unsigned char>.
 * Каналы хранятся в порядке R, G, B[, A] для каждого пикселя
 * построчно слева направо, сверху вниз.
 */
class Image {
   public:
    /**
     * @brief Загружает изображение из файла.
     * @param path Путь к PNG-файлу.
     * @throws std::runtime_error Если файл не найден или повреждён.
     */
    explicit Image(const std::string& path);

    /**
     * @brief Создаёт пустое изображение с заданными параметрами.
     * @param width  Ширина в пикселях (> 0).
     * @param height Высота в пикселях (> 0).
     * @param channels Количество каналов: 1 (серый), 3 (RGB), 4 (RGBA).
     * @throws std::invalid_argument Если параметры некорректны.
     */
    Image(int width, int height, int channels);

    /**
     * @brief Сохраняет изображение в PNG-файл.
     * @param path Путь к выходному файлу.
     * @throws std::runtime_error Если файл не удалось записать.
     */
    void save(const std::string& path) const;

    /** @return Ширина изображения в пикселях. */
    int width() const;

    /** @return Высота изображения в пикселях. */
    int height() const;

    /** @return Количество каналов (1, 3 или 4). */
    int channels() const;

    /**
     * @brief Возвращает ссылку на значение одного канала пикселя.
     * @param x       Столбец (0 ≤ x < width).
     * @param y       Строка  (0 ≤ y < height).
     * @param channel Номер канала (0-based).
     * @throws std::out_of_range Если координаты вне границ.
     */
    unsigned char& at(int x, int y, int channel);

    /**
     * @brief Константная версия at().
     * @param x       Столбец.
     * @param y       Строка.
     * @param channel Номер канала.
     * @return Значение канала.
     * @throws std::out_of_range Если координаты вне границ.
     */
    unsigned char at(int x, int y, int channel) const;

   private:
    int m_width;
    int m_height;
    int m_channels;
    std::vector<unsigned char> m_data;

    /**
     * @brief Проверяет, находятся ли координаты в границах изображения.
     * @param x       Столбец.
     * @param y       Строка.
     * @param channel Номер канала.
     * @return true, если координаты корректны, иначе false.
     */
    bool checkBounds(int x, int y, int channel) const;
};
