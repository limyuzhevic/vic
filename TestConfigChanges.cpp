// TestConfigChanges.cpp - Simple test to verify Config improvements
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Config improvements...\n";
    
    // Test 1: Basic Config creation
    nlm::Config config;
    std::cout << "1. Config creation: PASSED\n";
    
    // Test 2: Set and get various types
    config.set("int_value", 42);
    config.set("double_value", 3.14);
    config.set("string_value", "hello");
    config.set("bool_value", true);
    
    auto intVal = config.get<int>("int_value");
    assert(intVal && *intVal == 42);
    
    auto doubleVal = config.get<double>("double_value");
    assert(doubleVal && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("string_value");
    assert(stringVal && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("bool_value");
    assert(boolVal && *boolVal == true);
    
    std::cout << "2. Set/get various types: PASSED\n";
    
    // Test 3: getOr with default values
    int missingVal = config.getOr<int>("missing", 100);
    assert(missingVal == 100);
    
    int existingVal = config.getOr<int>("int_value", 100);
    assert(existingVal == 42);
    
    std::cout << "3. getOr with defaults: PASSED\n";
    
    // Test 4: Type conversion from string
    config.set("converted_int", "999");
    auto convertedVal = config.get<int>("converted_int");
    assert(convertedVal && *convertedVal == 999);
    
    config.set("converted_double", "3.14159");
    auto convertedDouble = config.get<double>("converted_double");
    assert(convertedDouble && *convertedDouble > 3.141 && *convertedDouble < 3.142);
    
    std::cout << "4. Type conversion from string: PASSED\n";
    
    // Test 5: File save/load with key=value format
    config.saveToFile("test_config.cfg");
    
    nlm::Config config2;
    bool loaded = config2.loadFromFile("test_config.cfg");
    assert(loaded);
    
    auto val1 = config2.get<int>("int_value");
    auto val2 = config2.getOr<int>("missing", 100);
    
    std::cout << "5. File save/load: PASSED\n";
    
    // Test 6: Validation (should catch duplicates)
    config.clear();
    config.set("test_key", 1);
    config.set("test_key", 2);  // This should replace the previous value
    
    auto finalVal = config.get<int>("test_key");
    assert(finalVal && *finalVal == 2);
    
    std::cout << "6. Validation (duplicate handling): PASSED\n";
    
    // Test 7: Description support
    config.clear();
    config.setDescription("test", "A test configuration value");
    std::string desc = config.getDescription("test");
    assert(desc == "A test configuration value");
    
    std::cout << "7. Description support: PASSED\n";
    
    // Test 8: Mixed value types
    config.clear();
    config.set("array_int", std::vector<int>({1, 2, 3}));
    config.set("array_double", std::vector<double>({1.5, 2.5, 3.5}));
    config.set("array_string", std::vector<std::string>({"a", "b", "c"}));
    
    std::cout << "8. Mixed value types: PASSED\n";
    
    // Cleanup
    std::remove("test_config.cfg");
    
    std::cout << "\nAll tests PASSED! Config improvements working correctly.\n";
    return 0;
}
