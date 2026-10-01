// NLM Test Main
// Phase 2: Real Neural Computation Tests
// Enhanced with neuromodulation and advanced plasticity tests

#include <iostream>
#include <cassert>
#include <memory>
#include <functional>

// Forward declarations of test functions
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

namespace test_plasticity {
    void runAll();
}

namespace test_neuromodulation {
    void runAll();
}

namespace test_structural {
    void runAll();
}

// Test runner helper functions
void runTestGroup(const std::string& name, std::function<void()> testFunc) {
    std::cout << "Running " << name << "..." << std::endl;
    try {
        testFunc();
        std::cout << "  PASSED" << std::endl;
    } catch (const std::exception& e) {
        std::cout << "  FAILED: " << e.what() << std::endl;
        throw; // Re-throw to be caught by main
    }
}

int main() {
    std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << "  - Hebbian Plasticity" << std::endl;
    std::cout << "  - Neuromodulation Effects" << std::endl;
    std::cout << "  - Structural Plasticity" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Core functionality tests
    std::cout << "=== Core System Tests ===" << std::endl;
    runTestGroup("Types tests", test_types::runAll);
    runTestGroup("Config tests", test_config::runAll);
    runTestGroup("Random tests", test_random::runAll);
    runTestGroup("Clock tests", test_clock::runAll);
    
    // Neural component tests
    std::cout << "\n=== Neural Component Tests ===" << std::endl;
    runTestGroup("Neuron tests", test_neuron::runAll);
    runTestGroup("Synapse tests", test_synapse::runAll);
    runTestGroup("Brain tests", test_brain::runAll);
    
    // Plasticity tests
    std::cout << "\n=== Plasticity Tests ===" << std::endl;
    runTestGroup("STDP tests", test_stdp::runAll);
    runTestGroup("Hebbian plasticity tests", test_plasticity::runAll);
    
    // Neuromodulation and structural tests
    std::cout << "\n=== Advanced Tests ===" << std::endl;
    runTestGroup("Neuromodulation tests", test_neuromodulation::runAll);
    runTestGroup("Structural plasticity tests", test_structural::runAll);
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        return 1;
    }
}