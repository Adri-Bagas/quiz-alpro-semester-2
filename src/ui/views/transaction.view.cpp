#include "view.hpp"
#include "../../models/book.hpp"
#include "../../controllers/transaction.controller.hpp"
#include "../components/table.hpp"
#include "../components/input_modal.hpp"
#include "../theme.hpp"
#include <ncurses.h>
#include <sstream>
#include <iomanip>
#include <algorithm>

namespace TransactionView {

    std::string draw(WINDOW *main_win) {
        // 1. Initialize Transaction Setup
        auto customer_name = InputModal::prompt_string(main_win, "Customer Name (ESC to cancel):", false);
        if (!customer_name) return "Continue";

        auto date = InputModal::get_date(main_win, "Date");
        if (!date) return "Continue";

        std::vector<CartItem> cart;

    render_cart:
        // Always read fresh in case stock changed
        std::vector<Book> all_books = BookModel::read();

        werase(main_win);
        wborder(main_win, BorderTheme::ls, BorderTheme::rs, BorderTheme::ts, BorderTheme::bs,
                BorderTheme::tl, BorderTheme::tr, BorderTheme::bl, BorderTheme::br);
        mvwprintw(main_win, 1, 2, "=== Shopping Cart ===");
        mvwprintw(main_win, 2, 2, "Customer: %s | Date: %s", customer_name->c_str(), date->c_str());

        // 2. Draw Cart Summary Table
        int current_y = 4;
        float running_total = 0.0f;
        
        mvwprintw(main_win, current_y++, 2, "%-5s | %-30s | %-5s | %-10s", "ID", "Title", "Qty", "Subtotal");
        mvwhline(main_win, current_y++, 2, '-', 60);

        if (cart.empty()) {
            mvwprintw(main_win, current_y++, 2, "Cart is empty. Press 'A' to add books.");
        } else {
            for (const auto& item : cart) {
                // Find book to get current price and title
                auto it = std::find_if(all_books.begin(), all_books.end(), [&](const Book& b){ return b.id == item.book_id; });
                if (it != all_books.end()) {
                    float subtotal = it->price * item.qty;
                    running_total += subtotal;
                    mvwprintw(main_win, current_y++, 2, "%-5d | %-30s | %-5d | $%.2f", 
                              item.book_id, it->title.substr(0, 28).c_str(), item.qty, subtotal);
                }
            }
        }

        mvwhline(main_win, current_y++, 2, '-', 60);
        mvwprintw(main_win, current_y++, 2, "Total: $%.2f", running_total);

        // 3. Draw Keybinds
        current_y += 2;
        wattron(main_win, A_BOLD);
        mvwprintw(main_win, current_y, 2, "[A] Add Book   [C] Checkout   [ESC] Cancel Sale");
        wattroff(main_win, A_BOLD);
        wrefresh(main_win);

        // 4. Cart Input Loop
        int ch;
        keypad(main_win, TRUE);
        while ((ch = wgetch(main_win))) {
            if (ch == 27) { // ESC cancels the whole transaction
                return "Continue"; 
            }
            else if (ch == 'c' || ch == 'C') {
                if (cart.empty()) {
                    InputModal::show_error(main_win, "Cart is empty!");
                    goto render_cart;
                }
                
                // Fire off the controller logic 
                std::string err = TransactionController::checkout(*customer_name, *date, cart);
                if (err.empty()) {
                    InputModal::show_success(main_win, "Checkout Successful!"); 
                    return "Continue"; // Exit back to main menu
                } else {
                    InputModal::show_error(main_win, err); // E.g., "Insufficient Stock"
                    goto render_cart;
                }
            }
            else if (ch == 'a' || ch == 'A') {
                goto pick_book;
            }
        }

    pick_book:
        // 5. Book Selection Screen (Temporarily takes over main_win)
        werase(main_win);
        wborder(main_win, BorderTheme::ls, BorderTheme::rs, BorderTheme::ts, BorderTheme::bs,
                BorderTheme::tl, BorderTheme::tr, BorderTheme::bl, BorderTheme::br);
        mvwprintw(main_win, 1, 2, "=== Select Book for Cart ===");
        wrefresh(main_win);

        int max_y, max_x;
        getmaxyx(main_win, max_y, max_x);
        WINDOW *table_win = derwin(main_win, max_y - 4, max_x - 4, 3, 2);

        std::vector<std::string> headers = {"ID", "Title", "Price", "Stock"};
        
        TableUI picker_table = make_table<Book>(all_books, headers, [](const Book &b) {
            std::stringstream price_stream;
            price_stream << std::fixed << std::setprecision(2) << b.price;
            return std::vector<std::string>{std::to_string(b.id), b.title, price_stream.str(), std::to_string(b.stock)};
        });

        // Override default actions so we don't trigger Create/Edit/Delete here
        picker_table.actions = {"Add to Cart", "Cancel"};

        std::string action = picker_table.run(table_win, 0, 0);

        if (action == "Add to Cart") {
            int target_idx = picker_table.get_selected_row();
            int selected_id = all_books[target_idx].id;
            int available_stock = all_books[target_idx].stock;

            if (available_stock <= 0) {
                InputModal::show_error(main_win, "Out of stock!");
            } else {
                auto qty = InputModal::get_int(main_win, "Enter Quantity (Max " + std::to_string(available_stock) + "):");
                if (qty && *qty > 0) {
                    if (*qty > available_stock) {
                        InputModal::show_error(main_win, "Not enough stock!");
                    } else {
                        // Check if book is already in cart, just merge quantity
                        auto cart_it = std::find_if(cart.begin(), cart.end(), [&](const CartItem& c){ return c.book_id == selected_id; });
                        if (cart_it != cart.end()) {
                            cart_it->qty += *qty; 
                        } else {
                            cart.push_back({selected_id, *qty});
                        }
                    }
                }
            }
        }

        // Clean up table window and jump back up to render the updated cart
        delwin(table_win);
        goto render_cart; 
    }
} // namespace TransactionView