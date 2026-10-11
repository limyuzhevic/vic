// Test Suite Runner Implementation
// Comprehensive test suite management and reporting for Phase 6 integration
// Implements the TestSuiteRunner class for unified test execution and reporting

#include "TestSuiteRunner.hpp"
#include "Phase6IntegrationTests.hpp"
#include "PerformanceTests.hpp"
#include "core/Logger/Logger.hpp"
#include <iostream>
#include <fstream>
#include <algorithm>
#include <cmath>

TestSuiteRunner::TestSuiteRunner() : start_time(std::chrono::high_resolution_clock::now()) {
    // Constructor
}

TestSuiteRunner::~TestSuiteRunner() {
    // Destructor
}

void TestSuiteRunner::initialize(const std::string& testSuiteName) {
    suite_name = testSuiteName;
    NLM_LOG_INFO("Initializing Test Suite Runner: " + suite_name);
    start_time = std::chrono::high_resolution_clock::now();
}

void TestSuiteRunner::cleanup() {
    NLM_LOG_INFO("Test Suite Runner cleanup completed");
}

void TestSuiteRunner::runAllTests() {
    NLM_LOG_INFO("Starting comprehensive test suite execution");
    
    runPhase6IntegrationTests();
    runPerformanceTests();
    
    // Calculate final metrics
    auto end_time = std::chrono::high_resolution_clock::now();
    metrics.total_duration_seconds = 
        std::chrono::duration_cast<std::chrono::seconds>(end_time - start_time).count();
    
    // Update metrics
    for (const auto& result : test_results) {
        metrics.total_tests++;
        metrics.category_counts[result.category]++;
        
        if (result.passed) {
            metrics.passed_tests++;
            metrics.category_passed[result.category]++;
        } else {
            metrics.failed_tests++;
        }
    }
    
    printSummary();
}

void TestSuiteRunner::runPhase6IntegrationTests() {
    NLM_LOG_INFO("Running Phase 6 Integration Tests");
    
    test_phase6_integration::test_neuron_integration();
    
    NLM_LOG_SUCCESS("Phase 6 Integration Tests completed");
}

void TestSuiteRunner::runPerformanceTests() {
    NLM_LOG_INFO("Running Performance Tests");
    
    test_performance::run_performance_tests();
    
    NLM_LOG_SUCCESS("Performance Tests completed");
}

void TestSuiteRunner::addTestResult(const TestResult& result) {
    test_results.push_back(result);
}

void TestSuiteRunner::printSummary() const {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "TEST SUITE SUMMARY" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    std::cout << "Suite Name: " << suite_name << std::endl;
    std::cout << "Total Tests: " << metrics.total_tests << std::endl;
    std::cout << "Passed: " << metrics.passed_tests << std::endl;
    std::cout << "Failed: " << metrics.failed_tests << std::endl;
    std::cout << std::fixed << std::setprecision(2);
    std::cout << "Pass Rate: " << metrics.getPassRate() << "%" << std::endl;
    std::cout << std::setprecision(3);
    std::cout << "Total Duration: " << metrics.total_duration_seconds << " seconds" << std::endl;
    
    std::cout << "\nCategory Breakdown:" << std::endl;
    for (const auto& category : metrics.category_counts) {
        std::cout << "  " << category.first << ": " << category.second << " tests, "
                  << (metrics.category_passed.find(category.first) != metrics.category_passed.end() 
                      ? metrics.category_passed.at(category.first) 
                      : 0) << " passed" << std::endl;
    }
}

void TestSuiteRunner::printDetailedReport() const {
    std::cout << "\n" << std::string(80, '=') << std::endl;
    std::cout << "DETAILED TEST REPORT" << std::endl;
    std::cout << std::string(80, '=') << std::endl;
    
    for (const auto& result : test_results) {
        printTestResult(result);
    }
}

void TestSuiteRunner::printTestResult(const TestResult& result) const {
    std::cout << std::fixed << std::setprecision(3);
    
    // Format test result line
    std::cout << result.name << " (" << result.category << "): ";
    
    if (result.passed) {
        std::cout << "PASS";
    } else {
        std::cout << "FAIL";
    }
    
    std::cout << " - Duration: " << formatDuration(result.duration);
    
    if (result.iteration_count > 0) {
        std::cout << " - Throughput: " << result.throughput << " ops/sec";
    }
    
    if (result.bandwidth > 0.0) {
        std::cout << " - Bandwidth: " << result.bandwidth << " GB/s";
    }
    
    std::cout << std::endl;
}

void TestSuiteRunner::printCategorySummary(const std::string& category) const {
    int total = metrics.category_counts.at(category);
    int passed = metrics.category_passed.find(category) != metrics.category_passed.end() 
                 ? metrics.category_passed.at(category) 
                 : 0;
    
    std::cout << category << ": " << passed << "/" << total << " (" 
              << std::fixed << std::setprecision(1) << (total > 0 ? (double)passed / total * 100.0 : 0.0) 
              << "%)" << std::endl;
}

std::string TestSuiteRunner::formatDuration(std::chrono::microseconds duration) const {
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    auto milliseconds = std::chrono::duration_cast<std::chrono::milliseconds>(duration % std::chrono::seconds(1));
    
    std::stringstream ss;
    ss << seconds.count() << "." << std::setw(3) << std::setfill('0') << milliseconds.count() << "s";
    return ss.str();
}

std::string TestSuiteRunner::formatTimestamp(const std::chrono::high_resolution_clock::time_point& time) const {
    auto duration = std::chrono::high_resolution_clock::now() - time;
    auto seconds = std::chrono::duration_cast<std::chrono::seconds>(duration);
    
    std::stringstream ss;
    ss << seconds.count() << "s ago";
    return ss.str();
}

std::string TestSuiteRunner::generateJSONReport() const {
    std::stringstream json;
    
    json << "{\n";
    json << "  \"suite_name\": \"" << suite_name << "\",\n";
    json << "  \"total_tests\": " << metrics.total_tests << ",\n";
    json << "  \"passed_tests\": " << metrics.passed_tests << ",\n";
    json << "  \"failed_tests\": " << metrics.failed_tests << ",\n";
    json << "  \"pass_rate\": " << std::fixed << std::setprecision(2) << metrics.getPassRate() << ",\n";
    json << "  \"total_duration_seconds\": " << std::setprecision(3) << metrics.total_duration_seconds << ",\n";
    json << "  \"tests\": [\n";
    
    for (size_t i = 0; i < test_results.size(); ++i) {
        const auto& result = test_results[i];
        
        json << "    {\n";
        json << "      \"name\": \"" << result.name << "\",\n";
        json << "      \"category\": \"" << result.category << "\",\n";
        json << "      \"passed\": " << (result.passed ? "true" : "false") << ",\n";
        json << "      \"duration\": " << result.duration.count() << ",\n";
        json << "      \"message\": \"" << result.message << "\"\n";
        
        if (i < test_results.size() - 1) {
            json << "    },\n";
        } else {
            json << "    }\n";
        }
    }
    
    json << "  ]\n";
    json << "}\n";
    
    return json.str();
}

std::string TestSuiteRunner::generateMarkdownReport() const {
    std::stringstream markdown;
    
    markdown << "# Test Suite Report" << std::endl;
    markdown << "## " << suite_name << std::endl;
    markdown << std::endl;
    
    markdown << "**Summary:** " << metrics.passed_tests << "/" << metrics.total_tests << " tests passed ("
              << std::fixed << std::setprecision(1) << metrics.getPassRate() << "%)" << std::endl;
    markdown << "**Duration:** " << std::setprecision(3) << metrics.total_duration_seconds << " seconds" << std::endl;
    markdown << std::endl;
    
    markdown << "## Test Results" << std::endl;
    markdown << std::endl;
    
    for (const auto& result : test_results) {
        std::string status = result.passed ? "✅ PASS" : "❌ FAIL";
        markdown << "### " << result.name << " (" << result.category << ")" << std::endl;
        markdown << status << " - Duration: " << formatDuration(result.duration);
        
        if (result.iteration_count > 0) {
            markdown << " - Throughput: " << result.throughput << " ops/sec";
        }
        
        if (result.bandwidth > 0.0) {
            markdown << " - Bandwidth: " << result.bandwidth << " GB/s";
        }
        
        if (!result.message.empty()) {
            markdown << " - " << result.message;
        }
        
        markdown << std::endl << std::endl;
    }
    
    markdown << "## Category Summary" << std::endl;
    markdown << std::endl;
    
    for (const auto& category : metrics.category_counts) {
        int passed = metrics.category_passed.find(category.first) != metrics.category_passed.end()
                     ? metrics.category_passed.at(category.first)
                     : 0;
        
        markdown << "### " << category.first << std::endl;
        markdown << passed << "/" << category.second << " tests passed ("
                  << std::fixed << std::setprecision(1) << (category.second > 0 ? (double)passed / category.second * 100.0 : 0.0)
                  << "%)" << std::endl;
        markdown << std::endl;
    }
    
    return markdown.str();
}

// Global test runner instance
TestSuiteRunner g_test_suite_runner;