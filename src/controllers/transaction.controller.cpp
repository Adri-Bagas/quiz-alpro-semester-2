#include "transaction.controller.hpp"
#include "../models/book.hpp" 
#include "../utils/ids.hpp"
#include "../models/transaction.hpp"
#include "../models/book.hpp"
#include <algorithm>

namespace TransactionController {

    std::string checkout(const std::string& customer_name, const std::string& date, const std::vector<CartItem>& cart) {
        if (cart.empty()) return "Cart is empty.";

        // Load complete state into memory
        std::vector<Book> books = BookModel::read();
        std::vector<Transaction> transactions = TransactionModel::read();
        Ids ids = IdsTracker::read();

        Transaction new_tx;
        new_tx.id = ids.transactions;
        new_tx.customer_name = customer_name;
        new_tx.date = date;
        new_tx.total = 0.0f;
        new_tx.item_total_qty = 0;

        int detail_id_counter = ids.transaction_details;

        // Process cart items
        for (const auto& item : cart) {
            // Find book by ID
            auto it = std::find_if(books.begin(), books.end(), [&](const Book& b) {
                return b.id == item.book_id;
            });

            if (it == books.end()) {
                return "Book ID " + std::to_string(item.book_id) + " not found.";
            }

            // Validate stock
            if (it->stock < item.qty) {
                return "Insufficient stock for: " + it->title + " (Stock: " + std::to_string(it->stock) + ")";
            }

            // Deduct stock, increment sold
            it->stock -= item.qty;
            it->sold += item.qty;

            // Build detail record
            TransactionDetail detail;
            detail.id = detail_id_counter;
            detail_id_counter++;
            detail.book = *it;
            detail.qty = item.qty;

            // Update transaction totals
            new_tx.total += (it->price * item.qty);
            new_tx.item_total_qty += item.qty;
            new_tx.details.push_back(detail);
        }

        // Commit
        transactions.push_back(new_tx);
        
        BookModel::write(books);
        TransactionModel::write(transactions);

        ids.transactions++;
        ids.transaction_details = detail_id_counter;
        IdsTracker::write(ids);

        return ""; // Empty string indicates success
    }
}