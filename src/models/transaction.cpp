#include "transaction.hpp"
#include "../utils/sort.hpp"
#include "../utils/json_file_handler.hpp"
#include <vector>

namespace TransactionModel {
    using json = nlohmann::json;
    constexpr const char *FILE_PATH = "./storage/transactions.json";

    std::vector<Transaction> read() {
        return JSONFileHandler::read<Transaction>(FILE_PATH);
    }


    void write(const std::vector<Transaction> &transactions) {
        JSONFileHandler::write(FILE_PATH, transactions);
    }

    Transaction find_first(std::function<bool(const Transaction &)> filter) {
        auto data = read();

        for (auto &transaction : data) {
            if (filter(transaction)) {
                return transaction;
            }
        }
        return {};
    }

    Transaction find_first(std::function<bool(const Transaction &)> filter, const std::vector<Transaction> &data) {
        for (auto &transaction : data) {
            if (filter(transaction)) {
                return transaction;
            }
        }
        return {};
    }

    std::vector<Transaction> find_all(std::function<bool(const Transaction &)> filter) {

        std::vector<Transaction> result;

        auto data = read();

        for (auto &transaction : data) {
            if (filter(transaction)) {
                result.push_back(transaction);
            }
        }
        
        return result;
    }

    std::vector<Transaction> find_all(std::function<bool(const Transaction &)> filter, const std::vector<Transaction>& data) {

        std::vector<Transaction> result;

        for (auto &transaction : data) {
            if (filter(transaction)) {
                result.push_back(transaction);
            }
        }
        
        return result;
    }

    void sort(std::vector<Transaction> &data, std::function<bool(const Transaction&, const Transaction&)> comp) {
        SortUtils::timsort(data.data(), data.size(), comp);
    }
} // namespace TransactionModel