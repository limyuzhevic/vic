// NLM Test Main - Agent System Tests
// Testing the new refactored agent structure

#include <iostream>
#include <cassert>

// Forward declarations of test functions
namespace test_agent {
    void runAll();
}

int main() {
    std::cout << "=== NLM Agent System Tests ===" << std::endl;
    std::cout << "Testing the new refactored agent structure:" << std::endl;
    std::cout << "  - Agent Brain Interface" << std::endl;
    std::cout << "  - Sensory Percept" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running Agent tests..." << std::endl;
    try {
        test_agent::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Agent Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Agent Tests FAILED ===" << std::endl;
        return 1;
    }
}