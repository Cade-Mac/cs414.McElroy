#include "file_handle.hpp"

#include <iostream>
#include <stdexcept>

FileHandle::FileHandle(const std::string& path) {
    file_ = std::fopen(path.c_str(), "r");
    if (file_ == nullptr) {
        throw std::runtime_error("cannot open file: " + path);
    }
    std::cerr << "[FileHandle] acquired " << path << '\n';
}

FileHandle::~FileHandle() noexcept {
    if (file_ != nullptr) {
        std::fclose(file_);
        std::cerr << "[FileHandle] released\n";
    }
}

FILE* FileHandle::get() const noexcept {
    return file_;
}
