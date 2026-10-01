#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing NLM Configuration System..." << std::endl;
    
    // Test 1: Create and initialize configuration
    nlm::Config config;
    assert(!config.has("test_key"));
    std::cout << "✓ Test 1: Configuration created successfully" << std::endl;
    
    // Test 2: Set various types of values
    config.set("int_value", 42, nlm::ConfigSource::Runtime);
    config.set("double_value", 3.14, nlm::ConfigSource::Runtime);
    config.set("string_value", std::string("hello"), nlm::ConfigSource::Runtime);
    config.set("bool_value", true, nlm::ConfigSource::Runtime);
    
    assert(config.has("int_value"));
    assert(config.has("double_value"));
    assert(config.has("string_value"));
    assert(config.has("bool_value"));
    std::cout << "✓ Test 2: Basic value types set successfully" << std::endl;
    
    // Test 3: Get values
    auto intVal = config.get<int>("int_value");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal.has_value() && *boolVal == true);
    std::cout << "✓ Test 3: Values retrieved successfully" << std::endl;
    
    // Test 4: Get with default values
    int missingVal = config.getOr<int>("missing_key", 100);
    assert(missingVal == 100);
    
    std::string missingStringVal = config.getOr<std::string>("missing_key2", "default");
    assert(missingStringVal == "default");
    std::cout << "✓ Test 4: Default values work correctly" << std::endl;
    
    // Test 5: Remove and check
    config.remove("int_value");
    assert(!config.has("int_value"));
    std::cout << "✓ Test 5: Remove operation works" << std::endl;
    
    // Test 6: Get all keys
    auto keys = config.getKeys();
    assert(keys.size() == 3); // double_value, string_value, bool_value
    std::cout << "✓ Test 6: GetKeys works correctly" << std::endl;
    
    // Test 7: Clear configuration
    config.clear();
    assert(!config.has("double_value"));
    assert(config.getKeys().empty());
    std::cout << "✓ Test 7: Clear operation works" << std::endl;
    
    // Test 8: Error handling - invalid key
    bool hasInvalid = config.has("invalid!@#key");
    assert(!hasInvalid); // Should not have invalid key
    std::cout << "✓ Test 8: Error handling for invalid keys" << std::endl;
    
    // Test 9: Summary generation
    std::string summary = config.summary();
    assert(!summary.empty());
    std::cout << "✓ Test 9: Summary generation works" << std::endl;
    
    std::cout << "\n✅ All tests passed! Configuration system is working correctly." << std::endl;
    
    return 0;
}