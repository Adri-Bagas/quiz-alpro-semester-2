#pragma once

#include <functional>
#include <ncurses.h>
#include <string>
#include <vector>

class TableUI {
private:
    std::vector<std::string> headers;
    std::vector<std::vector<std::string>> rows;
    std::vector<int> col_widths;

    // State trackers
    int selected_row = 0;
    int scroll_offset = 0;
    bool action_mode = false;
    int selected_action = 0;
    

    void calculate_widths();

public:
    TableUI(const std::vector<std::string> &headers, const std::vector<std::vector<std::string>> &rows);
    
    void draw(WINDOW *win, int start_y, int start_x);
    std::string run(WINDOW *win, int start_y = 1, int start_x = 1);
    std::vector<std::string> actions = {"Create", "Edit", "View", "Search", "Delete", "Cancel"};
    
    // Returns index of the row the user took action on
    int get_selected_row() const { return selected_row; }
};


template <typename T>
TableUI make_table (
    const std::vector<T> &data, 
    const std::vector<std::string> &headers,
    std::function<std::vector<std::string>(const T &)> extractor
) {
    std::vector<std::vector<std::string>> string_rows;

    for (const auto &item : data) {
        string_rows.push_back(extractor(item));
    }

    return TableUI(headers, string_rows);
}
