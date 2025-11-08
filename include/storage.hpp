#pragma once
#include <string>
#include <vector>
#include <utility>

class Storage
{
public:
    Storage(const std::string &filename);
    ~Storage();

    // Append a record to disk
    void append(const std::string &key, const std::string &value);

    // Delete a record from disk (for simplicity, could just mark tombstone)
    void remove(const std::string &key);

    // Load all key-value pairs from disk
    std::vector<std::pair<std::string, std::string>> load();
    //  compact method to remove deleted entries and reduce file size
    void compact();

private:
    std::string filename;
    int fd;
};
