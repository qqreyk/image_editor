cmake_minimum_required(VERSION 3.15)
project(ImageEditor VERSION 1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)

# --- stb (header-only, image load/write) ---
FetchContent_Declare(
    stb
    GIT_REPOSITORY https://github.com/nothings/stb.git
    GIT_TAG master
)
FetchContent_MakeAvailable(stb)

# --- doctest (unit testing framework) ---
# Используем FetchContent_Populate вместо MakeAvailable, чтобы не запускать
# CMakeLists.txt самого doctest (он несовместим с CMake 4.x)
FetchContent_Declare(
    doctest
    GIT_REPOSITORY https://github.com/doctest/doctest.git
    GIT_TAG v2.4.11
)
FetchContent_GetProperties(doctest)
if(NOT doctest_POPULATED)
    FetchContent_Populate(doctest)
    add_library(doctest::doctest INTERFACE IMPORTED)
    target_include_directories(doctest::doctest INTERFACE ${doctest_SOURCE_DIR})
endif()

# --- shared library (used by both app and tests) ---
add_library(image_editor_lib
    src/image.cpp
    src/filters.cpp
)
target_include_directories(image_editor_lib PUBLIC
    ${stb_SOURCE_DIR}
    src
)

# --- main executable ---
add_executable(image_editor src/main.cpp)
target_link_libraries(image_editor PRIVATE image_editor_lib)

# --- tests ---
enable_testing()

add_executable(tests tests/test_filters.cpp)
target_link_libraries(tests PRIVATE image_editor_lib doctest::doctest)
target_include_directories(tests PRIVATE ${stb_SOURCE_DIR} src)

add_test(NAME image_editor_tests COMMAND tests)
