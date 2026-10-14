// NLM Test Main - Google Test Integration
// This is the main test entry point that works with Google Test

#include <gtest/gtest.h>
#include <iostream>

int main(int argc, char **argv) {
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    // Initialize Google Test
    ::testing::InitGoogleTest(&argc, argv);
    
    // Run all tests
    int result = RUN_ALL_TESTS();
    
    if (result == 0) {
        std::cout << std::endl;
        std::cout << "=== All Tests PASSED ===" << std::endl;
    } else {
        std::cout << std::endl;
        std::cout << "=== Some Tests FAILED ===" << std::endl;
    }
    
    return result;
}