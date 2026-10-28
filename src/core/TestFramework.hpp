// TestFramework.hpp
#pragma once

#include "core/Types/Types.hpp"
#include "brain/Brain.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>
#include <chrono>

namespace nlm {

// Test configuration parameters
struct TestConfig {
    std::string name;
    std::string description;
    SimulationStep steps = 100;
    float currentLevel = 30.0f;
    bool recordMetrics = true;
    std::vector<std::string> neuronTypes;
    std::unordered_map<std::string, double> parameters;
    
    TestConfig(const std::string& n, const std::string& d = "") 
        : name(n), description(d) {}
};

// Test result structure
struct TestResult {
    std::string testName;
    bool passed = false;
    double executionTime = 0.0;
    std::unordered_map<std::string, double> metrics;
    std::string errorMessage;
    size_t neuronsTested = 0;
    size_t spikesGenerated = 0;
    double initialWeightMean = 0.0;
    double finalWeightMean = 0.0;
    double weightChange = 0.0;
};

// Base Test class
class Test {
public:
    virtual ~Test() = default;
    virtual const std::string& getName() const = 0;
    virtual const std::string& getDescription() const = 0;
    virtual TestResult run(std::shared_ptr<Brain> brain, const TestConfig& config) = 0;
    virtual bool setup(std::shared_ptr<Brain> brain, const TestConfig& config) { return true; }
    virtual void teardown(std::shared_ptr<Brain> brain, const TestConfig& config) {}
};

// Test Framework Manager
class TestFramework {
public:
    TestFramework();
    ~TestFramework();
    
    // Test registration
    void registerTest(std::unique_ptr<Test> test);
    std::shared_ptr<Test> getTest(const std::string& name) const;
    
    // Test execution
    TestResult runTest(const std::string& name, std::shared_ptr<Brain> brain, const TestConfig& config);
    std::vector<TestResult> runBatch(const std::vector<std::string>& testNames, std::shared_ptr<Brain> brain, const TestConfig& config);
    
    // Configuration management
    void addDefaultConfig(const std::string& key, const std::string& value);
    void setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    
    // Results management
    const std::vector<TestResult>& getResults() const { return results_; }
    void clearResults() { results_.clear(); }
    
    // Statistics
    double getAveragePassRate() const;
    size_t getTotalTestsRun() const { return results_.size(); }
    size_t getPassedTestsCount() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::vector<TestResult> results_;
};

} // namespace nlm