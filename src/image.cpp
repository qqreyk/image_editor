#include "image.h"

// Реализация stb_image подключается ровно один раз здесь
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image_write.h>

Image::Image(const std::string& path) {
    int w = 0, h = 0, c = 0;
    unsigned char* raw = stbi_load(path.c_str(), &w, &h, &c, 0);
    if (!raw) {
        std::string reason = stbi_failure_reason() ? stbi_failure_reason() : "неизвестно";
        throw std::runtime_error("Не удалось открыть файл: " + path + "\nПричина: " + reason);
    }
    m_width = w;
    m_height = h;
    m_channels = c;
    // Копируем данные в вектор, затем освобождаем буфер stb
    m_data.assign(raw, raw + static_cast<size_t>(w * h * c));
    stbi_image_free(raw);
}

Image::Image(int width, int height, int channels) {
    if (width <= 0 || height <= 0 || channels <= 0) {
        throw std::invalid_argument("Параметры изображения должны быть положительными");
    }
    m_width = width;
    m_height = height;
    m_channels = channels;
    m_data.assign(static_cast<size_t>(width * height * channels), 0);
}

void Image::save(const std::string& path) const {
    int result = stbi_write_png(path.c_str(), m_width, m_height, m_channels, m_data.data(),
                                m_width * m_channels);
    if (result == 0) {
        throw std::runtime_error("Не удалось сохранить файл: " + path);
    }
}

int Image::width() const { return m_width; }

int Image::height() const { return m_height; }

int Image::channels() const { return m_channels; }

bool Image::checkBounds(int x, int y, int channel) const {
    return x >= 0 && x < m_width && y >= 0 && y < m_height && channel >= 0 && channel < m_channels;
}

unsigned char& Image::at(int x, int y, int channel) {
    if (!checkBounds(x, y, channel)) {
        throw std::out_of_range("Координаты пикселя вне границ изображения");
    }
    return m_data[static_cast<size_t>((y * m_width + x) * m_channels + channel)];
}

unsigned char Image::at(int x, int y, int channel) const {
    if (!checkBounds(x, y, channel)) {
        throw std::out_of_range("Координаты пикселя вне границ изображения");
    }
    return m_data[static_cast<size_t>((y * m_width + x) * m_channels + channel)];
}
