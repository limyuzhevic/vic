#!/usr/bin/env g++11
// Quick test to verify basic functionality still works

#include "core/Config/Config.hpp"
#include <cassert>
#include <iostream>

int main() {
    std::cout << "Testing basic Config functionality..." << std::endl;
    
    // Test 1: Config creation
    nlm::Config config;
    assert(!config.has("test_key"));
    std::cout << "✓ Config creation test passed" << std::endl;
    
    // Test 2: Setting and getting values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    assert(config.has("int_value"));
    
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value());
    assert(*intVal == 42);
    std::cout << "✓ Setting and getting int test passed" << std::endl;
    
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value());
    assert(*doubleVal > 3.13 && *doubleVal < 3.15);
    std::cout << "✓ Setting and getting double test passed" << std::endl;
    
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value());
    assert(*stringVal == "hello");
    std::cout << "✓ Setting and getting string test passed" << std::endl;
    
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value());
    assert(*boolVal == true);
    std::cout << "✓ Setting and getting bool test passed" << std::endl;
    
    // Test 3: GetOr functionality
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    std::cout << "✓ GetOr for missing value test passed" << std::endl;
    
    config.set("existing", 50, nlm::ConfigSource::Runtime);
    val = config.getOr<int>("existing", 100);
    assert(val == 50);
    std::cout << "✓ GetOr for existing value test passed" << std::endl;
    
    // Test 4: Remove functionality
    config.set("test", 123, nlm::ConfigSource::Runtime);
    assert(config.has("test"));
    config.remove("test");
    assert(!config.has("test"));
    std::cout << "✓ Remove test passed" << std::endl;
    
    // Test 5: Get all keys
    config.set("key1", 1, nlm::ConfigSource::Runtime);
    config.set("key2", 2, nlm::ConfigSource::Runtime);
    config.set("key3", 3, nlm::ConfigSource::Runtime);
    
    auto keys = config.getKeys();
    assert(keys.size() == 3);
    std::cout << "✓ Get keys test passed" << std::endl;
    
    // Test 6: Summary functionality
    std::string summary = config.summary();
    assert(!summary.empty());
    std::cout << "✓ Summary test passed" << std::endl;
    
    std::cout << "\n✅ All basic Config functionality tests passed!" << std::endl;
    std::cout << "The enhanced Config class maintains backward compatibility while adding advanced features." << std::endl;
    
    return 0;
}
