// Comprehensive NLM Testing and Benchmarking Example
// This example demonstrates how to use the NLM testing framework
// Includes unit tests, performance benchmarks, and comparison testing

#include "include/nlm/TestingFramework.hpp"
#include "src/brain/Brain.hpp"
#include "src/core/Config/Config.hpp"
#include <iostream>
#include <vector>
#include <memory>

using namespace nlm;
using namespace nlm::testing;

// Example 1: Basic unit test
void testBrainCreation() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(100), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    assert(brain.getRegionCount() == 1);
    assert(brain.getTotalNeuronCount() == 100);
    
    std::cout << "✓ Brain creation test passed" << std::endl;
}

// Example 2: Initialization test
void testBrainInitialization() {
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(200), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config->set("connection_probability", 0.15, ConfigSource::Default);
    
    Brain brain(config);
    bool success = brain.initialize();
    assert(success);
    
    assert(brain.getTotalNeuronCount() == 200);
    assert(brain.getTotalSynapseCount() > 0);
    
    std::cout << "✓ Brain initialization test passed" << std::endl;
}

// Example 3: Performance benchmark
void benchmarkBrainStep(std::shared_ptr<Brain> brain) {
    // Warmup
    for (size_t i = 0; i < 100; ++i) {
        brain->step(i);
    }
    
    // Benchmark
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (size_t i = 0; i < 1000; ++i) {
        brain->step(i);
    }
    
    auto endTime = std::chrono::high_resolution_clock::now();
    double elapsedMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
    
    double stepsPerSecond = 1000.0 / (elapsedMs / 1000.0);
    double avgStepTimeMs = elapsedMs / 1000.0;
    
    std::cout << "Benchmark results:" << std::endl;
    std::cout << "  Steps per second: " << stepsPerSecond << std::endl;
    std::cout << "  Average step time: " << avgStepTimeMs << " ms" << std::endl;
}

// Example 4: Comparison test
void compareConfigurations() {
    std::vector<std::shared_ptr<Config>> configs;
    
    // Configuration 1: Standard settings
    auto config1 = std::make_shared<Config>();
    config1->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config1->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config1->set("connection_probability", 0.1, ConfigSource::Default);
    configs.push_back(config1);
    
    // Configuration 2: High connectivity
    auto config2 = std::make_shared<Config>();
    config2->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config2->set("region_count", static_cast<int64_t>(2), ConfigSource::Default);
    config2->set("connection_probability", 0.3, ConfigSource::Default);
    configs.push_back(config2);
    
    // Configuration 3: Low connectivity
    auto config3 = std::make_shared<Config>();
    config3->set("neuron_count", static_cast<int64_t>(1000), ConfigSource::Default);
    config3->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    config3->set("connection_probability", 0.05, ConfigSource::Default);
    configs.push_back(config3);
    
    ComparisonTester comparisonTester;
    auto results = comparisonTester.compare(configs, "Brain Performance Comparison", 500);
    
    std::cout << "Configuration Comparison Results:" << std::endl;
    for (const auto& result : results) {
        std::cout << "  " << result.at("test_name") << ": " << result.at("steps_per_second") << " steps/sec" << std::endl;
    }
}

// Example 5: Stress test
void stressTest() {
    StressTester stressTester;
    
    StressTester::StressTestConfig config;
    config.maxNeurons = 5000;
    config.testMemoryLimits = true;
    config.testPerformanceLimits = true;
    config.testNetworkLimits = true;
    
    auto results = stressTester.runStressTests(config);
    
    std::cout << "Stress Test Results:" << std::endl;
    for (const auto& result : results) {
        std::cout << "  " << result.testName << ": " << (result.passed ? "PASSED" : "FAILED") << std::endl;
        if (!result.passed) {
            std::cout << "    Error: " << result.errorMessage << std::endl;
        }
    }
}

// Example 6: Benchmark suite
void runBenchmarkSuite() {
    BenchmarkSuite benchmarkSuite("Brain Performance Benchmarks");
    
    auto config = std::make_shared<Config>();
    config->set("neuron_count", static_cast<int64_t>(500), ConfigSource::Default);
    config->set("region_count", static_cast<int64_t>(1), ConfigSource::Default);
    
    Brain brain(config);
    brain.initialize();
    
    BenchmarkConfig benchmarkConfig;
    benchmarkConfig.maxIterations = 5;
    benchmarkConfig.warmupSteps = 50;
    
    benchmarkSuite.addBenchmark("Step Performance", 
                               std::bind(benchmarkBrainStep, std::placeholders::_1),
                               benchmarkConfig);
    
    auto results = benchmarkSuite.runAll(std::make_shared<Brain>(brain));
    
    std::cout << "Benchmark Suite Results:" << std::endl;
    for (const auto& result : results) {
        std::cout << "  " << result.testName << ": " << result.executionTimeMs << " ms" << std::endl;
    }
}

// Main function demonstrating all testing features
int main() {
    // Initialize logger for testing
    auto logger = std::make_shared<Logger>();
    auto consoleLogger = std::make_shared<ConsoleLogger>(LogLevel::Info);
    logger->addLogger(consoleLogger);
    Logger::setGlobal(logger);
    
    NLM_LOG_INFO("Starting NLM Comprehensive Testing and Benchmarking Suite");
    
    // Create test suite
    TestSuite testSuite("Comprehensive NLM Tests");
    
    // Add unit tests
    testSuite.addTest("Brain Creation Test", testBrainCreation, "unit");
    testSuite.addTest("Brain Initialization Test", testBrainInitialization, "unit");
    
    // Run unit tests
    std::cout << "\n=== Running Unit Tests ===" << std::endl;
    auto unitTestResults = testSuite.runAll();
    
    // Display unit test results
    TestReporter::printSummary(unitTestResults);
    
    // Run other testing examples
    std::cout << "\n=== Running Performance Comparison ===" << std::endl;
    compareConfigurations();
    
    std::cout << "\n=== Running Stress Tests ===" << std::endl;
    stressTest();
    
    std::cout << "\n=== Running Benchmark Suite ===" << std::endl;
    runBenchmarkSuite();
    
    // Generate comprehensive report
    std::cout << "\n=== Generating Comprehensive Report ===" << std::endl;
    std::string reportFilename = "nlm_test_report_" + 
        std::to_string(std::chrono::system_clock::now().time_since_epoch().count()) + ".html";
    
    TestReporter::generateReport(unitTestResults, reportFilename, TestReporter::ReportFormat::HTML);
    std::cout << "Report generated: " << reportFilename << std::endl;
    
    std::cout << "\n=== Testing Suite Complete ===" << std::endl;
    
    return 0;
}
