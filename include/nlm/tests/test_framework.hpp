// TestResult structure for comprehensive results tracking
// Enhanced test framework for NLM
#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <optional>

namespace nlm {
namespace test {

// Test categories for classification
enum class TestCategory {
    Unit,
    Integration,
    Performance,
    Stress,
    Regression,
    Memory,
    Concurrency,
    Accuracy,
    Scaling
};

// Test status
enum class TestStatus {
    NotRun,
    Running,
    Passed,
    Failed,
    Skipped,
    Timeout,
    Aborted
};

// Test result structure
struct TestResult {
    std::string name;
    std::string description;
    TestCategory category;
    TestStatus status;
    std::string message;
    std::string file;
    int line;
    
    // Timing information
    std::chrono::nanoseconds executionTime;
    std::chrono::nanoseconds setupTime;
    std::chrono::nanoseconds teardownTime;
    
    // Memory tracking
    size_t memoryDelta;
    size_t peakMemoryUsage;
    
    // Statistical data
    std::vector<double> metrics;
    double averageMetric;
    double medianMetric;
    double stdDevMetric;
    
    // Threading info
    std::thread::id threadId;
    
    // Timestamp
    std::chrono::system_clock::time_point timestamp;
    
    TestResult(const std::string& testName, 
               const std::string& testDescription,
               TestCategory testCategory)
        : name(testName), description(testDescription), category(testCategory),
          status(TestStatus::NotRun), message(""), file(""), line(0),
          executionTime(0), setupTime(0), teardownTime(0),
          memoryDelta(0), peakMemoryUsage(0), averageMetric(0.0),
          medianMetric(0.0), stdDevMetric(0.0), timestamp(std::chrono::system_clock::now()) {}
};

// Test statistics for aggregation
struct TestStatistics {
    size_t totalTests{0};
    size_t passedTests{0};
    size_t failedTests{0};
    size_t skippedTests{0};
    size_t timeoutTests{0};
    std::chrono::nanoseconds totalExecutionTime{0};
    std::chrono::nanoseconds totalSetupTime{0};
    std::chrono::nanoseconds totalTeardownTime{0};
    size_t memoryLeaks{0};
    double memoryUsageGB{0.0};
    
    double passRate() const {
        return totalTests > 0 ? (static_cast<double>(passedTests) / totalTests) * 100.0 : 0.0;
    }
    
    std::string toString() const {
        std::stringstream ss;
        ss << "Total Tests: " << totalTests << "\n"
           << "Passed: " << passedTests << "\n"
           << "Failed: " << failedTests << "\n"
           << "Skipped: " << skippedTests << "\n"
           << "Timeout: " << timeoutTests << "\n"
           << "Pass Rate: " << std::fixed << std::setprecision(2) << passRate() << "%\n"
           << "Total Time: " << std::chrono::duration_cast<std::chrono::milliseconds>(totalExecutionTime).count() << "ms\n"
           << "Memory Usage: " << memoryUsageGB << "GB\n"
           << "Memory Leaks: " << memoryLeaks;
        return ss.str();
    }
};

// Fixture interface for setup/teardown
class TestFixture {
public:
    virtual ~TestFixture() = default;
    virtual void setup() {}
    virtual void teardown() {}
    virtual bool setupRequired() const { return false; }
    virtual bool teardownRequired() const { return false; }
};

// Test parameters for parameterization
struct TestParameters {
    std::string name;
    std::unordered_map<std::string, std::string> params;
    std::vector<double> parameterValues;
    
    TestParameters(const std::string& testName) : name(testName) {}
    
    template<typename T>
    void addParameter(const std::string& key, const T& value) {
        std::stringstream ss;
        ss << value;
        params[key] = ss.str();
    }
    
    template<typename T>
    void addParameterValue(const T& value) {
        std::stringstream ss;
        ss << value;
        parameterValues.push_back(std::stod(ss.str()));
    }
};

// Base test class
class TestBase {
protected:
    std::shared_ptr<TestFixture> fixture;
    std::shared_ptr<TestParameters> parameters;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    
public:
    virtual ~TestBase() = default;
    
    virtual void setFixture(std::shared_ptr<TestFixture> fix) {
        fixture = fix;
    }
    
    virtual void setParameters(std::shared_ptr<TestParameters> params) {
        parameters = params;
    }
    
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual TestCategory getCategory() const = 0;
    virtual std::string getFile() const = 0;
    virtual int getLine() const = 0;
    
    virtual void setup() {
        if (fixture && fixture->setupRequired()) {
            fixture->setup();
        }
    }
    
    virtual void teardown() {
        if (fixture && fixture->teardownRequired()) {
            fixture->teardown();
        }
    }
    
    virtual TestResult run() {
        startTime = std::chrono::system_clock::now();
        
        if (fixture && fixture->setupRequired()) {
            setup();
        }
        
        TestResult result(getName(), getDescription(), getCategory());
        result.status = TestStatus::Running;
        result.file = getFile();
        result.line = getLine();
        
        try {
            executeTest(result);
            endTime = std::chrono::system_clock::now();
            result.executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
            
            if (fixture && fixture->teardownRequired()) {
                teardown();
            }
            
            result.status = TestStatus::Passed;
            result.message = "Test passed";
        } catch (const std::exception& e) {
            endTime = std::chrono::system_clock::now();
            result.executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
            
            if (fixture && fixture->teardownRequired()) {
                teardown();
            }
            
            result.status = TestStatus::Failed;
            result.message = e.what();
        } catch (...) {
            endTime = std::chrono::system_clock::now();
            result.executionTime = std::chrono::duration_cast<std::chrono::nanoseconds>(endTime - startTime);
            
            if (fixture && fixture->teardownRequired()) {
                teardown();
            }
            
            result.status = TestStatus::Failed;
            result.message = "Unknown exception";
        }
        
        result.timestamp = endTime;
        result.threadId = std::this_thread::get_id();
        
        return result;
    }
    
protected:
    virtual void executeTest(TestResult& result) = 0;
};

// Test filter for selective test execution
class TestFilter {
public:
    virtual ~TestFilter() = default;
    
    virtual bool shouldInclude(const TestBase& test) const = 0;
    virtual bool shouldInclude(const TestResult& result) const = 0;
    
    // Helper filters
    static std::shared_ptr<TestFilter> categoryFilter(TestCategory category) {
        return std::make_shared<CategoryFilter>(category);
    }
    
    static std::shared_ptr<TestFilter> nameFilter(const std::string& pattern) {
        return std::make_shared<NameFilter>(pattern);
    }
    
    static std::shared_ptr<TestFilter> tagFilter(const std::string& tag) {
        return std::make_shared<TagFilter>(tag);
    }
    
private:
    class CategoryFilter : public TestFilter {
        TestCategory category_;
    public:
        CategoryFilter(TestCategory category) : category_(category) {}
        bool shouldInclude(const TestBase& test) const override {
            return test.getCategory() == category_;
        }
        bool shouldInclude(const TestResult& result) const override {
            return result.category == category_;
        }
    };
    
    class NameFilter : public TestFilter {
        std::string pattern_;
    public:
        NameFilter(const std::string& pattern) : pattern_(pattern) {}
        bool shouldInclude(const TestBase& test) const override {
            return test.getName().find(pattern_) != std::string::npos;
        }
        bool shouldInclude(const TestResult& result) const override {
            return result.name.find(pattern_) != std::string::npos;
        }
    };
    
    class TagFilter : public TestFilter {
        std::string tag_;
    public:
        TagFilter(const std::string& tag) : tag_(tag) {}
        bool shouldInclude(const TestBase& test) const override {
            // Tag matching would need to be implemented based on test tagging
            return true;
        }
        bool shouldInclude(const TestResult& result) const override {
            // Tag matching would need to be implemented based on test tagging
            return true;
        }
    };
};

// Test suite for organizing related tests
class TestSuite {
public:
    std::string name;
    std::vector<std::shared_ptr<TestBase>> tests;
    std::vector<std::shared_ptr<TestSuite>> subSuites;
    
    TestSuite(const std::string& suiteName) : name(suiteName) {}
    
    void addTest(std::shared_ptr<TestBase> test) {
        tests.push_back(test);
    }
    
    void addSuite(std::shared_ptr<TestSuite> suite) {
        subSuites.push_back(suite);
    }
    
    std::vector<std::shared_ptr<TestBase>> getAllTests() const {
        std::vector<std::shared_ptr<TestBase>> allTests;
        allTests.insert(allTests.end(), tests.begin(), tests.end());
        
        for (const auto& suite : subSuites) {
            auto nestedTests = suite->getAllTests();
            allTests.insert(allTests.end(), nestedTests.begin(), nestedTests.end());
        }
        
        return allTests;
    }
};

// Test runner for execution and reporting
class TestRunner {
public:
    TestStatistics runAll(std::shared_ptr<TestSuite> suite, 
                         const std::vector<std::shared_ptr<TestFilter>>& filters = {}) {
        TestStatistics stats;
        std::vector<std::shared_ptr<TestBase>> allTests = suite->getAllTests();n
        
        // Apply filters
        std::vector<std::shared_ptr<TestBase>> filteredTests;
        for (const auto& test : allTests) {
            bool include = true;
            for (const auto& filter : filters) {
                if (!filter->shouldInclude(*test)) {
                    include = false;
                    break;
                }
            }
            if (include) {
                filteredTests.push_back(test);
            }
        }
        
        // Run tests
        stats.totalTests = filteredTests.size();
        
        for (size_t i = 0; i < filteredTests.size(); ++i) {
            auto test = filteredTests[i];
            
            // Setup
            if (test->fixture && test->fixture->setupRequired()) {
                auto setupStart = std::chrono::system_clock::now();
                test->setup();
                auto setupEnd = std::chrono::system_clock::now();
                stats.totalSetupTime += std::chrono::duration_cast<std::chrono::nanoseconds>(setupEnd - setupStart);
            }
            
            // Run test
            auto result = test->run();
            
            // Update statistics
            switch (result.status) {
                case TestStatus::Passed: stats.passedTests++; break;
                case TestStatus::Failed: stats.failedTests++; break;
                case TestStatus::Skipped: stats.skippedTests++; break;
                case TestStatus::Timeout: stats.timeoutTests++; break;
                default: break;
            }
            
            stats.totalExecutionTime += result.executionTime;
            stats.totalTeardownTime += result.teardownTime;
            
            // Process result
            processResult(result);
        }
        
        return stats;
    }
    
    virtual void processResult(const TestResult& result) {
        // Log result
        std::cout << "Test: " << result.name << " - " << testStatusToString(result.status) 
                  << " (" << result.executionTime.count() << "ns)" << std::endl;
        
        if (result.status == TestStatus::Failed) {
            std::cout << "  FAILED: " << result.message << std::endl;
        }
    }
    
    std::string testStatusToString(TestStatus status) const {
        switch (status) {
            case TestStatus::Passed: return "PASSED";
            case TestStatus::Failed: return "FAILED";
            case TestStatus::Skipped: return "SKIPPED";
            case TestStatus::Timeout: return "TIMEOUT";
            case TestStatus::Running: return "RUNNING";
            case TestStatus::NotRun: return "NOT RUN";
            case TestStatus::Aborted: return "ABORTED";
            default: return "UNKNOWN";
        }
    }
};

}}} // namespace nlm::test
