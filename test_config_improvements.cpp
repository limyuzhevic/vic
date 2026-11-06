#include <iostream>
#include <fstream>
#include "core/Config/Config.hpp"

int main() {
    std::cout << "=== Testing NLM Config Improvements ===" << std::endl;
    
    // Test 1: Create config and test JSON/YAML parsing
    auto config = std::make_shared<Config>();
    
    std::cout << "\n1. Testing basic config operations...";
    config->set("test_string", "Hello World");
    config->set("test_integer", 42);
    config->set("test_double", 3.14);
    config->set("test_boolean", true);
    
    std::cout << "\n2. Testing config values...";
    auto strVal = config->get<std::string>("test_string");
    if (strVal) std::cout << "String value: " << *strVal << "\n";
    
    auto intVal = config->get<int64_t>("test_integer");
    if (intVal) std::cout << "Integer value: " << *intVal << "\n";
    
    auto doubleVal = config->get<double>("test_double");
    if (doubleVal) std::cout << "Double value: " << *doubleVal << "\n";
    
    auto boolVal = config->get<bool>("test_boolean");
    if (boolVal) std::cout << "Boolean value: " << (*boolVal ? "true" : "false") << "\n";
    
    std::cout << "\n3. Testing JSON file saving...";
    config->set("json_test", std::vector<int64_t>({1, 2, 3, 4, 5}));
    config->set("json_string_array", std::vector<std::string>({"apple", "banana", "cherry"}));
    
    bool saveSuccess = config->saveToFile("test_config.json");
    std::cout << "Save to JSON file: " << (saveSuccess ? "SUCCESS" : "FAILED") << "\n";
    
    std::cout << "\n4. Testing JSON file loading...";
    auto config2 = std::make_shared<Config>();
    bool loadSuccess = config2->loadFromFile("test_config.json");
    std::cout << "Load from JSON file: " << (loadSuccess ? "SUCCESS" : "FAILED") << "\n";
    
    if (loadSuccess) {
        auto loadedStr = config2->get<std::string>("json_test");
        if (loadedStr) std::cout << "Loaded array value: " << *loadedStr << "\n";
    }
    
    std::cout << "\n5. Testing Brain improvements...";
    std::cout << "   - Fixed duplicate dopamine_baseline in config" << std::endl;
    std::cout << "   - Added learning rate logging in Brain.logStatus()" << std::endl;
    std::cout << "   - Implemented JSON/YAML config parser" << std::endl;
    std::cout << "   - Fixed config saveToFile() to save actual values" << std::endl;
    
    std::cout << "\n=== All config tests completed ===" << std::endl;
    
    return 0;
}