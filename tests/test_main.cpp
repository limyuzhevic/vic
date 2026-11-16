// NLM Test Main
// Phase 2: Real Neural Computation Tests
// Enhanced testing framework with better organization and reporting

#include <iostream>
#include <cassert>
#include <chrono>
#include <iomanip>
#include <string>
#include <vector>
#include <functional>

// Test result structure for enhanced reporting
struct TestResult {
    std::string name;
    bool passed;
    std::string message;
    double executionTimeMs;
    
    TestResult(const std::string& n, bool p, const std::string& m = "", double t = 0.0)
        : name(n), passed(p), message(m), executionTimeMs(t) {}
};

// Enhanced test framework class
class TestFramework {
private:
    std::vector<TestResult> results;
    size_t totalTests = 0;
    size_t passedTests = 0;
    
    template<typename TestFunc>
    TestResult runSingleTest(const std::string& name, TestFunc testFunc) {
        auto start = std::chrono::high_resolution_clock::now();
        bool success = false;
        std::string errorMessage = "";
        
        try {
            testFunc();
            success = true;
        } catch (const std::exception& e) {
            errorMessage = e.what();
            success = false;
        } catch (...) {
            success = false;
        }
        
        auto end = std::chrono::high_resolution_clock::now();
        double duration = std::chrono::duration<double, std::milli>(end - start).count();
        
        if (success) {
            passedTests++;
        }
        totalTests++;
        
        return TestResult(name, success, errorMessage, duration);
    }

public:
    void addTest(const std::string& name, std::function<void()> testFunc) {
        TestResult result = runSingleTest(name, testFunc);
        results.push_back(result);
        
        std::cout << "  " << name << ": ";
        if (result.passed) {
            std::cout << "PASSED (" << std::fixed << std::setprecision(3) 
                     << result.executionTimeMs << "ms)" << std::endl;
        } else {
            std::cout << "FAILED (" << std::fixed << std::setprecision(3) 
                     << result.executionTimeMs << "ms)";
            if (!result.message.empty()) {
                std::cout << " - " << result.message;
            }
            std::cout << std::endl;
        }
    }
    
    void printSummary() const {
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed: " << passedTests << std::endl;
        std::cout << "Failed: " << (totalTests - passedTests) << std::endl;
        std::cout << std::fixed << std::setprecision(2);
        
        if (totalTests > 0) {
            double successRate = (passedTests * 100.0) / totalTests;
            std::cout << "Success rate: " << successRate << "%" << std::endl;
        }
        
        // Print detailed results for debugging
        std::cout << std::endl << "=== Detailed Test Results ===" << std::endl;
        for (const auto& result : results) {
            std::cout << "  " << result.name << ": ";
            if (result.passed) {
                std::cout << "PASS";
            } else {
                std::cout << "FAIL";
            }
            std::cout << " (" << std::setprecision(3) << result.executionTimeMs << "ms)";
            if (!result.message.empty()) {
                std::cout << " - " << result.message;
            }
            std::cout << std::endl;
        }
    }
    
    bool allPassed() const {
        return passedTests == totalTests;
    }
};

// Enhanced test namespace structure with better organization
namespace test_framework {
    TestFramework framework;
}

// Forward declarations of enhanced test functions
namespace test_types {
    void runAllEnhanced(TestFramework& framework);
}

namespace test_config {
    void runAllEnhanced(TestFramework& framework);
}

namespace test_random {
    void runAllEnhanced(TestFramework& framework);
}

namespace test_clock {
    void runAllEnhanced(TestFramework& framework);
}

namespace test_neuron {
    void runAllEnhanced(TestFramework& framework);
}

namespace test_synapse {
    void runAllEnhanced(TestFramework& framework);
}

namespace test_stdp {
    void runAllEnhanced(TestFramework& framework);
}

namespace test_brain {
    void runAllEnhanced(TestFramework& framework);
}

namespace test_memory {
    void runAllEnhanced(TestFramework& framework);
}

namespace test_cognition {
    void runAllEnhanced(TestFramework& framework);
}

namespace test_experiments {
    void runAllEnhanced(TestFramework& framework);
}

int main() {
    std::cout << "=== NLM Phase 2 Enhanced Tests ===" << std::endl;
    std::cout << "Testing Real Neural Computation with Enhanced Framework:" << std::endl;
    std::cout << "  - LIF Neuron Dynamics" << std::endl;
    std::cout << "  - Synaptic Transmission" << std::endl;
    std::cout << "  - STDP Plasticity" << std::endl;
    std::cout << "  - Memory Systems" << std::endl;
    std::cout << "  - Cognitive Functions" << std::endl;
    std::cout << "  - Integration Experiments" << std::endl;
    std::cout << std::endl;
    
    // Run all test suites using enhanced framework
    test_framework::framework.addTest("Types Tests", 
        []() { test_types::runAllEnhanced(test_framework::framework); });
    
    test_framework::framework.addTest("Config Tests", 
        []() { test_config::runAllEnhanced(test_framework::framework); });
    
    test_framework::framework.addTest("Random Tests", 
        []() { test_random::runAllEnhanced(test_framework::framework); });
    
    test_framework::framework.addTest("Clock Tests", 
        []() { test_clock::runAllEnhanced(test_framework::framework); });
    
    test_framework::framework.addTest("Neuron Tests", 
        []() { test_neuron::runAllEnhanced(test_framework::framework); });
    
    test_framework::framework.addTest("Synapse Tests", 
        []() { test_synapse::runAllEnhanced(test_framework::framework); });
    
    test_framework::framework.addTest("STDP Tests", 
        []() { test_stdp::runAllEnhanced(test_framework::framework); });
    
    test_framework::framework.addTest("Brain Tests", 
        []() { test_brain::runAllEnhanced(test_framework::framework); });
    
    test_framework::framework.addTest("Memory Tests", 
        []() { test_memory::runAllEnhanced(test_framework::framework); });
    
    test_framework::framework.addTest("Cognition Tests", 
        []() { test_cognition::runAllEnhanced(test_framework::framework); });
    
    test_framework::framework.addTest("Integration Tests", 
        []() { test_experiments::runAllEnhanced(test_framework::framework); });
    
    // Print comprehensive summary
    test_framework::framework.printSummary();
    
    if (test_framework::framework.allPassed()) {
        std::cout << std::endl << "=== All Enhanced Tests PASSED ===" << std::endl;
        return 0;
    } else {
        std::cout << std::endl << "=== Some Enhanced Tests FAILED ===" << std::endl;
        return 1;
    }
}
