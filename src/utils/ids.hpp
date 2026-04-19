#pragma once

#include "json_file_handler.hpp"
#include <nlohmann/json.hpp>

struct Ids {
    int books = 0;
    int transactions = 0;
    int transaction_details = 0;
};

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Ids, books, transactions, transaction_details);

namespace IdsTracker {
    using json = nlohmann::json;
    constexpr const char *FILE_PATH = "./storage/ids.json";
    inline Ids read () {
        return JSONFileHandler::read_single<Ids>(FILE_PATH);
    }

    inline void write (const Ids &ids) {
        JSONFileHandler::write_single(FILE_PATH, ids);
    }
}