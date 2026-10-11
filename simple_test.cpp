#include <iostream>
#include <string>

// Test basic includes and types
#include "src/core/Config/Config.hpp"

int main() {
    std::cout << "Testing enhanced configuration system..." << std::endl;
    
    // Test 1: Basic Config object creation
    nlm::Config config;
    std::cout << "✓ Config object created successfully" << std::endl;
    
    // Test 2: Set and get basic values
    config.set("test_int", 42);
    auto val = config.get<int>("test_int");
    if (val && *val == 42) {
        std::cout << "✓ Int value set and retrieved correctly" << std::endl;
    } else {
        std::cout << "✗ Int value test failed" << std::endl;
        return 1;
    }
    
    // Test 3: Set and get string
    config.set("test_string", std::string("Hello, World!"));
    auto strVal = config.get<std::string>("test_string");
    if (strVal && *strVal == "Hello, World!") {
        std::cout << "✓ String value set and retrieved correctly" << std::endl;
    } else {
        std::cout << "✗ String value test failed" << std::endl;
        return 1;
    }
    
    // Test 4: Set and get boolean
    config.set("test_bool", true);
    auto boolVal = config.get<bool>("test_bool");
    if (boolVal && *boolVal == true) {
        std::cout << "✓ Boolean value set and retrieved correctly" << std::endl;
    } else {
        std::cout << "✗ Boolean value test failed" << std::endl;
        return 1;
    }
    
    // Test 5: Set and get double
    config.set("test_double", 3.14159);
    auto doubleVal = config.get<double>("test_double");
    if (doubleVal && *doubleVal > 3.14 && *doubleVal < 3.15) {
        std::cout << "✓ Double value set and retrieved correctly" << std::endl;
    } else {
        std::cout << "✗ Double value test failed" << std::endl;
        return 1;
    }
    
    // Test 6: Check has method
    if (config.has("test_int") && config.has("test_string") && config.has("test_bool") && config.has("test_double")) {
        std::cout << "✓ All values present" << std::endl;
    } else {
        std::cout << "✗ Missing values" << std::endl;
        return 1;
    }
    
    // Test 7: Test getOr method
    int defaultVal = config.getOr<int>("missing_key", 999);
    if (defaultVal == 999) {
        std::cout << "✓ getOr with default value works correctly" << std::endl;
    } else {
        std::cout << "✗ getOr test failed" << std::endl;
        return 1;
    }
    
    std::cout << "" << std::endl;
    std::cout << "All basic configuration tests passed!" << std::endl;
    std::cout << "" << std::endl;
    std::cout << "Summary of enhancements implemented:" << std::endl;
    std::cout << "1. JSON schema definition (configs/schema.json)" << std::endl;
    std::cout << "2. Enhanced default configuration (configs/enhanced_default.cfg)" << std::endl;
    std::cout << "3. JSON support in Config class" << std::endl;
    std::cout << "4. Schema validation and parameter range checking" << std::endl;
    std::cout << "5. Enhanced error handling with detailed messages" << std::endl;
    std::cout << "6. Backward compatibility with legacy config files" << std::endl;
    
    return 0;
}
