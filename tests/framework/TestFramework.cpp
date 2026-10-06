// NLM Test Framework Implementation
#include "framework/TestFramework.hpp"
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <stdexcept>
#include <ctime>

namespace nlm::test {

// TestCase implementation
TestResult TestCase::run() {
    startTime = std::chrono::high_resolution_clock::now();
    try {
        testFunc();
        duration = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - startTime).count() / 1000.0;
        return TestResult::PASSED;
    } catch (const std::exception& e) {
        duration = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - startTime).count() / 1000.0;
        std::cerr << "Test case failed: " << info.name << " - " << e.what() << std::endl;
        return TestResult::FAILED;
    } catch (...) {
        duration = std::chrono::duration<double, std::milli>(std::chrono::high_resolution_clock::now() - startTime).count() / 1000.0;
        std::cerr << "Test case failed: " << info.name << " - Unknown error" << std::endl;
        return TestResult::ERROR;
    }
}

// TestSuite implementation
void TestSuite::addTest(std::unique_ptr<TestBase> test) {
    tests.push_back(std::move(test));
    stats.total++;
}

void TestSuite::run() {
    if (setup) setup();
    
    for (auto& test : tests) {
        test->setup();
        TestResult result = test->runTest();
        test->teardown();
        
        switch (result) {
            case TestResult::PASSED: stats.passed++; break;
            case TestResult::FAILED: stats.failed++; break;
            case TestResult::SKIPPED: stats.skipped++; break;
            case TestResult::ERROR: stats.errors++; break;
            case TestResult::DISABLED: break;
        }
    }
    
    if (teardown) teardown();
}

// TestRegistry implementation
TestRegistry& TestRegistry::getInstance() {
    static TestRegistry instance;
    return instance;
}

void TestRegistry::registerTest(std::unique_ptr<TestBase> test) {
    tests.push_back(std::move(test));
    stats.total++;
}

void TestRegistry::registerSuite(std::unique_ptr<TestSuite> suite) {
    suites.push_back(std::move(suite));
    stats.total++;
}

void TestRegistry::runAllTests() {
    for (auto& test : tests) {
        test->runTest();
    }
    
    for (auto& suite : suites) {
        suite->run();
    }
}

void TestRegistry::printReport(std::ostream& out) const {
    out << "=== NLM Test Framework Report ===" << std::endl;
    out << std::fixed << std::setprecision(3);
    
    out << "Total Tests: " << stats.total << std::endl;
    out << "Passed: " << stats.passed << std::endl;
    out << "Failed: " << stats.failed << std::endl;
    out << "Skipped: " << stats.skipped << std::endl;
    out << "Errors: " << stats.errors << std::endl;
    out << "Total Time: " << stats.totalTime << "s" << std::endl;
    
    out << "\nSeverity Breakdown:" << std::endl;
    for (const auto& pair : stats.severityCounts) {
        out << "  " << pair.first << ": " << pair.second << std::endl;
    }
}

// TestRunner implementation
TestRunner& TestRunner::getInstance() {
    static TestRunner instance;
    return instance;
}

void TestRunner::runTests() {
    // Implementation will be expanded
}

void TestRunner::runTestSuite(const TestSuite& suite) {
    // Implementation will be expanded
}

void TestRunner::runIndividualTest(const TestBase& test) {
    // Implementation will be expanded
}

// AssertFramework implementation
void AssertFramework::init() {
    // Initialization code
}

template<typename T>
void AssertFramework::assert(const std::string& message, T condition, 
                             const std::string& file, int line) {
    if (!condition) {
        std::cerr << formatMessage("Assertion failed: " + message, file, line) << std::endl;
        throw std::runtime_error("Assertion failed: " + message);
    }
}

template<typename T, typename U>
void AssertFramework::assertEq(const std::string& message, T actual, U expected,
                               const std::string& file, int line) {
    if (!(actual == expected)) {
        std::stringstream ss;
        ss << "Expected: " << expected << ", Actual: " << actual;
        std::cerr << formatMessage("Equality assertion failed: " + message + " - " + ss.str(), file, line) << std::endl;
        throw std::runtime_error("Equality assertion failed: " + message);
    }
}

template<typename T, typename U>
void AssertFramework::assertNE(const std::string& message, T actual, U expected,
                               const std::string& file, int line) {
    if (actual == expected) {
        std::stringstream ss;
        ss << "Expected not equal, but both are: " << actual;
        std::cerr << formatMessage("Inequality assertion failed: " + message + " - " + ss.str(), file, line) << std::endl;
        throw std::runtime_error("Inequality assertion failed: " + message);
    }
}

template<typename T, typename U>
void AssertFramework::assertLT(const std::string& message, T actual, U expected,
                               const std::string& file, int line) {
    if (!(actual < expected)) {
        std::stringstream ss;
        ss << "Expected < " << expected << ", Actual: " << actual;
        std::cerr << formatMessage("Less than assertion failed: " + message + " - " + ss.str(), file, line) << std::endl;
        throw std::runtime_error("Less than assertion failed: " + message);
    }
}

template<typename T, typename U>
void AssertFramework::assertGT(const std::string& message, T actual, U expected,
                               const std::string& file, int line) {
    if (!(actual > expected)) {
        std::stringstream ss;
        ss << "Expected > " << expected << ", Actual: " << actual;
        std::cerr << formatMessage("Greater than assertion failed: " + message + " - " + ss.str(), file, line) << std::endl;
        throw std::runtime_error("Greater than assertion failed: " + message);
    }
}

template<typename T, typename U>
void AssertFramework::assertLE(const std::string& message, T actual, U expected,
                               const std::string& file, int line) {
    if (!(actual <= expected)) {
        std::stringstream ss;
        ss << "Expected <= " << expected << ", Actual: " << actual;
        std::cerr << formatMessage("Less than or equal assertion failed: " + message + " - " + ss.str(), file, line) << std::endl;
        throw std::runtime_error("Less than or equal assertion failed: " + message);
    }
}

template<typename T, typename U>
void AssertFramework::assertGE(const std::string& message, T actual, U expected,
                               const std::string& file, int line) {
    if (!(actual >= expected)) {
        std::stringstream ss;
        ss << "Expected >= " << expected << ", Actual: " << actual;
        std::cerr << formatMessage("Greater than or equal assertion failed: " + message + " - " + ss.str(), file, line) << std::endl;
        throw std::runtime_error("Greater than or equal assertion failed: " + message);
    }
}

void AssertFramework::assertTrue(const std::string& message, bool condition,
                                 const std::string& file, int line) {
    if (!condition) {
        std::cerr << formatMessage("Boolean assertion failed: " + message + " (expected true)", file, line) << std::endl;
        throw std::runtime_error("Boolean assertion failed: " + message + " (expected true)");
    }
}

void AssertFramework::assertFalse(const std::string& message, bool condition,
                                  const std::string& file, int line) {
    if (condition) {
        std::cerr << formatMessage("Boolean assertion failed: " + message + " (expected false)", file, line) << std::endl;
        throw std::runtime_error("Boolean assertion failed: " + message + " (expected false)");
    }
}

template<typename T>
void AssertFramework::assertNull(const std::string& message, T* ptr,
                                 const std::string& file, int line) {
    if (ptr != nullptr) {
        std::stringstream ss;
        ss << "Pointer should be null, but is: " << ptr;
        std::cerr << formatMessage("Null pointer assertion failed: " + message + " - " + ss.str(), file, line) << std::endl;
        throw std::runtime_error("Null pointer assertion failed: " + message);
    }
}

template<typename T>
void AssertFramework::assertNotNull(const std::string& message, T* ptr,
                                    const std::string& file, int line) {
    if (ptr == nullptr) {
        std::cerr << formatMessage("Non-null pointer assertion failed: " + message + " (expected not null)", file, line) << std::endl;
        throw std::runtime_error("Non-null pointer assertion failed: " + message + " (expected not null)");
    }
}

void AssertFramework::assertThrow(const std::string& message,
                                  std::function<void()> testFunc,
                                  const std::string& file, int line) {
    try {
        testFunc();
        std::cerr << formatMessage("Exception assertion failed: " + message + " (expected exception but none thrown)", file, line) << std::endl;
        throw std::runtime_error("Exception assertion failed: " + message + " (expected exception but none thrown)");
    } catch (...) {
        // Exception thrown as expected
    }
}

void AssertFramework::assertNoThrow(const std::string& message,
                                    std::function<void()> testFunc,
                                    const std::string& file, int line) {
    try {
        testFunc();
        // No exception thrown as expected
    } catch (const std::exception& e) {
        std::stringstream ss;
        ss << "Unexpected exception: " << e.what();
        std::cerr << formatMessage("No-throw assertion failed: " + message + " - " + ss.str(), file, line) << std::endl;
        throw std::runtime_error("No-throw assertion failed: " + message);
    } catch (...) {
        std::cerr << formatMessage("No-throw assertion failed: " + message + " (unexpected exception type)", file, line) << std::endl;
        throw std::runtime_error("No-throw assertion failed: " + message + " (unexpected exception type)");
    }
}

std::string AssertFramework::formatMessage(const std::string& message,
                                           const std::string& file,
                                           int line) {
    std::stringstream ss;
    ss << "[NLM Test Framework] " << message;
    if (!file.empty()) {
        ss << " (" << file << ":" << line << ")";
    }
    ss << std::endl;
    return ss.str();
}

std::string AssertFramework::colorize(const std::string& text, const std::string& color) {
    // ANSI color codes
    static const std::unordered_map<std::string, std::string> colors = {
        {"red", "\033[31m"},
        {"green", "\033[32m"},
        {"yellow", "\033[33m"},
        {"blue", "\033[34m"},
        {"magenta", "\033[35m"},
        {"cyan", "\033[36m"},
        {"white", "\033[37m"},
        {"reset", "\033[0m"}
    };
    
    auto it = colors.find(color);
    if (it != colors.end()) {
        return it->second + text + colors.at("reset");
    }
    return text;
}

// Global assert framework instance
AssertFramework assertFramework;

// TestMemoryTracker implementation
TestMemoryTracker& TestMemoryTracker::getInstance() {
    static TestMemoryTracker instance;
    return instance;
}

void TestMemoryTracker::startTracking() {
    tracking = true;
    // Implementation would depend on platform
}

size_t TestMemoryTracker::getCurrentMemoryUsage() const {
    // Implementation would depend on platform
    return 0;
}

void TestMemoryTracker::stopTracking() {
    tracking = false;
    // Implementation would update peakMemoryUsage
}

// TestResourceMonitor implementation
TestResourceMonitor& TestResourceMonitor::getInstance() {
    static TestResourceMonitor instance;
    return instance;
}

TestResourceMonitor::ResourceUsage TestResourceMonitor::measure(std::function<void()> testFunc) {
    ResourceUsage usage;
    auto startTime = std::chrono::high_resolution_clock::now();
    
    // Measure CPU time
    clock_t cpuStart = clock();
    
    // Run the test
    testFunc();
    
    // Measure CPU time
    clock_t cpuEnd = clock();
    usage.cpuTime = static_cast<double>(cpuEnd - cpuStart) / CLOCKS_PER_SEC;
    
    // Measure wall time
    auto endTime = std::chrono::high_resolution_clock::now();
    usage.wallTime = std::chrono::duration<double, std::milli>(endTime - startTime).count() / 1000.0;
    
    return usage;
}

// Legacy compatibility function for existing tests
void runLegacyTest(void (*testFunc)()) {
    if (testFunc) {
        testFunc();
    }
}
}