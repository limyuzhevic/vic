// TestFramework.hpp
#pragma once

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <functional>
#include <stdexcept>
#include <any>
#include "core/Config/Config.hpp"
#include "core/Logger/Logger.hpp"

namespace nlm {
namespace test {

// Test execution result enumeration
enum class TestStatus {
    PASSED,
    FAILED,
    SKIPPED,
    ERROR,
    TIMEOUT
};

// Test severity levels
enum class TestSeverity {
    Critical,
    High,
    Medium,
    Low
};

// Test execution metrics
struct TestMetrics {
    std::chrono::nanoseconds executionTime{0};
    size_t memoryUsage{0};
    double cpuUsage{0.0};
    bool passed{false};
    std::string errorMessage;
    std::unordered_map<std::string, std::any> customMetrics;
};

// Test parameter structure for parameterized tests
struct TestParameter {
    std::string name;
    std::any value;
    std::string type;
    std::vector<std::any> possibleValues;
};

// Test dependency definition
struct TestDependency {
    std::string testName;
    bool mustPass{false};
    std::string description;
};

// Base test class
class TestBase {
public:
    virtual ~TestBase() = default;

    // Test metadata
    virtual std::string getName() const = 0;
    virtual std::string getDescription() const = 0;
    virtual TestSeverity getSeverity() const { return TestSeverity::Medium; }
    virtual std::string getCategory() const = 0;
    virtual std::vector<std::string> getTags() const { return {}; }

    // Test execution
    virtual TestStatus execute() = 0;
    virtual TestMetrics getMetrics() const { return TestMetrics(); }

    // Test configuration
    virtual void setUp() {}
    virtual void tearDown() {}
    virtual void setParameters(const std::vector<TestParameter>& params) {}
    virtual void addDependency(const TestDependency& dep) {}

    // Test control
    virtual bool shouldSkip() const { return false; }
    virtual std::chrono::nanoseconds getTimeout() const { return std::chrono::seconds(30); }

    // Test identification
    virtual size_t getId() const { return 0; }
    virtual void setId(size_t id) {}

    // Comparison operators for test ordering
    virtual bool operator<(const TestBase& other) const { return getName() < other.getName(); }
    virtual bool operator==(const TestBase& other) const { return getName() == other.getName(); }

protected:
    TestMetrics metrics_;
    size_t testId_ = 0;
    std::vector<TestParameter> parameters_;
    std::vector<TestDependency> dependencies_;

    void setMetrics(const TestMetrics& metrics) { metrics_ = metrics; }
};

// Test suite for grouping related tests
class TestSuite {
public:
    TestSuite(const std::string& name, const std::string& description = "")
        : name_(name), description_(description) {}

    std::string getName() const { return name_; }
    std::string getDescription() const { return description_; }

    void addTest(std::unique_ptr<TestBase> test) {
        tests_.push_back(std::move(test));
    }

    const std::vector<std::unique_ptr<TestBase>>& getTests() const { return tests_; }
    size_t getTestCount() const { return tests_.size(); }

    void setFilter(const std::function<bool(const TestBase&)>& filter) {
        filter_ = filter;
    }

    std::vector<std::unique_ptr<TestBase>> filterTests() const {
        std::vector<std::unique_ptr<TestBase>> filtered;
        for (auto& test : tests_) {
            if (!filter_ || filter_(*test)) {
                filtered.push_back(std::move(test));
            }
        }
        return filtered;
    }

    bool operator<(const TestSuite& other) const {
        return name_ < other.name_;
    }

private:
    std::string name_;
    std::string description_;
    std::vector<std::unique_ptr<TestBase>> tests_;
    std::function<bool(const TestBase&)> filter_;
};

// Test registry for managing test discovery and registration
class TestRegistry {
public:
    static TestRegistry& getInstance() {
        static TestRegistry instance;
        return instance;
    }

    void registerTest(std::unique_ptr<TestBase> test) {
        tests_.push_back(std::move(test));
    }

    void registerSuite(std::unique_ptr<TestSuite> suite) {
        suites_.push_back(std::move(suite));
    }

    const std::vector<std::unique_ptr<TestBase>>& getTests() const { return tests_; }
    const std::vector<std::unique_ptr<TestSuite>>& getSuites() const { return suites_; }

    std::unique_ptr<TestBase> getTest(const std::string& name) {
        for (auto& test : tests_) {
            if (test->getName() == name) {
                return std::move(test);
            }
        }
        return nullptr;
    }

    void clear() {
        tests_.clear();
        suites_.clear();
    }

    size_t getTestCount() const { return tests_.size(); }
    size_t getSuiteCount() const { return suites_.size(); }

private:
    TestRegistry() = default;
    std::vector<std::unique_ptr<TestBase>> tests_;
    std::vector<std::unique_ptr<TestSuite>> suites_;
};

// Test result structure for aggregation
struct TestResult {
    std::string testName;
    std::string suiteName;
    TestStatus status;
    TestMetrics metrics;
    std::string errorMessage;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point endTime;
    std::vector<std::string> tags;
    std::string category;
    size_t parameterIndex{0};
};

// Test runner for executing tests and collecting results
class TestRunner {
public:
    TestRunner() : logger_("TestRunner", LogLevel::Info) {}

    std::vector<TestResult> runTests(const std::vector<std::unique_ptr<TestBase>>& tests) {
        std::vector<TestResult> results;

        for (auto& test : tests) {
            results.push_back(runTest(std::move(test)));
        }

        return results;
    }

    std::vector<TestResult> runSuites(const std::vector<std::unique_ptr<TestSuite>>& suites) {
        std::vector<TestResult> allResults;

        for (auto& suite : suites) {
            auto filteredTests = suite->filterTests();
            auto suiteResults = runTests(filteredTests);
            allResults.insert(allResults.end(), suiteResults.begin(), suiteResults.end());
        }

        return allResults;
    }

    std::vector<TestResult> runAllTests() {
        auto& registry = TestRegistry::getInstance();
        return runSuites(registry.getSuites());
    }

    double calculatePassRate(const std::vector<TestResult>& results) const {
        if (results.empty()) return 0.0;

        size_t passedCount = 0;
        for (const auto& result : results) {
            if (result.status == TestStatus::PASSED) {
                passedCount++;
            }
        }

        return static_cast<double>(passedCount) / results.size() * 100.0;
    }

    std::chrono::nanoseconds calculateTotalExecutionTime(const std::vector<TestResult>& results) const {
        std::chrono::nanoseconds totalTime{0};
        for (const auto& result : results) {
            totalTime += std::chrono::duration_cast<std::chrono::nanoseconds>(
                result.endTime - result.startTime);
        }
        return totalTime;
    }

    void printResults(const std::vector<TestResult>& results) const {
        logger_.info("Test Results Summary:");
        logger_.info("=======================\n");

        size_t passed = 0, failed = 0, skipped = 0, error = 0, timeout = 0;

        for (const auto& result : results) {
            std::string statusStr;
            switch (result.status) {
                case TestStatus::PASSED: statusStr = "PASSED"; passed++; break;
                case TestStatus::FAILED: statusStr = "FAILED"; failed++; break;
                case TestStatus::SKIPPED: statusStr = "SKIPPED"; skipped++; break;
                case TestStatus::ERROR: statusStr = "ERROR"; error++; break;
                case TestStatus::TIMEOUT: statusStr = "TIMEOUT"; timeout++; break;
            }

            std::string line = "  " + result.testName + ": " + statusStr +
                              " (Suite: " + result.suiteName + ")";
            if (!result.errorMessage.empty()) {
                line += " - " + result.errorMessage;
            }
            logger_.info(line);
        }

        logger_.info("\nSummary Statistics:");
        logger_.info("  Total Tests: " + std::to_string(results.size()));
        logger_.info("  Passed: " + std::to_string(passed));
        logger_.info("  Failed: " + std::to_string(failed));
        logger_.info("  Skipped: " + std::to_string(skipped));
        logger_.info("  Error: " + std::to_string(error));
        logger_.info("  Timeout: " + std::to_string(timeout));
        logger_.info("  Pass Rate: " + std::to_string(calculatePassRate(results)) + "%");
    }

private:
    TestResult runTest(std::unique_ptr<TestBase> test) {
        TestResult result;
        result.testName = test->getName();
        result.suiteName = "";
        result.startTime = std::chrono::system_clock::now();

        try {
            // Check if test should be skipped
            if (test->shouldSkip()) {
                result.status = TestStatus::SKIPPED;
                logger_.info("Skipping test: " + test->getName());
            } else {
                // Set up test
                test->setUp();

                // Execute test
                result.status = test->execute();
                result.metrics = test->getMetrics();

                // Tear down test
                test->tearDown();

                if (result.status == TestStatus::PASSED) {
                    logger_.info("Test PASSED: " + test->getName());
                } else {
                    logger_.error("Test " + test->getName() + " " + testToString(result.status));
                }
            }
        } catch (const std::exception& e) {
            result.status = TestStatus::ERROR;
            result.errorMessage = e.what();
            logger_.error("Test ERROR " + test->getName() + ": " + std::string(e.what()));
        }

        result.endTime = std::chrono::system_clock::now();
        return result;
    }

    std::string testToString(TestStatus status) const {
        switch (status) {
            case TestStatus::PASSED: return "passed";
            case TestStatus::FAILED: return "failed";
            case TestStatus::SKIPPED: return "skipped";
            case TestStatus::ERROR: return "error";
            case TestStatus::TIMEOUT: return "timeout";
            default: return "unknown";
        }
    }

    Logger logger_;
};

} // namespace test
} // namespace nlm