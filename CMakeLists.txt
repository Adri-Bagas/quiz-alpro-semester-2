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

find_package(Curses QUIET)

if(NOT CURSES_FOUND)
    message(FATAL_ERROR 
        "\n"
        "====================================================================\n"
        " ERROR: Ncurses library is missing!\n"
        " Ncurses is a system library and cannot be downloaded automatically.\n"
        " \n"
        " Please install it using your operating system's package manager:\n"
        "   Ubuntu/Debian   : sudo apt-get install libncurses5-dev libncursesw5-dev\n"
        "   Fedora/openSUSE : sudo dnf install ncurses-devel\n"
        "   Arch Linux      : sudo pacman -S ncurses\n"
        "   macOS (Homebrew): brew install ncurses\n"
        "   Windows         : via MSYS2 or just install linux man...\n"
        "====================================================================\n"
    )
endif()

find_library(MENU_LIBRARY NAMES menu menuw)
if(NOT MENU_LIBRARY)
    message(FATAL_ERROR "Ncurses 'menu' library not found! Please ensure you installed the full ncurses development package.")
endif()

file(GLOB_RECURSE SOURCES "src/**/*.cpp")

add_executable(bookstore main.cpp ${SOURCES})

target_include_directories(bookstore PRIVATE ${CURSES_INCLUDE_DIRS})
target_link_libraries(bookstore PRIVATE nlohmann_json::nlohmann_json ${CURSES_LIBRARIES} menu)