#include "store.hpp"
#include <fstream>
#include <algorithm>

void Store::set(const std::string& key, const std::string& value) {
    data_[key] = value;
}

std::optional<std::string> Store::get(const std::string& key) const {
    auto it = data_.find(key);
    if (it != data_.end()) {
        return it->second;
    }
    return std::nullopt;
}

bool Store::remove(const std::string& key) {
    return data_.erase(key) > 0;
}

std::vector<std::pair<std::string, std::string>> Store::list() const {
    std::vector<std::pair<std::string, std::string>> result(data_.begin(), data_.end());
    std::sort(result.begin(), result.end());
    return result;
}

bool Store::save(const std::string& filename) const {
    std::ofstream out(filename); // RAII: Opens file on construction
    if (!out.is_open()) return false;
    
    for (const auto& [k, v] : list()) {
        out << k << " " << v << "\n";
    }
    return true; // RAII: Automatically closes file on stream destructor
}

bool Store::load(const std::string& filename) {
    std::ifstream in(filename); // RAII: Opens file on construction
    if (!in.is_open()) return false;
    
    data_.clear();
    std::string k, v;
    while (in >> k >> v) {
        data_[k] = v;
    }
    return true; // RAII: Automatically closes file on stream destructor
}