#include "filters.h"

#include <algorithm>
#include <stdexcept>

namespace filters {

static unsigned char clamp(int value) {
    if (value < 0) return 0;
    if (value > 255) return 255;
    return static_cast<unsigned char>(value);
}

void toGrayscale(Image& img) {
    if (img.channels() < 3) {
        throw std::invalid_argument("Для преобразования в ЧБ нужно минимум 3 канала");
    }
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            // формула взвешенной яркости (luma)
            int luma = static_cast<int>(0.299 * img.at(x, y, 0) + 0.587 * img.at(x, y, 1) +
                                        0.114 * img.at(x, y, 2));
            img.at(x, y, 0) = clamp(luma);
            img.at(x, y, 1) = clamp(luma);
            img.at(x, y, 2) = clamp(luma);
        }
    }
}

void adjustBrightness(Image& img, int delta) {
    int rgbChannels = std::min(img.channels(), 3);
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            for (int c = 0; c < rgbChannels; ++c) {
                img.at(x, y, c) = clamp(static_cast<int>(img.at(x, y, c)) + delta);
            }
        }
    }
}

void applyColorFilter(Image& img, int filterR, int filterG, int filterB) {
    if (filterR < 0 || filterR > 255 || filterG < 0 || filterG > 255 || filterB < 0 ||
        filterB > 255) {
        throw std::invalid_argument("Значения цветового фильтра должны быть в диапазоне [0, 255]");
    }
    if (img.channels() < 3) {
        throw std::invalid_argument("Для цветового фильтра нужно минимум 3 канала");
    }
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            img.at(x, y, 0) = clamp(img.at(x, y, 0) * filterR / 255);
            img.at(x, y, 1) = clamp(img.at(x, y, 1) * filterG / 255);
            img.at(x, y, 2) = clamp(img.at(x, y, 2) * filterB / 255);
        }
    }
}

void blur(Image& img) {
    // создаём копию для чтения исходных значений
    Image copy = img;
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            for (int c = 0; c < img.channels(); ++c) {
                int sum = 0;
                int count = 0;
                for (int dy = -1; dy <= 1; ++dy) {
                    for (int dx = -1; dx <= 1; ++dx) {
                        int nx = x + dx;
                        int ny = y + dy;
                        if (nx >= 0 && nx < img.width() && ny >= 0 && ny < img.height()) {
                            sum += copy.at(nx, ny, c);
                            ++count;
                        }
                    }
                }
                img.at(x, y, c) = clamp(sum / count);
            }
        }
    }
}

Image resizeImage(Image img, int factor) {
    if (factor == 0) {
        throw std::invalid_argument("Кратность не может быть равна нулю");
    }

    if (factor > 0) {
        // увеличение: каждый пиксель становится блоком factor×factor
        Image result(img.width() * factor, img.height() * factor, img.channels());
        for (int y = 0; y < img.height(); ++y) {
            for (int x = 0; x < img.width(); ++x) {
                for (int dy = 0; dy < factor; ++dy) {
                    for (int dx = 0; dx < factor; ++dx) {
                        for (int c = 0; c < img.channels(); ++c) {
                            result.at(x * factor + dx, y * factor + dy, c) = img.at(x, y, c);
                        }
                    }
                }
            }
        }
        return result;
    } else {
        // уменьшение: блок |factor|×|factor| усредняется в один пиксель
        int f = -factor;
        if (img.width() % f != 0 || img.height() % f != 0) {
            throw std::invalid_argument(
                "Размеры изображения должны делиться на кратность уменьшения");
        }
        Image result(img.width() / f, img.height() / f, img.channels());
        for (int y = 0; y < result.height(); ++y) {
            for (int x = 0; x < result.width(); ++x) {
                for (int c = 0; c < img.channels(); ++c) {
                    int sum = 0;
                    for (int dy = 0; dy < f; ++dy) {
                        for (int dx = 0; dx < f; ++dx) {
                            sum += img.at(x * f + dx, y * f + dy, c);
                        }
                    }
                    result.at(x, y, c) = clamp(sum / (f * f));
                }
            }
        }
        return result;
    }
}

void invertColors(Image& img) {
    int rgbChannels = std::min(img.channels(), 3);
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            for (int c = 0; c < rgbChannels; ++c) {
                img.at(x, y, c) = static_cast<unsigned char>(255 - img.at(x, y, c));
            }
        }
    }
}

void sharpen(Image& img) {
    // ядро резкости 3×3: центр=5, соседи по осям=-1, диагонали=0
    const int kernel[3][3] = {{0, -1, 0}, {-1, 5, -1}, {0, -1, 0}};
    Image copy = img;
    for (int y = 1; y < img.height() - 1; ++y) {
        for (int x = 1; x < img.width() - 1; ++x) {
            for (int c = 0; c < img.channels(); ++c) {
                int value = 0;
                for (int ky = 0; ky < 3; ++ky) {
                    for (int kx = 0; kx < 3; ++kx) {
                        value += kernel[ky][kx] * copy.at(x + kx - 1, y + ky - 1, c);
                    }
                }
                img.at(x, y, c) = clamp(value);
            }
        }
    }
}

Image rotate90(Image img) {
    // поворот на 90° по часовой: dst(x, y) = src(y, width-1-x)
    Image result(img.height(), img.width(), img.channels());
    for (int y = 0; y < img.height(); ++y) {
        for (int x = 0; x < img.width(); ++x) {
            for (int c = 0; c < img.channels(); ++c) {
                result.at(y, img.width() - 1 - x, c) = img.at(x, y, c);
            }
        }
    }
    return result;
}

}  // namespace filters
