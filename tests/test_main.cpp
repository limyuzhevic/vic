// NLM Test Main
// Phase 2: Real Neural Computation Tests

#include <iostream>

// Forward declarations of test functions
namespace test_types {
    std::vector<TestResult> runAllTests();
}

namespace test_config {
    std::vector<TestResult> runAllTests();
}

namespace test_random {
    std::vector<TestResult> runAllTests();
}

namespace test_clock {
    std::vector<TestResult> runAllTests();
}

namespace test_neuron {
    std::vector<TestResult> runAllTests();
}

namespace test_synapse {
    std::vector<TestResult> runAllTests();
}

namespace test_stdp {
    std::vector<TestResult> runAllTests();
}

namespace test_brain {
    std::vector<TestResult> runAllTests();
}

namespace test_neural_dynamics {
    std::vector<TestResult> runAllTests();
}

int main() {
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << std::endl;
    
    std::vector<TestResult> allResults;
    
    std::cout << "Running Types tests..." << std::endl;
    auto typesResults = test_types::runAllTests();
    allResults.insert(allResults.end(), typesResults.begin(), typesResults.end());
    std::cout << "  PASSED: " << typesResults.size() << " tests" << std::endl;
    
    std::cout << "Running Config tests..." << std::endl;
    auto configResults = test_config::runAllTests();
    allResults.insert(allResults.end(), configResults.begin(), configResults.end());
    std::cout << "  PASSED: " << configResults.size() << " tests" << std::endl;
    
    std::cout << "Running Random tests..." << std::endl;
    auto randomResults = test_random::runAllTests();
    allResults.insert(allResults.end(), randomResults.begin(), randomResults.end());
    std::cout << "  PASSED: " << randomResults.size() << " tests" << std::endl;
    
    std::cout << "Running Clock tests..." << std::endl;
    auto clockResults = test_clock::runAllTests();
    allResults.insert(allResults.end(), clockResults.begin(), clockResults.end());
    std::cout << "  PASSED: " << clockResults.size() << " tests" << std::endl;
    
    std::cout << "Running Neuron tests..." << std::endl;
    auto neuronResults = test_neuron::runAllTests();
    allResults.insert(allResults.end(), neuronResults.begin(), neuronResults.end());
    std::cout << "  PASSED: " << neuronResults.size() << " tests" << std::endl;
    
    std::cout << "Running Synapse tests..." << std::endl;
    auto synapseResults = test_synapse::runAllTests();
    allResults.insert(allResults.end(), synapseResults.begin(), synapseResults.end());
    std::cout << "  PASSED: " << synapseResults.size() << " tests" << std::endl;
    
    std::cout << "Running STDP tests..." << std::endl;
    auto stdpResults = test_stdp::runAllTests();
    allResults.insert(allResults.end(), stdpResults.begin(), stdpResults.end());
    std::cout << "  PASSED: " << stdpResults.size() << " tests" << std::endl;
    
    std::cout << "Running Brain tests..." << std::endl;
    auto brainResults = test_brain::runAllTests();
    allResults.insert(allResults.end(), brainResults.begin(), brainResults.end());
    std::cout << "  PASSED: " << brainResults.size() << " tests" << std::endl;
    
    std::cout << "Running Neural Dynamics tests..." << std::endl;
    auto neuralDynamicsResults = test_neural_dynamics::runAllTests();
    allResults.insert(allResults.end(), neuralDynamicsResults.begin(), neuralDynamicsResults.end());
    std::cout << "  PASSED: " << neuralDynamicsResults.size() << " tests" << std::endl;
    
    std::cout << std::endl;
    printTestSummary(allResults);
    
    if (allResults.empty()) {
        std::cout << "=== No tests found ===" << std::endl;
        return 1;
    }
    
    int failedCount = 0;
    for (const auto& result : allResults) {
        if (!result.passed) {
            failedCount++;
        }
    }
    
    std::cout << std::endl;
    if (failedCount == 0) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
        std::cout << "Total tests: " << allResults.size() << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        std::cout << "Failed: " << failedCount << " out of " << allResults.size() << std::endl;
        return 1;
    }
}
