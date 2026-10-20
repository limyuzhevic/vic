// Test program for the enhanced Config system
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing enhanced Config system..." << std::endl;
    
    // Test 1: Basic Config creation
    nlm::Config config;
    assert(!config.has("test"));
    std::cout << "✓ Basic Config creation passed" << std::endl;
    
    // Test 2: Setting and getting different types
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", std::string("hello"));
    config.set("bool_value", true);
    config.set("int_vector", std::vector<int>{1, 2, 3});
    config.set("string_vector", std::vector<std::string>{"a", "b", "c"});
    
    assert(config.getInt("int_value").value() == 42);
    assert(config.getDouble("double_value").value() == 3.14);
    assert(config.getString("string_value").value() == "hello");
    assert(config.getBool("bool_value").value() == true);
    assert(config.getIntVector("int_vector").value() == std::vector<int>{1, 2, 3});
    assert(config.getStringVector("string_value") == std::vector<std::string>{"a", "b", "c"});
    std::cout << "✓ Type-specific getters passed" << std::endl;
    
    // Test 3: getOr with defaults
    int val = config.getOr<int>("missing", 100);
    assert(val == 100);
    val = config.getOr<int>("int_value", 100);
    assert(val == 42);
    std::cout << "✓ getOr with defaults passed" << std::endl;
    
    // Test 4: Configuration categories and metadata
    config.setCategory("test_key", "test_category");
    config.setDescription("test_key", "Test key for category");
    config.setType("test_key", "string");
    config.setRequired("test_key", true);
    
    assert(config.getCategory("test_key") == "test_category");
    assert(config.getDescription("test_key") == "Test key for category");
    assert(config.getType("test_key") == "string");
    assert(config.isRequired("test_key") == true);
    std::cout << "✓ Categories and metadata passed" << std::endl;
    
    // Test 5: Remove and clear
    config.set("temp", 123);
    assert(config.has("temp"));
    config.remove("temp");
    assert(!config.has("temp"));
    std::cout << "✓ Remove passed" << std::endl;
    
    // Test 6: Get keys
    auto keys = config.getKeys();
    assert(keys.size() >= 6); // At least int_value, double_value, string_value, bool_value, int_vector, string_vector
    std::cout << "✓ Get keys passed" << std::endl;
    
    // Test 7: JSON export/import
    std::string jsonStr = config.toJSON(true);
    nlm::Config config2;
    config2.fromJSON(jsonStr);
    assert(config2.getInt("int_value").value() == 42);
    assert(config2.getString("string_value").value() == "hello");
    std::cout << "✓ JSON export/import passed" << std::endl;
    
    // Test 8: Simple format export
    std::string simpleStr = config.toSimple(true);
    assert(!simpleStr.empty());
    assert(simpleStr.find("#") != std::string::npos); // Should have comments
    std::cout << "✓ Simple format export passed" << std::endl;
    
    // Test 9: File loading (JSON)
    bool jsonLoadSuccess = config.loadFromFile("test_config.json", false);
    if (jsonLoadSuccess) {
        assert(config.getInt("test_int").has_value());
        std::cout << "✓ JSON file loading passed" << std::endl;
    } else {
        std::cout << "! JSON file loading skipped (file doesn't exist)" << std::endl;
    }
    
    // Test 10: File loading (simple)
    bool simpleLoadSuccess = config.loadFromFile("test_config.cfg", false);
    if (simpleLoadSuccess) {
        assert(config.getString("test_string").has_value());
        std::cout << "✓ Simple file loading passed" << std::endl;
    } else {
        std::cout << "! Simple file loading skipped (file doesn't exist)" << std::endl;
    }
    
    std::cout << "\nAll tests passed!" << std::endl;
    return 0;
}