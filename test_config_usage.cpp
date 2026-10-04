#include "src/core/Config/Config.hpp"
#include <iostream>
#include <string>

int main() {
    nlm::Config config;
    
    std::cout << "Testing Config system..." << std::endl;
    
    // Test setting values using ConfigValue constructor
    config.set("test_int", nlm::ConfigValue(42));
    config.set("test_double", nlm::ConfigValue(3.14));
    config.set("test_string", nlm::ConfigValue(std::string("hello")));
    config.set("test_bool", nlm::ConfigValue(true));
    
    // Test getting values
    auto int_val = config.get<int>("test_int");
    auto double_val = config.get<double>("test_double");
    auto string_val = config.get<std::string>("test_string");
    auto bool_val = config.get<bool>("test_bool");
    
    std::cout << "int: " << (int_val ? std::to_string(*int_val) : "null") << std::endl;
    std::cout << "double: " << (double_val ? std::to_string(*double_val) : "null") << std::endl;
    std::cout << "string: " << (string_val ? *string_val : "null") << std::endl;
    std::cout << "bool: " << (bool_val ? (*bool_val ? "true" : "false") : "null") << std::endl;
    
    // Test getOr with default
    int missing_val = config.getOr<int>("missing", 999);
    std::cout << "missing (with default): " << missing_val << std::endl;
    
    // Test summary
    std::cout << "\n" << config.summary() << std::endl;
    
    return 0;
}
