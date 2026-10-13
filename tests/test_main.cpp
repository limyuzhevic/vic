// NLM Test Main
// Phase 6: Final Integration Tests

#include <iostream>
#include <cassert>
#include <vector>
#include <string>
#include <memory>

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

namespace test_integration {
    void runAll();
}

namespace test_memory {
    void runAll();
}

namespace test_neuromodulation {
    void runAll();
}

namespace test_prediction {
    void runAll();
}

namespace test_development {
    void runAll();
}

namespace test_agent_brain {
    void runAll();
}

namespace test_python_bindings {
    void runAll();
}

// Test runner with comprehensive reporting
class TestRunner {
private:
    struct TestResult {
        std::string name;
        bool passed;
        std::string message;
        double execution_time;
    };
    
    std::vector<TestResult> results;
    int passed_count = 0;
    int failed_count = 0;
    
public:
    void addResult(const std::string& name, bool passed, const std::string& message = "") {
        TestResult result;
        result.name = name;
        result.passed = passed;
        result.message = message;
        result.execution_time = 0.0; // Would measure with high-resolution timer
        
        results.push_back(result);
        
        if (passed) {
            passed_count++;
            std::cout << "✓ " << name << " PASSED" << std::endl;
        } else {
            failed_count++;
            std::cout << "✗ " << name << " FAILED" << std::endl;
            if (!message.empty()) {
                std::cout << "  Message: " << message << std::endl;
            }
        }
    }
    
    void printSummary() {
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        std::cout << "Total tests: " << (passed_count + failed_count) << std::endl;
        std::cout << "Passed: " << passed_count << std::endl;
        std::cout << "Failed: " << failed_count << std::endl;
        
        if (failed_count == 0) {
            std::cout << "Status: ALL TESTS PASSED ✓" << std::endl;
        } else {
            std::cout << "Status: SOME TESTS FAILED ✗" << std::endl;
            
            // Detailed failure report
            std::cout << std::endl;
            std::cout << "=== Failed Tests ===" << std::endl;
            for (const auto& result : results) {
                if (!result.passed) {
                    std::cout << "- " << result.name << std::endl;
                    if (!result.message.empty()) {
                        std::cout << "  Details: " << result.message << std::endl;
                    }
                }
            }
        }
    }
};

int main() {
    std::cout << "=== NLM Phase 6 Final Integration Tests ===" << std::endl;
    std::cout << "Testing Complete Artificial Brain System:" << std::endl;
    std::cout << "  - Neural Integration" << std::endl;
    std::cout << "  - Memory Systems" << std::endl;
    std::cout << "  - Neuromodulation" << std::endl;
    std::cout << "  - Prediction Systems" << std::endl;
    std::cout << "  - Development" << std::endl;
    std::cout << "  - Agent Interface" << std::endl;
    std::cout << "  - Python Bindings" << std::endl;
    std::cout << std::endl;
    
    TestRunner runner;
    
    // Core system tests
    std::cout << "=== Core System Tests ===" << std::endl;
    try {
        std::cout << "Running Types tests..." << std::endl;
        test_types::runAll();
        runner.addResult("Types Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("Types Tests", false, e.what());
    }
    
    try {
        std::cout << "Running Config tests..." << std::endl;
        test_config::runAll();
        runner.addResult("Config Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("Config Tests", false, e.what());
    }
    
    try {
        std::cout << "Running Random tests..." << std::endl;
        test_random::runAll();
        runner.addResult("Random Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("Random Tests", false, e.what());
    }
    
    try {
        std::cout << "Running Clock tests..." << std::endl;
        test_clock::runAll();
        runner.addResult("Clock Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("Clock Tests", false, e.what());
    }
    
    try {
        std::cout << "Running Neuron tests..." << std::endl;
        test_neuron::runAll();
        runner.addResult("Neuron Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("Neuron Tests", false, e.what());
    }
    
    try {
        std::cout << "Running Synapse tests..." << std::endl;
        test_synapse::runAll();
        runner.addResult("Synapse Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("Synapse Tests", false, e.what());
    }
    
    try {
        std::cout << "Running STDP tests..." << std::endl;
        test_stdp::runAll();
        runner.addResult("STDP Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("STDP Tests", false, e.what());
    }
    
    try {
        std::cout << "Running Brain tests..." << std::endl;
        test_brain::runAll();
        runner.addResult("Brain Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("Brain Tests", false, e.what());
    }
    
    // Integration tests
    std::cout << std::endl;
    std::cout << "=== Integration Tests ===" << std::endl;
    try {
        std::cout << "Running Memory Integration tests..." << std::endl;
        test_memory::runAll();
        runner.addResult("Memory Integration Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("Memory Integration Tests", false, e.what());
    }
    
    try {
        std::cout << "Running Neuromodulation Integration tests..." << std::endl;
        test_neuromodulation::runAll();
        runner.addResult("Neuromodulation Integration Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("Neuromodulation Integration Tests", false, e.what());
    }
    
    try {
        std::cout << "Running Prediction Integration tests..." << std::endl;
        test_prediction::runAll();
        runner.addResult("Prediction Integration Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("Prediction Integration Tests", false, e.what());
    }
    
    try {
        std::cout << "Running Development Integration tests..." << std::endl;
        test_development::runAll();
        runner.addResult("Development Integration Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("Development Integration Tests", false, e.what());
    }
    
    try {
        std::cout << "Running AgentBrain Integration tests..." << std::endl;
        test_agent_brain::runAll();
        runner.addResult("AgentBrain Integration Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("AgentBrain Integration Tests", false, e.what());
    }
    
    // Modern framework tests
    std::cout << std::endl;
    std::cout << "=== Modern Framework Tests ===" << std::endl;
    try {
        std::cout << "Running Python Bindings tests..." << std::endl;
        test_python_bindings::runAll();
        runner.addResult("Python Bindings Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("Python Bindings Tests", false, e.what());
    }
    
    // Final integration test
    std::cout << std::endl;
    std::cout << "=== Final Integration Test ===" << std::endl;
    try {
        std::cout << "Running Phase 6 Integration tests..." << std::endl;
        test_integration::runAll();
        runner.addResult("Phase 6 Integration Tests", true);
    } catch (const std::exception& e) {
        runner.addResult("Phase 6 Integration Tests", false, e.what());
    }
    
    runner.printSummary();
    
    return (failed_count > 0) ? 1 : 0;
}

