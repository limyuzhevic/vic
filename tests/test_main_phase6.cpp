// NLM Phase 6 Integration Tests
// Tests complete integration of all Phase 6 systems

#include <iostream>

// Phase 6 integration test functions
namespace test_phase6_memory {
    void runAll();
}

namespace test_phase6_neuromodulation {
    void runAll();
}

namespace test_phase6_prediction {
    void runAll();
}

namespace test_phase6_cognition {
    void runAll();
}

namespace test_phase6_development {
    void runAll();
}

namespace test_phase6_complete {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 6 Integration Tests ===" << std::endl;
    std::cout << "Testing complete integration of all systems:" << std::endl;
    std::cout << "  - Memory systems integration" << std::endl;
    std::cout << "  - Neuromodulation systems integration" << std::endl;
    std::cout << "  - Prediction systems integration" << std::endl;
    std::cout << "  - Cognition systems integration" << std::endl;
    std::cout << "  - Development systems integration" << std::endl;
    std::cout << "  - Complete brain integration" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running Memory Integration tests..." << std::endl;
    try {
        test_phase6_memory::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Neuromodulation Integration tests..." << std::endl;
    try {
        test_phase6_neuromodulation::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Prediction Integration tests..." << std::endl;
    try {
        test_phase6_prediction::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Cognition Integration tests..." << std::endl;
    try {
        test_phase6_cognition::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Development Integration tests..." << std::endl;
    try {
        test_phase6_development::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Complete Integration tests..." << std::endl;
    try {
        test_phase6_complete::runAll();
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