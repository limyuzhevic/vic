// NLM Test Main
// Phase 6: Final Integration Tests
//
// This file provides comprehensive testing of the integrated NLM brain systems.
// Includes both Phase 2 core functionality tests and Phase 6 integration tests.

#include <iostream>
#include <cassert>

// Forward declarations of Phase 2 test functions
namespace test_types {
    void runAll();
}

namespace test_config {
    void runAll();
}

namespace test_random {
    void runAll();
}

namespace test_clock {
    void runAll();
}

namespace test_neuron {
    void runAll();
}

namespace test_synapse {
    void runAll();
}

namespace test_stdp {
    void runAll();
}

namespace test_brain {
    void runAll();
}

// Forward declarations of Phase 6 integration test functions
namespace test_phase6 {
    void runAll();
}

int main() {
    std::cout << "=== NLM Phase 6: Comprehensive Integration Tests ===" << std::endl;
    std::cout << "Testing the complete integrated artificial brain:" << std::endl;
    std::cout << "  - Core neural computation (Phase 2)" << std::endl;
    std::cout << "  - Memory systems integration" << std::endl;
    std::cout << "  - Neuromodulation systems" << std::endl;
    std::cout << "  - Prediction and cognition" << std::endl;
    std::cout << "  - Developmental processes" << std::endl;
    std::cout << "  - Checkpoint and replay systems" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Run Phase 2 core functionality tests
    std::cout << "=== Phase 2: Core Neural Computation Tests ===" << std::endl;
    
    std::cout << "Running Types tests..." << std::endl;
    try {
        test_types::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Config tests..." << std::endl;
    try {
        test_config::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Random tests..." << std::endl;
    try {
        test_random::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Clock tests..." << std::endl;
    try {
        test_clock::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Neuron tests..." << std::endl;
    try {
        test_neuron::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Synapse tests..." << std::endl;
    try {
        test_synapse::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running STDP tests..." << std::endl;
    try {
        test_stdp::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Brain tests..." << std::endl;
    try {
        test_brain::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Run Phase 6 integration tests
    std::cout << "=== Phase 6: Integration Tests ===" << std::endl;
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
        std::cout << "=== All Tests PASSED ===" << std::endl;
        std::cout << "The NLM brain is fully functional with all systems properly integrated!" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        std::cout << "Some NLM brain systems may require attention." << std::endl;
        return 1;
    }
}
