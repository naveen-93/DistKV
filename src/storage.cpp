#include "storage.hpp"
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <cstring>
#include <iostream>
#include <unordered_map>

Storage::Storage(const std::string &filename) : fd(-1)
{
    const std::string storage_dir = "storage";

    // Create storage directory if it doesn't exist
    if (mkdir(storage_dir.c_str(), 0755) == -1 && errno != EEXIST)
    {
        throw std::runtime_error("Failed to create storage directory: " +
                                 std::string(strerror(errno)));
    }

    // Build full file path
    this->filename = storage_dir + "/" + filename;

    // Open or create the file
    fd = open(this->filename.c_str(), O_RDWR | O_CREAT, 0644);
    if (fd < 0)
    {
        throw std::runtime_error("Failed to open storage file '" +
                                 this->filename + "': " + std::string(strerror(errno)));
    }
}

Storage::~Storage()
{
    if (fd >= 0)
    {
        close(fd);
    }
}

void Storage::append(const std::string &key, const std::string &value)
{
    // Validate inputs
    if (key.empty() || key.find(':') != std::string::npos ||
        key.find('\n') != std::string::npos)
    {
        throw std::invalid_argument("Invalid key format");
    }
    if (value.find('\n') != std::string::npos)
    {
        throw std::invalid_argument("Value cannot contain newlines");
    }

    std::string line = key + ":" + value + "\n";
    ssize_t written = 0;
    ssize_t total = line.size();

    // Ensure all bytes are written
    while (written < total)
    {
        ssize_t n = write(fd, line.c_str() + written, total - written);
        if (n < 0)
        {
            perror("write");
            throw std::runtime_error("Failed to write to storage");
        }
        written += n;
    }

    // Ensure durability
    if (fsync(fd) < 0)
    {
        perror("fsync");
    }
}

void Storage::remove(const std::string &key)
{
    // Validate key
    if (key.empty() || key.find(':') != std::string::npos ||
        key.find('\n') != std::string::npos)
    {
        throw std::invalid_argument("Invalid key format");
    }

    std::string line = key + ":__DELETE__\n";
    ssize_t written = 0;
    ssize_t total = line.size();

    while (written < total)
    {
        ssize_t n = write(fd, line.c_str() + written, total - written);
        if (n < 0)
        {
            perror("write");
            throw std::runtime_error("Failed to write deletion marker");
        }
        written += n;
    }

    if (fsync(fd) < 0)
    {
        perror("fsync");
    }
}

std::vector<std::pair<std::string, std::string>> Storage::load()
{
    std::unordered_map<std::string, std::string> kvmap;

    // Seek to beginning of file
    if (lseek(fd, 0, SEEK_SET) < 0)
    {
        perror("lseek");
        throw std::runtime_error("Failed to seek to beginning of file");
    }

    constexpr size_t BUF_SIZE = 4096;
    char buffer[BUF_SIZE];
    std::string leftover;

    ssize_t n;
    while ((n = read(fd, buffer, BUF_SIZE)) > 0)
    {
        // Combine leftover from previous iteration with new data
        std::string chunk = leftover + std::string(buffer, n);
        leftover.clear();

        size_t pos = 0;
        size_t newline;

        // Process each complete line
        while ((newline = chunk.find('\n', pos)) != std::string::npos)
        {
            std::string line = chunk.substr(pos, newline - pos);
            pos = newline + 1;

            // Parse key:value
            size_t sep = line.find(':');
            if (sep == std::string::npos || sep == 0)
            {
                // Invalid line format, skip
                continue;
            }

            std::string key = line.substr(0, sep);
            std::string val = line.substr(sep + 1);

            // Handle deletion marker
            if (val == "__DELETE__")
            {
                kvmap.erase(key);
            }
            else
            {
                kvmap[key] = val;
            }
        }

        // Save any incomplete line for next iteration
        if (pos < chunk.size())
        {
            leftover = chunk.substr(pos);
        }
    }

    if (n < 0)
    {
        perror("read");
        throw std::runtime_error("Failed to read from storage");
    }

    // Handle any remaining data (incomplete line at EOF - data corruption)
    if (!leftover.empty())
    {
        std::cerr << "Warning: Incomplete line at end of file: " << leftover << std::endl;
    }

    // Convert map to vector
    std::vector<std::pair<std::string, std::string>> data;
    data.reserve(kvmap.size());
    for (const auto &kv : kvmap)
    {
        data.push_back(kv);
    }

    return data;
}

//  compact method to remove deleted entries and reduce file size
void Storage::compact()
{
    // Load current data
    auto data = load();

    // Close current file
    close(fd);

    // Rewrite file with only active entries
    fd = open(filename.c_str(), O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0)
    {
        perror("open");
        throw std::runtime_error("Failed to reopen storage file for compaction");
    }

    // Write all active key-value pairs
    for (const auto &kv : data)
    {
        std::string line = kv.first + ":" + kv.second + "\n";
        ssize_t written = 0;
        ssize_t total = line.size();

        while (written < total)
        {
            ssize_t n = write(fd, line.c_str() + written, total - written);
            if (n < 0)
            {
                perror("write");
                throw std::runtime_error("Failed to write during compaction");
            }
            written += n;
        }
    }

    if (fsync(fd) < 0)
    {
        perror("fsync");
    }
}