// NLM Test Main - Phase 6 Integration Tests
// Testing the Phase 6 integration experiment structure

#include <iostream>
#include <cassert>

// Forward declarations of test functions
namespace test_phase6 {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing the Phase 6 integration experiment structure:" << std::endl;
    std::cout << "  - Phase6IntegratedExperiment" << std::endl;
    std::cout << "  - Phase6Demo" << std::endl;
    std::cout << "  - Memory integration" << std::endl;
    std::cout << "  - Neuromodulation integration" << std::endl;
    std::cout << "  - Checkpointing" << std::endl;
    std::cout << "  - Replay" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running Phase 6 integration tests..." << std::endl;
    try {
        test_phase6::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Phase 6 Integration Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Phase 6 Integration Tests FAILED ===" << std::endl;
        return 1;
    }
}