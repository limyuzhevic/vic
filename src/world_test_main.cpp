// NLM Test Main - World System Tests
// Testing the new refactored world structure

#include <iostream>
#include <cassert>

// Forward declarations of test functions
namespace test_world {
    void runAll();
}

int main() {
    std::cout << "=== NLM World System Tests ===" << std::endl;
    std::cout << "Testing the new refactored world structure:" << std::endl;
    std::cout << "  - SimpleWorld" << std::endl;
    std::cout << "  - Observation" << std::endl;
    std::cout << "  - Action" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running World tests..." << std::endl;
    try {
        test_world::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All World Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some World Tests FAILED ===" << std::endl;
        return 1;
    }
}