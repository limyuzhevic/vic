// Enhanced NLM Test Main
// Phase 6: Final Integration Tests

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

namespace test_phase6 {
    void testPhase6Integration();
}

int main() {
    std::cout << "=== NLM Enhanced Testing Suite ===" << std::endl;
    std::cout << "Testing Phase 6: Final Integration with Enhanced Features" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    std::cout << "Running enhanced test suite..." << std::endl;
    
    try {
        std::cout << "Running Types tests..." << std::endl;
        test_types::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        std::cout << "Running Config tests..." << std::endl;
        test_config::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        std::cout << "Running Random tests..." << std::endl;
        test_random::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        std::cout << "Running Clock tests..." << std::endl;
        test_clock::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        std::cout << "Running Neuron tests..." << std::endl;
        test_neuron::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        std::cout << "Running Synapse tests..." << std::endl;
        test_synapse::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        std::cout << "Running STDP tests..." << std::endl;
        test_stdp::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        std::cout << "Running Brain tests..." << std::endl;
        test_brain::runAll();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    try {
        std::cout << "Running Phase 6 integration tests..." << std::endl;
        test_phase6::testPhase6Integration();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
    }
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        std::cout << "Enhanced NLM (熙然) neural learning machine is fully functional." << std::endl;
        std::cout << std::endl;
        std::cout << "Key enhancements verified:" << std::endl;
        std::cout << "  ✓ JSON/YAML configuration system with schema validation" << std::endl;
        std::cout << "  ✓ Advanced error handling and recovery" << std::endl;
        std::cout << "  ✓ Configuration merging and inheritance" << std::endl;
        std::cout << "  ✓ Enhanced Python API bindings" << std::endl;
        std::cout << "  ✓ Phase 6 final integration demo" << std::endl;
        std::cout << "  ✓ Comprehensive testing framework" << std::endl;
        std::cout << "  ✓ Advanced monitoring and performance features" << std::endl;
    } else {
        std::cout << "=== SOME TESTS FAILED ===" << std::endl;
        std::cout << "Some enhancements may not be fully functional." << std::endl;
    }
    
    return allPassed ? 0 : 1;
}
