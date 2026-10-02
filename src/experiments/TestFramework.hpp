// Test Framework for NLM Phase 2
// Provides modular test execution and management

#pragma once

#include "../brain/Brain.hpp"
#include "../environment/Environment.hpp"
#include "Experiment.hpp"
#include <memory>
#include <vector>
#include <functional>
#include <chrono>

namespace nlm {

// Test Result structure for comprehensive reporting
struct TestResult {
    std::string testName;
    std::string description;
    bool passed;
    std::string errorMessage;
    double executionTime;
    std::map<std::string, double> metrics;
    
    TestResult(const std::string& name, const std::string& desc) 
        : testName(name), description(desc), passed(false), executionTime(0.0) {}
};

// Base Test Class - provides common interface for all test types
class Test {
public:
    Test(const std::string& name, const std::string& description);
    virtual ~Test();
    
    // Test interface
    virtual bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) = 0;
    virtual void setUp(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {}
    virtual void tearDown(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) {}
    
    // Metadata
    const std::string& getName() const { return name; }
    const std::string& getDescription() const { return description; }
    const std::vector<TestResult>& getResults() const { return results; }
    
    // Test categories
    enum class Category {
        Connectivity,
        Plasticity,
        Neuromodulation,
        Integration,
        Performance,
        Learning,
        Memory
    };
    
    void setCategory(Category category) { this->category = category; }
    Category getCategory() const { return category; }
    
    // Configuration
    void setConfigValue(const std::string& key, const std::string& value);
    std::string getConfigValue(const std::string& key) const;
    
protected:
    std::string name;
    std::string description;
    std::map<std::string, std::string> config;
    Category category;
    std::vector<TestResult> results;
};

// Test Factory - creates tests based on configuration
class TestFactory {
public:
    static TestFactory& getInstance();
    
    // Register test type
    template<typename T>
    void registerTestType(const std::string& name, const std::string& description, 
                         Test::Category category = Test::Category::Connectivity);
    
    // Create test instance
    std::unique_ptr<Test> createTest(const std::string& testType,
                                    const std::map<std::string, std::string>& config = {});
    
    // Get list of available tests
    std::vector<std::string> getAvailableTests() const;
    
    // Get test info
    std::pair<std::string, std::string> getTestInfo(const std::string& testType) const;
    
private:
    TestFactory();
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Test Suite - manages a collection of tests
class TestSuite {
public:
    TestSuite(const std::string& name);
    ~TestSuite();
    
    // Add test to suite
    void addTest(std::unique_ptr<Test> test);
    
    // Add test by type
    template<typename T>
    void addTest(const std::map<std::string, std::string>& config = {});
    
    // Run all tests in suite
    std::vector<TestResult> runAll(std::shared_ptr<Brain> brain, 
                                  std::shared_ptr<Environment> environment);
    
    // Run specific test category
    std::vector<TestResult> runCategory(Test::Category category,
                                       std::shared_ptr<Brain> brain,
                                       std::shared_ptr<Environment> environment);
    
    // Get suite name
    const std::string& getName() const { return name; }
    
    // Get all tests
    const std::vector<std::unique_ptr<Test>>& getTests() const { return tests; }
    
    // Save results
    bool saveResults(const std::string& filepath) const;
    
    // Generate summary report
    std::string generateSummaryReport() const;
    
private:
    std::string name;
    std::vector<std::unique_ptr<Test>> tests;
};

// Specialized Test Types - ready-to-use test implementations

class ConnectivityTest : public Test {
public:
    ConnectivityTest();
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) override;
};

class PlasticityTest : public Test {
public:
    PlasticityTest();
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) override;
};

class NeuromodulationTest : public Test {
public:
    NeuromodulationTest();
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) override;
};

class IntegrationTest : public Test {
public:
    IntegrationTest();
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) override;
};

class LearningTest : public Test {
public:
    LearningTest();
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) override;
};

class PerformanceTest : public Test {
public:
    PerformanceTest();
    bool run(std::shared_ptr<Brain> brain, std::shared_ptr<Environment> environment) override;
};

// Test Manager - high-level interface for test execution
class TestManager {
public:
    TestManager();
    ~TestManager();
    
    // Run predefined test suites
    std::vector<TestResult> runStandardTests(std::shared_ptr<Brain> brain,
                                            std::shared_ptr<Environment> environment);
    
    std::vector<TestResult> runQuickTests(std::shared_ptr<Brain> brain,
                                          std::shared_ptr<Environment> environment);
    
    std::vector<TestResult> runComprehensiveTests(std::shared_ptr<Brain> brain,
                                                  std::shared_ptr<Environment> environment);
    
    // Run custom test suite
    std::vector<TestResult> runCustomSuite(std::unique_ptr<TestSuite> suite,
                                          std::shared_ptr<Brain> brain,
                                          std::shared_ptr<Environment> environment);
    
    // Configure test manager
    void setDefaultConfig(const std::map<std::string, std::string>& config);
    void setVerboseMode(bool verbose) { verboseMode = verbose; }
    void setParallelMode(bool parallel) { parallelMode = parallel; }
    
    // Get test statistics
    struct TestStatistics {
        size_t totalTests;
        size_t passedTests;
        size_t failedTests;
        double totalExecutionTime;
        std::map<std::string, size_t> testsByCategory;
    };
    
    TestStatistics getStatistics(const std::vector<TestResult>& results) const;
    
private:
    std::map<std::string, std::string> defaultConfig;
    bool verboseMode;
    bool parallelMode;
};

} // namespace nlm
