#pragma once

#include <nlohmann/json.hpp>
#include <string>

struct Book {
    int id;
    std::string title;
    std::string author;
    std::string publisher;
    int year;
    int pages;
    float price;
    int stock = 0;
    int sold = 0;
};

inline void to_json(nlohmann::json& j, const Book& b) {
    j = nlohmann::json{{"id", b.id}, {"title", b.title}, {"author", b.author}, 
                       {"publisher", b.publisher}, {"year", b.year}, 
                       {"pages", b.pages}, {"price", b.price}, 
                       {"stock", b.stock}, {"sold", b.sold}};
}

inline void from_json(const nlohmann::json& j, Book& b) {
    b.id = j.at("id").get<int>();
    b.title = j.at("title").get<std::string>();
    b.author = j.at("author").get<std::string>();
    b.publisher = j.at("publisher").get<std::string>();
    b.year = j.at("year").get<int>();
    b.pages = j.at("pages").get<int>();
    b.price = j.at("price").get<float>();
    b.stock = j.value("stock", 0); // Default 0 if missing
    b.sold = j.value("sold", 0);   // Default 0 if missing
}

namespace BookModel {
    std::vector<Book> read();

    void write(const std::vector<Book> &books);
    std::optional<Book> find_first(std::function<bool(const Book &)> filter);
    std::optional<Book> find_first(std::function<bool(const Book &)> filter, std::vector<Book> data);

    std::vector<Book> find_all(std::function<bool(const Book &)> filter);
    std::vector<Book> find_all(std::function<bool(const Book &)> filter, std::vector<Book> data);

    void sort(std::vector<Book> data);
} // namespace BookModel