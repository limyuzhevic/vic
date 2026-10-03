// TestRunner.hpp
#pragma once

#include "TestFramework.hpp"
#include <thread>
#include <future>
#include <queue>
#include <atomic>

namespace nlm {
namespace test {

// Test configuration structure
struct TestConfiguration {
    std::string name;
    std::unordered_map<std::string, std::string> settings;
    bool enabled{true};
    std::string category;
    std::vector<std::string> tags;
    std::chrono::nanoseconds timeout{std::chrono::seconds(30)};
    std::string environment;
    std::string outputFormat;
    bool verbose{false};
    bool parallel{false};
    size_t parallelThreads{std::thread::hardware_concurrency()};

    TestConfiguration() = default;
    TestConfiguration(const std::string& name) : name(name) {}
};

// Advanced test runner with parallel execution support
class TestRunner {
public:
    TestRunner() : logger_("TestRunner", LogLevel::Info), stopSignal_(false) {}

    ~TestRunner() {
        stopSignal_ = true;
        for (auto& thread : workerThreads_) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }

    void setConfiguration(const TestConfiguration& config) {
        config_ = config;

        // Start worker threads if parallel execution is enabled
        if (config_.parallel && config_.parallelThreads > 1) {
            startWorkerThreads();
        }
    }

    std::vector<TestResult> runTests(const std::vector<std::unique_ptr<TestBase>>& tests) {
        if (config_.parallel && config_.parallelThreads > 1 && tests.size() > 1) {
            return runTestsParallel(tests);
        } else {
            return runTestsSequential(tests);
        }
    }

    std::vector<TestResult> runTestsSequential(const std::vector<std::unique_ptr<TestBase>>& tests) {
        std::vector<TestResult> results;
        results.reserve(tests.size());

        for (auto& test : tests) {
            results.push_back(runTest(std::move(test)));
        }

        return results;
    }

    std::vector<TestResult> runTestsParallel(const std::vector<std::unique_ptr<TestBase>>& tests) {
        std::vector<TestResult> results;
        results.reserve(tests.size());

        // Create tasks for each test
        std::vector<std::future<TestResult>> futures;
        futures.reserve(tests.size());

        for (auto& test : tests) {
            futures.push_back(std::async(std::launch::async, [this, test = std::move(test)]() {
                return runTest(std::move(test));
            }));
        }

        // Collect results as they complete
        for (auto& future : futures) {
            results.push_back(future.get());
        }

        return results;
    }

    std::vector<TestResult> runSuites(const std::vector<std::unique_ptr<TestSuite>>& suites) {
        std::vector<TestResult> allResults;

        for (auto& suite : suites) {
            auto filteredTests = suite->filterTests();
            auto suiteResults = runTests(filteredTests);

            // Add suite name to results
            for (auto& result : suiteResults) {
                result.suiteName = suite->getName();
            }

            allResults.insert(allResults.end(), suiteResults.begin(), suiteResults.end());
        }

        return allResults;
    }

    std::vector<TestResult> runAllTests() {
        auto& registry = TestRegistry::getInstance();
        return runSuites(registry.getSuites());
    }

    void generateReport(const std::vector<TestResult>& results, const std::string& outputPath = "") const {
        if (config_.outputFormat == "json" || outputPath.empty()) {
            generateJSONReport(results, outputPath);
        } else if (config_.outputFormat == "xml") {
            generateXMLReport(results, outputPath);
        } else if (config_.outputFormat == "csv") {
            generateCSVReport(results, outputPath);
        } else {
            generateConsoleReport(results);
        }
    }

    void printResults(const std::vector<TestResult>& results) const {
        if (config_.verbose) {
            printVerboseResults(results);
        } else {
            printSummaryResults(results);
        }
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

    void logPerformanceMetrics(const std::vector<TestResult>& results) const {
        auto totalTime = calculateTotalExecutionTime(results);
        double avgTimePerTest = results.empty() ? 0.0 : 
            static_cast<double>(totalTime.count()) / results.size() / 1e9;

        logger_.info("Performance Metrics:");
        logger_.info("  Total Execution Time: " + std::to_string(totalTime.count() / 1e9) + " seconds");
        logger_.info("  Average Time per Test: " + std::to_string(avgTimePerTest) + " seconds");
        logger_.info("  Tests per Second: " + std::to_string(results.size() / (totalTime.count() / 1e9)));

        // Log detailed metrics for each test
        for (const auto& result : results) {
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(
                result.endTime - result.startTime);

            logger_.info("Test: " + result.testName + ", Duration: " + 
                        std::to_string(duration.count() / 1e9) + "s, Status: " +
                        testStatusToString(result.status));
        }
    }

    void startWorkerThreads() {
        stopSignal_ = false;
        size_t threadCount = std::min(config_.parallelThreads, tests_.size());

        for (size_t i = 0; i < threadCount; ++i) {
            workerThreads_.emplace_back(&TestRunner::workerThread, this);
        }
    }

    void workerThread() {
        while (!stopSignal_) {
            std::unique_ptr<TestBase> test;
            {
                std::unique_lock<std::mutex> lock(queueMutex_);
                if (taskQueue_.empty()) {
                    std::this_thread::sleep_for(std::chrono::milliseconds(10));
                    continue;
                }

                test = std::move(taskQueue_.front());
                taskQueue_.pop();
            }

            if (test) {
                auto result = runTest(std::move(test));
                {
                    std::lock_guard<std::mutex> lock(resultsMutex_);
                    parallelResults_.push_back(std::move(result));
                }
            }
        }
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

                // Execute test with timeout
                result.status = executeWithTimeout(test.get());
                result.metrics = test->getMetrics();

                // Tear down test
                test->tearDown();

                if (result.status == TestStatus::PASSED) {
                    logger_.info("Test PASSED: " + test->getName());
                } else {
                    logger_.error("Test " + test->getName() + " " + testStatusToString(result.status));
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

    TestStatus executeWithTimeout(TestBase* test) {
        auto timeout = test->getTimeout();

        if (timeout.count() > 0) {
            auto future = std::async(std::launch::async, [test]() {
                return test->execute();
            });

            if (future.wait_for(timeout) == std::future_status::ready) {
                return future.get();
            } else {
                // Timeout occurred
                logger_.warning("Test " + test->getName() + " timed out after " + 
                              std::to_string(timeout.count() / 1e9) + " seconds");
                return TestStatus::TIMEOUT;
            }
        } else {
            return test->execute();
        }
    }

    void generateJSONReport(const std::vector<TestResult>& results, const std::string& outputPath) const {
        std::string json = "{\n";
        json += "  \"testCount\": " + std::to_string(results.size()) + ",\n";
        json += "  \"passed\": " + std::to_string(countResultsByStatus(results, TestStatus::PASSED)) + ",\n";
        json += "  \"failed\": " + std::to_string(countResultsByStatus(results, TestStatus::FAILED)) + ",\n";
        json += "  \"skipped\": " + std::to_string(countResultsByStatus(results, TestStatus::SKIPPED)) + ",\n";
        json += "  \"error\": " + std::to_string(countResultsByStatus(results, TestStatus::ERROR)) + ",\n";
        json += "  \"timeout\": " + std::to_string(countResultsByStatus(results, TestStatus::TIMEOUT)) + ",\n";
        json += "  \"passRate\": " + std::to_string(calculatePassRate(results)) + ",\n";
        json += "  \"totalExecutionTime\": " + std::to_string(calculateTotalExecutionTime(results).count()) + ",\n";
        json += "  \"tests\": [\n";

        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            json += "    {\n";
            json += "      \"name\": \"" + result.testName + "\",\n";
            json += "      \"suite\": \"" + result.suiteName + "\",\n";
            json += "      \"status\": \"" + testStatusToString(result.status) + "\",\n";
            json += "      \"startTime\": \"" + timeToString(result.startTime) + "\",\n";
            json += "      \"endTime\": \"" + timeToString(result.endTime) + "\",\n";
            json += "      \"duration\": " + std::to_string(
                std::chrono::duration_cast<std::chrono::nanoseconds>(result.endTime - result.startTime).count()) + "\n";

            if (!result.errorMessage.empty()) {
                json += "      \"error\": \"" + escapeString(result.errorMessage) + "\",\n";
            }

            json += "    }";
            if (i < results.size() - 1) {
                json += ",\n";
            }
        }

        json += "\n  ]\n";
        json += "}\n";

        if (!outputPath.empty()) {
            std::ofstream file(outputPath);
            if (file.is_open()) {
                file << json;
            }
        } else {
            std::cout << json << std::endl;
        }
    }

    void generateXMLReport(const std::vector<TestResult>& results, const std::string& outputPath) const {
        // Implementation for XML report generation
        // This is a simplified version - in production, you'd use a proper XML library
        std::string xml = "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        xml += "<test-results>\n";
        xml += "  <summary>\n";
        xml += "    <totalTests>" + std::to_string(results.size()) + "</totalTests>\n";
        xml += "    <passedTests>" + std::to_string(countResultsByStatus(results, TestStatus::PASSED)) + "</passedTests>\n";
        xml += "    <failedTests>" + std::to_string(countResultsByStatus(results, TestStatus::FAILED)) + "</failedTests>\n";
        xml += "    <passRate>" + std::to_string(calculatePassRate(results)) + "</passRate>\n";
        xml += "  </summary>\n";

        for (const auto& result : results) {
            xml += "  <test>\n";
            xml += "    <name>" + escapeString(result.testName) + "</name>\n";
            xml += "    <suite>" + escapeString(result.suiteName) + "</suite>\n";
            xml += "    <status>" + escapeString(testStatusToString(result.status)) + "</status>\n";
            xml += "    <startTime>" + timeToString(result.startTime) + "</startTime>\n";
            xml += "    <endTime>" + timeToString(result.endTime) + "</endTime>\n";
            if (!result.errorMessage.empty()) {
                xml += "    <error>" + escapeString(result.errorMessage) + "</error>\n";
            }
            xml += "  </test>\n";
        }

        xml += "</test-results>";

        if (!outputPath.empty()) {
            std::ofstream file(outputPath);
            if (file.is_open()) {
                file << xml;
            }
        } else {
            std::cout << xml << std::endl;
        }
    }

    void generateCSVReport(const std::vector<TestResult>& results, const std::string& outputPath) const {
        // Implementation for CSV report generation
        std::string csv = "TestName,Suite,Status,StartTime,EndTime,Duration,Error\n";

        for (const auto& result : results) {
            csv += escapeString(result.testName) + ",";
            csv += escapeString(result.suiteName) + ",";
            csv += escapeString(testStatusToString(result.status)) + ",";
            csv += timeToString(result.startTime) + ",";
            csv += timeToString(result.endTime) + ",";
            csv += std::to_string(
                std::chrono::duration_cast<std::chrono::nanoseconds>(result.endTime - result.startTime).count()) + ",";
            csv += escapeString(result.errorMessage) + "\n";
        }

        if (!outputPath.empty()) {
            std::ofstream file(outputPath);
            if (file.is_open()) {
                file << csv;
            }
        } else {
            std::cout << csv << std::endl;
        }
    }

    void generateConsoleReport(const std::vector<TestResult>& results) const {
        printResults(results);
        logPerformanceMetrics(results);
    }

    void printVerboseResults(const std::vector<TestResult>& results) const {
        printSummaryResults(results);

        for (const auto& result : results) {
            logger_.info("Test: " + result.testName + " (Suite: " + result.suiteName + ")");
            logger_.info("  Status: " + testStatusToString(result.status));
            logger_.info("  Duration: " + std::to_string(
                std::chrono::duration_cast<std::chrono::nanoseconds>(result.endTime - result.startTime).count() / 1e9) + " seconds");

            if (!result.errorMessage.empty()) {
                logger_.info("  Error: " + result.errorMessage);
            }
        }
    }

    void printSummaryResults(const std::vector<TestResult>& results) const {
        logger_.info("Test Results Summary:");
        logger_.info("=======================\n");

        size_t passed = 0, failed = 0, skipped = 0, error = 0, timeout = 0;

        for (const auto& result : results) {
            switch (result.status) {
                case TestStatus::PASSED: passed++; break;
                case TestStatus::FAILED: failed++; break;
                case TestStatus::SKIPPED: skipped++; break;
                case TestStatus::ERROR: error++; break;
                case TestStatus::TIMEOUT: timeout++; break;
            }
        }

        logger_.info("  Total Tests: " + std::to_string(results.size()));
        logger_.info("  Passed: " + std::to_string(passed));
        logger_.info("  Failed: " + std::to_string(failed));
        logger_.info("  Skipped: " + std::to_string(skipped));
        logger_.info("  Error: " + std::to_string(error));
        logger_.info("  Timeout: " + std::to_string(timeout));
        logger_.info("  Pass Rate: " + std::to_string(calculatePassRate(results)) + "%");
    }

    size_t countResultsByStatus(const std::vector<TestResult>& results, TestStatus status) const {
        size_t count = 0;
        for (const auto& result : results) {
            if (result.status == status) {
                count++;
            }
        }
        return count;
    }

    std::string testStatusToString(TestStatus status) const {
        switch (status) {
            case TestStatus::PASSED: return "PASSED";
            case TestStatus::FAILED: return "FAILED";
            case TestStatus::SKIPPED: return "SKIPPED";
            case TestStatus::ERROR: return "ERROR";
            case TestStatus::TIMEOUT: return "TIMEOUT";
            default: return "UNKNOWN";
        }
    }

    std::string timeToString(const std::chrono::system_clock::time_point& tp) const {
        auto time = std::chrono::system_clock::to_time_t(tp);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }

    std::string escapeString(const std::string& str) const {
        std::string escaped;
        for (char c : str) {
            switch (c) {
                case '\"': escaped += "\\\""; break;
                case '\\': escaped += "\\\\"; break;
                case '\n': escaped += "\\n"; break;
                case '\r': escaped += "\\r"; break;
                case '\t': escaped += "\\t"; break;
                default: escaped += c;
            }
        }
        return escaped;
    }

    std::vector<std::unique_ptr<TestBase>> tests_;
    std::vector<std::thread> workerThreads_;
    std::queue<std::unique_ptr<TestBase>> taskQueue_;
    std::vector<TestResult> parallelResults_;
    std::mutex queueMutex_;
    std::mutex resultsMutex_;
    std::atomic<bool> stopSignal_{false};
    TestConfiguration config_;
    Logger logger_;
};

} // namespace test
} // namespace nlm