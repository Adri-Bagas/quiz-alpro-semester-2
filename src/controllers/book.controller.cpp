#include "../utils/ids.hpp"
#include "book.controller.hpp"


namespace BookController {
    void createBook(
        std::vector<Book>& books, 
        std::string title, 
        std::string author, 
        std::string publisher, 
        int year, int pages, 
        float price = 0, 
        int stock = 0
    ) {
        Ids ids = IdsTracker::read();
        
        Book book {++(ids.books), title, author, publisher, year, pages, price, stock};
        books.push_back(book);

        BookModel::write(books);
        

        IdsTracker::write(ids);
    }

    void updateBook(
        std::vector<Book>& books, int id, 
        std::optional<std::string> title, 
        std::optional<std::string> author, 
        std::optional<std::string> publisher, 
        std::optional<int> year, 
        std::optional<int> pages, 
        std::optional<float> price, 
        std::optional<int> stock
    ) {
        
        auto book = BookModel::find_first([&](const Book& b) { return b.id == id; }, books);

        if (!book) {
            throw std::runtime_error("Book with id " + std::to_string(id) + " not found");
        }

        book->title = title.value_or(book->title);
        book->author = author.value_or(book->author);
        book->publisher = publisher.value_or(book->publisher);
        book->year = year.value_or(book->year);
        book->pages = pages.value_or(book->pages);
        book->price = price.value_or(book->price);
        book->stock = stock.value_or(book->stock);

        for (auto &elem : books) {
            if (elem.id == id) {
                elem = book.value();
            }
        }
        
        BookModel::write(books);
    }

    void deleteBook(std::vector<Book>& books,int id) {
        auto it = std::remove_if(books.begin(), books.end(), [id](const Book& b) {
            return b.id == id;
        });
        
        if (it != books.end()) {
            books.erase(it, books.end());
            BookModel::write(books);
        }
    }
}