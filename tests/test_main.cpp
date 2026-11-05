// NLM Test Main
// Phase 2: Real Neural Computation Tests

#include <iostream>
#include <cassert>
#include <memory>
#include <vector>
#include <string>
#include <chrono>

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

// Advanced test runner with error tracking and reporting
struct TestRunner {
    struct TestResult {
        std::string name;
        bool passed;
        std::string errorMessage;
        double durationMs;
        int testNumber;
        
        TestResult(const std::string& n, bool p = true, const std::string& err = "", double dur = 0.0, int num = 0)
            : name(n), passed(p), errorMessage(err), durationMs(dur), testNumber(num) {}
    };
    
    std::vector<TestResult> results;
    int totalTests;
    int passedTests;
    int failedTests;
    double totalDurationMs;
    
    TestRunner() : totalTests(0), passedTests(0), failedTests(0), totalDurationMs(0.0) {}
    
    template<typename TestFunc>
    void runTest(const std::string& name, TestFunc testFunc) {
        auto start = std::chrono::high_resolution_clock::now();
        totalTests++;
        
        try {
            testFunc();
            passedTests++;
            results.emplace_back(name, true, "", getElapsedMs(start));
        } catch (const std::exception& e) {
            failedTests++;
            results.emplace_back(name, false, std::string("Exception: ") + e.what(), getElapsedMs(start), totalTests);
        } catch (...) {
            failedTests++;
            results.emplace_back(name, false, "Unknown exception", getElapsedMs(start), totalTests);
        }
    }
    
    double getElapsedMs(std::chrono::high_resolution_clock::time_point start) {
        auto end = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
        return static_cast<double>(duration.count());
    }
    
    void printSummary() const {
        std::cout << std::endl;
        std::cout << "=== NLM TEST RUNNER SUMMARY ===" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Passed:      " << passedTests << std::endl;
        std::cout << "Failed:      " << failedTests << std::endl;
        std::cout << "Total time:  " << totalDurationMs << "ms" << std::endl;
        
        if (failedTests > 0) {
            std::cout << std::endl;
            std::cout << "=== FAILED TESTS ===" << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "Test " << result.testNumber << ": " << result.name << std::endl;
                    std::cout << "  Error: " << result.errorMessage << std::endl;
                    std::cout << "  Duration: " << result.durationMs << "ms" << std::endl;
                    std::cout << std::endl;
                }
            }
        }
        
        if (failedTests == 0) {
            std::cout << std::endl;
            std::cout << "✓ ALL TESTS PASSED ===" << std::endl;
        }
    }
};

// Enhanced test categories with better organization
namespace test_categories {
    
    void runCoreTests(TestRunner& runner) {
        std::cout << "Running Core Tests..." << std::endl;
        runner.runTest("Types", test_types::runAll);
        runner.runTest("Config", test_config::runAll);
        runner.runTest("Random", test_random::runAll);
        runner.runTest("Clock", test_clock::runAll);
    }
    
    void runNeuralTests(TestRunner& runner) {
        std::cout << "Running Neural Tests..." << std::endl;
        runner.runTest("Neuron", test_neuron::runAll);
        runner.runTest("Synapse", test_synapse::runAll);
        runner.runTest("STDP", test_stdp::runAll);
    }
    
    void runIntegrationTests(TestRunner& runner) {
        std::cout << "Running Integration Tests..." << std::endl;
        runner.runTest("Brain", test_brain::runAll);
    }
    
    void runAllTests() {
        std::cout << "=== NLM Phase 2 Tests ===" << std::endl;
        std::cout << "Testing Real Neural Computation:" << std::endl;
        std::cout << "  - LIF Neuron Dynamics" << std::endl;
        std::cout << "  - Synaptic Transmission" << std::endl;
        std::cout << "  - STDP Plasticity" << std::endl;
        std::cout << std::endl;
        
        TestRunner runner;
        
        // Run tests in categories with better organization
        runCoreTests(runner);
        runNeuralTests(runner);
        runIntegrationTests(runner);
        
        // Print comprehensive summary
        runner.printSummary();
        
        // Return appropriate exit code
        return runner.failedTests == 0 ? 0 : 1;
    }
    
} // namespace test_categories

int main() {
    try {
        return test_categories::runAllTests();
    } catch (const std::exception& e) {
        std::cout << std::endl;
        std::cout << "=== FATAL TEST RUNNER ERROR ===" << std::endl;
        std::cout << "Exception: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cout << std::endl;
        std::cout << "=== FATAL TEST RUNNER ERROR ===" << std::endl;
        std::cout << "Unknown exception occurred during test execution" << std::endl;
        return 1;
    }
}
