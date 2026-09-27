// Enhanced Test Infrastructure for NLM Brain Simulation
// Modern C++17/20 unit testing framework with rich assertions, fixtures, and reporting

#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <chrono>
#include <map>
#include <memory>
#include <functional>
#include <thread>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <variant>
#include <optional>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <queue>
#include <set>
#include <numeric>
#include <cmath>

namespace nlm {
namespace test {

// Enhanced Test result structure with comprehensive metadata
struct TestResult {
    std::string name;
    std::string testClass;
    std::string testSuite;
    std::vector<std::string> tags;
    std::string description;
    
    enum class Status {
        PASSED,
        FAILED,
        SKIPPED,
        TIMEOUT,
        ERROR,
        ABORTED
    };
    
    Status status = Status::PASSED;
    std::string failureMessage;
    std::string failureDetails;
    double executionTime = 0.0;
    uint64_t memoryUsage = 0;
    int lineNumber = 0;
    std::string fileName;
    std::string errorType;
    
    TestResult(const std::string& name, const std::string& testClass = "",
               const std::string& testSuite = "", const std::string& description = "") 
        : name(name), testClass(testClass), testSuite(testSuite),
          description(description), status(Status::PASSED), executionTime(0.0), 
          memoryUsage(0), lineNumber(0) {}
    
    std::string getFullName() const {
        std::stringstream ss;
        if (!testSuite.empty()) {
            ss << testSuite << ".";
        }
        if (!testClass.empty()) {
            ss << testClass << ".";
        }
        ss << name;
        return ss.str();
    }
    
    std::string getStatusString() const {
        switch (status) {
            case Status::PASSED: return "PASSED";
            case Status::FAILED: return "FAILED";
            case Status::SKIPPED: return "SKIPPED";
            case Status::TIMEOUT: return "TIMEOUT";
            case Status::ERROR: return "ERROR";
            case Status::ABORTED: return "ABORTED";
            default: return "UNKNOWN";
        }
    }
    
    bool isSuccess() const {
        return status == Status::PASSED || status == Status::SKIPPED;
    }
};

// Enhanced Test fixture base class with resource management
class TestFixture {
protected:
    std::string fixtureName;
    bool setupCalled = false;
    bool teardownCalled = false;
    std::vector<std::function<void()>> resources;
    
public:
    TestFixture(const std::string& name) : fixtureName(name) {}
    virtual ~TestFixture() {
        // Cleanup resources
        for (auto& cleanup : resources) {
            try { cleanup(); } catch (...) {}
        }
        resources.clear();
    }
    
    const std::string& getFixtureName() const { return fixtureName; }
    
    virtual void setup() {
        setupCalled = true;
    }
    
    virtual void teardown() {
        teardownCalled = true;
    }
    
    void registerResourceCleanup(std::function<void()> cleanup) {
        resources.push_back(cleanup);
    }
    
    bool isSetupCalled() const { return setupCalled; }
    bool isTeardownCalled() const { return teardownCalled; }
};

// Enhanced assertion exception with full context
class AssertionException : public std::runtime_error {
public:
    AssertionException(const std::string& message, const std::string& file, int line,
                      const std::string& function = "", const std::string& errorType = "Assertion")
        : std::runtime_error(message), fileName(file), lineNumber(line), functionName(function), errorType(errorType) {}
    
    std::string getFileName() const { return fileName; }
    int getLineNumber() const { return lineNumber; }
    std::string getFunctionName() const { return functionName; }
    std::string getErrorType() const { return errorType; }
    
private:
    std::string fileName;
    int lineNumber;
    std::string functionName;
    std::string errorType;
};

// Rich assertion framework with detailed error reporting
class TestAssertion {
public:
    static void fail(const std::string& message, const std::string& file, int line,
                    const std::string& function = "", const std::string& errorType = "Assertion") {
        std::stringstream ss;
        ss << "[" << errorType << "] " << message;
        if (!file.empty()) ss << " (File: " << file;
        if (line > 0) ss << ", Line: " << line;
        ss << ")";
        throw AssertionException(ss.str(), file, line, function, errorType);
    }
    
    static void equal(const std::string& expected, const std::string& actual,
                     const std::string& message, const std::string& file, int line,
                     const std::string& function = "") {
        if (expected != actual) {
            std::stringstream ss;
            ss << message << "\n  Expected: " << expected << "\n  Actual: " << actual;
            fail(ss.str(), file, line, function);
        }
    }
    
    template<typename T>
    static void equal(T expected, T actual, const std::string& message,
                     const std::string& file, int line, const std::string& function = "") {
        if (expected != actual) {
            std::stringstream ss;
            ss << message << "\n  Expected: " << expected << "\n  Actual: " << actual;
            fail(ss.str(), file, line, function);
        }
    }
    
    template<typename T>
    static void floatEqual(T expected, T actual, T tolerance,
                          const std::string& message, const std::string& file, int line,
                          const std::string& function = "") {
        if (std::abs(expected - actual) > tolerance) {
            std::stringstream ss;
            ss << message << "\n  Expected: " << expected << " (within " << tolerance << ")\n  Actual: " << actual
               << "\n  Difference: " << std::abs(expected - actual);
            fail(ss.str(), file, line, function);
        }
    }
    
    static void true(bool condition, const std::string& message,
                    const std::string& file, int line, const std::string& function = "") {
        if (!condition) {
            fail(message, file, line, function);
        }
    }
    
    static void false(bool condition, const std::string& message,
                     const std::string& file, int line, const std::string& function = "") {
        if (condition) {
            fail(message, file, line, function);
        }
    }
    
    static void notNull(const void* ptr, const std::string& message,
                       const std::string& file, int line, const std::string& function = "") {
        if (!ptr) {
            fail(message, file, line, function);
        }
    }
    
    static void throws(std::function<void()> func, const std::string& message,
                       const std::string& file, int line, const std::string& function = "",
                       const std::string& expectedException = "std::exception") {
        bool threw = false;
        std::string exceptionMsg;
        std::string exceptionType;
        
        try {
            func();
        } catch (const std::exception& e) {
            threw = true;
            exceptionMsg = e.what();
            exceptionType = typeid(e).name();
        } catch (...) {
            threw = true;
            exceptionMsg = "Unknown exception";
            exceptionType = "unknown";
        }
        
        if (!threw) {
            fail(message + " (Expected exception " + expectedException + ")", file, line, function);
        } else if (expectedException != "any" && exceptionType != expectedException) {
            fail(message + " (Expected " + expectedException + ", got " + exceptionType + ")", file, line, function);
        }
    }
    
    static void notThrows(std::function<void()> func, const std::string& message,
                          const std::string& file, int line, const std::string& function = "") {
        try {
            func();
        } catch (...) {
            fail(message, file, line, function);
        }
    }
    
    static void fails(const std::string& message, const std::string& file, int line,
                     const std::string& function = "") {
        fail(message, file, line, function, "ExpectedFailure");
    }
    
    static void notFails(std::function<void()> func, const std::string& message,
                         const std::string& file, int line, const std::string& function = "") {
        try {
            func();
        } catch (...) {
            fail(message + " (Expected test to not throw, but it did)", file, line, function);
        }
    }
    
    static void timeout(std::function<void()> func, std::chrono::nanoseconds timeout,
                       const std::string& message, const std::string& file, int line,
                       const std::string& function = "") {
        std::future<void> future = std::async(std::launch::async, [&]() { func(); });
        if (future.wait_for(timeout) == std::future_status::timeout) {
            fail(message + " (Test timed out after " + 
                   std::to_string(std::chrono::duration_cast<std::chrono::milliseconds>(timeout).count()) + "ms)",
                 file, line, function, "Timeout");
        }
    }
};

// Test registration and discovery system
class TestRegistry {
private:
    struct TestInfo {
        std::string name;
        std::string testClass;
        std::string testSuite;
        std::vector<std::string> tags;
        std::string description;
        std::function<void()> testFunction;
        int order;
        bool enabled;
        std::chrono::nanoseconds timeout;
        
        TestInfo(const std::string& name, const std::string& testClass,
                 const std::string& testSuite, const std::string& description,
                 std::function<void()> testFunction, int order = 0,
                 bool enabled = true, const std::vector<std::string>& tags = {},
                 std::chrono::nanoseconds timeout = std::chrono::nanoseconds(30000000000)) // 30 seconds default
            : name(name), testClass(testClass), testSuite(testSuite),
              description(description), testFunction(testFunction), order(order),
              enabled(enabled), tags(tags), timeout(timeout) {}
    };
    
    std::vector<TestInfo> tests;
    std::map<std::string, std::vector<TestInfo*>> testsByTag;
    std::mutex mutex;
    
public:
    static TestRegistry& getInstance() {
        static TestRegistry instance;
        return instance;
    }
    
    void registerTest(const std::string& name, const std::string& testClass,
                     const std::string& testSuite, const std::string& description,
                     std::function<void()> testFunction, int order = 0,
                     bool enabled = true, const std::vector<std::string>& tags = {},
                     std::chrono::nanoseconds timeout = std::chrono::nanoseconds(30000000000)) {
        std::lock_guard<std::mutex> lock(mutex);
        
        auto testInfo = TestInfo(name, testClass, testSuite,
                                 description, testFunction, order, enabled, tags, timeout);
        
        tests.push_back(testInfo);
        
        for (const auto& tag : tags) {
            testsByTag[tag].push_back(&tests.back());
        }
    }
    
    const std::vector<TestInfo>& getAllTests() const { return tests; }
    
    std::vector<TestInfo*> getTestsByTag(const std::string& tag) {
        std::lock_guard<std::mutex> lock(mutex);
        return testsByTag[tag];
    }
    
    std::vector<TestInfo*> getEnabledTests() {
        std::lock_guard<std::mutex> lock(mutex);
        std::vector<TestInfo*> enabledTests;
        for (auto& test : tests) {
            if (test.enabled) {
                enabledTests.push_back(&test);
            }
        }
        return enabledTests;
    }
    
    TestInfo* findTest(const std::string& testSuite, const std::string& testClass,
                      const std::string& testName) {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (auto& test : tests) {
            if (test.testSuite == testSuite && test.testClass == testClass && test.name == testName) {
                return &test;
            }
        }
        return nullptr;
    }
    
    void enableTest(const std::string& testSuite, const std::string& testClass,
                   const std::string& testName, bool enabled = true) {
        std::lock_guard<std::mutex> lock(mutex);
        
        for (auto& test : tests) {
            if (test.testSuite == testSuite && test.testClass == testClass && test.name == testName) {
                test.enabled = enabled;
                return;
            }
        }
    }
    
    void disableTest(const std::string& testSuite, const std::string& testClass,
                    const std::string& testName) {
        enableTest(testSuite, testClass, testName, false);
    }
    
    void clear() {
        std::lock_guard<std::mutex> lock(mutex);
        tests.clear();
        testsByTag.clear();
    }
    
    size_t count() const { return tests.size(); }
};

// Enhanced Benchmark infrastructure with detailed statistics
class Benchmark {
public:
    struct BenchmarkResult {
        std::string name;
        std::string description;
        double duration; // nanoseconds
        uint64_t memoryUsage;
        size_t iterationCount;
        double averageDuration;
        double minDuration;
        double maxDuration;
        double stdDeviation;
        double p95Duration;
        double p99Duration;
        std::map<std::string, double> customMetrics;
        
        BenchmarkResult(const std::string& name, const std::string& description = "")
            : name(name), description(description), duration(0.0), memoryUsage(0),
              iterationCount(0), averageDuration(0.0), minDuration(0.0), maxDuration(0.0),
              stdDeviation(0.0), p95Duration(0.0), p99Duration(0.0) {}
    };
    
    static BenchmarkResult run(const std::string& name,
                               const std::string& description,
                               std::function<void()> benchmark,
                               size_t iterations = 1000,
                               size_t warmupIterations = 100) {
        // Warmup
        for (size_t i = 0; i < warmupIterations; ++i) {
            benchmark();
        }
        
        // Actual benchmark
        std::vector<double> durations;
        durations.reserve(iterations);
        
        for (size_t i = 0; i < iterations; ++i) {
            auto start = std::chrono::high_resolution_clock::now();
            benchmark();
            auto end = std::chrono::high_resolution_clock::now();
            
            auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
            durations.push_back(static_cast<double>(duration.count()));
        }
        
        // Calculate statistics
        double sum = std::accumulate(durations.begin(), durations.end(), 0.0);
        double mean = sum / iterations;
        
        double sq_sum = 0.0;
        double min_duration = durations[0];
        double max_duration = durations[0];
        
        for (double duration : durations) {
            sq_sum += duration * duration;
            min_duration = std::min(min_duration, duration);
            max_duration = std::max(max_duration, duration);
        }
        
        double variance = (sq_sum / iterations) - (mean * mean);
        double std_dev = std::sqrt(variance);
        
        // Calculate percentiles
        std::sort(durations.begin(), durations.end());
        double p95 = durations.size() > 0 ? durations[static_cast<size_t>(0.95 * durations.size())] : 0.0;
        double p99 = durations.size() > 0 ? durations[static_cast<size_t>(0.99 * durations.size())] : 0.0;
        
        BenchmarkResult result(name, description);
        result.duration = durations.back();
        result.memoryUsage = 0; // Simplified - would use memory_profiler in real implementation
        result.iterationCount = iterations;
        result.averageDuration = mean;
        result.minDuration = min_duration;
        result.maxDuration = max_duration;
        result.stdDeviation = std_dev;
        result.p95Duration = p95;
        result.p99Duration = p99;
        
        return result;
    }
    
    static std::string formatResult(const BenchmarkResult& result) {
        std::stringstream ss;
        ss << std::fixed << std::setprecision(3);
        ss << "=== Benchmark: " << result.name << " ===" << std::endl;
        if (!result.description.empty()) {
            ss << "Description: " << result.description << std::endl;
        }
        ss << "  Iterations: " << result.iterationCount << std::endl;
        ss << "  Average: " << result.averageDuration << " ns" << std::endl;
        ss << "  Min: " << result.minDuration << " ns" << std::endl;
        ss << "  Max: " << result.maxDuration << " ns" << std::endl;
        ss << "  Std Dev: " << result.stdDeviation << " ns" << std::endl;
        ss << "  P95: " << result.p95Duration << " ns" << std::endl;
        ss << "  P99: " << result.p99Duration << " ns" << std::endl;
        ss << "  Memory: " << result.memoryUsage << " bytes" << std::endl;
        for (const auto& metric : result.customMetrics) {
            ss << "  " << metric.first << ": " << metric.second << std::endl;
        }
        ss << "========================" << std::endl;
        return ss.str();
    }
};

// Test runner with parallel execution support
class TestRunner {
private:
    std::vector<TestResult> results;
    std::mutex resultsMutex;
    std::atomic<int> completedTests{0};
    std::atomic<int> totalTests{0};
    std::chrono::time_point<std::chrono::high_resolution_clock> startTime;
    std::chrono::time_point<std::chrono::high_resolution_clock> endTime;
    
    struct ParallelTest {
        std::string testSuite;
        std::string testClass;
        std::string testName;
        std::function<void()> testFunction;
        std::vector<std::string> tags;
        std::chrono::nanoseconds timeout;
        std::string description;
        
        ParallelTest(const std::string& ts, const std::string& tc, const std::string& tn,
                    std::function<void()> func, const std::vector<std::string>& tgs,
                    std::chrono::nanoseconds to, const std::string& desc)
            : testSuite(ts), testClass(tc), testName(tn),
              testFunction(func), tags(tgs), timeout(to), description(desc) {}
    };
    
    std::queue<ParallelTest> testQueue;
    std::mutex queueMutex;
    std::condition_variable queueCV;
    bool stopQueue = false;
    std::vector<std::thread> workerThreads;
    size_t numWorkers;
    
public:
    TestRunner(size_t workers = std::thread::hardware_concurrency())
        : numWorkers(workers > 0 ? workers : 1) {}
    
    ~TestRunner() {
        stopWorkers();
    }
    
    void stopWorkers() {
        {
            std::lock_guard<std::mutex> lock(queueMutex);
            stopQueue = true;
        }
        queueCV.notify_all();
        
        for (auto& worker : workerThreads) {
            if (worker.joinable()) {
                worker.join();
            }
        }
        workerThreads.clear();
    }
    
    void startWorkers() {
        for (size_t i = 0; i < numWorkers; ++i) {
            workerThreads.emplace_back(&TestRunner::workerThread, this);
        }
    }
    
    void addTest(const std::string& testSuite, const std::string& testClass,
                 const std::string& testName, std::function<void()> testFunction,
                 const std::vector<std::string>& tags = {}, 
                 std::chrono::nanoseconds timeout = std::chrono::nanoseconds(30000000000),
                 const std::string& description = "") {
        std::lock_guard<std::mutex> lock(queueMutex);
        testQueue.emplace(testSuite, testClass, testName, testFunction, tags, timeout, description);
    }
    
    void runTests(const std::vector<std::string>& tags = {}) {
        startWorkers();
        
        // Get tests from registry
        auto allTests = TestRegistry::getInstance().getEnabledTests();
        
        for (auto& testInfo : allTests) {
            // Filter by tags if specified
            if (!tags.empty()) {
                bool hasRequiredTag = false;
                for (const auto& tag : tags) {
                    if (std::find(testInfo->tags.begin(), testInfo->tags.end(), tag) != testInfo->tags.end()) {
                        hasRequiredTag = true;
                        break;
                    }
                }
                if (!hasRequiredTag) continue;
            }
            
            addTest(testInfo->testSuite, testInfo->testClass, testInfo->name,
                   testInfo->testFunction, testInfo->tags, testInfo->timeout, testInfo->description);
        }
        
        totalTests = testQueue.size();
        completedTests = 0;
        startTime = std::chrono::high_resolution_clock::now();
        
        // Wait for all tests to complete
        while (completedTests < totalTests && !testQueue.empty()) {
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        
        endTime = std::chrono::high_resolution_clock::now();
        
        // Generate report
        generateReport();
    }
    
    void workerThread() {
        while (true) {
            ParallelTest test;
            {
                std::unique_lock<std::mutex> lock(queueMutex);
                queueCV.wait(lock, [this]() { return !testQueue.empty() || stopQueue; });
                
                if (stopQueue && testQueue.empty()) {
                    return;
                }
                
                test = std::move(testQueue.front());
                testQueue.pop();
            }
            
            TestResult result(test.testName, test.testClass, test.testSuite, test.description);
            result.tags = test.tags;
            
            auto startTime = std::chrono::high_resolution_clock::now();
            
            try {
                // Apply timeout
                std::future<void> testFuture = std::async(std::launch::async, [&]() {
                    test.testFunction();
                });
                
                // Wait for result with timeout
                if (testFuture.wait_for(test.timeout) == std::future_status::ready) {
                    testFuture.get(); // Will rethrow any exception
                    result.status = TestResult::Status::PASSED;
                } else {
                    // Timeout
                    result.status = TestResult::Status::TIMEOUT;
                    result.failureMessage = "Test timed out after " + std::to_string(
                        std::chrono::duration_cast<std::chrono::milliseconds>(test.timeout).count()) + "ms";
                }
                
            } catch (const AssertionException& e) {
                result.status = TestResult::Status::FAILED;
                result.failureMessage = e.what();
                result.failureDetails = "Assertion failed at " + e.getFileName() + ":" + std::to_string(e.getLineNumber());
                result.lineNumber = e.getLineNumber();
                result.fileName = e.getFileName();
                result.errorType = e.getErrorType();
            } catch (const std::exception& e) {
                result.status = TestResult::Status::ERROR;
                result.failureMessage = e.what();
                result.errorType = typeid(e).name();
            } catch (...) {
                result.status = TestResult::Status::ERROR;
                result.failureMessage = "Unknown exception";
                result.errorType = "unknown";
            }
            
            auto endTime = std::chrono::high_resolution_clock::now();
            result.executionTime = std::chrono::duration<double, std::nano>(endTime - startTime).count();
            
            {
                std::lock_guard<std::mutex> lock(resultsMutex);
                results.push_back(result);
            }
            
            completedTests++;
        }
    }
    
    const std::vector<TestResult>& getResults() const { return results; }
    
    void generateReport() {
        std::cout << "=== Enhanced NLM Test Results ===" << std::endl;
        std::cout << "Started at: " << std::chrono::duration<double>(startTime.time_since_epoch()).count() << "s" << std::endl;
        std::cout << "Total tests: " << totalTests << std::endl;
        std::cout << "Completed tests: " << completedTests << std::endl;
        
        int passed = 0, failed = 0, skipped = 0, timeout = 0, error = 0;
        
        // Sort results by test suite and name
        std::vector<TestResult> sortedResults = results;
        std::sort(sortedResults.begin(), sortedResults.end(),
                 [](const TestResult& a, const TestResult& b) {
                     if (a.testSuite != b.testSuite) return a.testSuite < b.testSuite;
                     if (a.testClass != b.testClass) return a.testClass < b.testClass;
                     return a.name < b.name;
                 });
        
        // Print header
        std::cout << std::left << std::setw(60) << "Test Name"
                  << std::left << std::setw(15) << "Status"
                  << std::left << std::setw(12) << "Duration"
                  << std::left << std::setw(10) << "Memory"
                  << std::endl;
        std::cout << std::string(97, '-') << std::endl;
        
        for (const auto& result : sortedResults) {
            std::cout << std::left << std::setw(60) << result.getFullName()
                      << std::left << std::setw(15) << result.getStatusString()
                      << std::left << std::setw(12) << std::fixed << std::setprecision(3) << result.executionTime << "ns"
                      << std::left << std::setw(10) << result.memoryUsage << "bytes" << std::endl;
            
            if (!result.failureMessage.empty()) {
                std::cout << "  ERROR: " << result.failureMessage << std::endl;
                if (!result.failureDetails.empty()) {
                    std::cout << "  DETAILS: " << result.failureDetails << std::endl;
                }
            }
        }
        
        std::cout << std::string(97, '-') << std::endl;
        
        for (const auto& result : sortedResults) {
            switch (result.status) {
                case TestResult::Status::PASSED: passed++; break;
                case TestResult::Status::FAILED: failed++; break;
                case TestResult::Status::SKIPPED: skipped++; break;
                case TestResult::Status::TIMEOUT: timeout++; break;
                case TestResult::Status::ERROR: error++; break;
                case TestResult::Status::ABORTED: error++; break;
            }
        }
        
        std::cout << "\n=== Summary ===" << std::endl;
        std::cout << "  Passed: " << passed << std::endl;
        std::cout << "  Failed: " << failed << std::endl;
        std::cout << "  Skipped: " << skipped << std::endl;
        std::cout << "  Timeout: " << timeout << std::endl;
        std::cout << "  Error: " << error << std::endl;
        std::cout << "  Total Time: " << std::chrono::duration<double>(endTime - startTime).count() << "s" << std::endl;
        std::cout << "  Tests per second: " << (totalTests > 0 ? totalTests / std::chrono::duration<double>(endTime - startTime).count() : 0) << std::endl;
        
        // Generate XML report
        generateXMLReport();
        
        // Generate JSON report
        generateJSONReport();
    }
    
    void generateXMLReport() {
        std::ofstream xmlFile("test_results.xml");
        if (!xmlFile.is_open()) return;
        
        xmlFile << "<?xml version=\"1.0\"?>" << std::endl;
        xmlFile << "<test-results>" << std::endl;
        xmlFile << "  <summary total_tests=\"" << totalTests
                << "\" passed=\"" << (totalTests - (failed + timeout + error))
                << "\" failed=\"" << (failed + timeout + error)
                << "\">" << std::endl;
        
        for (const auto& result : results) {
            xmlFile << "  <test name=\"" << result.getFullName() << "\" status=\"" << result.getStatusString()
                    << "\" duration=\"" << result.executionTime << "\" memory=\"" << result.memoryUsage
                    << "\">" << std::endl;
            if (!result.failureMessage.empty()) {
                xmlFile << "    <failure>" << result.failureMessage << "</failure>" << std::endl;
            }
            xmlFile << "  </test>" << std::endl;
        }
        
        xmlFile << "</summary>" << std::endl;
        xmlFile << "</test-results>" << std::endl;
        xmlFile.close();
    }
    
    void generateJSONReport() {
        std::ofstream jsonFile("test_results.json");
        if (!jsonFile.is_open()) return;
        
        jsonFile << "{" << std::endl;
        jsonFile << "  \"summary\": {" << std::endl;
        jsonFile << "    \"total_tests\": " << totalTests << "," << std::endl;
        jsonFile << "    \"passed\": " << (totalTests - (failed + timeout + error)) << "," << std::endl;
        jsonFile << "    \"failed\": " << (failed + timeout + error) << "," << std::endl;
        jsonFile << "    \"duration\": " << std::chrono::duration<double>(endTime - startTime).count() << std::endl;
        jsonFile << "  }," << std::endl;
        jsonFile << "  \"tests\": [" << std::endl;
        
        for (size_t i = 0; i < results.size(); ++i) {
            const auto& result = results[i];
            jsonFile << "    {" << std::endl;
            jsonFile << "      \"name\": \"" << result.getFullName() << "\"," << std::endl;
            jsonFile << "      \"status\": \"" << result.getStatusString() << "\"," << std::endl;
            jsonFile << "      \"duration\": " << result.executionTime << "," << std::endl;
            jsonFile << "      \"memory\": " << result.memoryUsage << std::endl;
            if (!result.failureMessage.empty()) {
                jsonFile << ",\" << std::endl;
                jsonFile << "      \"failure\": \"" << result.failureMessage << "\"" << std::endl;
            }
            jsonFile << "    }";
            if (i < results.size() - 1) jsonFile << ",";
            jsonFile << std::endl;
        }
        
        jsonFile << "  ]" << std::endl;
        jsonFile << "}" << std::endl;
        jsonFile.close();
    }
};

// Enhanced Test base class with full fixture support
class TestBase : public TestFixture {
protected:
    std::string testSuite;
    std::string testClass;
    std::vector<std::string> tags;
    std::string description;
    
public:
    TestBase(const std::string& name, const std::string& testSuite = "",
             const std::string& testClass = "", const std::vector<std::string>& tags = {},
             const std::string& description = "")
        : TestFixture(name), testSuite(testSuite), testClass(testClass),
          tags(tags), description(description) {}
    
    virtual ~TestBase() = default;
    
    const std::string& getTestSuite() const { return testSuite; }
    const std::string& getTestClass() const { return testClass; }
    const std::vector<std::string>& getTags() const { return tags; }
    const std::string& getDescription() const { return description; }
    
    virtual void runTest() {
        setup();
        try {
            executeTest();
            teardown();
        } catch (...) {
            teardown();
            throw;
        }
    }
    
    virtual void executeTest() = 0;
    
    // Enhanced assertion macros (these need to be included where the test is written)
    // These macros are defined in the .hpp files where they're used
    
    // Helper methods for common test patterns
    template<typename T>
    void assertEqual(const T& expected, const T& actual, const std::string& message = "") {
        if (expected != actual) {
            throw AssertionException(message + " (expected: " + std::to_string(expected) +
                                   ", actual: " + std::to_string(actual) + ")",
                                   __FILE__, __LINE__, "");
        }
    }
    
    template<typename T>
    void assertFloatEqual(T expected, T actual, T tolerance, const std::string& message = "") {
        if (std::abs(expected - actual) > tolerance) {
            throw AssertionException(message + " (expected: " + std::to_string(expected) +
                                   ", actual: " + std::to_string(actual) + ")",
                                   __FILE__, __LINE__, "");
        }
    }
    
    void assertTrue(bool condition, const std::string& message = "") {
        if (!condition) {
            throw AssertionException(message, __FILE__, __LINE__, "");
        }
    }
    
    void assertFalse(bool condition, const std::string& message = "") {
        if (condition) {
            throw AssertionException(message, __FILE__, __LINE__, "");
        }
    }
    
    void assertNotNull(const void* ptr, const std::string& message = "") {
        if (!ptr) {
            throw AssertionException(message, __FILE__, __LINE__, "");
        }
    }
    
    void assertThrows(std::function<void()> func, const std::string& message = "",
                     const std::string& expectedException = "std::exception") {
        bool threw = false;
        std::string exceptionType;
        
        try {
            func();
        } catch (const std::exception& e) {
            threw = true;
            exceptionType = typeid(e).name();
        } catch (...) {
            threw = true;
            exceptionType = "unknown";
        }
        
        if (!threw) {
            throw AssertionException(message + " (Expected exception " + expectedException + ")",
                                   __FILE__, __LINE__, "");
        } else if (expectedException != "any" && exceptionType != expectedException) {
            throw AssertionException(message + " (Expected " + expectedException + ", got " + exceptionType + ")",
                                   __FILE__, __LINE__, "");
        }
    }
    
    void assertNotThrows(std::function<void()> func, const std::string& message = "") {
        try {
            func();
        } catch (...) {
            throw AssertionException(message + " (Expected test to not throw, but it did)",
                                   __FILE__, __LINE__, "");
        }
    }
    
    // Parameterized test support
    template<typename T>
    class ParameterSet {
    public:
        std::string name;
        T value;
        
        ParameterSet(const std::string& name, const T& value) : name(name), value(value) {}
    };
    
    template<typename T>
    void runTestWithParameter(const std::string& paramName, const T& paramValue) {
        std::string paramNameStr = paramName;
        std::string paramValueStr = std::to_string(paramValue);
        executeTestWithParameter(paramNameStr, paramValueStr);
    }
    
    virtual void executeTestWithParameter(const std::string& paramName, const std::string& paramValue) {
        // Default implementation - subclasses can override
        executeTest();
    }
    
    // Test utilities
    namespace utils {
        class TimeUtils {
        public:
            static double getCurrentTime() {
                return std::chrono::duration<double>(
                    std::chrono::high_resolution_clock::now().time_since_epoch()).count();
            }
            
            static void sleep(double seconds) {
                std::this_thread::sleep_for(std::chrono::duration<double>(seconds));
            }
            
            static std::string formatTime(double seconds) {
                std::stringstream ss;
                ss << std::fixed << std::setprecision(3) << seconds << "s";
                return ss.str();
            }
        };
        
        class StringUtils {
        public:
            static std::string toLower(const std::string& str) {
                std::string result = str;
                std::transform(result.begin(), result.end(), result.begin(), ::tolower);
                return result;
            }
            
            static std::string trim(const std::string& str) {
                size_t start = str.find_first_not_of(" \t\n\r");
                size_t end = str.find_last_not_of(" \t\n\r");
                return (start == std::string::npos) ? "" : str.substr(start, end - start + 1);
            }
            
            static std::vector<std::string> split(const std::string& str, char delimiter) {
                std::vector<std::string> result;
                std::stringstream ss(str);
                std::string item;
                while (std::getline(ss, item, delimiter)) {
                    result.push_back(item);
                }
                return result;
            }
        };
        
        class MathUtils {
        public:
            template<typename T>
            static T clamp(T value, T min, T max) {
                return std::max(min, std::min(max, value));
            }
            
            template<typename T>
            static T lerp(T a, T b, T t) {
                return a + (b - a) * t;
            }
            
            static double mean(const std::vector<double>& values) {
                return std::accumulate(values.begin(), values.end(), 0.0) / values.size();
            }
            
            static double standardDeviation(const std::vector<double>& values) {
                double mean = MathUtils::mean(values);
                double sum = 0.0;
                for (double value : values) {
                    sum += (value - mean) * (value - mean);
                }
                return std::sqrt(sum / values.size());
            }
        };
    }
};

// Test macros for easy test writing
// These macros should be included in test files where they're used
namespace macros {
    // Core test macros
    #define NLM_TEST_SUITE(suiteName) extern "C" void nlm_test_suite_##suiteName()
    #define NLM_TEST_CASE(testName) extern "C" void nlm_test_case_##testName()
    #define NLM_TEST_FIXTURE(fixtureName) extern "C" void nlm_test_fixture_##fixtureName()
    
    // Enhanced assertion macros for use in test files
    #define ASSERT_EQUAL(expected, actual) nlm::test::TestAssertion::equal(expected, actual, "Assertion failed at " __FILE__ ":" #line, __FILE__, __LINE__, __func__)
    #define ASSERT_FLOAT_EQUAL(expected, actual, tolerance) nlm::test::TestAssertion::floatEqual(expected, actual, tolerance, "Assertion failed at " __FILE__ ":" #line, __FILE__, __LINE__, __func__)
    #define ASSERT_TRUE(condition) nlm::test::TestAssertion::true(condition, "Assertion failed at " __FILE__ ":" #line, __FILE__, __LINE__, __func__)
    #define ASSERT_FALSE(condition) nlm::test::TestAssertion::false(condition, "Assertion failed at " __FILE__ ":" #line, __FILE__, __LINE__, __func__)
    #define ASSERT_NOT_NULL(ptr) nlm::test::TestAssertion::notNull(ptr, "Assertion failed at " __FILE__ ":" #line, __FILE__, __LINE__, __func__)
    #define ASSERT_THROWS(statement, exceptionType) nlm::test::TestAssertion::throws([=](){ statement; }, "Assertion failed", __FILE__, __LINE__, __func__, exceptionType)
    #define ASSERT_NO_THROWS(statement) nlm::test::TestAssertion::notThrows([=](){ statement; }, "Assertion failed", __FILE__, __LINE__, __func__)
    #define ASSERT_FAIL(message) nlm::test::TestAssertion::fail(message, __FILE__, __LINE__, __func__, "ExpectedFailure")
    #define ASSERT_TIMEOUT(statement, timeoutMs) nlm::test::TestAssertion::timeout(statement, std::chrono::milliseconds(timeoutMs), "Assertion failed", __FILE__, __LINE__, __func__)
    
    // Benchmark macro
    #define BENCHMARK(name, func, iterations) nlm::test::Benchmark::run(name, func, iterations)
    
    // Test registration macro
    #define REGISTER_TEST(testClass, testSuite, testName, description, ...) \\
class TestClass##testName : public testClass { \\
public: \\
    TestClass##testName() : testClass(testName, testSuite, #testClass, {"unit", "integration"}, description) {} \\
    void executeTest() override { testName(__VA_ARGS__); } \\
};
}

} // namespace test
} // namespace nlm

// Test registry helper function for easy test registration
inline void registerTest(const std::string& name, const std::string& testClass,
                        const std::string& testSuite, const std::string& description,
                        std::function<void()> testFunction, int order = 0,
                        bool enabled = true, const std::vector<std::string>& tags = {}) {
    nlm::test::TestRegistry::getInstance().registerTest(name, testClass, testSuite, description,
                                                        testFunction, order, enabled, tags);
}

// Macro for easy test class definition
#define NLM_TEST_CLASS(className, testSuite, description, ...) \\
class className : public nlm::test::TestBase { \\
public: \\
    className() : nlm::test::TestBase(#className, testSuite, #className, {"unit", "integration"}, description) {} \\
    void executeTest() override { testBody(__VA_ARGS__); } \\
private: \\
    void testBody(__VA_ARGS__) \\

// Test framework main function for easy integration
inline void runTestFramework() {
    nlm::test::TestRunner runner;
    runner.runTests();
}
