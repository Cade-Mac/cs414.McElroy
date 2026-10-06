#include "store.hpp"
#include "transaction.hpp"
#include <iostream>
#include <sstream>

int main() {
    Store store;
    std::string line;

    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, line)) break;
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::string cmd;
        ss >> cmd;

        if (cmd == "QUIT" || cmd == "quit") {
            break;
        } else if (cmd == "SET") {
            std::string k, v;
            if (ss >> k >> v) store.set(k, v);
        } else if (cmd == "GET") {
            std::string k;
            if (ss >> k) {
                auto val = store.get(k);
                if (val) std::cout << *val << "\n";
                else std::cout << "(not found)\n";
            }
        } else if (cmd == "DELETE") {
            std::string k;
            if (ss >> k) store.remove(k);
        } else if (cmd == "LIST") {
            for (const auto& [k, v] : store.list()) {
                std::cout << k << " = " << v << "\n";
            }
        } else if (cmd == "SAVE") {
            std::string file;
            if (ss >> file) store.save(file);
        } else if (cmd == "LOAD") {
            std::string file;
            if (ss >> file) store.load(file);
        }
    }
    return 0;
}