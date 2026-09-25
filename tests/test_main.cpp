// NLM Test Main
// Phase 2: Real Neural Computation Tests
//
// This is the main test runner for NLM (Neural Learning Machine) tests.
// It includes tests for:
// - Core types and utilities (Phase 1)
// - Configuration system (Phase 1)
// - Random number generation (Phase 1)
// - Simulation clock (Phase 1)
// - Neural components (Phase 2)
// - Synapse dynamics (Phase 2)
// - Spike-Timing Dependent Plasticity (Phase 2)
// - Brain system (Phase 2)
// - Phase 6 integration tests (Phase 6)

#include <iostream>
#include <cassert>

// Forward declarations of test functions
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

namespace test_phase6_integration {
    void runAllPhase6Tests();
}

int main() {
    std::cout << "=== NLM Test Suite ===" << std::endl;
    std::cout << std::endl;
    std::cout << "Testing NLM Neural Learning Machine:" << std::endl;
    std::cout << "  - Core types and utilities (Phase 1)" << std::endl;
    std::cout << "  - Configuration system (Phase 1)" << std::endl;
    std::cout << "  - Random number generation (Phase 1)" << std::endl;
    std::cout << "  - Simulation clock (Phase 1)" << std::endl;
    std::cout << "  - Neural components (Phase 2)" << std::endl;
    std::cout << "  - Synapse dynamics (Phase 2)" << std::endl;
    std::cout << "  - Spike-Timing Dependent Plasticity (Phase 2)" << std::endl;
    std::cout << "  - Brain system (Phase 2)" << std::endl;
    std::cout << "  - Phase 6 integration tests (Phase 6)" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
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
    std::cout << "Running Phase 6 Integration Tests..." << std::endl;
    try {
        test_phase6_integration::runAllPhase6Tests();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
        std::cout << std::endl;
        std::cout << "NLM (熙然) Neural Learning Machine Test Summary:" << std::endl;
        std::cout << "  ✓ Core types and utilities" << std::endl;
        std::cout << "  ✓ Configuration system" << std::endl;
        std::cout << "  ✓ Random number generation" << std::endl;
        std::cout << "  ✓ Simulation clock" << std::endl;
        std::cout << "  ✓ Neural components" << std::endl;
        std::cout << "  ✓ Synapse dynamics" << std::endl;
        std::cout << "  ✓ Spike-Timing Dependent Plasticity" << std::endl;
        std::cout << "  ✓ Brain system" << std::endl;
        std::cout << "  ✓ Phase 6 integration" << std::endl;
        std::cout << std::endl;
        std::cout << "The NLM brain simulation is fully functional and ready for Phase 6 deployment!" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        return 1;
    }
}
