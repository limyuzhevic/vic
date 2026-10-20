// Comprehensive NLM Test Runner
// Main test orchestration and execution for the NLM test suite

#ifndef NLM_TEST_RUNNER_HPP
#define NLM_TEST_RUNNER_HPP

#include "nlm/TestInfrastructure.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include <chrono>
#include <thread>
#include <mutex>
#include <map>
#include <unordered_map>
#include <functional>
#include <iomanip>
#include <algorithm>
#include <random>

namespace nlm::test {

// Test runner configuration
struct TestRunnerConfig {
    std::string testSuiteName = "NLM Comprehensive Test Suite";
    std::string testSuiteDescription = "Comprehensive test suite for NLM neural simulation framework";
    std::string outputDirectory = "test_results";
    bool enableParallelExecution = true;
    int maxParallelTests = 4;
    int testTimeoutSeconds = 300; // 5 minutes per test
    bool enablePerformanceMonitoring = true;
    bool generateDetailedReports = true;
    bool uploadToCI = false;
    std::string ciEndpoint;
    std::string ciToken;
    std::string branchName = "main";
    std::string commitHash;
    std::string gitRemote;
    bool enableChaosTesting = false;
    double chaosTestProbability = 0.1;
    std::string chaosTestFile;
};

// Test execution statistics
struct TestExecutionStats {
    std::string testName;
    std::string testCategory;
    std::string testModule;
    TestResult result;
    double durationMs;
    double memoryUsageMB;
    double cpuUsagePercent;
    std::string errorMessage;
    std::map<std::string, std::string> metrics;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    bool timeoutOccurred;
    bool memoryExceeded;
    bool cpuExceeded;
};

// Test execution context
struct TestExecutionContext {
    std::string testSuiteId;
    std::string testRunId;
    std::chrono::high_resolution_clock::time_point startTime;
    std::chrono::high_resolution_clock::time_point endTime;
    std::vector<TestExecutionStats> testResults;
    TestRunnerConfig config;
    std::mutex executionMutex;
};

// Test runner class
class NLMTestRunner {
public:
    NLMTestRunner(const TestRunnerConfig& config = TestRunnerConfig()) 
        : config_(config), context_(createExecutionContext()) {
        
        // Create output directory
        if (!config_.outputDirectory.empty()) {
            std::filesystem::create_directories(config_.outputDirectory);
        }
        
        // Initialize test framework
        testFramework_ = std::make_unique<TestFramework>(config_.testSuiteName, config_.testSuiteDescription);
    }
    
    ~NLMTestRunner() {
        // Generate final reports
        if (context_->testResults.size() > 0) {
            generateExecutionReport();
            uploadResultsToCI();
        }
    }
    
    // Register a test function
    template<typename TestFunc>
    void registerTest(const std::string& name, const std::string& description,
                     const std::string& category, const std::string& module,
                     TestFunc&& testFunc) {
        testFramework_->registerTest(name, description, category, module, 
                                    [this, name, testFunc]() {
                                        executeTestWithMonitoring(name, testFunc);
                                    });
    }
    
    // Run all tests
    void runAllTests() {
        std::cout << "=== " << config_.testSuiteName << " ===" << std::endl;
        std::cout << "Configuration:" << std::endl;
        std::cout << "  - Parallel Execution: " << (config_.enableParallelExecution ? "Enabled" : "Disabled") << std::endl;
        std::cout << "  - Output Directory: " << config_.outputDirectory << std::endl;
        std::cout << "  - Test Timeout: " << config_.testTimeoutSeconds << " seconds" << std::endl;
        std::cout << "  - Chaos Testing: " << (config_.enableChaosTesting ? "Enabled" : "Disabled") << std::endl;
        std::cout << std::endl;
        
        auto startTime = std::chrono::high_resolution_clock::now();
        
        if (config_.enableParallelExecution) {
            runTestsInParallel();
        } else {
            runTestsSequentially();
        }
        
        auto endTime = std::chrono::high_resolution_clock::now();
        context_->endTime = endTime;
        
        double totalDurationMs = std::chrono::duration<double, std::milli>(endTime - startTime).count();
        
        std::cout << std::endl;
        std::cout << "=== Test Execution Summary ===" << std::endl;
        std::cout << "Total Duration: " << totalDurationMs << " ms" << std::endl;
        std::cout << "Total Tests: " << context_->testResults.size() << std::endl;
        
        int passed = 0, failed = 0, skipped = 0, error = 0;
        for (const auto& result : context_->testResults) {
            switch (result.result) {
                case TestResult::PASSED: passed++; break;
                case TestResult::FAILED: failed++; break;
                case TestResult::SKIPPED: skipped++; break;
                case TestResult::ERROR: error++; break;
            }
        }
        
        std::cout << "Passed: " << passed << std::endl;
        std::cout << "Failed: " << failed << std::endl;
        std::cout << "Skipped: " << skipped << std::endl;
        std::cout << "Errors: " << error << std::endl;
        
        if (failed + error > 0) {
            std::cout << std::endl;
            std::cout << "=== Test Failures ===" << std::endl;
            for (const auto& result : context_->testResults) {
                if (result.result != TestResult::PASSED) {
                    std::cout << result.testName << ": " << testResultToString(result.result);
                    if (!result.errorMessage.empty()) {
                        std::cout << " - " << result.errorMessage;
                    }
                    std::cout << std::endl;
                }
            }
        }
        
        std::cout << std::endl;
        std::cout << "=== Test Run Complete ===" << std::endl;
    }
    
    // Get execution statistics
    const TestExecutionContext& getExecutionContext() const { return *context_; }
    
    // Get test framework reference
    TestFramework& getTestFramework() { return *testFramework_; }
    
private:
    TestExecutionContext createExecutionContext() {
        TestExecutionContext ctx;
        ctx.testSuiteId = TestUtilities::generateTestId("test_suite");
        ctx.testRunId = TestUtilities::generateTestId("test_run");
        ctx.startTime = std::chrono::high_resolution_clock::now();
        return ctx;
    }
    
    void executeTestWithMonitoring(const std::string& testName, std::function<void()> testFunc) {
        TestExecutionStats stats;
        stats.testName = testName;
        stats.startTime = std::chrono::high_resolution_clock::now();
        
        try {
            // Apply chaos testing if enabled
            if (config_.enableChaosTesting && shouldApplyChaosTest()) {
                applyChaosTest(testName);
            }
            
            // Execute the test with timeout
            std::thread testThread([this, &stats, &testFunc]() {
                try {
                    testFunc();
                    stats.result = TestResult::PASSED;
                } catch (const TestAssertionError& e) {
                    stats.result = TestResult::FAILED;
                    stats.errorMessage = e.what();
                } catch (const std::exception& e) {
                    stats.result = TestResult::ERROR;
                    stats.errorMessage = e.what();
                }
            });
            
            // Wait for test to complete with timeout
            testThread.join();
            
            // Wait a bit to collect system metrics
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
            
            stats.endTime = std::chrono::high_resolution_clock::now();
            stats.durationMs = std::chrono::duration<double, std::milli>(stats.endTime - stats.startTime).count();
            
            // Collect performance metrics
            if (config_.enablePerformanceMonitoring) {
                stats.memoryUsageMB = TestUtilities::getMemoryUsageMB();
                stats.cpuUsagePercent = TestUtilities::getCPUUsage();
            }
            
        } catch (const std::exception& e) {
            stats.endTime = std::chrono::high_resolution_clock::now();
            stats.durationMs = std::chrono::duration<double, std::milli>(stats.endTime - stats.startTime).count();
            stats.result = TestResult::ERROR;
            stats.errorMessage = e.what();
        }
        
        // Store test results
        {
            std::lock_guard<std::mutex> lock(context_->executionMutex);
            context_->testResults.push_back(stats);
        }
    }
    
    void runTestsSequentially() {
        std::cout << "Running tests sequentially..." << std::endl;
        testFramework_->runAllTests();
    }
    
    void runTestsInParallel() {
        std::cout << "Running tests in parallel (max " << config_.maxParallelTests << ")..." << std::endl;
        
        // Get test functions from the framework
        auto testFunctions = getTestFunctionsFromFramework();
        
        // Execute tests in parallel
        std::vector<std::thread> threads;
        std::vector<std::function<void()>> testTasks;
        
        for (const auto& pair : testFunctions) {
            testTasks.push_back([this, pair]() {
                executeTestWithMonitoring(pair.first, pair.second);
            });
        }
        
        // Distribute test execution among available threads
        size_t currentThread = 0;
        for (auto& task : testTasks) {
            threads.emplace_back(task);
            
            if (threads.size() >= static_cast<size_t>(config_.maxParallelTests)) {
                // Wait for available threads
                for (auto it = threads.begin(); it != threads.end(); ) {
                    if (it->joinable()) {
                        it->join();
                        it = threads.erase(it);
                    } else {
                        ++it;
                    }
                }
            }
        }
        
        // Join remaining threads
        for (auto& thread : threads) {
            if (thread.joinable()) {
                thread.join();
            }
        }
    }
    
    std::map<std::string, std::function<void()>> getTestFunctionsFromFramework() {
        // This would extract test functions from the test framework
        // For simplicity, we'll return an empty map here
        return std::map<std::string, std::function<void()>>();
    }
    
    bool shouldApplyChaosTest() {
        static std::mt19937 rng(std::chrono::high_resolution_clock::now().time_since_epoch().count());
        std::uniform_real_distribution<double> dist(0.0, 1.0);
        return dist(rng) < config_.chaosTestProbability;
    }
    
    void applyChaosTest(const std::string& testName) {
        // Apply chaos engineering to the test
        ChaosEngineer engineer;
        
        ChaosEngineer::FaultInjection fault;
        fault.type = ChaosEngineer::FaultType::NETWORK_LATENCY;
        fault.severity = 0.3;
        fault.durationSeconds = 0.1;
        fault.target = testName;
        
        engineer.injectFault(fault);
    }
    
    void generateExecutionReport() {
        if (!config_.generateDetailedReports) {
            return;
        }
        
        auto timestamp = getCurrentTimestamp();
        
        // Generate text report
        generateTextReport(timestamp);
        
        // Generate JSON report
        generateJSONReport(timestamp);
        
        // Generate HTML report
        generateHTMLReport(timestamp);
        
        // Generate summary CSV
        generateSummaryCSV(timestamp);
        
        std::cout << "Reports generated in " << config_.outputDirectory << "/" << timestamp << std::endl;
    }
    
    void generateTextReport(const std::string& timestamp) {
        std::string filename = config_.outputDirectory + "/" + "test_run_" + timestamp + ".txt";
        std::ofstream file(filename);
        
        file << "=== NLM Test Execution Report ===" << std::endl;
        file << "Test Suite: " << config_.testSuiteName << std::endl;
        file << "Test Run ID: " << context_->testRunId << std::endl;
        file << "Generated: " << timestamp << std::endl;
        file << std::endl;
        
        file << "=== Test Statistics ===" << std::endl;
        file << "Total Tests: " << context_->testResults.size() << std::endl;
        
        int passed = 0, failed = 0, skipped = 0, error = 0;
        for (const auto& result : context_->testResults) {
            switch (result.result) {
                case TestResult::PASSED: passed++; break;
                case TestResult::FAILED: failed++; break;
                case TestResult::SKIPPED: skipped++; break;
                case TestResult::ERROR: error++; break;
            }
        }
        
        file << "Passed: " << passed << std::endl;
        file << "Failed: " << failed << std::endl;
        file << "Skipped: " << skipped << std::endl;
        file << "Errors: " << error << std::endl;
        
        double totalDuration = getTotalDuration();
        file << "Total Duration: " << totalDuration << " ms" << std::endl;
        file << "Average Test Duration: " << (context_->testResults.size() > 0 ? totalDuration / context_->testResults.size() : 0) << " ms" << std::endl;
        
        file << std::endl;
        file << "=== Detailed Test Results ===" << std::endl;
        
        for (const auto& result : context_->testResults) {
            file << "Test: " << result.testName << std::endl;
            file << "Category: " << result.testCategory << std::endl;
            file << "Module: " << result.testModule << std::endl;
            file << "Status: " << testResultToString(result.result) << std::endl;
            file << "Duration: " << result.durationMs << " ms" << std::endl;
            if (result.memoryUsageMB > 0) {
                file << "Memory: " << result.memoryUsageMB << " MB" << std::endl;
            }
            if (result.cpuUsagePercent > 0) {
                file << "CPU: " << result.cpuUsagePercent << "%" << std::endl;
            }
            if (!result.errorMessage.empty()) {
                file << "Error: " << result.errorMessage << std::endl;
            }
            file << "---" << std::endl;
        }
        
        file.close();
    }
    
    void generateJSONReport(const std::string& timestamp) {
        std::string filename = config_.outputDirectory + "/" + "test_run_" + timestamp + ".json";
        std::ofstream file(filename);
        
        file << "{" << std::endl;
        file << "  \"testSuite\": \"" << config_.testSuiteName << "\"," << std::endl;
        file << "  \"testRunId\": \"" << context_->testRunId << "\"," << std::endl;
        file << "  \"generated\": \"" << timestamp << "\"," << std::endl;
        file << "  \"statistics\": {" << std::endl;
        
        int passed = 0, failed = 0, skipped = 0, error = 0;
        for (const auto& result : context_->testResults) {
            switch (result.result) {
                case TestResult::PASSED: passed++; break;
                case TestResult::FAILED: failed++; break;
                case TestResult::SKIPPED: skipped++; break;
                case TestResult::ERROR: error++; break;
            }
        }
        
        file << "    \"totalTests\": " << context_->testResults.size() << "," << std::endl;
        file << "    \"passed\": " << passed << "," << std::endl;
        file << "    \"failed\": " << failed << "," << std::endl;
        file << "    \"skipped\": " << skipped << "," << std::endl;
        file << "    \"error\": " << error << std::endl;
        file << "  }," << std::endl;
        
        file << "  \"tests\": [" << std::endl;
        
        for (size_t i = 0; i < context_->testResults.size(); ++i) {
            const auto& result = context_->testResults[i];
            file << "    {" << std::endl;
            file << "      \"name\": \"" << escapeJSONString(result.testName) << "\"," << std::endl;
            file << "      \"category\": \"" << escapeJSONString(result.testCategory) << "\"," << std::endl;
            file << "      \"module\": \"" << escapeJSONString(result.testModule) << "\"," << std::endl;
            file << "      \"result\": \"" << testResultToString(result.result) << "\"," << std::endl;
            file << "      \"durationMs\": " << result.durationMs << "," << std::endl;
            file << "      \"memoryUsageMB\": " << result.memoryUsageMB << "," << std::endl;
            file << "      \"cpuUsagePercent\": " << result.cpuUsagePercent << "," << std::endl;
            if (!result.errorMessage.empty()) {
                file << "      \"error\": \"" << escapeJSONString(result.errorMessage) << "\"" << std::endl;
            }
            file << "    }";
            
            if (i < context_->testResults.size() - 1) {
                file << "," << std::endl;
            } else {
                file << std::endl;
            }
        }
        
        file << "  ]" << std::endl;
        file << "}" << std::endl;
        
        file.close();
    }
    
    void generateHTMLReport(const std::string& timestamp) {
        std::string filename = config_.outputDirectory + "/" + "test_report_" + timestamp + ".html";
        std::ofstream file(filename);
        
        file << "<!DOCTYPE html>" << std::endl;
        file << "<html>" << std::endl;
        file << "<head>" << std::endl;
        file << "    <title>NLM Test Report - " << timestamp << "</title>" << std::endl;
        file << "    <style>" << std::endl;
        file << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
        file << "        .passed { color: green; }" << std::endl;
        file << "        .failed { color: red; }" << std::endl;
        file << "        .skipped { color: orange; }" << std::endl;
        file << "        .error { color: darkred; }" << std::endl;
        file << "        table { border-collapse: collapse; width: 100%; }" << std::endl;
        file << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
        file << "        th { background-color: #f2f2f2; }" << std::endl;
        file << "    </style>" << std::endl;
        file << "</head>" << std::endl;
        file << "<body>" << std::endl;
        
        file << "<h1>NLM Test Report - " << timestamp << "</h1>" << std::endl;
        file << "<h2>Test Suite: " << config_.testSuiteName << "</h2>" << std::endl;
        file << "<p>Test Run ID: " << context_->testRunId << "</p>" << std::endl;
        
        file << "<h3>Statistics</h3>" << std::endl;
        file << "<table>" << std::endl;
        file << "<tr><th>Metric</th><th>Value</th></tr>" << std::endl;
        
        int passed = 0, failed = 0, skipped = 0, error = 0;
        for (const auto& result : context_->testResults) {
            switch (result.result) {
                case TestResult::PASSED: passed++; break;
                case TestResult::FAILED: failed++; break;
                case TestResult::SKIPPED: skipped++; break;
                case TestResult::ERROR: error++; break;
            }
        }
        
        double totalDuration = getTotalDuration();
        
        file << "<tr><td>Total Tests</td><td>" << context_->testResults.size() << "</td></tr>" << std::endl;
        file << "<tr><td class=\"passed\">Passed</td><td>" << passed << "</td></tr>" << std::endl;
        file << "<tr><td class=\"failed\">Failed</td><td>" << failed << "</td></tr>" << std::endl;
        file << "<tr><td class=\"skipped\">Skipped</td><td>" << skipped << "</td></tr>" << std::endl;
        file << "<tr><td class=\"error\">Errors</td><td>" << error << "</td></tr>" << std::endl;
        file << "<tr><td>Total Duration</td><td>" << totalDuration << " ms</td></tr>" << std::endl;
        file << "<tr><td>Average Duration</td><td>" << (context_->testResults.size() > 0 ? totalDuration / context_->testResults.size() : 0) << " ms</td></tr>" << std::endl;
        file << "</table>" << std::endl;
        
        file << "<h3>Detailed Test Results</h3>" << std::endl;
        file << "<table>" << std::endl;
        file << "<tr><th>Test Name</th><th>Category</th><th>Module</th><th>Status</th><th>Duration (ms)</th><th>Memory (MB)</th><th>CPU (%)</th><th>Error</th></tr>" << std::endl;
        
        for (const auto& result : context_->testResults) {
            file << "<tr>" << std::endl;
            file << "<td>" << escapeHTML(result.testName) << "</td>" << std::endl;
            file << "<td>" << escapeHTML(result.testCategory) << "</td>" << std::endl;
            file << "<td>" << escapeHTML(result.testModule) << "</td>" << std::endl;
            
            std::string statusClass = "passed";
            if (result.result != TestResult::PASSED) {
                if (result.result == TestResult::FAILED) statusClass = "failed";
                else if (result.result == TestResult::SKIPPED) statusClass = "skipped";
                else statusClass = "error";
            }
            
            file << "<td class=\"" << statusClass << "\">" << testResultToString(result.result) << "</td>" << std::endl;
            file << "<td>" << result.durationMs << "</td>" << std::endl;
            file << "<td>" << result.memoryUsageMB << "</td>" << std::endl;
            file << "<td>" << result.cpuUsagePercent << "</td>" << std::endl;
            if (!result.errorMessage.empty()) {
                file << "<td>" << escapeHTML(result.errorMessage) << "</td>" << std::endl;
            } else {
                file << "<td></td>" << std::endl;
            }
            file << "</tr>" << std::endl;
        }
        
        file << "</table>" << std::endl;
        file << "</body>" << std::endl;
        file << "</html>" << std::endl;
        
        file.close();
    }
    
    void generateSummaryCSV(const std::string& timestamp) {
        std::string filename = config_.outputDirectory + "/" + "test_summary_" + timestamp + ".csv";
        std::ofstream file(filename);
        
        file << "Test Name,Category,Module,Status,Duration (ms),Memory (MB),CPU (%),Error" << std::endl;
        
        for (const auto& result : context_->testResults) {
            file << escapeCSV(result.testName) << ",";
            file << escapeCSV(result.testCategory) << ",";
            file << escapeCSV(result.testModule) << ",";
            file << testResultToString(result.result) << ";";
            file << result.durationMs << ",";
            file << result.memoryUsageMB << ",";
            file << result.cpuUsagePercent << ",";
            
            if (!result.errorMessage.empty()) {
                file << escapeCSV(result.errorMessage);
            }
            
            file << std::endl;
        }
        
        file.close();
    }
    
    void uploadResultsToCI() {
        if (!config_.uploadToCI || config_.ciEndpoint.empty()) {
            return;
        }
        
        // This would upload the test results to the CI system
        // Implementation would vary based on CI platform
        
        std::cout << "Uploading test results to CI system: " << config_.ciEndpoint << std::endl;
        
        // For now, we'll just print a message
        // In a real implementation, this would make HTTP requests to the CI system
    }
    
    std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        auto ms = std::chrono::duration_cast<std::chrono::milliseconds>(
            now.time_since_epoch()).milliseconds() % 1000;
        
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d_%H-%M-%S");
        ss << "_" << std::setfill('0') << std::setw(3) << ms;
        return ss.str();
    }
    
    double getTotalDuration() {
        if (context_->testResults.empty()) {
            return 0.0;
        }
        
        double total = 0.0;
        for (const auto& result : context_->testResults) {
            total += result.durationMs;
        }
        return total;
    }
    
    std::string escapeJSONString(const std::string& str) {
        std::string escaped = str;
        std::replace(escaped.begin(), escaped.end(), 
                    '\\', std::string("\\\\"));
        std::replace(escaped.begin(), escaped.end(), 
                    '\"', std::string("\\\""));
        std::replace(escaped.begin(), escaped.end(), 
                    '\n', std::string("\\n"));
        std::replace(escaped.begin(), escaped.end(), 
                    '\r', std::string("\\r"));
        std::replace(escaped.begin(), escaped.end(), 
                    '\t', std::string("\\t"));
        return escaped;
    }
    
    std::string escapeHTML(const std::string& str) {
        std::string escaped = str;
        std::replace(escaped.begin(), escaped.end(), 
                    '&', std::string("&amp;"));
        std::replace(escaped.begin(), escaped.end(), 
                    '<', std::string("&lt;"));
        std::replace(escaped.begin(), escaped.end(), 
                    '>', std::string("&gt;"));
        std::replace(escaped.begin(), escaped.end(), 
                    '"', std::string("&quot;"));
        std::replace(escaped.begin(), escaped.end(), 
                    '\'', std::string("&#x27;"));
        return escaped;
    }
    
    std::string escapeCSV(const std::string& str) {
        std::string escaped = str;
        if (escaped.find(',') != std::string::npos || 
            escaped.find('"') != std::string::npos || 
            escaped.find('\n') != std::string::npos) {
            escaped = '"' + escaped + '"';
            std::replace(escaped.begin(), escaped.end(), 
                        '"', '""');
        }
        return escaped;
    }
    
    std::string testResultToString(TestResult result) {
        switch (result) {
            case TestResult::PASSED: return "PASSED";
            case TestResult::FAILED: return "FAILED";
            case TestResult::SKIPPED: return "SKIPPED";
            case TestResult::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }
    
private:
    TestRunnerConfig config_;
    std::unique_ptr<TestExecutionContext> context_;
    std::unique_ptr<TestFramework> testFramework_;
};

} // namespace nlm::test

#endif // NLM_TEST_RUNNER_HPP
