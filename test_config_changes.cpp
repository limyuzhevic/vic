// Simple test program to verify Config changes
#include "Config.hpp"
#include <iostream>
#include <fstream>

int main() {
    std::cout << "Testing Config changes...\n";
    
    // Test 1: Create a Config object and set some values
    nlm::Config config;
    
    config.set("test_int", 42);
    config.set("test_string", std::string("hello"));
    config.set("test_bool", true);
    config.set("test_double", 3.14);
    
    // Test 2: Save to JSON file
    bool saveResult = config.saveToFile("test_config.json");
    std::cout << "Save to JSON: " << (saveResult ? "SUCCESS" : "FAILED") << "\n";
    
    // Test 3: Load from JSON file
    nlm::Config config2;
    bool loadResult = config2.loadFromFile("test_config.json");
    std::cout << "Load from JSON: " << (loadResult ? "SUCCESS" : "FAILED") << "\n";
    
    // Test 4: Test get functionality
    auto intValue = config2.get<int>("test_int");
    auto stringValue = config2.get<std::string>("test_string");
    auto boolValue = config2.get<bool>("test_bool");
    auto doubleValue = config2.get<double>("test_double");
    
    std::cout << "Get int: " << (intValue ? std::to_string(*intValue) : "NOT FOUND") << "\n";
    std::cout << "Get string: " << (stringValue ? *stringValue : "NOT FOUND") << "\n";
    std::cout << "Get bool: " << (boolValue ? std::to_string(*boolValue) : "NOT FOUND") << "\n";
    std::cout << "Get double: " << (doubleValue ? std::to_string(*doubleValue) : "NOT FOUND") << "\n";
    
    // Test 5: Test getOr functionality
    int intDefault = config2.getOr<int>("test_int", 0);
    double doubleDefault = config2.getOr<double>("test_double", 0.0);
    
    std::cout << "GetOr int: " << intDefault << "\n";
    std::cout << "GetOr string: " << stringDefault << "\n";
    std::cout << "GetOr bool: " << boolDefault << "\n";
    std::cout << "GetOr double: " << doubleDefault << "\n";
    
    std::cout << "\nAll tests completed!\n";
    
    return 0;
}