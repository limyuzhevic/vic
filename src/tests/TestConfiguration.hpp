#pragma once

#include <string>
#include <map>
#include <unordered_map>
#include <vector>
#include <chrono>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <functional>
#include "src/core/Types/Types.hpp"

namespace nlm {

enum class TestLevel {
    UNIT,
    INTEGRATION,
    PERFORMANCE,
    STRESS,
    MEMORY
};

enum class TestStatus {
    PASSED,
    FAILED,
    SKIPPED,
    ERROR,
    TIMEOUT
};

enum class TestCategory {
    NEURAL_DYNAMICS,
    PLASTICITY,
    MEMORY_SYSTEM,
    PREDICTION,
    COGNITION,
    NEUROMODULATION,
    DEVELOPMENT,
    MULTI_AGENT,
    PERFORMANCE,
    MEMORY_LEAK,
    INTEGRATION,
    REGRESSION
};

struct TestParameters {
    std::string name;
    TestLevel level;
    TestCategory category;
    std::unordered_map<std::string, std::string> properties;
    std::chrono::duration<double> timeout;
    std::chrono::duration<double> maxMemory;
    std::vector<std::string> tags;
    bool enabled;
    
    TestParameters(const std::string& n, TestLevel l, TestCategory c)
        : name(n), level(l), category(c), timeout(10.0), maxMemory(1024.0), enabled(true) {}
};

struct TestResult {
    std::string testName;
    TestStatus status;
    TestParameters params;
    std::chrono::duration<double> duration;
    std::chrono::duration<double> memoryUsage;
    std::string failureMessage;
    std::string errorMessage;
    int exitCode;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::map<std::string, std::string> metrics;
    
    TestResult(const std::string& name, const TestParameters& p)
        : testName(name), status(TestStatus::SKIPPED), params(p), 
          duration(0.0), memoryUsage(0.0), exitCode(0) {}
};

struct BenchmarkResult {
    std::string benchmarkName;
    double executionTime;
    double memoryUsage;
    double cpuUsage;
    int64_t operationsPerSecond;
    std::chrono::duration<double> duration;
};

class TestConfiguration {
private:
    static TestConfiguration* instance;
    std::mutex configMutex;
    
    std::unordered_map<std::string, std::string> globalConfig;
    std::unordered_map<std::string, TestParameters> testConfigs;
    std::vector<std::string> enabledCategories;
    std::vector<std::string> disabledCategories;
    
    TestConfiguration() = default;
    
public:
    static TestConfiguration& getInstance() {
        if (!instance) {
            instance = new TestConfiguration();
        }
        return *instance;
    }
    
    void setGlobalConfig(const std::string& key, const std::string& value);
    const std::string& getGlobalConfig(const std::string& key) const;
    
    void addTestConfig(const std::string& testName, const TestParameters& config);
    TestParameters& getTestConfig(const std::string& testName);
    
    void enableCategory(TestCategory category);
    void disableCategory(TestCategory category);
    bool isCategoryEnabled(TestCategory category) const;
    
    std::vector<std::string> getEnabledCategories() const;
    
    void setDefaultTimeout(double seconds);
    void setDefaultMemoryLimit(double megabytes);
    
    std::string toString() const;
    
    ~TestConfiguration() = default;
};

} // namespace nlm
