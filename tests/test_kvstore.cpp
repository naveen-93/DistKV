#include "kvstore.hpp"
#include <iostream>
#include <cassert>

void test_basic_operations() {
    std::cout << "Testing basic operations..." << std::endl;
    
    KVStore kv("test.db");
    
    // Test put
    assert(kv.put("name", "John"));
    assert(kv.put("age", "25"));
    assert(kv.put("city", "NYC"));
    
    // Test get
    std::string val;
    assert(kv.get("name", val) && val == "John");
    assert(kv.get("age", val) && val == "25");
    assert(kv.get("city", val) && val == "NYC");
    
    // Test non-existent key
    assert(!kv.get("country", val));
    
    // Test update
    assert(kv.put("age", "26"));
    assert(kv.get("age", val) && val == "26");
    
    std::cout << "✓ Basic operations passed" << std::endl;
}

void test_remove() {
    std::cout << "Testing remove..." << std::endl;
    
    KVStore kv("test.db");
    
    kv.put("key1", "value1");
    kv.put("key2", "value2");
    
    // Remove existing key
    assert(kv.remove("key1"));
    
    std::string val;
    assert(!kv.get("key1", val));
    assert(kv.get("key2", val) && val == "value2");
    
    // Remove non-existent key
    assert(!kv.remove("key1"));
    assert(!kv.remove("nonexistent"));
    
    std::cout << "✓ Remove passed" << std::endl;
}

void test_persistence() {
    std::cout << "Testing persistence..." << std::endl;
    
    // Create store and add data
    {
        KVStore kv("test_persist.db");
        kv.put("persistent", "data");
        kv.put("another", "value");
        kv.persist();
    }
    
    // Reopen and verify data persisted
    {
        KVStore kv("test_persist.db");
        std::string val;
        assert(kv.get("persistent", val) && val == "data");
        assert(kv.get("another", val) && val == "value");
    }
    
    std::cout << "✓ Persistence passed" << std::endl;
}

void test_empty_key() {
    std::cout << "Testing empty key..." << std::endl;
    
    KVStore kv("test.db");
    
    // Empty key should fail
    assert(!kv.put("", "value"));
    
    std::cout << "✓ Empty key validation passed" << std::endl;
}

void test_compaction() {
    std::cout << "Testing compaction..." << std::endl;
    
    KVStore kv("test_compact.db");
    
    // Add and remove many keys
    for (int i = 0; i < 100; i++) {
        kv.put("key" + std::to_string(i), "value" + std::to_string(i));
    }
    
    for (int i = 0; i < 50; i++) {
        kv.remove("key" + std::to_string(i));
    }
    
    // Compact should clean up deleted entries
    kv.persist();
    
    // Verify remaining keys
    std::string val;
    for (int i = 0; i < 50; i++) {
        assert(!kv.get("key" + std::to_string(i), val));
    }
    for (int i = 50; i < 100; i++) {
        assert(kv.get("key" + std::to_string(i), val));
        assert(val == "value" + std::to_string(i));
    }
    
    std::cout << "✓ Compaction passed" << std::endl;
}

int main() {
    try {
        test_basic_operations();
        test_remove();
        test_persistence();
        test_empty_key();
        test_compaction();
        
        std::cout << "\n✓ All tests passed!" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    }
}