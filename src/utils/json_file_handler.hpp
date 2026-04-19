#pragma once
#include <fstream>
#include <vector>
#include <nlohmann/json.hpp>

namespace JSONFileHandler {
    template <typename T>
    T read_single(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) return {};
        
        nlohmann::json j;
        file >> j;
        return j.get<T>();
    }
    template <typename T>
    std::vector<T> read(const std::string& path) {
        std::ifstream file(path);
        if (!file.is_open()) return {};
        
        nlohmann::json j;
        file >> j;
        return j.get<std::vector<T>>();
    }

    template <typename T>
    void write_single(const std::string& path, const T& data) {
        std::ofstream file(path);
        if (!file.is_open()) return;
        
        nlohmann::json j = data; 
        file << j.dump(4);
    }

    template <typename T>
    void write(const std::string& path, const std::vector<T>& data) {
        std::ofstream file(path);
        if (!file.is_open()) return;
        
        nlohmann::json j = data; 
        file << j.dump(4);
    }
}