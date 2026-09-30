// Main test file for legacy NLM tests
// This runs the original test suite for backward compatibility

#include <iostream>

// Legacy test functions (declared in their respective headers)
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
    std::cout << "=== Legacy NLM Test Suite ===" << std::endl << std::endl;
    
    // Run legacy tests in order
    test_types::runAll();
    test_config::runAll();
    test_random::runAll();
    test_clock::runAll();
    test_neuron::runAll();
    test_synapse::runAll();
    test_stdp::runAll();
    test_brain::runAll();
    
    std::cout << std::endl;
    std::cout << "=== All Legacy Tests Passed ===" << std::endl;
    
    return 0;
}