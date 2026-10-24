// NLM Enhanced Test Framework
// Phase 6: Complete Artificial Brain Testing

#include <iostream>
#include <cassert>
#include <vector>
#include <memory>
#include <chrono>
#include <algorithm>
#include <numeric>

// NLM includes
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "brain/Brain.hpp"
#include "brain/Neuron.hpp"
#include "brain/Synapse.hpp"
#include "sensory/SensoryInput.hpp"
#include "motor/Action.hpp"
#include "environment/Environment.hpp"
#include "agent/AgentBrain.hpp"
#include "agent/AgentBody.hpp"
#include "agent/SensoryPercept.hpp"
#include "development/DevelopmentSystem.hpp"
#include "neuromodulation/Dopamine.hpp"
#include "neuromodulation/Curiosity.hpp"
#include "neuromodulation/Novelty.hpp"
#include "neuromodulation/PredictionError.hpp"
#include "memory/NeuralWorkingMemory.hpp"
#include "memory/NeuralEpisodicMemory.hpp"
#include "memory/NeuralAssociativeMemory.hpp"
#include "prediction/PredictionSystem.hpp"
#include "cognition/NeuralPlanner.hpp"
#include "cognition/ConceptFormation.hpp"
#include "attentionalselection/AttentionalSelection.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "sensory/Vision.hpp"
#include "sensory/Audio.hpp"
#include "sensory/InternalSignals.hpp"

// Test forward declarations
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

namespace test_memory {
    void runAll();
}

namespace test_neuromodulation {
    void runAll();
}

namespace test_prediction {
    void runAll();
}

namespace test_cognition {
    void runAll();
}

namespace test_integration {
    void runAll();
}

namespace test_performance {
    void runAll();
}

namespace test_error_handling {
    void runAll();
}

namespace test_serialization {
    void runAll();
}

class TestFramework {
public:
    struct TestResult {
        std::string name;
        std::string category;
        bool passed;
        std::string message;
        double execution_time_ms;
        
        TestResult(const std::string& testName, const std::string& testCategory, 
                  bool testPassed, const std::string& testMessage, double timeMs)
            : name(testName), category(testCategory), passed(testPassed), 
              message(testMessage), execution_time_ms(timeMs) {}
    };
    
    static TestFramework& getInstance() {
        static TestFramework instance;
        return instance;
    }
    
    void addTestResult(const TestResult& result) {
        results.push_back(result);
    }
    
    void runAllTests() {
        std::cout << "=== NLM Enhanced Test Framework ===" << std::endl;
        std::cout << "Testing Complete Artificial Brain (Phase 6)" << std::endl;
        std::cout << std::endl;
        
        // Reset results
        results.clear();
        
        // Run test suites
        runTestSuite("Core Types", test_types::runAll);
        runTestSuite("Configuration", test_config::runAll);
        runTestSuite("Random Generation", test_random::runAll);
        runTestSuite("Clock System", test_clock::runAll);
        runTestSuite("Neuron Dynamics", test_neuron::runAll);
        runTestSuite("Synapse Functions", test_synapse::runAll);
        runTestSuite("STDP Plasticity", test_stdp::runAll);
        runTestSuite("Brain Integration", test_brain::runAll);
        runTestSuite("Memory Systems", test_memory::runAll);
        runTestSuite("Neuromodulation", test_neuromodulation::runAll);
        runTestSuite("Prediction", test_prediction::runAll);
        runTestSuite("Cognition", test_cognition::runAll);
        runTestSuite("System Integration", test_integration::runAll);
        runTestSuite("Performance", test_performance::runAll);
        runTestSuite("Error Handling", test_error_handling::runAll);
        runTestSuite("Serialization", test_serialization::runAll);
        
        // Print summary
        printSummary();
    }
    
    void runTestSuite(const std::string& name, std::function<void()> testFunc) {
        std::cout << "Running " << name << " tests..." << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        try {
            testFunc();
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            
            TestResult result(name, name + " Suite", true, "All tests passed", duration);
            addTestResult(result);
            std::cout << "  PASSED (" << duration << " ms)" << std::endl;
        } catch (const std::exception& e) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            
            TestResult result(name, name + " Suite", false, std::string("Exception: ") + e.what(), duration);
            addTestResult(result);
            std::cout << "  FAILED (" << duration << " ms): " << e.what() << std::endl;
        } catch (...) {
            auto endTime = std::chrono::high_resolution_clock::now();
            double duration = std::chrono::duration<double, std::milli>(endTime - startTime).count();
            
            TestResult result(name, name + " Suite", false, "Unknown exception occurred", duration);
            addTestResult(result);
            std::cout << "  FAILED (" << duration << " ms): Unknown exception" << std::endl;
        }
    }
    
    void printSummary() {
        std::cout << std::endl;
        std::cout << "=== Test Summary ===" << std::endl;
        
        int totalTests = results.size();
        int passedTests = 0;
        int failedTests = 0;
        
        std::cout << "Category                 | Status     | Duration (ms)  | Message" << std::endl;
        std::cout << "-------------------------|------------|----------------|-------" << std::endl;
        
        for (const auto& result : results) {
            std::string status = result.passed ? "PASSED" : "FAILED";
            std::string duration = std::to_string(result.execution_time_ms);
            std::string message = result.message.length() > 50 ? 
                result.message.substr(0, 47) + "..." : result.message;
            
            // Truncate long messages
            if (message.length() > 50) {
                message = message.substr(0, 47) + "...";
            }
            
            std::cout << std::left << std::setw(25) << result.category 
                      << "| " << std::left << std::setw(12) << status
                      << "| " << std::left << std::setw(16) << duration
                      << "| " << message << std::endl;
            
            if (result.passed) passedTests++;
            else failedTests++;
        }
        
        std::cout << std::endl;
        std::cout << "Total Test Suites: " << totalTests << std::endl;
        std::cout << "Passed:              " << passedTests << std::endl;
        std::cout << "Failed:              " << failedTests << std::endl;
        
        if (failedTests == 0) {
            std::cout << std::endl;
            std::cout << "=== ALL TESTS PASSED ===" << std::endl;
        } else {
            std::cout << std::endl;
            std::cout << "=== SOME TESTS FAILED ===" << std::endl;
        }
    }
    
    double getTotalExecutionTime() const {
        double total = 0.0;
        for (const auto& result : results) {
            total += result.execution_time_ms;
        }
        return total;
    }
    
    int getPassedCount() const {
        int count = 0;
        for (const auto& result : results) {
            if (result.passed) count++;
        }
        return count;
    }
    
    int getFailedCount() const {
        int count = 0;
        for (const auto& result : results) {
            if (!result.passed) count++;
        }
        return count;
    }
    
private:
    TestFramework() = default;
    std::vector<TestResult> results;
};

int main() {
    // Initialize global logger
    auto logger = std::make_shared<nlm::Logger>();
    auto consoleLogger = std::make_shared<nlm::ConsoleLogger>(nlm::LogLevel::Info);
    logger->addLogger(consoleLogger);
    nlm::Logger::setGlobal(logger);
    
    nlm::NLM_LOG_INFO("=== NLM Enhanced Test Framework ===");
    nlm::NLM_LOG_INFO("Phase 6: Complete Artificial Brain Testing");
    
    // Run enhanced test framework
    TestFramework::getInstance().runAllTests();
    
    double totalTime = TestFramework::getInstance().getTotalExecutionTime();
    nlm::NLM_LOG_INFO("");
    nlm::NLM_LOG_INFO("=== Testing Complete ===");
    nlm::NLM_LOG_INFO("Total execution time: " + std::to_string(totalTime) + " ms");
    nlm::NLM_LOG_INFO("Tests passed: " + std::to_string(TestFramework::getInstance().getPassedCount()));
    nlm::NLM_LOG_INFO("Tests failed: " + std::to_string(TestFramework::getInstance().getFailedCount()));
    
    return TestFramework::getInstance().getFailedCount() > 0 ? 1 : 0;
}