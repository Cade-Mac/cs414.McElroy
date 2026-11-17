#ifndef FILE_HANDLE_HPP
#define FILE_HANDLE_HPP

#include <cstdio>
#include <string>

class FileHandle {
public:
    explicit FileHandle(const std::string& path);
    ~FileHandle() noexcept;

    FileHandle(const FileHandle&) = delete;
    FileHandle& operator=(const FileHandle&) = delete;
    FileHandle(FileHandle&&) = delete;
    FileHandle& operator=(FileHandle&&) = delete;

    FILE* get() const noexcept;

private:
    FILE* file_ = nullptr;
};

#endif
