#include "storage.hpp"
#include <iostream>
#include <cassert>
#include <unistd.h>

void test_basic_append_and_load() {
    std::cout << "Testing basic append and load..." << std::endl;
    
    {
        Storage storage("test_basic.db");
        storage.append("key1", "value1");
        storage.append("key2", "value2");
        storage.append("key3", "value3");
    }
    
    {
        Storage storage("test_basic.db");
        auto data = storage.load();
        
        assert(data.size() == 3);
        
        bool found1 = false, found2 = false, found3 = false;
        for (const auto& kv : data) {
            if (kv.first == "key1") {
                assert(kv.second == "value1");
                found1 = true;
            } else if (kv.first == "key2") {
                assert(kv.second == "value2");
                found2 = true;
            } else if (kv.first == "key3") {
                assert(kv.second == "value3");
                found3 = true;
            }
        }
        
        assert(found1 && found2 && found3);
    }
    
    std::cout << "✓ Basic append and load passed" << std::endl;
}

void test_update_value() {
    std::cout << "Testing value updates..." << std::endl;
    
    {
        Storage storage("test_update.db");
        storage.append("name", "Alice");
        storage.append("name", "Bob");
        storage.append("age", "25");
        storage.append("age", "30");
    }
    
    {
        Storage storage("test_update.db");
        auto data = storage.load();
        
        for (const auto& kv : data) {
            if (kv.first == "name") {
                assert(kv.second == "Bob");
            } else if (kv.first == "age") {
                assert(kv.second == "30");
            }
        }
    }
    
    std::cout << "✓ Value updates passed" << std::endl;
}

void test_remove() {
    std::cout << "Testing remove..." << std::endl;
    
    {
        Storage storage("test_remove.db");
        storage.append("key1", "value1");
        storage.append("key2", "value2");
        storage.append("key3", "value3");
        storage.remove("key2");
    }
    
    {
        Storage storage("test_remove.db");
        auto data = storage.load();
        
        assert(data.size() == 2);
        
        for (const auto& kv : data) {
            assert(kv.first != "key2");
        }
    }
    
    std::cout << "✓ Remove passed" << std::endl;
}

void test_compact() {
    std::cout << "Testing compact..." << std::endl;
    
    {
        Storage storage("test_compact.db");
        
        // Add many entries
        for (int i = 0; i < 100; i++) {
            storage.append("key" + std::to_string(i), "value" + std::to_string(i));
        }
        
        // Update some
        for (int i = 0; i < 50; i++) {
            storage.append("key" + std::to_string(i), "updated" + std::to_string(i));
        }
        
        // Remove some
        for (int i = 50; i < 75; i++) {
            storage.remove("key" + std::to_string(i));
        }
        
        // Compact
        storage.compact();
    }
    
    {
        Storage storage("test_compact.db");
        auto data = storage.load();
        
        // Should have 75 entries (0-49 updated, 75-99 original)
        assert(data.size() == 75);
        
        for (const auto& kv : data) {
            int key_num = std::stoi(kv.first.substr(3));
            
            if (key_num < 50) {
                assert(kv.second == "updated" + std::to_string(key_num));
            } else if (key_num >= 50 && key_num < 75) {
                assert(false); // These should be deleted
            } else {
                assert(kv.second == "value" + std::to_string(key_num));
            }
        }
    }
    
    std::cout << "✓ Compact passed" << std::endl;
}

void test_empty_file() {
    std::cout << "Testing empty file..." << std::endl;
    
    {
        Storage storage("test_empty.db");
        auto data = storage.load();
        assert(data.empty());
    }
    
    std::cout << "✓ Empty file passed" << std::endl;
}

void test_special_characters() {
    std::cout << "Testing special characters..." << std::endl;
    
    {
        Storage storage("test_special.db");
        storage.append("email", "test@example.com");
        storage.append("path", "/usr/local/bin");
        storage.append("json", "{\"name\":\"value\"}");
    }
    
    {
        Storage storage("test_special.db");
        auto data = storage.load();
        
        for (const auto& kv : data) {
            if (kv.first == "email") {
                assert(kv.second == "test@example.com");
            } else if (kv.first == "path") {
                assert(kv.second == "/usr/local/bin");
            } else if (kv.first == "json") {
                assert(kv.second == "{\"name\":\"value\"}");
            }
        }
    }
    
    std::cout << "✓ Special characters passed" << std::endl;
}

void test_persistence() {
    std::cout << "Testing persistence across instances..." << std::endl;
    
    // Clean up any existing file
    unlink("storage/test_persist_new.db");
    
    {
        Storage storage("test_persist_new.db");
        storage.append("persistent", "data");
    }
    
    {
        Storage storage("test_persist_new.db");
        auto data = storage.load();
        
        std::cout << "After first write, data.size() = " << data.size() << std::endl;
        for (const auto& kv : data) {
            std::cout << "  " << kv.first << " = " << kv.second << std::endl;
        }
        
        assert(data.size() == 1);
        
        bool found = false;
        for (const auto& kv : data) {
            if (kv.first == "persistent" && kv.second == "data") {
                found = true;
            }
        }
        assert(found);
    }
    
    {
        Storage storage("test_persist_new.db");
        storage.append("more", "data");
    }
    
    {
        Storage storage("test_persist_new.db");
        auto data = storage.load();
        
        std::cout << "After second write, data.size() = " << data.size() << std::endl;
        for (const auto& kv : data) {
            std::cout << "  " << kv.first << " = " << kv.second << std::endl;
        }
        
        assert(data.size() == 2);
        
        bool found1 = false, found2 = false;
        for (const auto& kv : data) {
            if (kv.first == "persistent") found1 = true;
            if (kv.first == "more") found2 = true;
        }
        assert(found1 && found2);
    }
    
    std::cout << "✓ Persistence passed" << std::endl;
}

void test_large_values() {
    std::cout << "Testing large values..." << std::endl;
    
    std::string large_value(10000, 'x');
    
    {
        Storage storage("test_large.db");
        storage.append("large", large_value);
        storage.append("small", "y");
    }
    
    {
        Storage storage("test_large.db");
        auto data = storage.load();
        
        for (const auto& kv : data) {
            if (kv.first == "large") {
                assert(kv.second == large_value);
            } else if (kv.first == "small") {
                assert(kv.second == "y");
            }
        }
    }
    
    std::cout << "✓ Large values passed" << std::endl;
}

int main() {
    // Clean up all test files before starting
    unlink("storage/test_basic.db");
    unlink("storage/test_update.db");
    unlink("storage/test_remove.db");
    unlink("storage/test_compact.db");
    unlink("storage/test_empty.db");
    unlink("storage/test_special.db");
    unlink("storage/test_persist_new.db");
    unlink("storage/test_large.db");
    
    try {
        test_empty_file();
        test_basic_append_and_load();
        test_update_value();
        test_remove();
        test_compact();
        test_special_characters();
        test_persistence();
        test_large_values();
        
        std::cout << "\n✓ All storage tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}