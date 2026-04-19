#include "../../models/book.hpp"
#include "../../controllers/book.controller.hpp"
#include "../components/input_modal.hpp"
#include "../components/table.hpp"
#include "../../utils/sort.hpp"
#include "../theme.hpp"
#include "view.hpp"
#include <iomanip>
#include <ncurses.h>
#include <sstream>

namespace BookView {

    void show_details_modal(WINDOW *parent, const Book &b) {
        int max_y, max_x;
        getmaxyx(stdscr, max_y, max_x); // Use stdscr to center on whole screen

        int win_h = 15;
        int win_w = 55;
        int start_y = (max_y - win_h) / 2;
        int start_x = (max_x - win_w) / 2;

        // Create floating modal window
        WINDOW *modal = newwin(win_h, win_w, start_y, start_x);

        // Draw outer frame
        box(modal, 0, 0);
        wattron(modal, A_BOLD);
        mvwprintw(modal, 1, (win_w - 12) / 2, "BOOK DETAILS");
        wattroff(modal, A_BOLD);
        mvwhline(modal, 2, 1, '-', win_w - 2);

        // Draw data rows
        mvwprintw(modal, 3, 3, "%-12s: %d", "ID", b.id);
        mvwprintw(modal, 4, 3, "%-12s: %s", "Title", b.title.c_str());
        mvwprintw(modal, 5, 3, "%-12s: %s", "Author", b.author.c_str());
        mvwprintw(modal, 6, 3, "%-12s: %s", "Publisher", b.publisher.c_str());
        mvwprintw(modal, 7, 3, "%-12s: %d", "Year", b.year);
        mvwprintw(modal, 8, 3, "%-12s: %d", "Pages", b.pages);
        mvwprintw(modal, 9, 3, "%-12s: $%.2f", "Price", b.price);
        mvwprintw(modal, 10, 3, "%-12s: %d", "Stock", b.stock);
        mvwprintw(modal, 11, 3, "%-12s: %d", "Sold", b.sold);

        mvwhline(modal, 13, 1, '-', win_w - 2);
        mvwprintw(modal, win_h - 1, (win_w - 26) / 2, "[ Press ANY KEY to close ]");

        wrefresh(modal);

        // Hijack input loop. Blocks execution until user presses a key.
        wgetch(modal);

        // Clean up memory
        delwin(modal);

        // Mark parent window as dirty. Forces redraw of table underneath.
        touchwin(parent);
        wrefresh(parent);
    }

    std::string draw(WINDOW *main_win) {

        std::vector<Book> books = BookModel::read();
        bool show_idr = false;

    actually_start_draw:

        std::vector<std::string> headers = {"ID", "Title", "Price", "Stock"};
        

        TableUI book_table = make_table<Book>(
            books, 
            headers, 
            [&](const Book &b) {
                std::stringstream pricestream;

                if (show_idr) {
                    pricestream << "Rp" << std::fixed << std::setprecision(0) << (b.price * 17000.0f);
                } else {
                    pricestream << "$" << std::fixed << std::setprecision(2) << b.price;
                }

            return std::vector<std::string>{
                std::to_string(b.id), 
                b.title, 
                pricestream.str(),                            
                std::to_string(b.stock)};
        });

        book_table.actions = {"Create", "Edit", "Search", "View", "Sort", "Currency","Delete", "Cancel"};

        werase(main_win);
        wborder(main_win, BorderTheme::ls, BorderTheme::rs, BorderTheme::ts, BorderTheme::bs,
                BorderTheme::tl, BorderTheme::tr, BorderTheme::bl, BorderTheme::br);
        mvwprintw(main_win, 1, 2, "=== Book Management ===");
        wrefresh(main_win);

        int max_y, max_x;
        getmaxyx(main_win, max_y, max_x);

        WINDOW *table_win = derwin(main_win, max_y - 4, max_x - 4, 3, 2);

        std::string action = book_table.run(table_win, 0, 0);

        if (action == "RESIZE") {
            delwin(table_win);
            return "RESIZE";
        } else if (action == "Create") {
            // Sequence of prompts. Abort if user presses ESC.
            auto title = InputModal::prompt_string(main_win, "Title:", false);
            if (!title) {
                delwin(table_win);
                goto actually_start_draw;
            }

            auto author = InputModal::prompt_string(main_win, "Author:", false);
            if (!author) {
                delwin(table_win);
                goto actually_start_draw;
            }

            auto publisher = InputModal::prompt_string(main_win, "Publisher:", false);
            if (!publisher) {
                delwin(table_win);
                goto actually_start_draw;
            }

            auto year = InputModal::get_int(main_win, "Year:");
            if (!year) {
                delwin(table_win);
                goto actually_start_draw;
            }

            auto pages = InputModal::get_int(main_win, "Pages:");
            if (!pages) {
                delwin(table_win);
                goto actually_start_draw;
            }

            // Using string prompt for float, converting safely
            auto price_str = InputModal::prompt_string(main_win, "Price:", true);
            if (!price_str) {
                delwin(table_win);
                goto actually_start_draw;
            }
            float price = price_str->empty() ? 0.0f : std::stof(*price_str);

            auto stock = InputModal::get_int(main_win, "Stock:");
            if (!stock) {
                delwin(table_win);
                goto actually_start_draw;
            }

            // Execute creation
            BookController::createBook(books, *title, *author, *publisher, *year, *pages, price,
                                       *stock);

            // Reload data from disk
            books = BookModel::read();
            delwin(table_win);
            goto actually_start_draw;

        } else if (action == "Edit") {
            int target_idx = book_table.get_selected_row();
            int id = books[target_idx].id;

            InputModal::show_error(main_win, std::to_string(id));

            // Optional prompts. std::nullopt means skip/keep same.
            auto title = InputModal::prompt_string(main_win, "New Title (ESC to keep):", false);
            auto author = InputModal::prompt_string(main_win, "New Author (ESC to keep):", false);
            auto publisher =
                InputModal::prompt_string(main_win, "New Publisher (ESC to keep):", false);
            auto year = InputModal::get_int(main_win, "New Year (ESC to keep):");
            auto pages = InputModal::get_int(main_win, "New Pages (ESC to keep):");

            auto price_str = InputModal::prompt_string(main_win, "New Price (ESC to keep):", true);
            std::optional<float> price = std::nullopt;
            if (price_str && !price_str->empty())
                price = std::stof(*price_str);

            auto stock = InputModal::get_int(main_win, "New Stock (ESC to keep):");

            // Execute update
            BookController::updateBook(books, id, title, author, publisher, year, pages, price,
                                       stock);

            // Reload data
            books = BookModel::read();
            delwin(table_win);
            goto actually_start_draw;

        } else if (action == "Search") {
            auto input = InputModal::prompt_string(main_win, "Search for :", false);

            if (!input) {
                delwin(table_win);
                goto actually_start_draw;
            }

            auto to_lower = [](std::string s) {
                std::transform(s.begin(), s.end(), s.begin(), ::tolower);
                return s;
            };

            // Convert search term to lowercase
            std::string term = input.value();
            std::transform(term.begin(), term.end(), term.begin(), ::tolower);

            books = BookModel::find_all(
                [&](const Book &b) {
                    return to_lower(b.title).find(term) != std::string::npos ||
                           to_lower(b.author).find(term) != std::string::npos ||
                           to_lower(b.publisher).find(term) != std::string::npos ||
                           std::to_string(b.year).find(term) != std::string::npos ||
                           std::to_string(b.pages).find(term) != std::string::npos ||
                           std::to_string(b.price).find(term) != std::string::npos ||
                           std::to_string(b.stock).find(term) != std::string::npos ||
                           std::to_string(b.sold).find(term) != std::string::npos;
                },
                BookModel::read()); // pass freshly read data so search resets properly

            delwin(table_win);
            goto actually_start_draw;

        } else if (action == "View") {
            int target_idx = book_table.get_selected_row();
            show_details_modal(main_win, books[target_idx]);
            book_table.draw(table_win, 0, 0); // Quick redraw, no goto needed

        } else if (action == "Delete") {
            int target_idx = book_table.get_selected_row();
            int id = books[target_idx].id;

            // Safety confirmation
            auto confirm = InputModal::prompt_string(
                main_win, "Type 'yes' to delete ID " + std::to_string(id) + ":", false);

            if (confirm && confirm.value() == "yes") {
                BookController::deleteBook(books, id);
                books = BookModel::read(); // Reload
            }

            delwin(table_win);
            goto actually_start_draw;

        } else if (action == "Sort") {
            auto col_str = InputModal::prompt_string(main_win, "Sort by (1:ID, 2:Title, 3:Price, 4:Stock):", true);
            if (!col_str || col_str->empty()) { delwin(table_win); goto actually_start_draw; }

            auto dir_str = InputModal::prompt_string(main_win, "Order (1:Ascending, 2:Descending):", true);
            if (!dir_str || dir_str->empty()) { delwin(table_win); goto actually_start_draw; }

            int col = std::stoi(*col_str);
            int dir = std::stoi(*dir_str);

            // Execute in-place sort on the vector's underlying array
            SortUtils::timsort(books.data(), books.size(), [col, dir](const Book& a, const Book& b) {
                bool asc = (dir == 1);
                if (col == 1) return asc ? (a.id < b.id) : (a.id > b.id);
                if (col == 2) return asc ? (a.title < b.title) : (a.title > b.title);
                if (col == 3) return asc ? (a.price < b.price) : (a.price > b.price);
                if (col == 4) return asc ? (a.stock < b.stock) : (a.stock > b.stock);
                return a.id < b.id; // default fallback
            });

            delwin(table_win);
            goto actually_start_draw;
        } else if (action == "Currency") {
            show_idr = !show_idr;
            delwin(table_win);
            goto actually_start_draw;
            
        } else if (action == "EXIT") {
            delwin(table_win);
            return "EXIT";
        }

        // Loop execution handles anything that doesn't trigger a goto or return
        delwin(table_win);
        return "Continue";
    }
} // namespace BookView
