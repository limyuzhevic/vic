#ifndef NLM_TEST_RUNNER_HPP
#define NLM_TEST_RUNNER_HPP

#include "tests/CoreNeuralTests.hpp"
#include <iostream>
#include <string>

namespace nlm {

class TestRunner {
public:
    static int runAllTests() {
        std::cout << "=== NLM Test Suite Runner ===" << std::endl;
        std::cout << "Running comprehensive neural component tests..." << std::endl << std::endl;
        
        try {
            CoreNeuralTests::runAllTests();
            
            std::cout << std::endl;
            std::cout << "=== ALL TESTS PASSED ===" << std::endl;
            std::cout << "Core neural components are functioning correctly." << std::endl;
            return 0;
            
        } catch (const std::exception& e) {
            std::cerr << std::endl;
            std::cerr << "=== TEST FAILURE ===" << std::endl;
            std::cerr << "Exception caught: " << e.what() << std::endl;
            return 1;
        } catch (...) {
            std::cerr << std::endl;
            std::cerr << "=== TEST FAILURE ===" << std::endl;
            std::cerr << "Unknown exception caught" << std::endl;
            return 1;
        }
    }
    
    static void printTestSummary() {
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Core neural component tests:".blue() << std::endl;
        std::cout << "  - Neuron functionality and move semantics".green() << std::endl;
        std::cout << "  - Synapse creation and properties".green() << std::endl;
        std::cout << "  - STDP plasticity rules".green() << std::endl;
        std::cout << "  - Hebbian plasticity rules".green() << std::endl;
        std::cout << "  - Neural population management".green() << std::endl;
        std::cout << "  - Neural region operations".green() << std::endl;
        std::cout << "  - Spike system event handling".green() << std::endl;
        std::cout << std::endl;
        std::cout << "These tests verify the basic functionality of neural computation components".reset() << std::endl;
    }
};

} // namespace nlm

#endif // NLM_TEST_RUNNER_HPP
