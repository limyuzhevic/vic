#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <functional>
#include <chrono>
#include <thread>
#include <mutex>
#include <atomic>
#include <future>
#include <sstream>
#include <iomanip>
#include <optional>
#include "../brain/Brain.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Test execution modes
enum class ExecutionMode {
    Sequential,
    Parallel,
    Dependent
};

// Progress tracking
struct ProgressInfo {
    std::string testName;
    size_t currentStep;
    size_t totalSteps;
    double percentage;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdate;
    std::atomic<bool> cancelled;
    
    ProgressInfo(const std::string& name, size_t total)
        : testName(name), currentStep(0), totalSteps(total), 
          percentage(0.0), cancelled(false) {
        startTime = std::chrono::steady_clock::now();
        lastUpdate = startTime;
    }
};

// Test execution result
struct TestResult {
    std::string testName;
    bool success;
    std::string errorMessage;
    std::chrono::milliseconds executionTime;
    std::chrono::milliseconds wallClockTime;
    double finalScore;
    std::map<std::string, std::string> metrics;
    
    TestResult(const std::string& name) 
        : testName(name), success(false), executionTime(0), 
          wallClockTime(0), finalScore(0.0) {}
};

// Test parameters base
struct TestParameters {
    std::string testType;
    std::map<std::string, std::string> params;
    std::vector<std::string> dependencies;
    
    template<typename T>
    T getParam(const std::string& key, const T& defaultValue) const {
        auto it = params.find(key);
        if (it == params.end()) return defaultValue;
        
        // Simple conversion - would need proper implementation for complex types
        if constexpr (std::is_same_v<T, int>) {
            return std::stoi(it->second);
        } else if constexpr (std::is_same_v<T, double>) {
            return std::stod(it->second);
        } else if constexpr (std::is_same_v<T, bool>) {
            std::string val = it->second;
            return val == "true" || val == "1" || val == "yes";
        } else if constexpr (std::is_same_v<T, std::string>) {
            return it->second;
        }
        return defaultValue;
    }
};

// Test function type
using TestFunction = std::function<bool(std::shared_ptr<Brain>, const TestParameters&)>;

// Test metadata
struct TestMetadata {
    std::string name;
    std::string description;
    std::vector<std::string> defaultParameters;
    std::vector<std::string> dependencies;
    std::function<void(std::shared_ptr<Brain>, const TestParameters&)> setupFunc;
    std::function<void(std::shared_ptr<Brain>, const TestParameters&)> cleanupFunc;
    std::function<void(std::shared_ptr<Brain>, const TestParameters&, ProgressInfo&)> progressCallback;
};

// Main TestRunner class
class TestRunner {
public:
    TestRunner(std::shared_ptr<Config> config, std::shared_ptr<Logger> logger);
    ~TestRunner();
    
    // Test registration
    void registerTest(const std::string& name, const TestMetadata& metadata);
    void registerTest(const std::string& name, TestFunction testFunc, const std::string& description = "");
    
    // Test execution
    TestResult runTest(const std::string& testName, const TestParameters& params);
    std::vector<TestResult> runTests(const std::vector<std::string>& testNames, const std::vector<TestParameters>& paramsList);
    
    // Configuration
    void setExecutionMode(ExecutionMode mode);
    void setMaxParallelTests(int maxParallel);
    void setProgressReporting(bool enabled);
    void setTimeout(std::chrono::seconds timeout);
    
    // Progress and metrics
    void setProgressCallback(std::function<void(const ProgressInfo&)> callback);
    std::vector<ProgressInfo> getProgressInfo() const;
    std::vector<TestResult> getResults() const;
    
    // Results management
    void clearResults();
    bool saveResults(const std::string& filepath) const;
    bool loadResults(const std::string& filepath);
    
    // Validation
    bool validateTestParameters(const std::string& testName, const TestParameters& params) const;
    std::vector<std::string> getAvailableTests() const;
    
    // Statistics
    double getAverageScore() const;
    std::chrono::milliseconds getTotalExecutionTime() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal helper methods
    bool executeTestSequentially(const std::string& testName, const TestParameters& params);
    bool executeTestInParallel(const std::string& testName, const TestParameters& params);
    bool executeTestWithDependencies(const std::string& testName, const TestParameters& params);
    
    void updateProgress(const std::string& testName, size_t current, size_t total);
    void finalizeProgress(const std::string& testName, bool success, const std::string& error = "");
    
    TestFunction getTestFunction(const std::string& testName) const;
    TestMetadata getTestMetadata(const std::string& testName) const;
    
    std::chrono::steady_clock::time_point getStartTime() const;
    void recordResult(const TestResult& result);
};

} // namespace nlm
