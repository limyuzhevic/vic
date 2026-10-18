// NLM Comprehensive Test Main
// Phase 6: Modular Architecture Tests

#include <iostream>
#include <cassert>

// Forward declarations of test functions (existing tests)
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

// Forward declarations of new comprehensive test functions
namespace test_framework {
    void runAll();
}

namespace test_config {
    void runAll();
}

namespace test_brain {
    void runAll();
}

namespace test_agentbrain {
    void runAll();
}

namespace test_simpleworld {
    void runAll();
}

namespace test_advanced_actions {
    void runAll();
}

namespace test_plugin {
    void runAll();
}

namespace test_python_bindings {
    void runAll();
}

namespace test_integration {
    void runAll();
}

namespace test_performance {
    void runAll();
}

namespace test_compatibility {
    void runAll();
}

namespace test_quality {
    void runAll();
}

int main() {
    std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
    std::cout << "Testing Enhanced NLM Brain Simulator (Phase 6: Modular Architecture)" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Run framework tests first
    std::cout << "=== Framework Tests ===" << std::endl;
    std::cout << "Running test framework infrastructure..." << std::endl;
    try {
        test_framework::runAll();
        std::cout << "Framework tests PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "Framework tests FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Run existing tests
    std::cout << "=== Original Tests (Phase 2) ===" << std::endl;
    
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
    
    // Run enhanced comprehensive tests
    std::cout << "=== Enhanced Comprehensive Tests (Phase 6) ===" << std::endl;
    
    std::cout << "Running Config System Tests..." << std::endl;
    try {
        test_config::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Brain System Tests..." << std::endl;
    try {
        test_brain::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running AgentBrain Tests..." << std::endl;
    try {
        test_agentbrain::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running SimpleWorld Tests..." << std::endl;
    try {
        test_simpleworld::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Advanced Actions Tests..." << std::endl;
    try {
        test_advanced_actions::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Plugin System Tests..." << std::endl;
    try {
        test_plugin::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << "Running Python Bindings Tests..." << std::endl;
    try {
        test_python_bindings::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Run integration tests
    std::cout << "=== Integration Tests ===" << std::endl;
    
    std::cout << "Running System Integration Tests..." << std::endl;
    try {
        test_integration::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Run performance tests
    std::cout << "=== Performance Tests ===" << std::endl;
    
    std::cout << "Running Performance Tests..." << std::endl;
    try {
        test_performance::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Run compatibility tests
    std::cout << "=== Compatibility Tests ===" << std::endl;
    
    std::cout << "Running Compatibility Tests..." << std::endl;
    try {
        test_compatibility::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    // Run quality tests
    std::cout << "=== Quality Tests ===" << std::endl;
    
    std::cout << "Running Quality Tests..." << std::endl;
    try {
        test_quality::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    
    if (allPassed) {
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "NLM Brain Simulator is ready for production!" << std::endl;
        return 0;
    } else {
        std::cout << "=== SOME TESTS FAILED ===" << std::endl;
        std::cout << "Please review the failed tests above." << std::endl;
        return 1;
    }
}
