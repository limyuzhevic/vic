#pragma once

#include <string>
#include <vector>
#include <memory>
#include <map>
#include <chrono>
#include "core/Logger/Logger.hpp"

namespace nlm {

class Brain;
class ConfigurationManager;

class AdvancedTestConfiguration {
public:
    enum class TestMode {
        Basic,
        Learning,
        Plasticity,
        Integration,
        Stress,
        Performance
    };
    
    enum class ValidationLevel {
        None,
        Basic,
        Comprehensive,
        Strict
    };
    
    struct StressTestParams {
        double currentStressLevel = 0.0;
        uint32_t durationSteps = 0;
        bool enableRecovery = true;
        float recoveryThreshold = 0.8f;
    };
    
    struct PerformanceBenchmarkParams {
        bool measureMemoryUsage = true;
        bool measureCpuUsage = true;
        uint32_t warmupSteps = 100;
        uint32_t measurementSteps = 1000;
        std::vector<std::string> operationsToBenchmark;
    };
    
    AdvancedTestConfiguration();
    ~AdvancedTestConfiguration() = default;
    
    // Set test mode
    void setTestMode(TestMode mode);
    TestMode getTestMode() const;
    
    // Set validation level
    void setValidationLevel(ValidationLevel level);
    ValidationLevel getValidationLevel() const;
    
    // Set stress test parameters
    void setStressTestParams(const StressTestParams& params);
    const StressTestParams& getStressTestParams() const;
    
    // Set performance benchmark parameters
    void setPerformanceBenchmarkParams(const PerformanceBenchmarkParams& params);
    const PerformanceBenchmarkParams& getPerformanceBenchmarkParams() const;
    
    // Add custom configuration parameter
    void addParameter(const std::string& key, const std::string& value);
    
    // Get configuration parameter
    std::string getParameter(const std::string& key) const;
    
    // Get all parameters
    std::map<std::string, std::string> getAllParameters() const;
    
    // Validate configuration
    std::vector<std::string> validate() const;
    
    // Generate configuration summary
    std::string generateSummary() const;
    
    // Reset to defaults
    void resetToDefaults();
    
private:
    TestMode testMode_;
    ValidationLevel validationLevel_;
    StressTestParams stressTestParams_;
    PerformanceBenchmarkParams performanceBenchmarkParams_;
    std::map<std::string, std::string> customParameters_;
    
    // Default parameter values
    void loadDefaultParameters();
};

class AdvancedTestRunner {
public:
    AdvancedTestRunner(std::shared_ptr<ConfigurationManager> config);
    ~AdvancedTestRunner() = default;
    
    // Run advanced tests based on configuration
    int runAdvancedTests(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config);
    
    // Run stress test
    bool runStressTest(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config);
    
    // Run performance benchmark
    bool runPerformanceBenchmark(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config);
    
    // Run integration test
    bool runIntegrationTest(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config);
    
    // Get test results
    std::string getTestResults() const;
    
    // Clear test results
    void clearTestResults();
    
private:
    std::shared_ptr<ConfigurationManager> config_;
    std::string testResults_;
    
    // Stress test implementation
    bool runStressTestImpl(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config);
    
    // Performance benchmark implementation
    bool runPerformanceBenchmarkImpl(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config);
    
    // Integration test implementation
    bool runIntegrationTestImpl(std::shared_ptr<Brain> brain, const AdvancedTestConfiguration& config);
    
    // Measure memory usage
    double measureMemoryUsage() const;
    
    // Measure CPU usage
    double measureCpuUsage() const;
    
    // Log test progress
    void logTestProgress(const std::string& testName, double progress) const;
};

} // namespace nlm