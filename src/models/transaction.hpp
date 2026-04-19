#pragma once

#include "book.hpp"
#include <nlohmann/json.hpp>
#include <string>
#include <vector>

struct TransactionDetail {
    int id;
    Book book;
    int qty;
};

struct Transaction {
    int id;
    std::string customer_name;
    std::string date;
    float total;
    int item_total_qty;
    std::vector<TransactionDetail> details;
};

inline void to_json(nlohmann::json& j, const TransactionDetail& d) {
    j = nlohmann::json{{"id", d.id}, {"book", d.book}, {"qty", d.qty}};
}

inline void from_json(const nlohmann::json& j, TransactionDetail& d) {
    d.id = j.at("id").get<int>();
    d.book = j.at("book").get<Book>(); // Recursively calls Book's from_json
    d.qty = j.at("qty").get<int>();
}

inline void to_json(nlohmann::json& j, const Transaction& t) {
    j = nlohmann::json{{"id", t.id}, {"customer_name", t.customer_name}, 
                       {"date", t.date}, {"total", t.total}, 
                       {"item_total_qty", t.item_total_qty}, {"details", t.details}};
}

inline void from_json(const nlohmann::json& j, Transaction& t) {
    t.id = j.at("id").get<int>();
    t.customer_name = j.at("customer_name").get<std::string>();
    t.date = j.at("date").get<std::string>();
    t.total = j.at("total").get<int>();
    t.item_total_qty = j.at("item_total_qty").get<int>();
    t.details = j.at("details").get<std::vector<TransactionDetail>>();
}

namespace TransactionModel {
    std::vector<Transaction> read();

    void write(const std::vector<Transaction> &transactions);
    Transaction find_first(std::function<bool(const Transaction &)> filter);
    Transaction find_first(std::function<bool(const Transaction &)> filter, std::vector<Transaction> data);

    std::vector<Transaction> find_all(std::function<bool(const Transaction &)> filter);
    std::vector<Transaction> find_all(std::function<bool(const Transaction &)> filter, std::vector<Transaction> data);

    void sort(std::vector<Transaction> data);
} // namespace TransactionModel