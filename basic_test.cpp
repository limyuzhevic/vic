#include "core/Config/Config.hpp"
#include <iostream>
#include <fstream>
#include <string>

// Simple test to verify basic functionality matches Python bindings expectations
int main() {
    std::cout << "=== Testing Basic Config Functionality ===" << std::endl;
    
    nlm::Config config;
    
    // Test basic set/get operations (matching Python bindings API)
    std::cout << "1. Testing set/get string operations..." << std::endl;
    config.set("test_key", std::string("test_value"));
    if (config.has("test_key")) {
        std::string value = config.get<std::string>("test_key").value();
        std::cout << "   PASS: Retrieved string value: '" << value << "'" << std::endl;
    } else {
        std::cout << "   FAIL: Key not found after set" << std::endl;
        return 1;
    }
    
    // Test getInt
    std::cout << "2. Testing set/get int operations..." << std::endl;
    config.set("int_key", 42);
    int intValue = config.get<int>("int_key").value();
    std::cout << "   PASS: Retrieved int value: " << intValue << std::endl;
    
    // Test getDouble
    std::cout << "3. Testing set/get double operations..." << std::endl;
    config.set("double_key", 3.14159);
    double doubleValue = config.get<double>("double_key").value();
    std::cout << "   PASS: Retrieved double value: " << doubleValue << std::endl;
    
    // Test getBool
    std::cout << "4. Testing set/get bool operations..." << std::endl;
    config.set("bool_key", true);
    bool boolValue = config.get<bool>("bool_key").value();
    std::cout << "   PASS: Retrieved bool value: " << (boolValue ? "true" : "false") << std::endl;
    
    // Test getOr with missing key
    std::cout << "5. Testing getOr with missing key..." << std::endl;
    int defaultValue = config.getOr<int>("missing_key", 999);
    std::cout << "   PASS: Retrieved default value for missing key: " << defaultValue << std::endl;
    
    // Test has with non-existent key
    std::cout << "6. Testing has with non-existent key..." << std::endl;
    if (!config.has("non_existent_key")) {
        std::cout << "   PASS: Correctly reports non-existent key" << std::endl;
    } else {
        std::cout << "   FAIL: Reports non-existent key as existing" << std::endl;
        return 1;
    }
    
    // Test set with string (should work as per Python bindings)
    std::cout << "7. Testing set with string..." << std::endl;
    config.set("string_set", "new_string_value");
    std::string newString = config.get<std::string>("string_set").value();
    std::cout << "   PASS: Set and retrieved new string value: '" << newString << "'" << std::endl;
    
    // Test getKeys
    std::cout << "8. Testing getKeys..." << std::endl;
    std::vector<std::string> keys = config.getKeys();
    std::cout << "   PASS: Retrieved " << keys.size() << " keys" << std::endl;
    
    // Test clear
    std::cout << "9. Testing clear..." << std::endl;
    config.clear();
    if (config.getKeys().empty()) {
        std::cout << "   PASS: Config cleared successfully" << std::endl;
    } else {
        std::cout << "   FAIL: Config not cleared" << std::endl;
        return 1;
    }
    
    std::cout << "\n=== Basic Config Tests PASSED ===" << std::endl;
    
    return 0;
}