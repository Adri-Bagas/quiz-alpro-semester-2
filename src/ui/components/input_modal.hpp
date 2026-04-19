#pragma once
#include <ncurses.h>
#include <string>
#include <optional>

namespace InputModal {
    void show_error(WINDOW* parent, const std::string& msg);

    void show_success(WINDOW* parent, const std::string& msg);
    
    std::optional<std::string> prompt_string(WINDOW* parent, const std::string& title, bool digits_only = false, const std::string& default_val = "");
    
    std::optional<int> get_int(WINDOW* parent, const std::string& title);
    
    std::optional<std::string> get_date(WINDOW* parent, const std::string& title);
}