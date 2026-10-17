// run_regression_tests.cpp - Regression test runner for NLM
#include "TestFramework.hpp"
#include "TestUtils.hpp"
#include "StatisticalTests.cpp"
#include "MemoryTests.cpp"
#include "BehavioralTests.cpp"

namespace nlm {

// Regression test runner for validating system stability
class RegressionTestRunner {
public:
    void runTest() {
        TestRunner runner(false);  // Silent mode for regression tests
        
        // Register regression tests
        register_statistical_regression_tests(runner);
        register_performance_regression_tests(runner);
        register_memory_retention_tests(runner);
        register_learning_progress_tests(runner);
        
        std::cout << "=== NLM Regression Test Suite ===" << std::endl;
        runner.runAll();
    }
private:
    void register_statistical_regression_tests(TestRunner& runner) {
        TEST_CASE(StatisticalRegression, "Statistical Validation Tests");
        TEST_CASE(StatisticalLearning, "Learning Curve Regression Tests");
        TEST_CASE(StatisticalMemory, "Memory Effect Hypothesis Tests");
        TEST_CASE(StatisticalDevelopment, "Developmental Regression Tests");
    }
    
    void register_performance_regression_tests(TestRunner& runner) {
        TEST_CASE(PerformanceRegression, "Performance Regression Tests");
        TEST_CASE(SystemStability, "System Stability Tests");
        TEST_CASE(IntegrationRegression, "Integration Regression Tests");
    }
    
    void register_memory_retention_tests(TestRunner& runner) {
        TEST_CASE(MemoryRetentionRegression, "Memory Retention Regression Tests");
        TEST_CASE(MemoryDecayRegression, "Memory Decay Regression Tests");
        TEST_CASE(MemoryReplayRegression, "Memory Replay Regression Tests");
    }
    
    void register_learning_progress_tests(TestRunner& runner) {
        TEST_CASE(LearningProgressRegression, "Learning Progress Regression Tests");
        TEST_CASE(DevelopmentalRegression, "Developmental Progress Regression Tests");
        TEST_CASE(ContinualLearningRegression, "Continual Learning Regression Tests");
    }
};

// Performance metrics validation
namespace PerformanceRegression {
    bool validatePerformanceBaseline(const std::string& testName, double baselineValue, double currentValue, double tolerance) {
        if (std::abs(currentValue - baselineValue) > tolerance) {
            std::cout << "FAIL: " << testName << " performance regression. "
                     << "Baseline: " << baselineValue << ", Current: " << currentValue << " (tolerance: " << tolerance << ")" << std::endl;
            return false;
        }
        std::cout << "PASS: " << testName << " performance within tolerance." << std::endl;
        return true;
    }
    
    bool validateScalability(const std::string& testName, size_t inputSize, double expectedGrowth, double actualGrowth) {
        double ratio = actualGrowth / expectedGrowth;
        if (ratio < 0.8 || ratio > 1.2) {  // 20% tolerance
            std::cout << "FAIL: " << testName << " scalability regression. "
                     << "Expected growth: " << expectedGrowth << ", Actual: " << actualGrowth << std::endl;
            return false;
        }
        std::cout << "PASS: " << testName << " scalability within expected range." << std::endl;
        return true;
    }
}}

// Entry point for regression tests
int main(int argc, char* argv[]) {
    std::cout << "=== NLM Regression Test Suite ===" << std::endl;
    
    // Parse command line arguments
    bool verbose = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if (arg == "--verbose" || arg == "-v") {
            verbose = true;
        }
    }
    
    RegressionTestRunner testSuite;
    testSuite.runTest();
    
    return 0;
}
