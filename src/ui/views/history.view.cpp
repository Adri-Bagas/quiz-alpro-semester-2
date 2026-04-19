#include "view.hpp"
#include "../../models/transaction.hpp"
#include "../components/table.hpp"
#include "../theme.hpp"
#include <iomanip>
#include <sstream>
#include <algorithm>

namespace HistoryView {

    void show_details_modal(WINDOW* parent, const Transaction& tx) {
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x);

        int num_items = tx.details.size();
        // Calculate height: base layout (12 lines) + items, maxed to screen height - 4
        int win_h = std::min(max_y - 4, 12 + num_items); 
        int win_w = 65;
        int start_y = (max_y - win_h) / 2;
        int start_x = (max_x - win_w) / 2;

        WINDOW* modal = newwin(win_h, win_w, start_y, start_x);
        
        box(modal, 0, 0);
        wattron(modal, A_BOLD);
        mvwprintw(modal, 1, (win_w - 19) / 2, "TRANSACTION DETAILS");
        wattroff(modal, A_BOLD);
        mvwhline(modal, 2, 1, '-', win_w - 2);

        // Header Data
        mvwprintw(modal, 3, 3, "%-12s: %d", "Tx ID", tx.id);
        mvwprintw(modal, 4, 3, "%-12s: %s", "Customer", tx.customer_name.c_str());
        mvwprintw(modal, 5, 3, "%-12s: %s", "Date", tx.date.c_str());
        
        mvwhline(modal, 7, 1, '-', win_w - 2);
        mvwprintw(modal, 8, 3, "%-30s | %-5s | %-10s", "Book Title", "Qty", "Subtotal");
        mvwhline(modal, 9, 1, '-', win_w - 2);

        // Item List
        int curr_y = 10;
        for (int i = 0; i < num_items && curr_y < win_h - 4; ++i) {
            const auto& d = tx.details[i];
            float subtotal = d.book.price * d.qty;
            mvwprintw(modal, curr_y++, 3, "%-30s | %-5d | $%.2f", 
                      d.book.title.substr(0, 28).c_str(), d.qty, subtotal);
        }

        // Footer Data
        mvwhline(modal, win_h - 3, 1, '-', win_w - 2);
        mvwprintw(modal, win_h - 2, 3, "TOTAL ITEMS: %d", tx.item_total_qty);
        mvwprintw(modal, win_h - 2, 38, "TOTAL: $%.2f", tx.total);

        mvwprintw(modal, win_h - 1, (win_w - 26) / 2, "[ Press ANY KEY to close ]");

        wrefresh(modal);
        wgetch(modal);
        delwin(modal);
        
        touchwin(parent);
        wrefresh(parent);
    }

    std::string draw(WINDOW *main_win) {
        std::vector<Transaction> txs = TransactionModel::read();

    actually_start_draw:
        std::vector<std::string> headers = {"ID", "Date", "Customer", "Items", "Total"};

        TableUI history_table = make_table<Transaction>(txs, headers, [](const Transaction &t) {
            std::stringstream total_stream;
            total_stream << std::fixed << std::setprecision(2) << t.total;
            return std::vector<std::string>{
                std::to_string(t.id),
                t.date,
                t.customer_name,
                std::to_string(t.item_total_qty),
                "$" + total_stream.str()
            };
        });

        // Override standard actions
        history_table.actions = {"View Details", "Cancel"};

        werase(main_win);
        wborder(main_win, BorderTheme::ls, BorderTheme::rs, BorderTheme::ts, BorderTheme::bs,
                BorderTheme::tl, BorderTheme::tr, BorderTheme::bl, BorderTheme::br);
        mvwprintw(main_win, 1, 2, "=== Transaction History ===");
        wrefresh(main_win);

        int max_y, max_x;
        getmaxyx(main_win, max_y, max_x);

        WINDOW *table_win = derwin(main_win, max_y - 4, max_x - 4, 3, 2);

        // Start table at 0,0 inside derived window
        std::string action = history_table.run(table_win, 0, 0);

        if (action == "RESIZE") {
            delwin(table_win);
            return "RESIZE";
        } 
        else if (action == "View Details") {
            int target_idx = history_table.get_selected_row();
            if (target_idx >= 0 && target_idx < (int)txs.size()) {
                show_details_modal(main_win, txs[target_idx]);
                history_table.draw(table_win, 0, 0); 
            }
        } 
        else if (action == "Cancel" || action == "EXIT") {
            delwin(table_win);
            return "Continue";
        }

        delwin(table_win);
        goto actually_start_draw;
    }
} // namespace HistoryView