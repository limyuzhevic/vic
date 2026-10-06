// NLM Test Main
// Phase 2: Real Neural Computation Tests
// Enhanced with NLM Test Framework

#include <iostream>
#include <cassert>
#include "framework/TestFramework.hpp"

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

// Legacy test integration with new framework
class LegacyTestAdapter : public nlm::test::TestBase {
public:
    LegacyTestAdapter(const std::string& name, std::function<void()> testFunc)
        : testFunc(testFunc) {
        setName(name);
        setCategory(nlm::test::TestSeverity::UNIT);
    }
    
    void runTest() override {
        try {
            testFunc();
        } catch (...) {
            throw;
        }
    }
    
private:
    std::function<void()> testFunc;
};

// Enhanced test execution with modern framework
void runEnhancedTests() {
    std::cout << "=== Enhanced NLM Test Framework ===" << std::endl;
    
    // Initialize test framework
    nlm::test::initializeTestFramework();
    
    // Configure test framework
    auto& config = nlm::test::getConfiguration();
    config.verboseOutput = true;
    config.showProgress = true;
    config.enableMemoryTracker = true;
    config.enablePerformanceBenchmarking = true;
    
    // Run legacy tests through enhanced framework
    std::cout << "Running legacy tests through enhanced framework..." << std::endl;
    
    // Create legacy test adapters
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<LegacyTestAdapter>("Legacy Types Tests", []() {
            test_types::runAll();
        })
    );
    
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<LegacyTestAdapter>("Legacy Config Tests", []() {
            test_config::runAll();
        })
    );
    
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<LegacyTestAdapter>("Legacy Random Tests", []() {
            test_random::runAll();
        })
    );
    
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<LegacyTestAdapter>("Legacy Clock Tests", []() {
            test_clock::runAll();
        })
    );
    
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<LegacyTestAdapter>("Legacy Neuron Tests", []() {
            test_neuron::runAll();
        })
    );
    
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<LegacyTestAdapter>("Legacy Synapse Tests", []() {
            test_synapse::runAll();
        })
    );
    
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<LegacyTestAdapter>("Legacy STDP Tests", []() {
            test_stdp::runAll();
        })
    );
    
    nlm::test::TestRegistry::getInstance().registerTest(
        std::make_unique<LegacyTestAdapter>("Legacy Brain Tests", []() {
            test_brain::runAll();
        })
    );
    
    // Run enhanced tests
    std::cout << "Running enhanced tests..." << std::endl;
    nlm::test::TestRegistry::getInstance().runAllTests();
    
    // Get test statistics
    nlm::test::TestStats stats = nlm::test::TestRegistry::getInstance().getStats();
    
    // Print enhanced report
    std::cout << "=== Enhanced Test Report ===" << std::endl;
    std::cout << "Total Tests: " << stats.total << std::endl;
    std::cout << "Passed: " << stats.passed << std::endl;
    std::cout << "Failed: " << stats.failed << std::endl;
    std::cout << "Skipped: " << stats.skipped << std::endl;
    std::cout << "Errors: " << stats.errors << std::endl;
    std::cout << "Total Time: " << stats.totalTime << "s" << std::endl;
    
    // Cleanup
    nlm::test::cleanupTestFramework();
}

int main() {
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Run enhanced tests
    runEnhancedTests();
    
    // Also run legacy tests for backward compatibility
    std::cout << std::endl;
    std::cout << "=== Running Legacy Tests (Backward Compatibility) ===" << std::endl;
    
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
    if (allPassed) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        return 1;
    }
}
