#include "test_advanced_features.cpp"
#include "test_brain.cpp"
#include "test_config.cpp"
#include "test_clock.cpp"
#include "test_random.cpp"
#include "test_synapse.cpp"
#include "test_stdp.cpp"
#include "test_types.cpp"
#include "test_neuron.cpp"
#include "test_main.cpp"

// Main test runner
int main(int argc, char* argv[]) {
    std::cout << "NLM Test Suite" << std::endl;
    std::cout << "==============" << std::endl;
    
    // Run all tests
    std::cout << "Running Core Tests..." << std::endl;
    std::cout << std::endl;
    
    // Run core NLM tests
    test_brain::runAll();
    test_config::runAll();
    test_clock::runAll();
    test_random::runAll();
    test_synapse::runAll();
    test_stdp::runAll();
    test_types::runAll();
    test_neuron::runAll();
    test_main::runAll();
    
    std::cout << std::endl;
    std::cout << "Running Advanced Feature Tests..." << std::endl;
    std::cout << std::endl;
    
    // Run advanced feature tests
    nlm::test_advanced::runAllAdvancedTests();
    
    std::cout << std::endl;
    std::cout << "===================================" << std::endl;
    std::cout << "All tests completed successfully!" << std::endl;
    std::cout << "===================================" << std::endl;
    
    return 0;
}