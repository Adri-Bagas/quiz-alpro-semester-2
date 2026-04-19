#pragma once
#include <vector>
#include <string>


struct CartItem {
    int book_id;
    int qty;
};

namespace TransactionController {
    // For error return string, success empty
    std::string checkout(const std::string& customer_name, const std::string& date, const std::vector<CartItem>& cart);
}