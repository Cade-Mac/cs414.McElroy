#ifndef STORE_HPP
#define STORE_HPP

#include <string>
#include <unordered_map>
#include <optional>
#include <vector>
#include <utility>

class Store {
public:
    void set(const std::string& key, const std::string& value);
    std::optional<std::string> get(const std::string& key) const;
    bool remove(const std::string& key);
    std::vector<std::pair<std::string, std::string>> list() const;
    
    bool save(const std::string& filename) const;
    bool load(const std::string& filename);

    bool operator==(const Store& other) const = default;

private:
    std::unordered_map<std::string, std::string> data_;
};

#endif