// NLM Test Main
// Phase 2: Real Neural Computation Tests

#include "tests/TestFramework.hpp"
#include "tests/test_framework_comprehensive.cpp"
#include <iostream>

int main() {
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    // Run comprehensive tests using the test framework
    nlm::test::runAll();
    
    return 0;
}