// NLM Test Main
// Phase 2: Real Neural Computation Tests
// Enhanced with comprehensive testing including new Neuron API

#include <iostream>
#include <cassert>
#include <cmath>

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

namespace test_agentbrain {
    void runAll();
}

namespace test_plasticity {
    void runAll();
}

// Test framework helper
class TestFramework {
public:
    static bool runTest(const std::string& testName, std::function<bool()> testFunc) {
        try {
            bool result = testFunc();
            if (result) {
                std::cout << "  PASSED: " << testName << std::endl;
                return true;
            } else {
                std::cout << "  FAILED: " << testName << " (returned false)" << std::endl;
                return false;
            }
        } catch (const std::exception& e) {
            std::cout << "  FAILED: " << testName << " (exception: " << e.what() << ")" << std::endl;
            return false;
        } catch (...) {
            std::cout << "  FAILED: " << testName << " (unknown exception)" << std::endl;
            return false;
        }
    }
};

int main() {
    std::cout << "=== NLM Phase 2 Enhanced Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << "  - Advanced Neuron Configuration" << std::endl;
    std::cout << "  - AgentBrain Enhanced Features" << std::endl;
    std::cout << "  - Learning Subsystem Management" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Core NLM tests
    std::cout << "=== Core NLM Tests ===" << std::endl;
    std::cout << "Running Types tests..." << std::endl;
    allPassed &= TestFramework::runTest("Types", test_types::runAll);
    
    std::cout << "Running Config tests..." << std::endl;
    allPassed &= TestFramework::runTest("Config", test_config::runAll);
    
    std::cout << "Running Random tests..." << std::endl;
    allPassed &= TestFramework::runTest("Random", test_random::runAll);
    
    std::cout << "Running Clock tests..." << std::endl;
    allPassed &= TestFramework::runTest("Clock", test_clock::runAll);
    
    std::cout << "Running Neuron tests..." << std::endl;
    allPassed &= TestFramework::runTest("Neuron", test_neuron::runAll);
    
    std::cout << "Running Synapse tests..." << std::endl;
    allPassed &= TestFramework::runTest("Synapse", test_synapse::runAll);
    
    std::cout << "Running STDP tests..." << std::endl;
    allPassed &= TestFramework::runTest("STDP", test_stdp::runAll);
    
    std::cout << "Running Brain tests..." << std::endl;
    allPassed &= TestFramework::runTest("Brain", test_brain::runAll);
    
    std::cout << std::endl;
    std::cout << "=== Enhanced Neuron API Tests ===" << std::endl;
    allPassed &= TestFramework::runTest("AgentBrain Enhanced Features", test_agentbrain::runAll);
    
    std::cout << std::endl;
    std::cout << "=== Advanced Learning Subsystem Tests ===" << std::endl;
    allPassed &= TestFramework::runTest("Learning Subsystem", test_plasticity::runAll);
    
    std::cout << std::endl;
    if (allPassed) {
        std::cout << "=== All Tests PASSED ===" << std::endl;
        std::cout << "NLM Phase 2 neural computation framework is fully functional!" << std::endl;
        std::cout << "All core features, plasticity mechanisms, and enhanced APIs work correctly." << std::endl;
        return 0;
    } else {
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        std::cout << "Some tests failed. Please check the output above for details." << std::endl;
        return 1;
    }
}
