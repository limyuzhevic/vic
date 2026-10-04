#include "src/core/Config/Config.hpp"
#include <iostream>
#include <cassert>
#include <string>

int main() {
    std::cout << "Testing Config system..." << std::endl;
    
    nlm::Config config;
    
    // Test basic functionality
    config.set("test_int", 42);
    config.set("test_double", 3.14);
    config.set("test_string", std::string("hello"));
    config.set("test_bool", true);
    
    // Test get methods
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value() && *intVal == 42);
    
    auto doubleVal = config.get<double>("test_double");
    assert(doubleVal.has_value() && *doubleVal > 3.13 && *doubleVal < 3.15);
    
    auto stringVal = config.get<std::string>("test_string");
    assert(stringVal.has_value() && *stringVal == "hello");
    
    auto boolVal = config.get<bool>("test_bool");
    assert(boolVal.has_value() && *boolVal == true);
    
    // Test getOr with default
    int missingVal = config.getOr<int>("missing", 100);
    assert(missingVal == 100);
    
    // Test has and remove
    assert(config.has("test_int"));
    config.remove("test_int");
    assert(!config.has("test_int"));
    
    std::cout << "All basic tests passed!" << std::endl;
    
    // Test JSON save/load
    config.set("json_int", 123);
    config.set("json_string", std::string("json_value"));
    config.set("json_float", 45.67);
    config.set("json_bool", false);
    
    bool saveSuccess = config.saveToFile("/tmp/test_config.json");
    std::cout << "JSON save successful: " << (saveSuccess ? "true" : "false") << std::endl;
    
    nlm::Config config2;
    bool loadSuccess = config2.loadFromFile("/tmp/test_config.json");
    std::cout << "JSON load successful: " << (loadSuccess ? "true" : "false") << std::endl;
    
    if (loadSuccess) {
        auto loadedInt = config2.get<int>("json_int");
        auto loadedString = config2.get<std::string>("json_string");
        
        if (loadedInt.has_value() && loadedString.has_value()) {
            std::cout << "Loaded json_int: " << *loadedInt << std::endl;
            std::cout << "Loaded json_string: " << *loadedString << std::endl;
            std::cout << "JSON round-trip test PASSED!" << std::endl;
        }
    }
    
    std::cout << "Config system test completed successfully!" << std::endl;
    return 0;
}
