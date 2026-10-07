// NLM Test Main
// Phase 2: Real Neural Computation Tests
// Enhanced with Phase 6 Memory Integration Tests

#include <iostream>
#include <cassert>
#include <chrono>
#include <iomanip>

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

namespace test_performance_integration {
    int main();
}

namespace test_memory_enhancement {
    void runAll();
}

namespace test_memory_enhancement {
    void runAll();
}

// Test framework improvements
class TestFramework {
public:
    static void runTest(const std::string& name, std::function<void()> testFunc) {
        std::cout << "Running " << name << "..." << std::endl;
        auto startTime = std::chrono::high_resolution_clock::now();
        
        try {
            testFunc();
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            std::cout << "  PASSED (" << duration.count() << "ms)" << std::endl;
            passedTests++;
            totalTests++;
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(endTime - startTime);
            
            std::cout << "  FAILED (" << duration.count() << "ms): " << e.what() << std::endl;
            failedTests++;
            totalTests++;
        } catch (...) {
            std::cout << "  FAILED (unknown exception)" << std::endl;
            failedTests++;
            totalTests++;
        }
    }
    
    static void printSummary() {
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << failedTests << std::endl;
        std::cout << "Success rate: " << (totalTests > 0 ? (passedTests * 100.0 / totalTests) : 0) << "%" << std::endl;
        
        if (failedTests > 0) {
            std::cout << std::endl;
            std::cout << "FAILED TESTS:" << std::endl;
            // Implementation would track which specific tests failed
            std::cout << "  - " << failedTests << " test(s) failed" << std::endl;
        }
    }
    
    static int getFailureCount() {
        return failedTests;
    }
    
private:
    static inline int passedTests = 0;
    static inline int failedTests = 0;
    static inline int totalTests = 0;
};

int main() {
    std::cout << "=== NLM Phase 6 Comprehensive Tests ===" << std::endl;
    std::cout << "Testing Phase 2: Real Neural Computation" << std::endl;
    std::cout << "Testing Phase 6: Enhanced Memory Integration" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Core Neural Computation Tests:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << "  - System Integration" << std::endl;
    std::cout << std::endl;
    
    std::cout << "Enhanced Memory Integration Tests:" << std::endl;
    std::cout << "  - Fuzzy Pattern Completion" << std::endl;
    std::cout << "  - Emotional Memory Prioritization" << std::endl;
    std::cout << "  - Hierarchical Memory Architecture" << std::endl;
    std::cout << "  - Neuromodulation Integration" << std::endl;
    std::cout << std::endl;
    
    bool allPassed = true;
    
    // Test framework improved reporting
    TestFramework::runTest("Types tests", test_types::runAll);
    TestFramework::runTest("Config tests", test_config::runAll);
    TestFramework::runTest("Random tests", test_random::runAll);
    TestFramework::runTest("Clock tests", test_clock::runAll);
    TestFramework::runTest("Neuron tests", test_neuron::runAll);
    TestFramework::runTest("Synapse tests", test_synapse::runAll);
    TestFramework::runTest("STDP tests", test_stdp::runAll);
    TestFramework::runTest("Brain tests", test_brain::runAll);
    
    std::cout << std::endl;
    std::cout << "Enhanced Memory Tests:" << std::endl;
    TestFramework::runTest("Memory Enhancement tests", test_memory_enhancement::runAll);
    
    TestFramework::printSummary();
    
    if (TestFramework::getFailureCount() > 0) {
        std::cout << std::endl;
        std::cout << "=== Some Tests FAILED ===" << std::endl;
        return 1;
    } else {
        std::cout << std::endl;
        std::cout << "=== All Tests PASSED ===" << std::endl;
        return 0;
    }
}
