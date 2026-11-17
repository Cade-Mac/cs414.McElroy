#include "file_handle.hpp"

#include <cstdio>
#include <iostream>
#include <stdexcept>
#include <string>

bool read_line(FILE* file, std::string& line) {
    line.clear();
    char buffer[4096];
    while (std::fgets(buffer, sizeof buffer, file)) {
        line += buffer;
        if (line.back() == '\n') {
            line.pop_back();
            return true;
        }
    }
    return !line.empty();
}

int count_matches_cpp(const std::string& path,
                      const std::string& needle,
                      bool fail_after_first_line = false) {
    if (needle.empty()) {
        throw std::invalid_argument("search string must not be empty");
    }

    FileHandle handle(path);
    std::string line;
    int count = 0;

    while (read_line(handle.get(), line)) {
        if (line.find(needle) != std::string::npos) {
            ++count;
        }
        if (fail_after_first_line) {
            throw std::runtime_error("simulated failure");
        }
    }
    return count;
}

void run_test(const std::string& label, const std::string& path,
              const std::string& needle, bool fail_after_first_line = false) {
    std::cout << '\n' << label << '\n';
    try {
        int count = count_matches_cpp(path, needle, fail_after_first_line);
        std::cout << "result: " << count << '\n';
    } catch (const std::exception& e) {
        std::cout << "exception: " << e.what() << '\n';
    }
}

int main(int argc, char* argv[]) {
    std::string data = argc > 1 ? argv[1] : "data";

    run_test("T1 sample.txt, search red", data + "/sample.txt", "red");
    run_test("T2 sample.txt, search orange", data + "/sample.txt", "orange");
    run_test("T3 empty.txt, search red", data + "/empty.txt", "red");
    run_test("T4 missing.txt, search red", data + "/missing.txt", "red");
    run_test("T5 sample.txt, search red, forced failure", data + "/sample.txt", "red", true);
    run_test("T6 sample.txt, empty search string", data + "/sample.txt", "");
    run_test("T7 long_line.txt, search red", data + "/long_line.txt", "red");

    return 0;
}
