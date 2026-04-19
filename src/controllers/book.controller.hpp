#pragma once
#include "../models/book.hpp"


namespace BookController {
    void createBook(
        std::vector<Book>& books, 
        std::string title, 
        std::string author, 
        std::string publisher, 
        int year, 
        int pages, 
        float price, 
        int stock
    );

    void updateBook(
        std::vector<Book>& books, int id, 
        std::optional<std::string> title, 
        std::optional<std::string> author, 
        std::optional<std::string> publisher, 
        std::optional<int> year, 
        std::optional<int> pages, 
        std::optional<float> price, 
        std::optional<int> stock
    );

    void deleteBook(std::vector<Book>& books,int id);
}