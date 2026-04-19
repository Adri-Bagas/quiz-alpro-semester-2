#include "book.hpp"
#include "../utils/sort.hpp"
#include "../utils/json_file_handler.hpp"
#include <optional>
#include <vector>

namespace BookModel {
    using json = nlohmann::json;
    constexpr const char *FILE_PATH = "./storage/books.json";

    std::vector<Book> read() {
        return JSONFileHandler::read<Book>(FILE_PATH);
    }

    void write(const std::vector<Book> &books) {
        JSONFileHandler::write(FILE_PATH, books);
    }

    std::optional<Book> find_first(std::function<bool(const Book &)> filter) {
        auto data = read();

        for (auto &book : data) {
            if (filter(book)) {
                return book;
            }
        }
        return std::nullopt;
    };
    std::optional<Book> find_first(std::function<bool(const Book &)> filter, std::vector<Book> data) {
        for (auto &book : data) {
            if (filter(book)) {
                return book;
            }
        }
        return std::nullopt;
    };

    std::vector<Book> find_all(std::function<bool(const Book &)> filter) {
        std::vector<Book> result;

        auto data = read();

        for (auto &book : data) {
            if (filter(book)) {
                result.push_back(book);
            }
        }
        
        return result;
    };
    std::vector<Book> find_all(std::function<bool(const Book &)> filter, std::vector<Book> data) {
        std::vector<Book> result;

        for (auto &book : data) {
            if (filter(book)) {
                result.push_back(book);
            }
        }
        
        return result;
    };

    

    void sort(std::vector<Book> &data, std::function<bool(const Book&, const Book&)> comp) {
        SortUtils::timsort(data.data(), data.size(), comp);
    };
} // namespace BookModel