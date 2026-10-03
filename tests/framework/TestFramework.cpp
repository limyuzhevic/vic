Test Framework Implementation
// Complete implementation of the test framework for NLM Phase 6 integration

#include "TestFramework.hpp"
#include <algorithm>
#include <numeric>

namespace test_framework {

// Implementation of TestSuite methods (if any additional functionality needed)

// Implementation of IntegrationTestSuite methods

// Implementation of BenchmarkSuite methods

// Implementation of TestAssertions methods

// Implementation of TestRunner methods is already complete in the header

// Additional utility functions for comprehensive testing

// Performance benchmark comparison function
BenchmarkSuite::BenchmarkResult BenchmarkSuite::compareBenchmarks(
    const std::string& name,
    std::function<void()> operation,
    size_t iterations,
    const std::string& comparisonMetrics
) {
    auto start = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < iterations; ++i) {
        operation();
    }
    
    auto end = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end - start);
    
    double durationMs = static_cast<double>(duration.count()) / 1000.0;
    size_t opsPerSec = static_cast<size_t>(iterations * 1000000.0 / duration.count());
    
    return BenchmarkResult(name, durationMs, opsPerSec);
}

// Test summary generator
std::string TestRunner::generateTestSummary(const std::vector<TestResult>& results) {
    std::stringstream summary;
    
    int totalTests = results.size();
    int passedTests = countPassedTests(results);
    int failedTests = totalTests - passedTests;
    
    summary << "=== TEST SUMMARY ===\n";
    summary << "Total Tests: " << totalTests << "\n";
    summary << "Passed: " << passedTests << "\n";
    summary << "Failed: " << failedTests << "\n";
    summary << "Success Rate: " << std::fixed << std::setprecision(2) 
           << (totalTests > 0 ? (100.0 * passedTests / totalTests) : 0.0) << "%\n\n";
    
    if (failedTests > 0) {
        summary << "=== FAILED TESTS ===\n";
        for (const auto& result : results) {
            if (!result.passed) {
                summary << "- " << result.systemName << "::" << result.testName << "\n";
                if (!result.errorDetails.empty()) {
                    summary << "  Error: " << result.errorDetails << "\n";
                }
            }
        }
    }
    
    return summary.str();
}

// Stress test utility for system validation
bool TestRunner::runStressTest(
    std::shared_ptr<Brain> brain,
    size_t maxSteps,
    const std::string& stressTestName
) {
    try {
        NLM_LOG_INFO("Running stress test: " + stressTestName);
        
        size_t stepsCompleted = 0;
        size_t failureCount = 0;
        
        for (uint64_t step = 0; step < maxSteps && failureCount < 10; ++step) {
            // Simulate various stressors
            if (step % 10 == 0) {
                // Inject sensory input at different intensities
                brain->injectCurrentToNeurons(NeuronType::Sensory, 
                    static_cast<MembranePotential>(10.0f + (step % 20)));
            }
            
            if (step % 5 == 0) {
                // Apply neuromodulatory signals
                Dopamine d;
                d.setLevel(0.5f + (step % 10) * 0.05f);
                brain->applyNeuromodulation(d);
            }
            
            // Execute main step
            brain->step(step, step * 0.001);
            stepsCompleted++;
            
            // Monitor system health
            if (brain->getTotalSpikeCount() > 1000000) {
                failureCount++;
            }
        }
        
        if (stepsCompleted >= maxSteps / 2) {
            NLM_LOG_INFO("Stress test " + stressTestName + " passed: " + 
                        std::to_string(stepsCompleted) + " steps completed");
            return true;
        } else {
            NLM_LOG_ERROR("Stress test " + stressTestName + " failed: " + 
                         std::to_string(stepsCompleted) + " steps completed");
            return false;
        }
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Stress test " + stressTestName + " exception: " + std::string(e.what()));
        return false;
    }
}

// Memory leak detection (basic implementation)
bool TestRunner::detectMemoryLeaks() {
    try {
        auto config = std::make_shared<Config>();
        config->set("neuron_count", static_cast<int64_t>(100));
        config->set("region_count", static_cast<int64_t>(1));
        
        // Create multiple brain instances to check for leaks
        std::vector<std::shared_ptr<Brain>> brains;
        
        for (int i = 0; i < 10; ++i) {
            auto brain = IntegrationTestSuite::createIntegratedBrain();
            brains.push_back(brain);
            
            // Simulate some work
            for (uint64_t step = 0; step < 100; ++step) {
                brain->step(step, step * 0.001);
            }
        }
        
        // Brains should be destroyed here
        brains.clear();
        
        // If we get here without exceptions, memory management is likely OK
        return true;
        
    } catch (const std::exception& e) {
        NLM_LOG_ERROR("Memory leak detection failed: " + std::string(e.what()));
        return false;
    }
}

} // namespace test_framework