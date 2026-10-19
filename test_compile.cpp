// Quick test to see if we can compile Config.cpp
#include "src/core/Config/Config.hpp"
#include <iostream>

int main() {
    nlm::Config config;
    std::cout << "Config created successfully" << std::endl;
    
    // Test some basic operations
    config.set("test_int", 42);
    config.set("test_float", 3.14f);
    config.set("test_bool", true);
    config.set("test_string", "hello");
    
    std::cout << config.summary() << std::endl;
    return 0;
}
