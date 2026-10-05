#include "Config.hpp"
#include <iostream>
#include <cassert>

int main() {
    std::cout << "Testing Config implementation..." << std::endl;
    
    nlm::Config config;
    config.set("test", 123);
    
    std::cout << "Test passed!" << std::endl;
    return 0;
}
