// NLM Test Main - Phase 3 Experiment Tests
// Testing the Phase 3 experiment structure

#include <iostream>
#include <cassert>

// Forward declarations of test functions
namespace test_phase3 {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 3 Experiment Tests ===" << std::endl;
    std::cout << "Testing the Phase 3 experiment structure:" << std::endl;
    std::cout << "  - Phase3Experiment" << std::endl;
    std::cout << "  - Phase3Demo" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running Phase 3 tests..." << std::endl;
    try {
        test_phase3::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Phase 3 Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Phase 3 Tests FAILED ===" << std::endl;
        return 1;
    }
}