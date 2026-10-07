// IntegrationTest.hpp - Test suite for NLM Performance Profiling System
// Comprehensive tests for all profiling components

#pragma once

/**
 * IntegrationTest - Comprehensive test suite for NLM performance profiling
 * 
 * This test suite provides comprehensive testing for all components of the
 * NLM performance profiling system, including:
 * 
 * 1. Core Infrastructure Tests
 *    - PerformanceProfiler tests
 *    - ProfileDataPoint tests
 *    - ProfileSession tests
 * 
 * 2. Performance Monitoring Tests
 *    - PerformanceMonitor tests
 *    - CPU monitoring tests
 *    - Memory monitoring tests
 *    - Thread monitoring tests
 *    - I/O monitoring tests
 * 
 * 3. Profiling Data Collection Tests
 *    - SystemDataCollector tests
 *    - SimulationDataCollector tests
 *    - MemoryDataCollector tests
 *    - ThreadDataCollector tests
 *    - IODataCollector tests
 * 
 * 4. Analysis and Reporting Tests
 *    - NLPerformanceAnalyzer tests
 *    - Bottleneck analysis tests
 *    - Optimization recommendation tests
 *    - Comparative analysis tests
 *    - Trend analysis tests
 *    - Automated test execution tests
 * 
 * 5. Export and Integration Tests
 *    - JSON exporter tests
 *    - CSV exporter tests
 *    - YAML exporter tests
 *    - XML exporter tests
 *    - Database exporter tests
 *    - Integration adapter tests
 *    - Debug system integration tests
 *    - Visualization integration tests
 * 
 * 6. Advanced Features Tests
 *    - Dynamic profiling activation tests
 *    - Memory leak detection tests
 *    - Race condition detection tests
 *    - CPU cache profiling tests
 *    - Hook system tests
 * 
 * 7. Integration Tests
 *    - NLMIntegrationManager tests
 *    - Component lifecycle tests
 *    - Event handling tests
 *    - Configuration management tests
 * 
 * 8. Debug System Integration Tests
 *    - Logger integration tests
 *    - Debug event integration tests
 *    - Performance integration tests
 * 
 * Features:
 * - Unit tests for all components
 * - Integration tests for component interactions
 * - Performance tests for system efficiency
 * - Memory leak detection tests
 * - Race condition detection tests
 * - Error handling tests
 * - Configuration validation tests
 * - Export format tests
 * - Debug system integration tests
 * - Real-time monitoring tests
 * - Comprehensive test reporting
 * 
 * This test suite is designed to be:
 * - Comprehensive: Tests all major functionality
 * - Maintainable: Well-structured and easy to extend
 * - Robust: Handles edge cases and error conditions
 * - Performance-aware: Avoids performance overhead during testing
 * - Configurable: Allows flexible test configuration
 * - Reportable: Generates detailed test reports
 */

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <atomic>
#include <functional>
#include <algorithm>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <mutex>
#include <thread>
#include <future>
#include <map>
#include <variant>
#include <optional>
#include <numeric>
#include <cmath>
#include <type_traits>
#include <set>
#include <queue>
#include <list>
#include <regex>
#include <numeric>
#include <filesystem>
#include <random>

namespace nlm {

/**
 * Test result structure
 */
struct TestResult {
    std::string testName;
    std::string testSuite;
    bool passed;
    std::string message;
    std::chrono::milliseconds duration;
    std::string errorDetails;
    
    TestResult() : passed(false), duration(0) {}
    TestResult(const std::string& name, const std::string& suite) 
        : testName(name), testSuite(suite), passed(false), duration(0) {}
};

/**
 * Test suite result
 */
struct TestSuiteResult {
    std::string suiteName;
    std::vector<TestResult> testResults;
    bool passedAll;
    std::chrono::milliseconds totalDuration;
    size_t passedCount;
    size_t failedCount;
    size_t skippedCount;
    
    TestSuiteResult() : passedAll(true), totalDuration(0), passedCount(0), 
                       failedCount(0), skippedCount(0) {}
};

/**
 * Test configuration
 */
struct TestConfig {
    std::string testName;
    bool enabled = true;
    std::chrono::milliseconds timeout;
    bool setupRequired;
    bool teardownRequired;
    std::unordered_map<std::string, std::string> parameters;
    
    TestConfig() : timeout(std::chrono::milliseconds(1000)), setupRequired(false), teardownRequired(false) {}
};

/**
 * Base test class
 */
class TestBase {
public:
    virtual ~TestBase() = default;
    
    virtual std::string getTestName() const = 0;
    virtual std::string getTestSuite() const = 0;
    virtual bool setup() = 0;
    virtual bool execute() = 0;
    virtual bool teardown() = 0;
    
    virtual bool isEnabled() const { return enabled_; }
    virtual void setEnabled(bool enabled) { enabled_ = enabled; }
    
    virtual std::chrono::milliseconds getTimeout() const { return timeout_; }
    virtual void setTimeout(std::chrono::milliseconds timeout) { timeout_ = timeout; }
    
    std::string runTest();
    
protected:
    bool enabled_ = true;
    std::chrono::milliseconds timeout_ = std::chrono::milliseconds(1000);
};

/**
 * Test helper functions
 */
class TestHelper {
public:
    // Random number generation
    static double randomDouble(double min = 0.0, double max = 1.0);
    static int64_t randomInt64(int64_t min = 0, int64_t max = 1000);
    static uint64_t randomUint64(uint64_t min = 0, uint64_t max = 1000);
    static size_t randomSize(size_t min = 0, size_t max = 100);
    static std::string randomString(size_t length);
    
    // String utilities
    static bool stringsEqualIgnoreCase(const std::string& a, const std::string& b);
    static std::string toLower(const std::string& str);
    static std::string toUpper(const std::string& str);
    static bool contains(const std::string& str, const std::string& substr);
    static std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);
    
    // File utilities
    static bool fileExists(const std::string& path);
    static std::string readFile(const std::string& path);
    static bool writeFile(const std::string& path, const std::string& content);
    static void createDirectory(const std::string& path);
    
    // Math utilities
    static double mean(const std::vector<double>& values);
    static double variance(const std::vector<double>& values, double mean = 0.0);
    static double standardDeviation(const std::vector<double>& values, double mean = 0.0);
    static double median(const std::vector<double>& values);
    static double percentile(const std::vector<double>& values, double percentile);
    static double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y);
    
    // Time utilities
    static std::string formatTimeNs(double timeNs);
    static std::string formatMemoryBytes(size_t bytes);
    static std::string formatPercentage(double value);
    
    // Validation utilities
    static bool isWithinRange(double value, double min, double max);
    static bool isWithinRange(int64_t value, int64_t min, int64_t max);
    static bool isWithinRange(size_t value, size_t min, size_t max);
    static bool stringsAreEqualIgnoringWhitespace(const std::string& a, const std::string& b);
};

/**
 * PerformanceProfiler tests
 */
class PerformanceProfilerTest : public TestBase {
private:
    std::shared_ptr<NLPerformanceProfiler> profiler_;
    
public:
    std::string getTestName() const override { return "PerformanceProfilerTest"; }
    std::string getTestSuite() const override { return "CoreInfrastructure"; }
    
    bool setup() override;
    bool execute() override;
    bool teardown() override;
};

/**
 * PerformanceMonitor tests
 */
class PerformanceMonitorTest : public TestBase {
private:
    std::shared_ptr<PerformanceMonitor> monitor_;
    
public:
    std::string getTestName() const override { return "PerformanceMonitorTest"; }
    std::string getTestSuite() const override { return "PerformanceMonitoring"; }
    
    bool setup() override;
    bool execute() override;
    bool teardown() override;
};

/**
 * ProfilingDataCollector tests
 */
class ProfilingDataCollectorTest : public TestBase {
private:
    std::shared_ptr<ProfilingDataCollector> dataCollector_;
    
public:
    std::string getTestName() const override { return "ProfilingDataCollectorTest"; }
    std::string getTestSuite() const override { return "DataCollection"; }
    
    bool setup() override;
    bool execute() override;
    bool teardown() override;
};

/**
 * NLPerformanceAnalyzer tests
 */
class NLPerformanceAnalyzerTest : public TestBase {
private:
    std::shared_ptr<NLPerformanceAnalyzer> analyzer_;
    
public:
    std::string getTestName() const override { return "NLPerformanceAnalyzerTest"; }
    std::string getTestSuite() const override { return "AnalysisAndReporting"; }
    
    bool setup() override;
    bool execute() override;
    bool teardown() override;
};

/**
 * ExportIntegration tests
 */
class ExportIntegrationTest : public TestBase {
private:
    std::shared_ptr<ExportIntegrationManager> exportManager_;
    
public:
    std::string getTestName() const override { return "ExportIntegrationTest"; }
    std::string getTestSuite() const override { return "ExportAndIntegration"; }
    
    bool setup() override;
    bool execute() override;
    bool teardown() override;
};

/**
 * AdvancedProfiler tests
 */
class AdvancedProfilerTest : public TestBase {
private:
    std::shared_ptr<AdvancedProfilerManager> advancedProfiler_;
    
public:
    std::string getTestName() const override { return "AdvancedProfilerTest"; }
    std::string getTestSuite() const override { return "AdvancedFeatures"; }
    
    bool setup() override;
    bool execute() override;
    bool teardown() override;
};

/**
 * NLMIntegrationManager tests
 */
class NLMIntegrationManagerTest : public TestBase {
private:
    std::shared_ptr<NLMIntegrationManager> integrationManager_;
    
public:
    std::string getTestName() const override { return "NLMIntegrationManagerTest"; }
    std::string getTestSuite() const override { return "Integration"; }
    
    bool setup() override;
    bool execute() override;
    bool teardown() override;
};

/**
 * DebugSystemIntegration tests
 */
class DebugSystemIntegrationTest : public TestBase {
private:
    std::shared_ptr<NLMIntegrationManager> integrationManager_;
    
public:
    std::string getTestName() const override { return "DebugSystemIntegrationTest"; }
    std::string getTestSuite() const override { return "DebugIntegration"; }
    
    bool setup() override;
    bool execute() override;
    bool teardown() override;
};

/**
 * PerformanceTestSuite
 */
class PerformanceTestSuite {
private:
    std::vector<std::shared_ptr<TestBase>> tests_;
    std::vector<TestSuiteResult> results_;
    
public:
    PerformanceTestSuite();
    
    void addTest(std::shared_ptr<TestBase> test);
    bool runAllTests();
    bool runTestSuite(const std::string& suiteName);
    
    TestSuiteResult getTestSuiteResult(const std::string& suiteName) const;
    std::vector<std::string> getTestSuiteNames() const;
    
    void printTestResults() const;
    std::string generateTestReport() const;
    bool saveTestReport(const std::string& outputPath) const;
    
    void clearResults();
    size_t getTotalTestsCount() const;
    size_t getPassedTestsCount() const;
    size_t getFailedTestsCount() const;
    size_t getSkippedTestsCount() const;
    
    double getSuccessRate() const;
};

/**
 * IntegrationTestFactory
 */
class IntegrationTestFactory {
public:
    static std::shared_ptr<TestBase> createTest(const std::string& testName);
    static std::shared_ptr<PerformanceTestSuite> createTestSuite(const std::string& suiteName);
    static std::vector<std::string> getAvailableTests();
    static std::vector<std::string> getAvailableTestSuites();
};

} // namespace nlm