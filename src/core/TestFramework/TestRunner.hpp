#pragma once

#include "TestBase.hpp"
#include "TestConfig.hpp"
#include <memory>
#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <functional>

namespace nlm {

class TestRunner {
public:
    struct TestResult {
        std::string testName;
        std::string testCategory;
        std::string testPhase;
        bool passed;
        bool skipped;
        std::string errorMessage;
        std::chrono::milliseconds duration;
        std::string output;
        std::map<std::string, std::string> metrics;
        std::map<std::string, std::string> metadata;
        
        // Constructor for passed tests
        TestResult(const std::string& name, const std::string& category, const std::string& phase)
            : testName(name), testCategory(category), testPhase(phase),
              passed(true), skipped(false), duration(0), metrics(), metadata() {}
        
        // Constructor for failed tests
        TestResult(const std::string& name, const std::string& category, const std::string& phase, 
                   const std::string& error)
            : testName(name), testCategory(category), testPhase(phase),
              passed(false), skipped(false), errorMessage(error), duration(0) {}
        
        // Constructor for skipped tests
        TestResult(const std::string& name, const std::string& category, const std::string& phase,
                   const std::string& reason, bool skipped = true)
            : testName(name), testCategory(category), testPhase(phase),
              passed(false), skipped(skipped), errorMessage(reason), duration(0) {}
    };
    
    // Main test execution
    static std::vector<TestResult> runTests(
        const std::vector<std::unique_ptr<TestBase>>& tests,
        const TestConfig& config,
        std::shared_ptr<Brain> brain = nullptr);
    
    // Test execution with progress tracking
    static std::vector<TestResult> runTestsWithProgress(
        const std::vector<std::unique_ptr<TestBase>>& tests,
        const TestConfig& config,
        std::shared_ptr<Brain> brain = nullptr);
    
    // Run single test
    static TestResult runTest(
        std::unique_ptr<TestBase>& test,
        const TestConfig& config,
        std::shared_ptr<Brain> brain = nullptr);
    
    // Test filtering and selection
    static std::vector<std::unique_ptr<TestBase>> selectTests(
        const TestRegistry& registry,
        const TestConfig& config);
    
    // Result analysis
    static bool analyzeResults(const std::vector<TestResult>& results);
    static std::string generateSummary(const std::vector<TestResult>& results);
    static std::map<std::string, std::string> calculateStatistics(const std::vector<TestResult>& results);
    
    // Test lifecycle management
    static bool setupTestEnvironment(const TestConfig& config, std::shared_ptr<Brain>& brain);
    static bool teardownTestEnvironment(const TestConfig& config, std::shared_ptr<Brain>& brain);
    
    // Brain management
    static std::shared_ptr<Brain> createBrain(const TestConfig& config);
    static bool resetBrain(std::shared_ptr<Brain> brain, const TestConfig& config);
    static bool saveBrainState(std::shared_ptr<Brain> brain, const std::string& filepath);
    static bool loadBrainState(std::shared_ptr<Brain> brain, const std::string& filepath);
    
    // Configuration validation
    static bool validateTestConfig(const TestConfig& config, std::vector<std::string>& errors);
    static bool validateTestDependencies(const std::vector<std::unique_ptr<TestBase>>& tests);
    
private:
    static std::vector<TestResult> executeTestSequence(
        const std::vector<std::unique_ptr<TestBase>>& tests,
        const TestConfig& config,
        std::shared_ptr<Brain> brain);
    
    static TestResult executeSingleTest(
        std::unique_ptr<TestBase>& test,
        const TestConfig& config,
        std::shared_ptr<Brain> brain,
        bool& brainResetRequired);
};

} // namespace nlm
