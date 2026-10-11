#include "TestFramework.hpp"
#include <iostream>
#include <chrono>
#include <thread>
#include <sstream>
#include <iomanip>
#include <cmath>
#include <algorithm>
#include <numeric>

namespace nlm {

// TestCase implementation
TestFramework::TestCase::TestCase(const std::string& name, TestLevel level, TestCategory category)
    : name(name), level(level), category(category) {}

void TestFramework::TestCase::setup() {}

void TestFramework::TestCase::teardown() {}

// ParameterizedTestCase implementation
TestFramework::ParameterizedTestCase::ParameterizedTestCase(const std::string& name, TestLevel level, TestCategory category)
    : TestCase(name, level, category) {}

void TestFramework::ParameterizedTestCase::addParameter(const std::string& key, const std::string& value) {
    ParameterSet paramSet;
    paramSet.push_back(std::make_pair(key, value));
    parameterSets.push_back(paramSet);
}

void TestFramework::ParameterizedTestCase::run() {
    for (const auto& params : parameterSets) {
        runParameterizedTest(params);
    }
}

// PropertyBasedTestCase implementation
TestFramework::PropertyBasedTestCase::PropertyBasedTestCase(const std::string& name, TestLevel level, TestCategory category)
    : TestCase(name, level, category), maxTestsCount(100) {}

void TestFramework::PropertyBasedTestCase::setPropertyGenerator(std::function<std::string(int)> generator) {
    propertyGenerator = generator;
}

void TestFramework::PropertyBasedTestCase::setPropertyValidator(std::function<bool(const std::string&)> validator) {
    propertyValidator = validator;
}

void TestFramework::PropertyBasedTestCase::run() {
    if (!propertyGenerator || !propertyValidator) {
        throw std::runtime_error("Property test not properly configured: missing generator or validator");
    }
    
    int testsRun = 0;
    int successfulTests = 0;
    
    for (int i = 0; i < maxTestsCount && testsRun < maxTestsCount; ++i) {
        std::string input = propertyGenerator(i);
        testsRun++;
        
        if (propertyValidator(input)) {
            runPropertyTest(input);
            successfulTests++;
        }
    }
}

// FuzzTestCase implementation
TestFramework::FuzzTestCase::FuzzTestCase(const std::string& name, TestLevel level, TestCategory category)
    : TestCase(name, level, category), maxIterations(1000), timeout(5.0) {}

void TestFramework::FuzzTestCase::setInputGenerator(std::function<std::string(int)> generator) {
    inputGenerator = generator;
}

void TestFramework::FuzzTestCase::run() {
    if (!inputGenerator) {
        throw std::runtime_error("Fuzz test not properly configured: missing input generator");
    }
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    for (int i = 0; i < maxIterations; ++i) {
        auto currentTime = std::chrono::high_resolution_clock::now();
        auto elapsed = std::chrono::duration<double>(currentTime - startTime).count();
        
        if (elapsed > timeout) {
            break;
        }
        
        std::string input = inputGenerator(i);
        runFuzzTest(input);
    }
}

// ModelBasedTestCase implementation
std::string TestFramework::ModelBasedTestCase::State::toString() const {
    std::stringstream ss;
    ss << "State({";
    bool first = true;
    for (const auto& pair : values) {
        if (!first) ss << ", ";
        ss << "\"" << pair.first << "\": \"" << pair.second << "\"";
        first = false;
    }
    ss << "})";
    return ss.str();
}

TestFramework::ModelBasedTestCase::ModelBasedTestCase(const std::string& name, TestLevel level, TestCategory category)
    : TestCase(name, level, category) {}

void TestFramework::ModelBasedTestCase::setTransitionFunction(std::function<State(const State&, const std::string&)> func) {
    transitionFunc = func;
}

void TestFramework::ModelBasedTestCase::setPropertyFunction(std::function<bool(const State&)> func) {
    propertyFunc = func;
}

void TestFramework::ModelBasedTestCase::run() {
    if (!transitionFunc || !propertyFunc) {
        throw std::runtime_error("Model-based test not properly configured: missing transition or property function");
    }
    
    State currentState = initialState;
    
    // Test property on initial state
    if (propertyFunc(currentState)) {
        runStateTransitionTest(currentState, "INITIAL", currentState);
    }
    
    // Test state transitions
    std::vector<std::string> testInputs = {"input1", "input2", "input3", "reset", "continue"};
    for (const auto& input : testInputs) {
        State nextState = transitionFunc(currentState, input);
        
        // Test property on next state
        if (propertyFunc(nextState)) {
            runStateTransitionTest(currentState, input, nextState);
        }
        
        currentState = nextState;
    }
}

// TestFramework implementation
TestFramework::TestFramework() {}

void TestFramework::registerTest(std::unique_ptr<TestCase> testCase) {
    testCases.push_back(std::move(testCase));
}

void TestFramework::registerTestSuite(const std::string& suiteName, const std::vector<std::unique_ptr<TestCase>>& testCases) {
    for (auto& testCase : testCases) {
        testCase->name = suiteName + "::" + testCase->getName();
        registerTest(std::move(testCase));
    }
}

void TestFramework::setConfiguration(const TestConfiguration& config) {
    this->config = config;
}

void TestFramework::runAllTests() {
    std::cout << "=== NLM Enhanced Test Suite ===" << std::endl;
    
    for (auto& testCase : testCases) {
        runTestCase(*testCase);
    }
    
    std::cout << std::endl << "=== Test Summary ===" << std::endl;
    std::cout << formatTestSummary(testResults) << std::endl;
}

void TestFramework::runTestsByCategory(TestCategory category) {
    for (auto& testCase : testCases) {
        if (testCase->getCategory() == category) {
            runTestCase(*testCase);
        }
    }
}

void TestFramework::runTestsByLevel(TestLevel level) {
    for (auto& testCase : testCases) {
        if (testCase->getLevel() == level) {
            runTestCase(*testCase);
        }
    }
}

void TestFramework::runTestCase(TestCase& testCase) {
    std::lock_guard<std::mutex> lock(resultsMutex);
    
    TestParameters params(testCase.getName(), testCase.getLevel(), testCase.getCategory());
    TestResult result(testCase.getName(), params);
    
    auto startTime = std::chrono::high_resolution_clock::now();
    result.startTime = startTime;
    
    try {
        if (config.isCategoryEnabled(testCase.getCategory()) && testCase.getName().find("DISABLED") == std::string::npos) {
            std::cout << "Running test: " << testCase.getName() << " (Level: " 
                     << static_cast<int>(testCase.getLevel()) << ")" << std::endl;
            
            testCase.setup();
            
            auto setupStart = std::chrono::high_resolution_clock::now();
            testCase.run();
            auto setupEnd = std::chrono::high_resolution_clock::now();
            
            testCase.teardown();
            
            auto endTime = std::chrono::high_resolution_clock::now();
            
            result.status = TestStatus::PASSED;
            result.duration = std::chrono::duration<double>(endTime - startTime);
            result.endTime = endTime;
            
            std::cout << "  PASSED (Duration: " << result.duration.count() << "s)" << std::endl;
        } else {
            result.status = TestStatus::SKIPPED;
            std::cout << "Skipping test: " << testCase.getName() << std::endl;
        }
    } catch (const std::exception& e) {
        auto endTime = std::chrono::high_resolution_clock::now();
        result.status = TestStatus::FAILED;
        result.failureMessage = e.what();
        result.duration = std::chrono::duration<double>(endTime - startTime);
        result.endTime = endTime;
        
        std::cout << "  FAILED: " << e.what() << std::endl;
    } catch (...) {
        auto endTime = std::chrono::high_resolution_clock::now();
        result.status = TestStatus::ERROR;
        result.errorMessage = "Unknown exception occurred";
        result.duration = std::chrono::duration<double>(endTime - startTime);
        result.endTime = endTime;
        
        std::cout << "  ERROR: Unknown exception" << std::endl;
    }
    
    testResults.push_back(result);
}

void TestFramework::executeBenchmark(const std::string& name, std::function<void()> benchmarkFunc) {
    BenchmarkResult result;
    result.benchmarkName = name;
    
    auto startTime = std::chrono::high_resolution_clock::now();
    
    benchmarkFunc();
    
    auto endTime = std::chrono::high_resolution_clock::now();
    result.duration = std::chrono::duration<double>(endTime - startTime);
    result.executionTime = result.duration.count();
    
    benchmarkResults.push_back(result);
}

// Assertion implementation
void TestFramework::Assertion::require(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("REQUIRE FAILED: " + message);
    }
}

void TestFramework::Assertion::ensure(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("ENSURE FAILED: " + message);
    }
}

void TestFramework::Assertion::assertEqual(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected != actual) {
        throw std::runtime_error("ASSERT_EQUAL FAILED: " + message + ". Expected: '" + 
                               expected + "', Got: '" + actual + "'");
    }
}

void TestFramework::Assertion::assertEqual(double expected, double actual, double tolerance, const std::string& message) {
    if (std::abs(expected - actual) > tolerance) {
        throw std::runtime_error("ASSERT_EQUAL FAILED: " + message + ". Expected: " + 
                               std::to_string(expected) + ", Got: " + std::to_string(actual) +
                               ", Tolerance: " + std::to_string(tolerance));
    }
}

void TestFramework::Assertion::assertNotEqual(const std::string& expected, const std::string& actual, const std::string& message) {
    if (expected == actual) {
        throw std::runtime_error("ASSERT_NOT_EQUAL FAILED: " + message + ". Unexpectedly equal: '" + 
                               expected + "'");
    }
}

void TestFramework::Assertion::assertGreaterThan(double expected, double actual, const std::string& message) {
    if (expected <= actual) {
        throw std::runtime_error("ASSERT_GREATER_THAN FAILED: " + message + ". Expected: " + 
                               std::to_string(expected) + " > " + std::to_string(actual));
    }
}

void TestFramework::Assertion::assertLessThan(double expected, double actual, const std::string& message) {
    if (expected >= actual) {
        throw std::runtime_error("ASSERT_LESS_THAN FAILED: " + message + ". Expected: " + 
                               std::to_string(expected) + " < " + std::to_string(actual));
    }
}

void TestFramework::Assertion::assertInRange(double value, double min, double max, const std::string& message) {
    if (value < min || value > max) {
        throw std::runtime_error("ASSERT_IN_RANGE FAILED: " + message + ". Value: " + 
                               std::to_string(value) + " not in range [" + std::to_string(min) + ", " +
                               std::to_string(max) + "]");
    }
}

void TestFramework::Assertion::assertTrue(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("ASSERT_TRUE FAILED: " + message);
    }
}

void TestFramework::Assertion::assertFalse(bool condition, const std::string& message) {
    if (condition) {
        throw std::runtime_error("ASSERT_FALSE FAILED: " + message);
    }
}

void TestFramework::Assertion::assertNull(const void* ptr, const std::string& message) {
    if (ptr != nullptr) {
        throw std::runtime_error("ASSERT_NULL FAILED: " + message + ". Pointer is not null");
    }
}

void TestFramework::Assertion::assertNotNull(const void* ptr, const std::string& message) {
    if (ptr == nullptr) {
        throw std::runtime_error("ASSERT_NOT_NULL FAILED: " + message + ". Pointer is null");
    }
}

void TestFramework::Assertion::assertEmpty(const std::string& str, const std::string& message) {
    if (!str.empty()) {
        throw std::runtime_error("ASSERT_EMPTY FAILED: " + message + ". String is not empty: '" + 
                               str + "'");
    }
}

void TestFramework::Assertion::assertNotEmpty(const std::string& str, const std::string& message) {
    if (str.empty()) {
        throw std::runtime_error("ASSERT_NOT_EMPTY FAILED: " + message + ". String is empty");
    }
}

void TestFramework::Assertion::assertContains(const std::string& str, const std::string& substr, const std::string& message) {
    if (str.find(substr) == std::string::npos) {
        throw std::runtime_error("ASSERT_CONTAINS FAILED: " + message + ". Substring not found: '" + 
                               substr + "' in string: '" + str + "'");
    }
}

void TestFramework::Assertion::assertNotContains(const std::string& str, const std::string& substr, const std::string& message) {
    if (str.find(substr) != std::string::npos) {
        throw std::runtime_error("ASSERT_NOT_CONTAINS FAILED: " + message + ". Unexpected substring found: '" + 
                               substr + "' in string: '" + str + "'");
    }
}

void TestFramework::Assertion::assertThrows(std::function<void()> func, const std::string& message) {
    try {
        func();
        throw std::runtime_error("ASSERT_THROWS FAILED: " + message + ". Exception was not thrown");
    } catch (...) {
        // Expected
    }
}

void TestFramework::Assertion::assertNoThrow(std::function<void()> func, const std::string& message) {
    try {
        func();
    } catch (...) {
        throw std::runtime_error("ASSERT_NO_THROW FAILED: " + message + ". Unexpected exception thrown");
    }
}

void TestFramework::Assertion::assertPerformance(const std::string& benchmarkName, double targetTime, double tolerance, const std::string& message) {
    auto it = std::find_if(benchmarkResults.begin(), benchmarkResults.end(),
                          [&benchmarkName](const BenchmarkResult& br) { return br.benchmarkName == benchmarkName; });
    
    if (it == benchmarkResults.end()) {
        throw std::runtime_error("ASSERT_PERFORMANCE FAILED: " + message + ". Benchmark not found: " + benchmarkName);
    }
    
    double actualTime = it->executionTime;
    double threshold = targetTime * (1.0 + tolerance);
    
    if (actualTime > threshold) {
        throw std::runtime_error("ASSERT_PERFORMANCE FAILED: " + message + ". Actual time: " + 
                               std::to_string(actualTime) + "s > threshold: " + std::to_string(threshold) + "s");
    }
}

void TestFramework::Assertion::assertMemoryUsage(const std::string& benchmarkName, double targetMemoryMB, double tolerance, const std::string& message) {
    auto it = std::find_if(benchmarkResults.begin(), benchmarkResults.end(),
                          [&benchmarkName](const BenchmarkResult& br) { return br.benchmarkName == benchmarkName; });
    
    if (it == benchmarkResults.end()) {
        throw std::runtime_error("ASSERT_MEMORY_USAGE FAILED: " + message + ". Benchmark not found: " + benchmarkName);
    }
    
    double actualMemory = it->memoryUsage;
    double threshold = targetMemoryMB * (1.0 + tolerance);
    
    if (actualMemory > threshold) {
        throw std::runtime_error("ASSERT_MEMORY_USAGE FAILED: " + message + ". Actual memory: " + 
                               std::to_string(actualMemory) + "MB > threshold: " + std::to_string(threshold) + "MB");
    }
}

bool TestFramework::Assertion::propertyMatch(const std::string& input, std::function<bool(const std::string&)> validator) {
    return validator(input);
}

double TestFramework::Assertion::calculateProperty(const std::string& input, std::function<double(const std::string&)> calculator) {
    return calculator(input);
}

std::string TestFramework::formatTestResult(const TestResult& result) {
    std::stringstream ss;
    ss << "  Test: " << result.testName << std::endl;
    ss << "    Status: " << static_cast<int>(result.status) << std::endl;
    ss << "    Duration: " << result.duration.count() << "s" << std::endl;
    if (!result.failureMessage.empty()) {
        ss << "    Failure: " << result.failureMessage << std::endl;
    }
    if (!result.errorMessage.empty()) {
        ss << "    Error: " << result.errorMessage << std::endl;
    }
    ss << "    Metrics:" << std::endl;
    for (const auto& metric : result.metrics) {
        ss << "      " << metric.first << ": " << metric.second << std::endl;
    }
    return ss.str();
}

std::string TestFramework::formatTestSummary(const std::vector<TestResult>& results) {
    std::stringstream ss;
    ss << "Total Tests: " << results.size() << std::endl;
    
    int passed = 0, failed = 0, skipped = 0, error = 0, timeout = 0;
    
    for (const auto& result : results) {
        switch (result.status) {
            case TestStatus::PASSED: passed++; break;
            case TestStatus::FAILED: failed++; break;
            case TestStatus::SKIPPED: skipped++; break;
            case TestStatus::ERROR: error++; break;
            case TestStatus::TIMEOUT: timeout++; break;
        }
    }
    
    ss << "  PASSED: " << passed << std::endl;
    ss << "  FAILED: " << failed << std::endl;
    ss << "  SKIPPED: " << skipped << std::endl;
    ss << "  ERROR: " << error << std::endl;
    ss << "  TIMEOUT: " << timeout << std::endl;
    
    return ss.str();
}

std::string TestFramework::formatBenchmarkResult(const BenchmarkResult& result) {
    std::stringstream ss;
    ss << "Benchmark: " << result.benchmarkName << std::endl;
    ss << "  Execution Time: " << result.executionTime << "s" << std::endl;
    ss << "  Memory Usage: " << result.memoryUsage << "MB" << std::endl;
    ss << "  CPU Usage: " << result.cpuUsage << "%" << std::endl;
    ss << "  Operations/sec: " << result.operationsPerSecond << std::endl;
    return ss.str();
}

} // namespace nlm
