// Simple test to verify Config changes
#include "Config.hpp"
#include <iostream>

int main() {
    std::cout << "Testing Config changes..." << std::endl;
    
    nlm::Config config;
    
    // Test setting values
    config.set("test_int", 42);
    config.set("test_string", std::string("hello"));
    config.set("test_bool", true);
    config.set("test_double", 3.14);
    
    // Test getting values
    auto int_val = config.get<int>("test_int");
    auto str_val = config.get<std::string>("test_string");
    auto bool_val = config.get<bool>("test_bool");
    auto double_val = config.get<double>("test_double");
    
    std::cout << "Get int: " << (int_val ? std::to_string(*int_val) : "NOT FOUND") << std::endl;
    std::cout << "Get string: " << (str_val ? *str_val : "NOT FOUND") << std::endl;
    std::cout << "Get bool: " << (bool_val ? std::to_string(*bool_val) : "NOT FOUND") << std::endl;
    std::cout << "Get double: " << (double_val ? std::to_string(*double_val) : "NOT FOUND") << std::endl;
    
    // Test getOr values
    int int_default = config.getOr<int>("test_int", 0);
    std::string str_default = config.getOr<std::string>("test_string", "default");
    bool bool_default = config.getOr<bool>("test_bool", false);
    double double_default = config.getOr<double>("test_double", 0.0);
    
    std::cout << "GetOr int: " << int_default << std::endl;
    std::cout << "GetOr string: " << str_default << std::endl;
    std::cout << "GetOr bool: " << bool_default << std::endl;
    std::cout << "GetOr double: " << double_default << std::endl;
    
    std::cout << "\nConfig changes test completed successfully!" << std::endl;
    
    return 0;
}