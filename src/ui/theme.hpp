#pragma once
#include <ncurses.h>

namespace BorderTheme {
    // 'inline' prevents multiple definition errors when included in multiple files.
    // 'constexpr' makes them strictly immutable and evaluated at compile time.
    inline constexpr chtype ls = '|';
    inline constexpr chtype rs = '|';
    inline constexpr chtype ts = '-';
    inline constexpr chtype bs = '-';
    inline constexpr chtype tl = '+';
    inline constexpr chtype tr = '+';
    inline constexpr chtype bl = '+';
    inline constexpr chtype br = '+';
}