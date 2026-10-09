// NLM Comprehensive Test Runner
// Main test runner that orchestrates all test suites

#include "nlm/TestFramework.hpp"
#include <iostream>
#include <string>
#include <vector>

// Forward declarations of test functions
void test_types_runAll();
void test_config_runAll();
void test_random_runAll();
void test_clock_runAll();
void test_neuron_runAll();
void test_synapse_runAll();
void test_stdp_runAll();
void test_brain_runAll();
void test_environment_runAll();
void test_plasticity_runAll();
void test_memory_runAll();
void test_cognition_runAll();
void test_integration_runAll();

int main(int argc, char** argv) {
    std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
    std::cout << "Phase 2: Real Neural Computation Tests" << std::endl;
    std::cout << std::endl;
    
    // Get the global test runner
    nlm::TestRunner& runner = nlm::getDefaultTestRunner();
    
    // Register all test suites
    std::cout << "Registering test suites..." << std::endl;
    
    runner.addTest("Types Tests", test_types_runAll, "Core Types");
    runner.addTest("Config Tests", test_config_runAll, "Configuration");
    runner.addTest("Random Tests", test_random_runAll, "Random Generation");
    runner.addTest("Clock Tests", test_clock_runAll, "Time Management");
    runner.addTest("Neuron Tests", test_neuron_runAll, "Neuron Dynamics");
    runner.addTest("Synapse Tests", test_synapse_runAll, "Synaptic Transmission");
    runner.addTest("STDP Tests", test_stdp_runAll, "Plasticity Learning");
    runner.addTest("Brain Tests", test_brain_runAll, "Neural Computation");
    
    std::cout << std::endl;
    std::cout << "Running " << runner.getResults().size() << " test suites..." << std::endl;
    std::cout << std::endl;
    
    // Run all tests
    runner.runAll();
    
    // Return appropriate exit code
    size_t failedCount = 0;
    for (const auto& result : runner.getResults()) {
        if (!result.passed) {
            failedCount++;
        }
    }
    
    if (failedCount > 0) {
        std::cout << std::endl;
        std::cout << "FAILED: " << failedCount << " test suite(s) failed" << std::endl;
        return 1;
    } else {
        std::cout << std::endl;
        std::cout << "SUCCESS: All test suites passed!" << std::endl;
        return 0;
    }
}

