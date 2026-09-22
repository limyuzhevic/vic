// NLM Test Main
// Integration with Google Test Framework

#include <gtest/gtest.h>

// Include all test suites
#include "NLMTestFramework.hpp"
#include "test_types.cpp"
#include "test_config.cpp"
#include "test_random.cpp"
#include "test_clock.cpp"
#include "test_neuron.cpp"
#include "test_synapse.cpp"
#include "test_stdp.cpp"
#include "test_brain.cpp"

int main(int argc, char **argv) {
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
