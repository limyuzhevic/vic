#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <chrono>
#include <map>
#include <set>
#include <sstream>
#include <fstream>
#include <algorithm>

namespace nlm {

// Test result structure
struct TestResult {
    std::string testName;
    std::string testCategory;
    bool passed;
    std::string message;
    double executionTime;
    std::map<std::string, std::string> metrics;
    std::string errorDetails;
    
    TestResult(const std::string& name, const std::string& category) 
        : testName(name), testCategory(category), passed(false), 
          executionTime(0.0) {}
};

// Test case base class
class TestCase {
public:
    virtual ~TestCase() = default;
    
    virtual std::string getName() const = 0;
    virtual std::string getCategory() const = 0;
    virtual bool runTest() = 0;
    virtual void setup() {}
    virtual void teardown() {}
    
    virtual std::map<std::string, std::string> getParameters() const { return {}; }
    virtual void setParameters(const std::map<std::string, std::string>& params) {}
};

// Performance test base class
class PerformanceTest : public TestCase {
public:
    virtual double getTargetExecutionTime() const = 0;
    virtual double getAcceptableOverhead() const { return 1.5; } // 150% overhead allowed
    virtual std::string getPerformanceMetric() const { return "execution_time"; }
    
    virtual bool runTest() override {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool result = runPerformanceTest();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        if (!result) return false;
        return executionTime <= (getTargetExecutionTime() * getAcceptableOverhead());
    }
    
    virtual bool runPerformanceTest() = 0;
protected:
    double executionTime;
};

// Memory test base class
class MemoryTest : public TestCase {
public:
    virtual size_t getMemoryLimit() const = 0;
    virtual std::string getMemoryMetric() const { return "memory_usage"; }
    
    virtual bool runTest() override {
        auto startTime = std::chrono::high_resolution_clock::now();
        bool result = runMemoryTest();
        auto endTime = std::chrono::high_resolution_clock::now();
        
        executionTime = std::chrono::duration<double>(endTime - startTime).count();
        
        return result && checkMemoryUsage() <= getMemoryLimit();
    }
    
    virtual bool runMemoryTest() = 0;
    virtual size_t checkMemoryUsage() const = 0;
protected:
    double executionTime;
};

// Test runner class
class TestRunner {
public:
    TestRunner();
    ~TestRunner();
    
    void addTest(std::unique_ptr<TestCase> test);
    void addTests(const std::vector<std::unique_ptr<TestCase>>& tests);
    
    void setTestFilter(const std::string& filter);
    void setTestCategories(const std::vector<std::string>& categories);
    void setPerformanceMode(bool enabled);
    void setMemoryMode(bool enabled);
    
    std::vector<TestResult> runTests();
    TestResult runSingleTest(size_t testIndex);
    
    void setTestSetup(std::function<void()> setup);
    void setTestTeardown(std::function<void()> teardown);
    
    // Test statistics
    struct TestStatistics {
        size_t totalTests;
        size_t passedTests;
        size_t failedTests;
        double averageExecutionTime;
        double minExecutionTime;
        double maxExecutionTime;
        std::map<std::string, size_t> categoryStats;
    };
    
    TestStatistics getStatistics() const;
    
    // Test export
    void exportResults(const std::string& filepath);
    void generateReport(const std::string& filepath);
    std::string generateTextReport() const;
    
private:
    std::vector<std::unique_ptr<TestCase>> tests_;
    std::string testFilter_;
    std::vector<std::string> testCategories_;
    bool performanceMode_;
    bool memoryMode_;
    
    std::function<void()> testSetup_;
    std::function<void()> testTeardown_;
    
    std::vector<TestResult> results_;
};

// Unit test examples
class NeuronUnitTest : public TestCase {
public:
    std::string getName() const override { return "Neuron Unit Test"; }
    std::string getCategory() const override { return "Unit"; }
    
    bool runTest() override;
    void setup() override;
    void teardown() override;
    
    std::map<std::string, std::string> getParameters() const override;
    void setParameters(const std::map<std::string, std::string>& params) override;
};

class SynapseUnitTest : public TestCase {
public:
    std::string getName() const override { return "Synapse Unit Test"; }
    std::string getCategory() const override { return "Unit"; }
    
    bool runTest() override;
};

class PlasticityUnitTest : public TestCase {
public:
    std::string getName() const override { return "Plasticity Unit Test"; }
    std::string getCategory() const override { return "Unit"; }
    
    bool runTest() override;
};

class MemoryUnitTest : public TestCase {
public:
    std::string getName() const override { return "Memory Unit Test"; }
    std::string getCategory() const override { return "Unit"; }
    
    bool runTest() override;
};

// Integration test examples
class BrainIntegrationTest : public TestCase {
public:
    std::string getName() const override { return "Brain Integration Test"; }
    std::string getCategory() const override { return "Integration"; }
    
    bool runTest() override;
};

class SystemIntegrationTest : public TestCase {
public:
    std::string getName() const override { return "System Integration Test"; }
    std::string getCategory() const override { return "Integration"; }
    
    bool runTest() override;
};

// Performance test examples
class PerformanceTestExample : public PerformanceTest {
public:
    std::string getName() const override { return "Performance Test"; }
    std::string getCategory() const override { return "Performance"; }
    
    double getTargetExecutionTime() const override;
    
    bool runPerformanceTest() override;
};

// Memory test examples
class MemoryTestExample : public MemoryTest {
public:
    std::string getName() const override { return "Memory Test"; }
    std::string getCategory() const override { return "Memory"; }
    
    size_t getMemoryLimit() const override;
    
    bool runMemoryTest() override;
    size_t checkMemoryUsage() const override;
};

// Validation test examples
class ValidationTest : public TestCase {
public:
    std::string getName() const override { return "Validation Test"; }
    std::string getCategory() const override { return "Validation"; }
    
    bool runTest() override;
    
    virtual bool validateConfiguration() const;
    virtual bool validateDataIntegrity() const;
    virtual bool validatePerformanceTargets() const;
};

// Test factory class
class TestFactory {
public:
    static TestFactory& getInstance() {
        static TestFactory instance;
        return instance;
    }
    
    void registerTestType(const std::string& name, 
                         std::function<std::unique_ptr<TestCase>()> factory);
    
    std::unique_ptr<TestCase> createTest(const std::string& name);
    
    std::vector<std::string> getRegisteredTestTypes() const;
    
private:
    TestFactory() = default;
    std::map<std::string, std::function<std::unique_ptr<TestCase>()>> testFactories_;
};

// Validation framework
class ValidationFramework {
public:
    ValidationFramework();
    ~ValidationFramework();
    
    void addValidator(std::function<bool(const TestResult&)> validator);
    bool validateTestResult(const TestResult& result) const;
    std::vector<std::string> getValidationErrors(const TestResult& result) const;
    
    void setValidationRules(const std::vector<std::string>& rules);
    std::vector<std::string> getValidationRules() const;
    
    // Validation statistics
    struct ValidationStats {
        size_t totalTests;
        size_t passedValidation;
        size_t failedValidation;
        std::map<std::string, size_t> ruleStats;
    };
    
    ValidationStats getStats() const;
    
private:
    std::vector<std::function<bool(const TestResult&)>> validators_;
    std::vector<std::string> validationRules_;
    ValidationStats stats_;
};

// Test report generator
class TestReportGenerator {
public:
    static std::string generateHTMLReport(const std::vector<TestResult>& results);
    static std::string generateJSONReport(const std::vector<TestResult>& results);
    static std::string generateXMLReport(const std::vector<TestResult>& results);
    static std::string generateTextReport(const std::vector<TestResult>& results);
    
    static std::string generatePerformanceReport(const std::vector<TestResult>& performanceResults);
    static std::string generateMemoryReport(const std::vector<TestResult>& memoryResults);
    static std::string generateIntegrationReport(const std::vector<TestResult>& integrationResults);
    
private:
    static std::string escapeHTML(const std::string& text);
    static std::string formatTime(double seconds);
    static std::string formatMemorySize(size_t bytes);
    static std::string getTestStatusIcon(bool passed);
};

// Test utilities
class TestUtilities {
public:
    static std::string generateTestID();
    static std::string generateRandomString(size_t length);
    static double generateRandomDouble(double min, double max);
    static size_t generateRandomSize(size_t min, size_t max);
    
    static bool fileExists(const std::string& filepath);
    static std::string getCurrentTimestamp();
    static void sleep(double seconds);
    
    static std::vector<std::string> splitString(const std::string& str, char delimiter);
    static std::string joinStrings(const std::vector<std::string>& strings, const std::string& delimiter);
    
    static double calculateMean(const std::vector<double>& values);
    static double calculateStandardDeviation(const std::vector<double>& values);
    static double calculateMedian(const std::vector<double>& values);
    
    static std::string getPlatformInfo();
    static std::string getSystemInfo();
    
private:
    static std::mt19937 rng_;
};

// Test configuration
struct TestConfiguration {
    std::string testSuiteName;
    std::vector<std::string> enabledCategories;
    bool performanceMode;
    bool memoryMode;
    double timeoutSeconds;
    size_t maxMemoryUsage;
    std::string outputDirectory;
    bool generateReports;
    bool runContinuousIntegration;
    
    TestConfiguration() : testSuiteName("NLM Test Suite"), performanceMode(true), 
                         memoryMode(true), timeoutSeconds(30.0), maxMemoryUsage(1024 * 1024),
                         generateReports(true), runContinuousIntegration(false) {}
};

} // namespace nlm