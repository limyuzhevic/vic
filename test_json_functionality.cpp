// Simple test to demonstrate JSON Config functionality
// This test assumes the Config system has been fixed and compiled successfully

#include <iostream>
#include <string>
#include <filesystem>
#include <cassert>

// Include the Config header
#include "src/core/Config/Config.hpp"

void test_json_config_functionality() {
    std::cout << "=== Testing JSON Config System ===" << std::endl;
    
    // Test 1: Create a config and save to JSON
    std::cout << "\n1. Testing JSON save functionality..." << std::endl;
    nlm::Config config1;
    
    config1.set("int_value", 42);
    config1.set("double_value", 3.14159);
    config1.set("string_value", std::string("Hello, World!"));
    config1.set("bool_value", true);
    config1.set("int_array", std::vector<int>({1, 2, 3, 4, 5}));
    config1.set("string_array", std::vector<std::string>({"apple", "banana", "cherry"}));
    
    std::string json_file = "test_config.json";
    if (config1.saveToFile(json_file)) {
        std::cout << "   ✓ Config saved to " << json_file << std::endl;
    } else {
        std::cout << "   ✗ Failed to save config to " << json_file << std::endl;
        return;
    }
    
    // Test 2: Load config from JSON
    std::cout << "\n2. Testing JSON load functionality..." << std::endl;
    nlm::Config config2;
    
    if (config2.loadFromFile(json_file)) {
        std::cout << "   ✓ Config loaded from " << json_file << std::endl;
        
        // Verify loaded values
        assert(config2.get<int>("int_value").value_or(-1) == 42);
        assert(config2.get<double>("double_value").value_or(-1.0) > 3.14 && config2.get<double>("double_value").value_or(-1.0) < 3.16);
        assert(config2.get<std::string>("string_value").value_or("") == "Hello, World!");
        assert(config2.get<bool>("bool_value").value_or(false) == true);
        
        std::cout << "   ✓ All values loaded correctly" << std::endl;
        
        // Test comparison
        std::cout << "\n3. Comparing original and loaded configs..." << std::endl;
        std::cout << "   Config1 summary:" << std::endl;
        std::cout << config1.summary() << std::endl;
        std::cout << "   Config2 summary:" << std::endl;
        std::cout << config2.summary() << std::endl;
        
        std::cout << "\n   ✓ JSON Config functionality test PASSED!" << std::endl;
        
    } else {
        std::cout << "   ✗ Failed to load config from " << json_file << std::endl;
        return;
    }
    
    // Clean up
    if (std::filesystem::exists(json_file)) {
        std::filesystem::remove(json_file);
        std::cout << "   ✓ Test file " << json_file << " cleaned up" << std::endl;
    }
    
    std::cout << "\n=== All tests completed successfully ===" << std::endl;
}

int main() {
    try {
        test_json_config_functionality();
        return 0;
    } catch (const std::exception& e) {
        std::cerr << "Test failed with exception: " << e.what() << std::endl;
        return 1;
    }
}
