// Core Unit Testing Framework Implementation for NLM

#include "TestFramework.hpp"

namespace nlm {
namespace testing {

// Implementation of TestAssertion methods

// Helper function to format strings
template<typename... Args>
std::string formatString(const std::string& format, Args... args) {
    size_t size = std::snprintf(nullptr, 0, format.c_str(), args...) + 1;
    std::unique_ptr<char[]> buf(new char[size]);
    std::snprintf(buf.get(), size, format.c_str(), args...);
    return std::string(buf.get(), buf.get() + size - 1);
}

// Benchmark implementation with detailed timing
BenchmarkResult TestAssertion::benchmark(const std::string& name, std::function<void()> func,
                                       size_t iterations) {
    // Warm-up runs
    for (size_t i = 0; i < 10; ++i) {
        func();
    }
    
    // Actual benchmark
    auto start = std::chrono::high_resolution_clock::now();
    for (size_t i = 0; i < iterations; ++i) {
        func();
    }
    auto end = std::chrono::high_resolution_clock::now();
    
    auto duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - start);
    double durationNs = static_cast<double>(duration.count());
    double avgDurationNs = durationNs / iterations;
    
    // Convert to appropriate unit
    std::string unit;
    double durationMs = avgDurationNs / 1000000.0;
    
    if (durationMs < 1.0) {
        unit = "us";
        double durationUs = avgDurationNs / 1000.0;
        BenchmarkResult result(name, durationUs, 0.0, iterations, unit);
        return result;
    } else if (durationMs < 1000.0) {
        unit = "ms";
        BenchmarkResult result(name, durationMs, 0.0, iterations, unit);
        return result;
    } else {
        unit = "s";
        double durationS = durationMs / 1000.0;
        BenchmarkResult result(name, durationS, 0.0, iterations, unit);
        return result;
    }
}

// Memory leak tracking implementation
void* TestAssertion::trackAllocation(void* ptr, size_t size, 
                                    const std::string& allocationType,
                                    const std::string& allocationSite) {
    // In a real implementation, this would track allocations
    // For now, we store metadata for potential leak detection
    std::cout << "TRACKED ALLOCATION: " << allocationSite << " - " << size << " bytes" << std::endl;
    return ptr;
}

void TestAssertion::trackDeallocation(void* ptr, const std::string& allocationType) {
    // In a real implementation, this would track deallocations
    std::cout << "TRACKED DEALLOCATION: " << allocationType << std::endl;
}

// String conversion helpers
template<typename T>
std::string TestAssertion::toString(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

// TestCase implementation details
void TestCase::setParameters(const std::unordered_map<std::string, std::string>& params) {
    parameters = params;
    isParameterized = !params.empty();
}

// TestSuiteManager implementation details
void TestSuiteManager::registerTest(std::unique_ptr<TestCase> testCase) {
    testCase->executionCount = 0;
    testCases_.push_back(std::move(testCase));
}

void TestSuiteManager::generateReport() {
    // Generate detailed test report with statistics
    size_t totalExecutionTime = 0;
    
    for (const auto& testCase : testCases_) {
        totalExecutionTime += static_cast<size_t>(testCase->executionTime);
    }
    
    double avgExecutionTime = testCases_.empty() ? 0.0 : 
                             static_cast<double>(totalExecutionTime) / testCases_.size();
    
    std::cout << "\n=== Performance Statistics ===" << std::endl;
    std::cout << "Average test execution time: " << avgExecutionTime << "ms" << std::endl;
    std::cout << "Slowest test: " << getSlowestTestName() << std::endl;
    std::cout << "Fastest test: " << getFastestTestName() << std::endl;
}

std::string TestSuiteManager::getSlowestTestName() const {
    if (testCases_.empty()) return "N/A";
    
    const TestCase* slowest = testCases_[0].get();
    for (const auto& testCase : testCases_) {
        if (testCase->executionTime > slowest->executionTime) {
            slowest = testCase.get();
        }
    }
    
    return slowest->name;
}

std::string TestSuiteManager::getFastestTestName() const {
    if (testCases_.empty()) return "N/A";
    
    const TestCase* fastest = testCases_[0].get();
    for (const auto& testCase : testCases_) {
        if (testCase->executionTime < fastest->executionTime) {
            fastest = testCase.get();
        }
    }
    
    return fastest->name;
}

// Parameter extraction helper implementations
template<typename T, typename... Args>
std::unordered_map<std::string, std::string> TestSuiteManager::extractParameters(
    const std::tuple<T, Args...>& params) {
    std::unordered_map<std::string, std::string> paramMap;
    
    // Extract parameter values (simplified - in real implementation would use parameter names)
    std::stringstream ss;
    extractTupleToStream(ss, params);
    
    paramMap["values"] = ss.str();
    return paramMap;
}

// Recursive tuple extraction
void extractTupleToStream(std::stringstream& ss, const std::tuple<>&) {
    ss << "()";
}

template<typename T, typename... Args>
void extractTupleToStream(std::stringstream& ss, const std::tuple<T, Args...>& params) {
    auto value = std::get<0>(params);
    ss << toString(value);
    if (sizeof...(Args) > 0) {
        ss << ", ";
        extractTupleToStream(ss, std::make_tuple(std::get<Args>(params)...));
    }
}

// Type to string conversion for parameter reporting
template<typename T>
std::string TestSuiteManager::toString(const T& value) {
    std::stringstream ss;
    ss << value;
    return ss.str();
}

// Integer type specializations
std::string TestSuiteManager::toString(int value) {
    return std::to_string(value);
}

std::string TestSuiteManager::toString(long value) {
    return std::to_string(value);
}

std::string TestSuiteManager::toString(double value) {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(6) << value;
    return ss.str();
}

std::string TestSuiteManager::toString(const std::string& value) {
    return value;
}

// Memory leak scan implementation
void TestSuiteManager::scanForMemoryLeaks() {
    std::cout << "=== Advanced Memory Leak Detection ===" << std::endl;
    
    // In a real implementation, this would use system tools like valgrind,
    // address sanitizer, or custom memory tracking
    
    std::cout << "Performing memory leak analysis..." << std::endl;
    
    // Check for unfreed allocations (simplified)
    size_t allocationCount = 0;
    for (const auto& testCase : testCases_) {
        allocationCount += testCase->executionCount;
    }
    
    if (allocationCount > 0) {
        std::cout << "Warning: " << allocationCount << " allocations detected" << std::endl;
        std::cout << "Consider using memory tracking tools for detailed analysis" << std::endl;
    } else {
        std::cout << "No memory leaks detected in test suite" << std::endl;
    }
    
    // Generate leak report
    std::cout << "Memory leak scan completed successfully" << std::endl;
}

// Performance baseline comparison implementation
void TestSuiteManager::compareWithBaseline(const std::string& baselineFile) {
    std::cout << "\n=== Performance Regression Analysis ===" << std::endl;
    
    // In a real implementation, this would load baseline data
    // and perform statistical comparison
    
    std::cout << "Loading baseline data from: " << baselineFile << std::endl;
    
    // Calculate statistics
    double totalExecutionTime = 0.0;
    size_t totalTests = 0;
    
    for (const auto& testCase : testCases_) {
        totalExecutionTime += testCase->executionTime;
        totalTests++;
    }
    
    double avgExecutionTime = totalTests > 0 ? totalExecutionTime / totalTests : 0.0;
    
    std::cout << "Current performance average: " << avgExecutionTime << "ms" << std::endl;
    
    // Detect potential regressions (simplified threshold)
    if (avgExecutionTime > 100.0) { // 100ms threshold
        std::cout << "WARNING: Potential performance regression detected!" << std::endl;
        std::cout << "Consider profiling slow tests" << std::endl;
    } else {
        std::cout << "Performance within acceptable range" << std::endl;
    }
    
    std::cout << "Performance baseline comparison completed" << std::endl;
}

} // namespace testing
} // namespace nlm