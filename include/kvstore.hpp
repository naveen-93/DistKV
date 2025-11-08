#pragma once
#include <string>
#include <optional>
#include <mutex>
#include <unordered_map>
#include "storage.hpp"

class KVStore
{
public:
    KVStore(const std::string &storage_file);

    // Store key-value pair
    bool put(const std::string &key, const std::string &value);

    // Retrieve value by key
    bool get(const std::string &key, std::string& val);

    // Delete key
    bool remove(const std::string &key);

    // Persist current in-memory state to storage
    void persist();

private:
    std::mutex mtx;                                     // thread-safe access
    std::unordered_map<std::string, std::string> store; // in memory key-val store
    Storage storage;                                    // persistent layer
};
