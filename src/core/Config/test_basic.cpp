#include "Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Config implementation..." << std::endl;
    
    nlm::Config config;
    
    // Test basic setting and getting
    config.set("test_int", 42);
    assert(config.has("test_int"));
    
    auto intVal = config.get<int>("test_int");
    assert(intVal.has_value());
    assert(*intVal == 42);
    
    std::cout << "Basic test passed!" << std::endl;
    return 0;
}
