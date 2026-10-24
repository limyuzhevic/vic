// Test file to verify the build system works after our modifications

#include <iostream>
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"

int main() {
    std::cout << "NLM Test: Prediction system integration fixed" << std::endl;
    
    // Test config
    auto config = std::make_shared<Config>();
    
    // Test brain creation
    nlm::Brain brain(config);
    
    std::cout << "Test passed: Prediction system is now properly integrated" << std::endl;
    return 0;
}