// IntegrationTestFramework.cpp
// Implementation of the Integration Test Framework

#include "IntegrationTestFramework.hpp"
#include <fstream>
#include <iomanip>
#include <sstream>

namespace nlm {

IntegrationTestResult::IntegrationTestResult() : metadata({{"timestamp", getTimestamp()}}) {}

void IntegrationTestResult::recordTest(const std::string& name, const std::string& category,
                                      bool passed, const std::string& details,
                                      double performanceScore) {
    TestCase test;
    test.name = name;
    test.category = category;
    test.passed = passed;
    test.details = details;
    test.performanceScore = performanceScore;
    
    testCases.push_back(test);
}

void IntegrationTestResult::recordPerformance(const PerformanceMetrics& perfMetrics) {
    metrics = perfMetrics;
}

void IntegrationTestResult::saveResults(const std::string& filepath) const {
    std::ofstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file for writing: " + filepath);
    }
    
    file << "NLM Integration Test Results" << std::endl;
    file << "===========================" << std::endl;
    file << "Timestamp: " << metadata.at("timestamp") << std::endl;
    file << std::endl;
    
    file << "Performance Summary:" << std::endl;
    file << "  Total Duration: " << metrics.totalDuration.count() << " microseconds" << std::endl;
    file << "  CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
    file << "  Memory Usage: " << metrics.memoryUsage << " MB" << std::endl;
    file << "  Operations per Second: " << metrics.operationsPerSecond << std::endl;
    file << std::endl;
    
    file << "Test Results by Category:" << std::endl;
    
    std::map<std::string, std::vector<TestCase>> testsByCategory;
    for (const auto& test : testCases) {
        testsByCategory[test.category].push_back(test);
    }
    
    for (const auto& pair : testsByCategory) {
        const std::string& category = pair.first;
        const std::vector<TestCase>& tests = pair.second;
        
        file << "  " << category << ": " << tests.size() << " tests" << std::endl;
        for (const auto& test : tests) {
            file << "    " << (test.passed ? "✓" : "✗") << " " << test.name;
            if (!test.details.empty() && !test.passed) {
                file << " (" << test.details << ")";
            }
            file << " - Performance: " << std::fixed << std::setprecision(2) << test.performanceScore << std::endl;
        }
    }
    
    file << std::endl;
    file << "Summary:" << std::endl;
    file << "  Total Tests: " << getTestCount() << std::endl;
    file << "  Passed: " << getPassedCount() << std::endl;
    file << "  Failed: " << getFailedCount() << std::endl;
    file << "  Success Rate: " << std::fixed << std::setprecision(2) 
          << (getTestCount() > 0 ? (static_cast<double>(getPassedCount()) / getTestCount() * 100.0) : 0.0) << "%" << std::endl;
    file << "  Average Performance Score: " << std::fixed << std::setprecision(3) 
          << getAveragePerformanceScore() << std::endl;
}

void IntegrationTestResult::printResults() const {
    std::cout << "NLM Integration Test Results" << std::endl;
    std::cout << "===========================" << std::endl;
    std::cout << "Timestamp: " << metadata.at("timestamp") << std::endl;
    std::cout << std::endl;
    
    std::cout << "Performance Summary:" << std::endl;
    std::cout << "  Total Duration: " << metrics.totalDuration.count() << " microseconds" << std::endl;
    std::cout << "  CPU Usage: " << metrics.cpuUsage << "%" << std::endl;
    std::cout << "  Memory Usage: " << metrics.memoryUsage << " MB" << std::endl;
    std::cout << "  Operations per Second: " << metrics.operationsPerSecond << std::endl;
    std::cout << std::endl;
    
    std::cout << "Test Cases:" << std::endl;
    for (const auto& test : testCases) {
        std::cout << "  " << (test.passed ? "✓" : "✗") << " " << test.name;
        std::cout << " [" << test.category << "]";
        std::cout << " - " << test.details;
        if (test.passed) {
            std::cout << " - Performance: " << std::fixed << std::setprecision(3) << test.performanceScore;
        }
        std::cout << std::endl;
    }
    
    std::cout << std::endl;
    std::cout << "Summary:" << std::endl;
    std::cout << "  Total Tests: " << getTestCount() << std::endl;
    std::cout << "  Passed: " << getPassedCount() << std::endl;
    std::cout << "  Failed: " << getFailedCount() << std::endl;
    std::cout << "  Success Rate: " << std::fixed << std::setprecision(2) 
              << (getTestCount() > 0 ? (static_cast<double>(getPassedCount()) / getTestCount() * 100.0) : 0.0) << "%" << std::endl;
    std::cout << "  Average Performance Score: " << std::fixed << std::setprecision(3) 
              << getAveragePerformanceScore() << std::endl;
}

size_t IntegrationTestResult::getTestCount() const { return testCases.size(); }
size_t IntegrationTestResult::getPassedCount() const {
    return std::count_if(testCases.begin(), testCases.end(),
                        [](const TestCase& t) { return t.passed; });
}
size_t IntegrationTestResult::getFailedCount() const {
    return std::count_if(testCases.begin(), testCases.end(),
                        [](const TestCase& t) { return !t.passed; });
}

double IntegrationTestResult::getAveragePerformanceScore() const {
    if (testCases.empty()) return 0.0;
    double sum = 0.0;
    for (const auto& test : testCases) {
        sum += test.performanceScore;
    }
    return sum / testCases.size();
}

std::string IntegrationTestResult::getTimestamp() {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    std::stringstream ss;
    ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
    return ss.str();
}

} // namespace nlm