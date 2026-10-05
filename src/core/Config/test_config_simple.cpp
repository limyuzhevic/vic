#include "Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Config implementation..." << std::endl;
    
    // Test 1: Basic creation and setting
    nlm::Config config;
    assert(!config.has("test_int"));
    
    config.set("test_int", 42);
    assert(config.has("test_int"));
    
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    // Test 2: Multiple types
    config.set("test_double", 3.14159);
    config.set("test_string", std::string("Hello World"));
    config.set("test_bool", true);
    
    assert(config.get<double>("test_double").value() > 3.14 && 
           config.get<double>("test_double").value() < 3.15);
    assert(config.get<std::string>("test_string").value() == "Hello World");
    assert(config.get<bool>("test_bool").value() == true);
    
    // Test 3: getOr with default
    int missing = config.getOr<int>("missing_key", 999);
    assert(missing == 999);
    
    // Test 4: Remove
    config.remove("test_int");
    assert(!config.has("test_int"));
    
    // Test 5: Keys
    config.set("key1", 1);
    config.set("key2", 2);
    config.set("key3", 3);
    
    auto keys = config.getKeys();
    assert(keys.size() == 6);  // test_double, test_string, test_bool, key1, key2, key3
    assert(std::find(keys.begin(), keys.end(), "test_double") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "test_string") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "test_bool") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key1") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key2") != keys.end());
    assert(std::find(keys.begin(), keys.end(), "key3") != keys.end());
    
    // Test 6: Thread safety (basic test - multiple sets)
    config.set("thread_test", 1);
    config.set("thread_test", 2); // Update
    assert(config.get<int>("thread_test").value() == 2);
    
    // Test 7: Summary
    std::string summary = config.summary();
    assert(!summary.empty());
    assert(summary.find("Configuration") != std::string::npos);
    
    // Test 8: Validation (basic check)
    config.validate(); // Should not throw
    assert(!config.hasErrors());
    
    std::cout << "All tests passed!" << std::endl;
    
    return 0;
}
