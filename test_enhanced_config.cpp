#include <iostream>
#include "core/Config/Config.hpp"

int main() {
    std::cout << "Testing Enhanced Config System" << std::endl;
    
    nlm::Config config;
    
    // Test basic functionality
    config.set("test_int", 42, nlm::ConfigSource::Runtime);
    config.set("test_string", "Hello World", nlm::ConfigSource::Runtime);
    config.set("test_double", 3.14159, nlm::ConfigSource::Runtime);
    config.set("test_bool", true, nlm::ConfigSource::Runtime);
    
    // Test get methods
    auto intVal = config.get<int>("test_int");
    if (intVal.has_value()) {
        std::cout << "test_int = " << *intVal << std::endl;
    }
    
    auto stringVal = config.get<std::string>("test_string");
    if (stringVal.has_value()) {
        std::cout << "test_string = " << *stringVal << std::endl;
    }
    
    // Test summary
    std::cout << "Config summary:" << std::endl << config.summary();
    
    return 0;
}