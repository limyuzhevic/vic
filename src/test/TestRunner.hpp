#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <iostream>
#include <chrono>
#include <thread>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"
#include "core/SimulationClock/SimulationClock.hpp"
#include "brain/Brain.hpp"

namespace nlm {

// Test result structure
struct TestResult {
    std::string name;
    bool passed;
    double duration;
    std::string description;
    std::map<std::string, std::string> metrics;
    std::string errorMessage;
    std::string output;
};

// Test case interface
class TestCase {
public:
    virtual ~TestCase() = default;
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual TestResult run(std::shared_ptr<Config> config) = 0;
};

// Test runner for executing tests
class TestRunner {
public:
    TestRunner();
    ~TestRunner() = default;
    
    // Add a test case
    void addTest(std::unique_ptr<TestCase> test);
    
    // Run all tests
    std::vector<TestResult> runAll(std::shared_ptr<Config> config);
    
    // Run a specific test by name
    TestResult runTest(const std::string& testName, std::shared_ptr<Config> config);
    
    // Run tests with progress reporting
    std::vector<TestResult> runAllWithProgress(std::shared_ptr<Config> config,
                                            ProgressReporter& reporter);
    
    // Get all test names
    std::vector<std::string> getTestNames() const;
    
    // Get test by name
    TestCase* getTest(const std::string& testName);
    
    // Clear all tests
    void clearTests();
    
    // Set test output format
    void setOutputFormat(const std::string& format);
    
    // Enable/disable verbose output
    void setVerbose(bool verbose);
    
    // Get summary of test results
    std::string generateSummary(const std::vector<TestResult>& results) const;
    
    // Save test results to file
    bool saveResults(const std::vector<TestResult>& results,
                    const std::string& filepath,
                    const std::string& format = "json") const;

private:
    std::vector<std::unique_ptr<TestCase>> tests_;
    std::string outputFormat_;
    bool verbose_ = false;
};

// Concrete test cases for NLM
class BasicConnectivityTest : public TestCase {
public:
    std::string getName() const override { return "Basic Connectivity Test"; }
    std::string getDescription() const override { 
        return "Tests basic neural connectivity and spike propagation"; 
    }
    TestResult run(std::shared_ptr<Config> config) override;
};

class PlasticityExperimentTest : public TestCase {
public:
    std::string getName() const override { return "Plasticity Experiment Test"; }
    std::string getDescription() const override { 
        return "Tests synaptic plasticity learning experiment"; 
    }
    TestResult run(std::shared_ptr<Config> config) override;
};

class STDPVerificationTest : public TestCase {
public:
    std::string getName() const override { return "STDP Verification Test"; }
    std::string getDescription() const override { 
        return "Tests Spike-Timing-Dependent Plasticity"; 
    }
    TestResult run(std::shared_ptr<Config> config) override;
};

class ConfigValidationTest : public TestCase {
public:
    std::string getName() const override { return "Configuration Validation Test"; }
    std::string getDescription() const override { 
        return "Validates configuration parameters and settings"; 
    }
    TestResult run(std::shared_ptr<Config> config) override;
};

class PerformanceBenchmarkTest : public TestCase {
public:
    std::string getName() const override { return "Performance Benchmark Test"; }
    std::string getDescription() const override { 
        return "Benchmarks neural network performance"; 
    }
    TestResult run(std::shared_ptr<Config> config) override;
};

// Integration test runner
class IntegrationTestRunner {
public:
    IntegrationTestRunner() = default;
    
    // Run all integration tests
    std::vector<TestResult> runAllIntegrationTests(std::shared_ptr<Config> config);
    
    // Run specific integration tests
    std::vector<TestResult> runIntegrationTests(const std::vector<std::string>& testNames,
                                              std::shared_ptr<Config> config);
    
    // Generate integration test report
    std::string generateIntegrationReport(const std::vector<TestResult>& results) const;
    
    // Save integration test report
    bool saveIntegrationReport(const std::vector<TestResult>& results,
                              const std::string& filepath,
                              const std::string& format = "html") const;

private:
    TestRunner testRunner_;
};

} // namespace nlm