// NLM Test Main
// Phase 2: Real Neural Computation Tests

#include "nlm/TestFramework.hpp"
#include <iostream>

// Forward declarations of test suites
void test_types_runAll();
void test_config_runAll();
void test_random_runAll();
void test_clock_runAll();
void test_neuron_runAll();
void test_synapse_runAll();
void test_stdp_runAll();
void test_brain_runAll();

// Test runner for this file
namespace nlm {
    class MainTestRunner {
    public:
        MainTestRunner() {
            registerTests();
        }
        
        void registerTests() {
            getDefaultTestRunner().addTest("Types Tests", []() { test_types_runAll(); }, "Core Types");
            getDefaultTestRunner().addTest("Config Tests", []() { test_config_runAll(); }, "Configuration");
            getDefaultTestRunner().addTest("Random Tests", []() { test_random_runAll(); }, "Random Generation");
            getDefaultTestRunner().addTest("Clock Tests", []() { test_clock_runAll(); }, "Time Management");
            getDefaultTestRunner().addTest("Neuron Tests", []() { test_neuron_runAll(); }, "Neuron Dynamics");
            getDefaultTestRunner().addTest("Synapse Tests", []() { test_synapse_runAll(); }, "Synaptic Transmission");
            getDefaultTestRunner().addTest("STDP Tests", []() { test_stdp_runAll(); }, "Plasticity Learning");
            getDefaultTestRunner().addTest("Brain Tests", []() { test_brain_runAll(); }, "Neural Computation");
        }
    };
    
    MainTestRunner mainTestRunner;
}

int main() {
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    // Run all tests using the improved test framework
    nlm::getDefaultTestRunner().runAll();
    
    return 0;
}

