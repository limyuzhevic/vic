#include "framework/TestFramework.hpp"
#include <iostream>

// Forward declarations of original test functions
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

// Using the new test framework
namespace {
    class TestSuite : public TestBase {
    public:
        TestSuite() {}
        ~TestSuite() {}
        
        void setup() override {
            // Framework setup
        }
        
        void executeTest() override {
            // Run all test suites using the new framework
            runAllTests();
        }
        
        void cleanup() override {
            // Framework cleanup
        }
    };
}

// The main test runner using the new comprehensive test framework
int main() {
    std::cout << "=== NLM Phase 2 Comprehensive Test Suite ===" << std::endl;
    std::cout << "Testing Real Neural Computation with enhanced framework:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics (Fixed)" << std::endl;
    std::cout << "  - Synaptic Transmission (Fixed)" << std::endl;
    std::cout << "  - STDP Plasticity (Fixed)" << std::endl;
    std::cout << "  - Configuration System (Enhanced)" << std::endl;
    std::cout << "  - Spike System (Improved)" << std::endl;
    std::cout << std::endl;
    
    // Run the comprehensive test suite
    return TestSuite().runTest().passed ? 0 : 1;
}

// Include the test framework implementation
#include "framework/TestFramework.cpp"

// Original unit test functions (maintained for backward compatibility)
namespace {
    void runUnitTests() {
        std::cout << "Running original unit tests..." << std::endl;
        
        bool allPassed = true;
        
        std::cout << "  Types tests..." << std::endl;
        try {
            test_types::runAll();
            std::cout << "    PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        
        std::cout << "  Config tests..." << std::endl;
        try {
            test_config::runAll();
            std::cout << "    PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        
        std::cout << "  Random tests..." << std::endl;
        try {
            test_random::runAll();
            std::cout << "    PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        
        std::cout << "  Clock tests..." << std::endl;
        try {
            test_clock::runAll();
            std::cout << "    PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        
        std::cout << "  Neuron tests..." << std::endl;
        try {
            test_neuron::runAll();
            std::cout << "    PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        
        std::cout << "  Synapse tests..." << std::endl;
        try {
            test_synapse::runAll();
            std::cout << "    PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        
        std::cout << "  STDP tests..." << std::endl;
        try {
            test_stdp::runAll();
            std::cout << "    PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        
        std::cout << "  Brain tests..." << std::endl;
        try {
            test_brain::runAll();
            std::cout << "    PASSED" << std::endl;
        } catch (const std::exception& e) {
            std::cout << "    FAILED: " << e.what() << std::endl;
            allPassed = false;
        }
        
        std::cout << std::endl;
        if (allPassed) {
            std::cout << "=== All Original Tests PASSED ===" << std::endl;
        } else {
            std::cout << "=== Some Original Tests FAILED ===" << std::endl;
        }
    }
}

// Include the enhanced CLI test implementation
#include "tests/test_enhanced_cli.cpp"
