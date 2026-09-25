// Test Framework Implementation
// Core test framework implementation for NLM
#pragma once

#include "test_framework.hpp"
#include <algorithm>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <filesystem>

namespace nlm {
namespace test {

// TestResult serialization
std::string TestResult::toString() const {
    std::stringstream ss;
    ss << "Test: " << name << std::endl;
    ss << "  Status: " << static_cast<int>(status) << std::endl;
    ss << "  Category: " << static_cast<int>(category) << std::endl;
    ss << "  Execution Time: " << executionTime.count() << "ns" << std::endl;
    ss << "  Setup Time: " << setupTime.count() << "ns" << std::endl;
    ss << "  Teardown Time: " << teardownTime.count() << "ns" << std::endl;
    ss << "  Message: " << message << std::endl;
    ss << "  File: " << file << " (line " << line << ")" << std::endl;
    return ss.str();
}

// TestRunner XML report generation
void TestRunner::generateXMLReport(const std::vector<TestResult>& results, 
                                   const std::string& filename) {
    std::ofstream file(filename);
    if (!file) return;
    
    file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
    file << "<testsuite name=\"NLM Tests\">" << std::endl;
    file << "  <properties>" << std::endl;
    file << "    <property name=\"timestamp\" value=\"" 
          << std::chrono::system_clock::now() << "\"/>" << std::endl;
    file << "  </properties>" << std::endl;
    
    for (const auto& result : results) {
        file << "  <testcase name=\"" << result.name << "\" "
             << "file=\"" << result.file << "\" "
             << "line=\"" << result.line << "\">" << std::endl;
        
        if (result.status == TestStatus::Failed) {
            file << "    <failure message=\"" << result.message << "\">" << std::endl;
            file << "    " << result.message << "" << std::endl;
            file << "    </failure>" << std::endl;
        }
        
        file << "    <system-out>" << std::endl;
        file << "      Execution time: " << result.executionTime.count() << "ns" << std::endl;
        file << "      Category: " << static_cast<int>(result.category) << std::endl;
        file << "    </system-out>" << std::endl;
        file << "  </testcase>" << std::endl;
    }
    
    file << "</testsuite>" << std::endl;
}

// TestRunner JSON report generation
void TestRunner::generateJSONReport(const std::vector<TestResult>& results, 
                                    const std::string& filename) {
    std::ofstream file(filename);
    if (!file) return;
    
    file << "{" << std::endl;
    file << "  \"timestamp\": \"" << std::chrono::system_clock::now() << "\"," << std::endl;
    file << "  \"testResults\": [" << std::endl;
    
    for (size_t i = 0; i < results.size(); ++i) {
        const auto& result = results[i];
        file << "    {" << std::endl;
        file << "      \"name\": \"" << result.name << "\"," << std::endl;
        file << "      \"status\": " << static_cast<int>(result.status) << "," << std::endl;
        file << "      \"category\": " << static_cast<int>(result.category) << ",\" << std::endl;
        file << "      \"executionTime\": " << result.executionTime.count() << "," << std::endl;
        file << "      \"setupTime\": " << result.setupTime.count() << "," << std::endl;
        file << "      \"teardownTime\": " << result.teardownTime.count() << "," << std::endl;
        file << "      \"message\": \"" << result.message << "\"," << std::endl;
        file << "      \"file\": \"" << result.file << "\"," << std::endl;
        file << "      \"line\": " << result.line << std::endl;
        
        if (i < results.size() - 1) {
            file << "    },";
        } else {
            file << "    }";
        }
        
        file << std::endl;
    }
    
    file << "  ]" << std::endl;
    file << "}" << std::endl;
}

// TestRunner CSV report generation
void TestRunner::generateCSVReport(const std::vector<TestResult>& results, 
                                   const std::string& filename) {
    std::ofstream file(filename);
    if (!file) return;
    
    file << "TestName,Status,Category,ExecutionTime(ns),SetupTime(ns),TeardownTime(ns),MemoryDelta,Message,File,Line" << std::endl;
    
    for (const auto& result : results) {
        file << "\"" << result.name << "\"," 
             << static_cast<int>(result.status) << "," 
             << static_cast<int>(result.category) << "," 
             << result.executionTime.count() << "," 
             << result.setupTime.count() << ",\" 
             << result.teardownTime.count() << "," 
             << result.memoryDelta << ",\" 
             << result.message << "\",\"" 
             << result.file << "\"," 
             << result.line << std::endl;
    }
}

// TestFilter matching utilities
bool TestFilter::matches(const TestBase& test, const TestFilter& filter) {
    return filter.shouldInclude(test);
}

// TestSuite helper functions
TestSuite TestSuite::createUnitTests() {
    TestSuite suite("Unit Tests");
    return suite;
}

TestSuite TestSuite::createIntegrationTests() {
    TestSuite suite("Integration Tests");
    return suite;
}

TestSuite TestSuite::createPerformanceTests() {
    TestSuite suite("Performance Tests");
    return suite;
}

TestSuite TestSuite::createStressTests() {
    TestSuite suite("Stress Tests");
    return suite;
}

TestSuite TestSuite::createRegressionTests() {
    TestSuite suite("Regression Tests");
    return suite;
}

TestSuite TestSuite::createMemoryTests() {
    TestSuite suite("Memory Tests");
    return suite;
}

TestSuite TestSuite::createConcurrencyTests() {
    TestSuite suite("Concurrency Tests");
    return suite;
}

TestSuite TestSuite::createAccuracyTests() {
    TestSuite suite("Accuracy Tests");
    return suite;
}

// TestRunner utility functions
TestStatistics TestRunner::calculateStatistics(const std::vector<TestResult>& results) {
    TestStatistics stats;
    
    stats.totalTests = results.size();
    
    for (const auto& result : results) {
        switch (result.status) {
            case TestStatus::Passed: stats.passedTests++; break;
            case TestStatus::Failed: stats.failedTests++; break;
            case TestStatus::Skipped: stats.skippedTests++; break;
            case TestStatus::Timeout: stats.timeoutTests++; break;
            default: break;
        }
        
        stats.totalExecutionTime += result.executionTime;
        stats.totalSetupTime += result.setupTime;
        stats.totalTeardownTime += result.teardownTime;
        
        if (result.memoryDelta > 0) {
            // Count potential memory leaks
            stats.memoryLeaks++;
        }
    }
    
    if (stats.totalTests > 0) {
        stats.memoryUsageGB = static_cast<double>(stats.memoryLeaks) / 1024.0 / 1024.0; // Simplified
    }
    
    return stats;
}

// TestResult comparison utilities
bool TestResult::operator<(const TestResult& other) const {
    return executionTime < other.executionTime;
}

bool TestResult::operator>(const TestResult& other) const {
    return executionTime > other.executionTime;
}

// TestFilter specialization helpers
TestFilter CategoryFilter::createAll() {
    return TestFilter::categoryFilter(TestCategory::Unit);
}

TestFilter TestFilter::categoryFilter(TestCategory category) {
    return std::make_shared<CategoryFilter>(category);
}

TestFilter TestFilter::nameFilter(const std::string& pattern) {
    return std::make_shared<NameFilter>(pattern);
}

TestFilter TestFilter::tagFilter(const std::string& tag) {
    return std::make_shared<TagFilter>(tag);
}

}}} // namespace nlm::test
