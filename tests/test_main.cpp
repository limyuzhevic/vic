// NLM Test Main
// Phase 2: Real Neural Computation Tests

#include "TestFramework.h"
#include <iostream>

// Forward declarations of test registration functions
namespace test_types {
    void registerTypesTests(TestRunner& runner);
}

namespace test_config {
    void registerConfigTests(TestRunner& runner);
}

namespace test_random {
    void registerRandomTests(TestRunner& runner);
}

namespace test_clock {
    void registerClockTests(TestRunner& runner);
}

namespace test_neuron {
    void registerNeuronTests(TestRunner& runner);
}

namespace test_synapse {
    void registerSynapseTests(TestRunner& runner);
}

namespace test_stdp {
    void registerSTDPTests(TestRunner& runner);
}

namespace test_brain {
    void registerBrainTests(TestRunner& runner);
}

int main() {
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    TestRunner runner;
    
    // Register all test suites
    test_types::registerTypesTests(runner);
    test_config::registerConfigTests(runner);
    test_random::registerRandomTests(runner);
    test_clock::registerClockTests(runner);
    test_neuron::registerNeuronTests(runner);
    test_synapse::registerSynapseTests(runner);
    test_stdp::registerSTDPTests(runner);
    test_brain::registerBrainTests(runner);
    
    try {
        runner.runAll();
        std::cout << std::endl;
        std::cout << "=== All Tests PASSED ===" << std::endl;
        return 0;
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        return 1;
    }
}
