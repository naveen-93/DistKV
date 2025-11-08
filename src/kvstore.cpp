#include "kvstore.hpp"

KVStore::KVStore(const std::string &storage_file) : storage(storage_file) {
    std::lock_guard<std::mutex> lock(mtx);
    auto data = storage.load();
    store.reserve(data.size());
    for (const auto &kv : data) {
        store.emplace(kv.first, kv.second);
    }
}

bool KVStore::put(const std::string &key, const std::string &value) {
    if (key.empty()) {
        return false;
    }
    
    std::lock_guard<std::mutex> lock(mtx);
    store[key] = value;
    storage.append(key, value);
    return true;
}

bool KVStore::get(const std::string &key, std::string &val) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = store.find(key);
    if (it != store.end()) {
        val = it->second;
        return true;
    }
    return false;
}

bool KVStore::remove(const std::string &key) {
    std::lock_guard<std::mutex> lock(mtx);
    auto it = store.find(key);
    if (it != store.end()) {
        store.erase(it);
        storage.remove(key);
        return true;
    }
    return false;
}

void KVStore::persist() {
    std::lock_guard<std::mutex> lock(mtx);
    storage.compact();
}