// Test program to verify Config.cpp changes
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <fstream>

int main() {
    nlm::Config config;
    
    // Test setting various values
    config.set("string_key", std::string("test_value"), nlm::ConfigSource::Runtime);
    config.set("int_key", 42, nlm::ConfigSource::Runtime);
    config.set("double_key", 3.14, nlm::ConfigSource::Runtime);
    config.set("bool_key", true, nlm::ConfigSource::Runtime);
    
    // Test getting values
    auto strVal = config.get<std::string>("string_key");
    auto intVal = config.get<int>("int_key");
    auto dblVal = config.get<double>("double_key");
    auto boolVal = config.get<bool>("bool_key");
    
    std::cout << "String value: " << (strVal ? strVal.value() : "NULL") << std::endl;
    std::cout << "Int value: " << (intVal ? intVal.value() : -1) << std::endl;
    std::cout << "Double value: " << (dblVal ? dblVal.value() : -1.0) << std::endl;
    std::cout << "Bool value: " << (boolVal ? boolVal.value() : false) << std::endl;
    
    // Test saveToFile
    bool saved = config.saveToFile("test_config.cfg");
    std::cout << "Saved to file: " << (saved ? "Yes" : "No") << std::endl;
    
    // Test loadFromFile (simple format)
    nlm::Config config2;
    bool loaded = config2.loadFromFile("test_config.cfg");
    std::cout << "Loaded from file: " << (loaded ? "Yes" : "No") << std::endl;
    
    return 0;
}