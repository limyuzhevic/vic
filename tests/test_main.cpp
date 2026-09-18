// NLM Test Main
// Phase 2: Real Neural Computation Tests
// Updated with configurable parameters and improved error handling

#include <iostream>
#include <cassert>
#include "TestConfig.hpp"
#include "TestStatistics.hpp"
#include "TestUtilities.hpp"

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

int main() {
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    // Initialize test configuration
    nlm::initializeTestConfig();
    auto& config = nlm::getTestConfig();
    
    std::cout << "=== Test Configuration ===" << std::endl;
    nlm::printTestConfigSummary(config);
    std::cout << std::endl;
    
    // Start test suite
    nlm::TestStatistics::getInstance().startSuite("NLM Phase 2 Neural Computation Tests");
    
    // Initialize test status manager
    nlm::TestStatusManager& testStatus = nlm::TestStatusManager::getInstance();
    
    bool allPassed = true;
    
    // Run Types tests with error handling
    std::cout << "Running Types tests..." << std::endl;
    try {
        nlm::TestStatistics::getInstance().startTestCase("Types Tests");
        nlm::TestIsolation::executeWithIsolation(
            "Types Tests",
            []() { test_types::runAll(); },
            config
        );
        nlm::TestStatistics::getInstance().endTestCase(true);
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, e.what());
    } catch (...) {
        std::cout << "  FAILED: Unknown error" << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, "Unknown error");
    }
    
    // Run Config tests with error handling
    std::cout << "Running Config tests..." << std::endl;
    try {
        nlm::TestStatistics::getInstance().startTestCase("Config Tests");
        nlm::TestIsolation::executeWithIsolation(
            "Config Tests",
            []() { test_config::runAll(); },
            config
        );
        nlm::TestStatistics::getInstance().endTestCase(true);
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, e.what());
    } catch (...) {
        std::cout << "  FAILED: Unknown error" << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, "Unknown error");
    }
    
    // Run Random tests with error handling
    std::cout << "Running Random tests..." << std::endl;
    try {
        nlm::TestStatistics::getInstance().startTestCase("Random Tests");
        nlm::TestIsolation::executeWithIsolation(
            "Random Tests",
            []() { test_random::runAll(); },
            config
        );
        nlm::TestStatistics::getInstance().endTestCase(true);
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, e.what());
    } catch (...) {
        std::cout << "  FAILED: Unknown error" << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, "Unknown error");
    }
    
    // Run Clock tests with error handling
    std::cout << "Running Clock tests..." << std::endl;
    try {
        nlm::TestStatistics::getInstance().startTestCase("Clock Tests");
        nlm::TestIsolation::executeWithIsolation(
            "Clock Tests",
            []() { test_clock::runAll(); },
            config
        );
        nlm::TestStatistics::getInstance().endTestCase(true);
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, e.what());
    } catch (...) {
        std::cout << "  FAILED: Unknown error" << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, "Unknown error");
    }
    
    // Run Neuron tests with error handling
    std::cout << "Running Neuron tests..." << std::endl;
    try {
        nlm::TestStatistics::getInstance().startTestCase("Neuron Tests");
        nlm::TestIsolation::executeWithIsolation(
            "Neuron Tests",
            []() { test_neuron::runAll(); },
            config
        );
        nlm::TestStatistics::getInstance().endTestCase(true);
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, e.what());
    } catch (...) {
        std::cout << "  FAILED: Unknown error" << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, "Unknown error");
    }
    
    // Run Synapse tests with error handling
    std::cout << "Running Synapse tests..." << std::endl;
    try {
        nlm::TestStatistics::getInstance().startTestCase("Synapse Tests");
        nlm::TestIsolation::executeWithIsolation(
            "Synapse Tests",
            []() { test_synapse::runAll(); },
            config
        );
        nlm::TestStatistics::getInstance().endTestCase(true);
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, e.what());
    } catch (...) {
        std::cout << "  FAILED: Unknown error" << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, "Unknown error");
    }
    
    // Run STDP tests with error handling
    std::cout << "Running STDP tests..." << std::endl;
    try {
        nlm::TestStatistics::getInstance().startTestCase("STDP Tests");
        nlm::TestIsolation::executeWithIsolation(
            "STDP Tests",
            []() { test_stdp::runAll(); },
            config
        );
        nlm::TestStatistics::getInstance().endTestCase(true);
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, e.what());
    } catch (...) {
        std::cout << "  FAILED: Unknown error" << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, "Unknown error");
    }
    
    // Run Brain tests with error handling
    std::cout << "Running Brain tests..." << std::endl;
    try {
        nlm::TestStatistics::getInstance().startTestCase("Brain Tests");
        nlm::TestIsolation::executeWithIsolation(
            "Brain Tests",
            []() { test_brain::runAll(); },
            config
        );
        nlm::TestStatistics::getInstance().endTestCase(true);
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, e.what());
    } catch (...) {
        std::cout << "  FAILED: Unknown error" << std::endl;
        allPassed = false;
        nlm::TestStatistics::getInstance().endTestCase(false, "Unknown error");
    }
    
    // End test suite
    nlm::TestStatistics::getInstance().endSuite();
    
    // Print test statistics
    std::cout << "\n=== Final Test Statistics ===" << std::endl;
    nlm::TestStatistics::getInstance().printStatistics();
    
    // Update global test config statistics
    nlm::updateTestStatistics(config, allPassed);
    
    // Print final test configuration summary
    std::cout << "\n=== Final Test Configuration Summary ===" << std::endl;
    nlm::printTestConfigSummary(config);
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        return 1;
    }
}