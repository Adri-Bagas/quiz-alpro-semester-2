#include "table.hpp"

TableUI::TableUI(const std::vector<std::string> &headers,
                 const std::vector<std::vector<std::string>> &rows)
    : headers(headers), rows(rows) {
    calculate_widths();
}

void TableUI::calculate_widths() {
    col_widths.assign(headers.size(), 0);
    for (size_t i = 0; i < headers.size(); ++i) {
        col_widths[i] = headers[i].length();
    }
    for (const auto &row : rows) {
        for (size_t i = 0; i < row.size() && i < col_widths.size(); ++i) {
            if (row[i].length() > (size_t)col_widths[i]) {
                col_widths[i] = row[i].length();
            }
        }
    }
}

void TableUI::draw(WINDOW *win, int start_y, int start_x) {
    int max_y, max_x;
    getmaxyx(win, max_y, max_x);

    // Calculate available lines for rows
    int available_lines = max_y - start_y - 4; 
    
    // Auto-adjust scroll offset
    if (selected_row < scroll_offset) scroll_offset = selected_row;
    if (selected_row >= scroll_offset + available_lines) scroll_offset = selected_row - available_lines + 1;

    werase(win);
    int current_y = start_y;

    // Draw Headers
    wmove(win, current_y, start_x);
    for (size_t i = 0; i < headers.size(); ++i) {
        wprintw(win, "| %-*s ", col_widths[i], headers[i].c_str());
    }
    wprintw(win, "|");
    current_y++;

    // Draw Separator
    wmove(win, current_y, start_x);
    for (size_t i = 0; i < headers.size(); ++i) {
        wprintw(win, "+-");
        for (int w = 0; w < col_widths[i]; ++w) waddch(win, '-');
        wprintw(win, "-");
    }
    wprintw(win, "+");
    current_y++;

    // Draw Rows (Paginated)
    int end_idx = std::min((int)rows.size(), scroll_offset + available_lines);
    for (int r = scroll_offset; r < end_idx; ++r) {
        wmove(win, current_y, start_x);

        if (r == selected_row && !action_mode) wattron(win, A_REVERSE);
        else if (r == selected_row && action_mode) wattron(win, A_BOLD);

        for (size_t i = 0; i < rows[r].size() && i < headers.size(); ++i) {
            wprintw(win, "| %-*s ", col_widths[i], rows[r][i].c_str());
        }
        wprintw(win, "|");

        wattroff(win, A_REVERSE);
        wattroff(win, A_BOLD);
        current_y++;
    }

    // Draw Bottom Actions
    int action_y = max_y - 2;
    if (action_mode) {
        int x = start_x;
        mvwprintw(win, action_y, x, "Action: ");
        x += 8;
        for (size_t i = 0; i < actions.size(); ++i) {
            if ((int)i == selected_action) wattron(win, A_REVERSE);
            mvwprintw(win, action_y, x, "[ %s ]", actions[i].c_str());
            wattroff(win, A_REVERSE);
            x += actions[i].length() + 5;
        }
    } else {
        mvwprintw(win, action_y, start_x, "[ESC] Back  [UP/DOWN] Scroll  [ENTER] Options");
    }

    wrefresh(win);
}

std::string TableUI::run(WINDOW *win, int start_y, int start_x) {
    keypad(win, TRUE);
    draw(win, start_y, start_x);
    
    int ch;
    while ((ch = wgetch(win))) {
        if (ch == KEY_RESIZE) {
            return "RESIZE"; 
        }
        
        if (!action_mode) {
            if (ch == 27) return "EXIT"; // ESC
            else if (ch == KEY_UP && selected_row > 0) selected_row--;
            else if (ch == KEY_DOWN && selected_row < (int)rows.size() - 1) selected_row++;
            else if (ch == 10 && !rows.empty()) { // Enter
                action_mode = true;
                selected_action = 0;
            }
        } else {
            if (ch == 27) action_mode = false; // ESC out of action
            else if (ch == KEY_LEFT && selected_action > 0) selected_action--;
            else if (ch == KEY_RIGHT && selected_action < (int)actions.size() - 1) selected_action++;
            else if (ch == 10) return actions[selected_action]; // Return selected action string
        }
        draw(win, start_y, start_x);
    }
    return "Cancel";
}