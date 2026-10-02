// NLM Test Main - Phase 4 Experiment Tests
// Testing the Phase 4 experiment structure

#include <iostream>
#include <cassert>

// Forward declarations of test functions
namespace test_phase4 {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 4 Experiment Tests ===" << std::endl;
    std::cout << "Testing the Phase 4 experiment structure:" << std::endl;
    std::cout << "  - Phase4Experiment" << std::endl;
    std::cout << "  - Phase4Demo" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running Phase 4 tests..." << std::endl;
    try {
        test_phase4::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Phase 4 Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Phase 4 Tests FAILED ===" << std::endl;
        return 1;
    }
}