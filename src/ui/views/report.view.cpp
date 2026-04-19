#include "../../models/book.hpp"
#include "../../models/transaction.hpp"
#include "../../utils/sort.hpp"
#include "../theme.hpp"
#include "view.hpp"
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <vector>

namespace ReportView {

    std::string draw(WINDOW *main_win) {

        bool show_idr = false;
    actually_start_draw: // Fetch fresh data
        std::vector<Transaction> txs = TransactionModel::read();
        std::vector<Book> books = BookModel::read();

        // Calculate Revenue and Sales
        float total_revenue = 0.0f;
        int total_items_sold = 0;

        for (const auto &tx : txs) {
            total_revenue += tx.total;
            total_items_sold += tx.item_total_qty;
        }

        SortUtils::timsort(books.data(), books.size(),
                           [](const Book &a, const Book &b) { return a.sold > b.sold; });

        // Sort books by 'sold' in descending order
        std::sort(books.begin(), books.end(),
                  [](const Book &a, const Book &b) { return a.sold > b.sold; });

        werase(main_win);
        wborder(main_win, BorderTheme::ls, BorderTheme::rs, BorderTheme::ts, BorderTheme::bs,
                BorderTheme::tl, BorderTheme::tr, BorderTheme::bl, BorderTheme::br);

        mvwprintw(main_win, 1, 2, "=== Business Reports ===");

        int curr_y = 3;

        // Draw Summary Section
        wattron(main_win, A_BOLD);
        mvwprintw(main_win, curr_y++, 3, "[ SUMMARY ]");
        wattroff(main_win, A_BOLD);

        std::stringstream rev_stream;
        if (show_idr) {
            rev_stream << "Rp" << std::fixed << std::setprecision(0) << (total_revenue * 17000.0f);
        } else {
            rev_stream << "$" << std::fixed << std::setprecision(2) << total_revenue;
        }

        mvwprintw(main_win, curr_y++, 3, "%-20s : %s", "Total Revenue", rev_stream.str().c_str());
        mvwprintw(main_win, curr_y++, 3, "%-20s : %d", "Total Items Sold", total_items_sold);

        curr_y += 2;

        // Draw Top Sellers Section
        wattron(main_win, A_BOLD);
        mvwprintw(main_win, curr_y++, 3, "[ TOP 5 BEST SELLERS ]");
        wattroff(main_win, A_BOLD);

        if (books.empty()) {
            mvwprintw(main_win, curr_y++, 3, "No books available.");
        } else {
            // Safely limit to 5, or total books if less than 5
            int limit = std::min(5, (int)books.size());
            for (int i = 0; i < limit; ++i) {
                // Truncate long titles to fit cleanly
                mvwprintw(main_win, curr_y++, 3, "%d. %-35s (Sold: %d)", i + 1,
                          books[i].title.substr(0, 32).c_str(), books[i].sold);
            }
        }

        int max_y, max_x;
        getmaxyx(main_win, max_y, max_x);
        mvwprintw(main_win, max_y - 2, (max_x - 47) / 2,
                  "[ PRESS 'C' FOR CURRENCY | ANY OTHER TO EXIT ]");

        wrefresh(main_win);

        // Input loop. Waits for any key to exit.
        keypad(main_win, TRUE);
        int ch;
        while ((ch = wgetch(main_win))) {
            if (ch == KEY_RESIZE) {
                return "RESIZE";
            }
            if (ch == 'c' || ch == 'C') {
                show_idr = !show_idr;
                goto actually_start_draw;
            }
            break;
        }

        return "Continue";
    }
} // namespace ReportView