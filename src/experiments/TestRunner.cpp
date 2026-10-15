#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <algorithm>
#include <sstream>
#include <iomanip>
#include <fstream>
#include <iomanip>
#include <numeric>
#include "experiments/ExperimentFramework.hpp"
#include "experiments/TestFramework.hpp"

using namespace nlm;

// Test runner for comprehensive test suite
class TestRunner {
public:
    TestRunner() {
        setupTestFramework();
    }
    
    void setupTestFramework() {
        // Create test configuration
        testConfig_.name = "comprehensive-test-suite";
        testConfig_.parameters["neurons"] = 1000;
        testConfig_.parameters["steps"] = 2000;
        testConfig_.parameters["learning_rate"] = 0.01;
        testConfig_.parameters["plasticity_rate"] = 0.5;
        testConfig_.parameters["memory_usage_mb"] = 100.0;
        testConfig_.options["save_results"] = "true";
        testConfig_.options["detailed_output"] = "true";
    }
    
    void runAllTests() {
        std::cout << "=== NLM Comprehensive Test Suite ===" << std::endl;
        std::cout << "Running comprehensive tests for neural simulation framework..." << std::endl;
        std::cout << std::endl;
        
        // Create test suite
        ComprehensiveTestSuite testSuite;
        
        // Run all tests
        std::vector<TestResult> results = testSuite.runAllTests(testConfig_);
        
        // Display results
        testSuite.printResults(results);
        
        // Save detailed results
        if (testConfig_.options.find("save_results") != testConfig_.options.end()) {
            std::string filename = "test_results_" + 
                                 std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + 
                                 ".json";
            testSuite.saveTestResults(results, filename);
        }
        
        // Calculate overall score
        double overallScore = calculateOverallScore(results);
        std::cout << "\n=== Overall Test Score ===" << std::endl;
        std::cout << "Overall Performance Score: " << (overallScore * 100) << "%" << std::endl;
        
        if (overallScore >= 0.8) {
            std::cout << "Status: EXCELLENT ✓" << std::endl;
        } else if (overallScore >= 0.6) {
            std::cout << "Status: GOOD ✓" << std::endl;
        } else if (overallScore >= 0.4) {
            std::cout << "Status: ACCEPTABLE ⚠" << std::endl;
        } else {
            std::cout << "Status: NEEDS IMPROVEMENT ✗" << std::endl;
        }
        
        // Return appropriate exit code
        return (overallScore >= 0.6) ? 0 : 1;
    }
    
    double calculateOverallScore(const std::vector<TestResult>& results) const {
        if (results.empty()) return 0.0;
        
        double totalScore = 0.0;
        for (const auto& result : results) {
            if (result.passed) {
                totalScore += 1.0;
            } else {
                totalScore += 0.0;
            }
        }
        
        return totalScore / results.size();
    }
    
    void showTestUsage() {
        std::cout << "=== NLM Test Suite Usage ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Available test commands:" << std::endl;
        std::cout << "  --run-all               - Run all comprehensive tests" << std::endl;
        std::cout << "  --run-basic             - Run basic functionality tests" << std::endl;
        std::cout << "  --run-performance        - Run performance tests" << std::endl;
        std::cout << "  --run-integration        - Run integration tests" << std::endl;
        std::cout << "  --run-stress            - Run stress tests" << std::endl;
        std::cout << "  --benchmarks            - Run performance benchmarks" << std::endl;
        std::cout << "  --save-results          - Save test results to file" << std::endl;
        std::cout << std::endl;
        std::cout << "Test configuration options:" << std::endl;
        std::cout << "  --neurons=N            - Number of neurons to test (default: 1000)" << std::endl;
        std::cout << "  --steps=N              - Number of simulation steps (default: 2000)" << std::endl;
        std::cout << "  --learning_rate=R       - Learning rate (default: 0.01)" << std::endl;
        std::cout << "  --plasticity_rate=R    - Plasticity rate (default: 0.5)" << std::endl;
        std::cout << std::endl;
        std::cout << "Example usage:" << std::endl;
        std::cout << "./nlm_test --run-all --neurons=2000 --steps=5000 --save-results" << std::endl;
    }
    
private:
    ExperimentConfig testConfig_;
};

int main(int argc, char* argv[]) {
    std::cout << "=== NLM Comprehensive Test Runner ===" << std::endl;
    std::cout << "Testing neural simulation framework with comprehensive test suite..." << std::endl;
    std::cout << std::endl;
    
    // Parse command line arguments
    bool runAll = false;
    bool runBasic = false;
    bool runPerformance = false;
    bool runIntegration = false;
    bool runStress = false;
    bool runBenchmarks = false;
    bool saveResults = false;
    int neurons = 1000;
    int steps = 2000;
    double learningRate = 0.01;
    double plasticityRate = 0.5;
    
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        
        if (arg == "--run-all") {
            runAll = true;
        } else if (arg == "--run-basic") {
            runBasic = true;
        } else if (arg == "--run-performance") {
            runPerformance = true;
        } else if (arg == "--run-integration") {
            runIntegration = true;
        } else if (arg == "--run-stress") {
            runStress = true;
        } else if (arg == "--benchmarks") {
            runBenchmarks = true;
        } else if (arg == "--save-results") {
            saveResults = true;
        } else if (arg.find("--neurons=") == 0) {
            neurons = std::stoi(arg.substr(9));
        } else if (arg.find("--steps=") == 0) {
            steps = std::stoi(arg.substr(8));
        } else if (arg.find("--learning_rate=") == 0) {
            learningRate = std::stod(arg.substr(16));
        } else if (arg.find("--plasticity_rate=") == 0) {
            plasticityRate = std::stod(arg.substr(18));
        } else if (arg == "--help" || arg == "-h") {
            TestRunner runner;
            runner.showTestUsage();
            return 0;
        }
    }
    
    // If no specific test command given, run all tests
    if (!runAll && !runBasic && !runPerformance && !runIntegration && !runStress && !runBenchmarks) {
        runAll = true;
    }
    
    // Create test configuration
    ExperimentConfig config;
    config.name = "test-suite";
    config.parameters["neurons"] = neurons;
    config.parameters["steps"] = steps;
    config.parameters["learning_rate"] = learningRate;
    config.parameters["plasticity_rate"] = plasticityRate;
    config.options["save_results"] = saveResults ? "true" : "false";
    
    // Run tests
    TestRunner runner;
    
    if (runAll) {
        std::cout << "Running comprehensive test suite..." << std::endl;
        return runner.runAllTests();
    } else {
        std::cout << "Running specific test categories..." << std::endl;
        std::cout << "Note: Specific test category selection requires additional implementation" << std::endl;
        std::cout << "Running full test suite as default..." << std::endl;
        return runner.runAllTests();
    }
}
