#include "input_modal.hpp"
#include <cctype>
#include <regex>
#include <algorithm>
#include <ctime>
#include <iomanip>

namespace InputModal {

    // Helper for validation errors
    void show_error(WINDOW* parent, const std::string& msg) {
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        
        int win_h = 5;
        int win_w = std::max(40, (int)msg.length() + 10);
        WINDOW* modal = newwin(win_h, win_w, (max_y - win_h) / 2, (max_x - win_w) / 2);
        
        box(modal, 0, 0);
        wattron(modal, A_BOLD);
        mvwprintw(modal, 1, (win_w - 5) / 2, "ERROR");
        wattroff(modal, A_BOLD);
        mvwprintw(modal, 2, (win_w - msg.length()) / 2, "%s", msg.c_str());
        mvwprintw(modal, 4, (win_w - 24) / 2, "[ Press ANY KEY to retry ]");
        
        wrefresh(modal);
        wgetch(modal);
        delwin(modal);
        
        touchwin(parent);
        wrefresh(parent);
    }

    void show_success(WINDOW* parent, const std::string& msg) {
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);
        
        int win_h = 5;
        int win_w = std::max(40, (int)msg.length() + 10);
        WINDOW* modal = newwin(win_h, win_w, (max_y - win_h) / 2, (max_x - win_w) / 2);
        
        box(modal, 0, 0);
        wattron(modal, A_BOLD);
        mvwprintw(modal, 1, (win_w - 5) / 2, "SUCCESS");
        wattroff(modal, A_BOLD);
        mvwprintw(modal, 2, (win_w - msg.length()) / 2, "%s", msg.c_str());
        mvwprintw(modal, 4, (win_w - 24) / 2, "[ Press ANY KEY to retry ]");
        
        wrefresh(modal);
        wgetch(modal);
        delwin(modal);
        
        touchwin(parent);
        wrefresh(parent);
    }

    std::string get_current_date() {
        std::time_t t = std::time(nullptr);
        std::tm* now = std::localtime(&t);
        std::stringstream ss;
        ss << std::put_time(now, "%Y-%m-%d");
        return ss.str();
    }

    std::optional<std::string> prompt_string(WINDOW* parent, const std::string& title, bool digits_only, const std::string& default_val) {
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);

        int win_h = 5;
        int win_w = 50; 
        int start_y = (max_y - win_h) / 2;
        int start_x = (max_x - win_w) / 2;

        WINDOW* modal = newwin(win_h, win_w, start_y, start_x);
        keypad(modal, TRUE);

        box(modal, 0, 0);
        mvwprintw(modal, 1, 2, "%s", title.c_str());
        mvwprintw(modal, 2, 2, "> ");
        
        // Initialize input with default value
        std::string input = default_val;
        
        // Draw the default value immediately before waiting for input
        mvwprintw(modal, 2, 4, "%s", input.c_str());
        wrefresh(modal);

        int ch;
        std::optional<std::string> result = std::nullopt;
        
        while ((ch = wgetch(modal))) {
            if (ch == 27) { // ESC
                break;
            } 
            else if (ch == '\n' || ch == KEY_ENTER) { // Enter
                if (!input.empty()) {
                    result = input;
                    break;
                }
            } 
            else if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b') { // Backspace
                if (!input.empty()) {
                    input.pop_back();
                    mvwprintw(modal, 2, 4, "%-44s", " "); 
                }
            } 
            else if (isprint(ch)) { 
                if (digits_only && !isdigit(ch) && ch != '-' && ch != '.') continue;
                
                if (input.length() < (size_t)(win_w - 6)) {
                    input += (char)ch;
                }
            }
            
            mvwprintw(modal, 2, 4, "%s", input.c_str());
            wrefresh(modal);
        }

        delwin(modal);
        touchwin(parent);
        wrefresh(parent);
        
        return result;
    }

    // Type wrapper: Integer
    std::optional<int> get_int(WINDOW* parent, const std::string& title) {
        auto res = prompt_string(parent, title, true);
        if (res) {
            try { return std::stoi(*res); } 
            catch (...) { return std::nullopt; }
        }
        return std::nullopt;
    }

    // Type wrapper: Date
    std::optional<std::string> get_date(WINDOW* parent, const std::string& title) {
        // Fetch the system date
        std::string today = get_current_date();
        
        while (true) {
            
            auto res = prompt_string(parent, title + " (YYYY-MM-DD)", false, today);
            if (!res) return std::nullopt; 
            
            std::regex date_pat(R"(\d{4}-\d{2}-\d{2})");
            if (std::regex_match(*res, date_pat)) return res;
            
            show_error(parent, "Invalid format! Use YYYY-MM-DD.");
        }
    }
}