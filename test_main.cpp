// Enhanced NLM Test Main with Comprehensive Reporting
// Phase 2: Real Neural Computation Tests with Full Test Coverage

#include "nlm_test_utilities.h"
#include <iostream>
#include <cassert>
#include <map>

// Forward declarations of test functions
namespace test_types {
    bool runAllTests(nlm::test::TestFramework& framework);
}

namespace test_config {
    bool runAllTests(nlm::test::TestFramework& framework);
}

namespace test_random {
    bool runAllTests(nlm::test::TestFramework& framework);
}

namespace test_clock {
    bool runAllTests(nlm::test::TestFramework& framework);
}

namespace test_neuron {
    bool runAllTests(nlm::test::TestFramework& framework);
}

namespace test_synapse {
    bool runAllTests(nlm::test::TestFramework& framework);
}

namespace test_stdp {
    bool runAllTests(nlm::test::TestFramework& framework);
}

namespace test_brain {
    bool runAllTests(nlm::test::TestFramework& framework);
}

int main() {
    nlm::test::TestFramework framework;
    
    std::cout << "=== NLM Phase 2 Tests - Comprehensive Test Suite ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << "  - Memory Management" << std::endl;
    std::cout << "  - Integration Testing" << std::endl;
    std::cout << "  - Performance Testing" << std::endl;
    std::cout << std::endl;
    
    // Run all test suites
    std::cout << "=== CORE COMPONENTS ===" << std::endl;
    bool typesPassed = test_types::runAllTests(framework);
    bool configPassed = test_config::runAllTests(framework);
    bool randomPassed = test_random::runAllTests(framework);
    bool clockPassed = test_clock::runAllTests(framework);
    
    std::cout << "\n=== BRAIN LEVEL TESTING ===" << std::endl;
    bool neuronPassed = test_neuron::runAllTests(framework);
    bool synapsePassed = test_synapse::runAllTests(framework);
    bool stdpPassed = test_stdp::runAllTests(framework);
    
    std::cout << "\n=== SYSTEM INTEGRATION ===" << std::endl;
    bool brainPassed = test_brain::runAllTests(framework);
    
    // Generate comprehensive report
    framework.printReport();
    
    // Check if all tests passed
    if (typesPassed && configPassed && randomPassed && clockPassed &&
        neuronPassed && synapsePassed && stdpPassed && brainPassed) {
        std::cout << "\n=== ALL TESTS PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "\n=== SOME TESTS FAILED ===" << std::endl;
        return 1;
    }
}
