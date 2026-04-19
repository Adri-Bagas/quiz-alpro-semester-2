cmake_minimum_required(VERSION 3.10)
project(NcursesApp)

include(FetchContent)

FetchContent_Declare(
    json
    URL https://github.com/nlohmann/json/releases/download/v3.12.0/json.tar.xz
    DOWNLOAD_EXTRACT_TIMESTAMP TRUE
)

FetchContent_MakeAvailable(json)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)

find_package(Curses REQUIRED)

file(GLOB_RECURSE SOURCES "src/**/*.cpp")

add_executable(bookstore main.cpp ${SOURCES})

target_include_directories(bookstore PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(bookstore PRIVATE nlohmann_json::nlohmann_json ${CURSES_LIBRARIES} menu)