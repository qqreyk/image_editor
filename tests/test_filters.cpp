#define DOCTEST_CONFIG_IMPLEMENT_WITH_MAIN
#include <doctest/doctest.h>

#include "filters.h"
#include "image.h"

static Image makeSolidImage(int w, int h, unsigned char r, unsigned char g, unsigned char b) {
    Image img(w, h, 3);
    for (int y = 0; y < h; ++y) {
        for (int x = 0; x < w; ++x) {
            img.at(x, y, 0) = r;
            img.at(x, y, 1) = g;
            img.at(x, y, 2) = b;
        }
    }
    return img;
}

TEST_CASE("toGrayscale: красный пиксель становится серым") {
    Image img = makeSolidImage(2, 2, 255, 0, 0);
    filters::toGrayscale(img);
    CHECK(img.at(0, 0, 0) == img.at(0, 0, 1));
    CHECK(img.at(0, 0, 1) == img.at(0, 0, 2));
}

TEST_CASE("toGrayscale: белый остаётся белым") {
    Image img = makeSolidImage(1, 1, 255, 255, 255);
    filters::toGrayscale(img);
    CHECK(img.at(0, 0, 0) == 255);
    CHECK(img.at(0, 0, 1) == 255);
    CHECK(img.at(0, 0, 2) == 255);
}

TEST_CASE("toGrayscale: чёрный остаётся чёрным") {
    Image img = makeSolidImage(1, 1, 0, 0, 0);
    filters::toGrayscale(img);
    CHECK(img.at(0, 0, 0) == 0);
    CHECK(img.at(0, 0, 1) == 0);
    CHECK(img.at(0, 0, 2) == 0);
}

TEST_CASE("toGrayscale: выбрасывает исключение для 1-канального изображения") {
    Image img(2, 2, 1);
    CHECK_THROWS_AS(filters::toGrayscale(img), std::invalid_argument);
}

TEST_CASE("adjustBrightness: увеличение яркости") {
    Image img = makeSolidImage(1, 1, 100, 100, 100);
    filters::adjustBrightness(img, 50);
    CHECK(img.at(0, 0, 0) == 150);
    CHECK(img.at(0, 0, 1) == 150);
    CHECK(img.at(0, 0, 2) == 150);
}

TEST_CASE("adjustBrightness: уменьшение яркости") {
    Image img = makeSolidImage(1, 1, 100, 100, 100);
    filters::adjustBrightness(img, -50);
    CHECK(img.at(0, 0, 0) == 50);
}

TEST_CASE("adjustBrightness: ограничение сверху — не выходит за 255") {
    Image img = makeSolidImage(1, 1, 200, 200, 200);
    filters::adjustBrightness(img, 100);
    CHECK(img.at(0, 0, 0) == 255);
}

TEST_CASE("adjustBrightness: ограничение снизу — не выходит за 0") {
    Image img = makeSolidImage(1, 1, 50, 50, 50);
    filters::adjustBrightness(img, -200);
    CHECK(img.at(0, 0, 0) == 0);
}

TEST_CASE("invertColors: инверсия белого даёт чёрный") {
    Image img = makeSolidImage(1, 1, 255, 255, 255);
    filters::invertColors(img);
    CHECK(img.at(0, 0, 0) == 0);
    CHECK(img.at(0, 0, 1) == 0);
    CHECK(img.at(0, 0, 2) == 0);
}

TEST_CASE("invertColors: инверсия чёрного даёт белый") {
    Image img = makeSolidImage(1, 1, 0, 0, 0);
    filters::invertColors(img);
    CHECK(img.at(0, 0, 0) == 255);
}

TEST_CASE("invertColors: двойная инверсия возвращает исходное") {
    Image img = makeSolidImage(2, 2, 120, 80, 200);
    filters::invertColors(img);
    filters::invertColors(img);
    CHECK(img.at(0, 0, 0) == 120);
    CHECK(img.at(0, 0, 1) == 80);
    CHECK(img.at(0, 0, 2) == 200);
}

TEST_CASE("resizeImage: увеличение в 2 раза") {
    Image img = makeSolidImage(4, 4, 100, 150, 200);
    Image result = filters::resizeImage(img, 2);
    CHECK(result.width() == 8);
    CHECK(result.height() == 8);
    CHECK(result.at(0, 0, 0) == 100);
}

TEST_CASE("resizeImage: уменьшение в 2 раза") {
    Image img = makeSolidImage(4, 4, 80, 80, 80);
    Image result = filters::resizeImage(img, -2);
    CHECK(result.width() == 2);
    CHECK(result.height() == 2);
}

TEST_CASE("resizeImage: нулевой factor вызывает исключение") {
    Image img = makeSolidImage(4, 4, 0, 0, 0);
    CHECK_THROWS_AS(filters::resizeImage(img, 0), std::invalid_argument);
}

TEST_CASE("resizeImage: некратное уменьшение вызывает исключение") {
    Image img = makeSolidImage(3, 3, 0, 0, 0);
    CHECK_THROWS_AS(filters::resizeImage(img, -2), std::invalid_argument);
}

TEST_CASE("rotate90: размеры меняются местами") {
    Image img = makeSolidImage(6, 4, 0, 0, 0);
    Image result = filters::rotate90(img);
    CHECK(result.width() == 4);
    CHECK(result.height() == 6);
}

TEST_CASE("rotate90: четыре поворота дают исходное изображение") {
    Image img = makeSolidImage(4, 4, 200, 100, 50);
    img.at(0, 0, 0) = 10;
    img.at(1, 0, 1) = 20;
    Image r = filters::rotate90(filters::rotate90(filters::rotate90(filters::rotate90(img))));
    CHECK(r.at(0, 0, 0) == 10);
    CHECK(r.at(1, 0, 1) == 20);
}

TEST_CASE("applyColorFilter: красный фильтр обнуляет G и B") {
    Image img = makeSolidImage(1, 1, 200, 200, 200);
    filters::applyColorFilter(img, 255, 0, 0);
    CHECK(img.at(0, 0, 0) == 200);
    CHECK(img.at(0, 0, 1) == 0);
    CHECK(img.at(0, 0, 2) == 0);
}

TEST_CASE("applyColorFilter: некорректный параметр вызывает исключение") {
    Image img = makeSolidImage(1, 1, 100, 100, 100);
    CHECK_THROWS_AS(filters::applyColorFilter(img, 300, 0, 0), std::invalid_argument);
    CHECK_THROWS_AS(filters::applyColorFilter(img, 0, -1, 0), std::invalid_argument);
}
