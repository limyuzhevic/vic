// Main test runner for NLM test suite

#include "test_config.cpp"
#include "test_config_comprehensive.cpp"
#include "test_brain.cpp"
#include "test_brain_comprehensive.cpp"
#include "test_synapse.cpp"
#include "test_error_handling.cpp"
#include "test_clock.cpp"
#include "test_neuron.cpp"
#include "test_random.cpp"
#include "test_stdp.cpp"
#include "test_types.cpp"

int main() {
    std::cout << "=== NLM Test Suite ===" << std::endl;
    std::cout << std::endl;
    
    // Run all test suites
    test_config::runAll();
    std::cout << std::endl;
    
    test_config_comprehensive::runAll();
    std::cout << std::endl;
    
    test_brain::runAll();
    std::cout << std::endl;
    
    test_brain_comprehensive::runAll();
    std::cout << std::endl;
    
    test_synapse::runAll();
    std::cout << std::endl;
    
    test_error_handling::runAll();
    std::cout << std::endl;
    
    test_clock::runAll();
    std::cout << std::endl;
    
    test_neuron::runAll();
    std::cout << std::endl;
    
    test_random::runAll();
    std::cout << std::endl;
    
    test_stdp::runAll();
    std::cout << std::endl;
    
    test_types::runAll();
    std::cout << std::endl;
    
    std::cout << "=== All Test Suites Completed Successfully ===" << std::endl;
    return 0;
}
