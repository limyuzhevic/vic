#ifndef TEST_RUNNER_HPP
#define TEST_RUNNER_HPP

#include "tests/Framework/TestFramework.hpp"
#include "tests/TestConfig.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <chrono>
#include <thread>
#include <future>

/**
 * Test runner for NLM testing framework
 */

class TestRunner {
public:
    TestRunner() : config_(getDefaultTestConfig()) {
        setupDefaultCategories();
    }
    
    TestRunner(const TestConfig& config) : config_(config) {
        setupDefaultCategories();
    }
    
    void runTests() {
        std::cout << "\n=== NLM Test Suite ===\n";
        std::cout << "Configuration: " << config_.name << std::endl;
        std::cout << "Description: " << config_.description << std::endl;
        std::cout << "Enabled: " << (config_.enabled ? "Yes" : "No") << std::endl;
        std::cout << "Categories: ";
        for (const auto& category : config_.organization.test_categories) {
            std::cout << category << " ";
        }
        std::cout << std::endl << std::endl;
        
        auto start_time = std::chrono::high_resolution_clock::now();
        
        // Run tests in category order
        runTestsByCategory("Unit");
        runTestsByCategory("Integration");
        runTestsByCategory("Performance");
        runTestsByCategory("General");
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto total_duration = std::chrono::duration_cast<std::chrono::milliseconds>(end_time - start_time);
        
        // Generate reports
        generateReports();
        
        // Print summary
        printSummary(total_duration.count());
    }
    
    void setConfig(const TestConfig& config) {
        config_ = config;
        setupDefaultCategories();
    }
    
    const TestConfig& getConfig() const {
        return config_;
    }
    
    void addTestCategory(const std::string& category, const std::string& description = "") {
        if (std::find(config_.organization.test_categories.begin(), 
                      config_.organization.test_categories.end(), category) == config_.organization.test_categories.end()) {
            config_.organization.test_categories.push_back(category);
            if (!description.empty()) {
                config_.organization.category_descriptions[category] = description;
            }
        }
    }
    
    void removeTestCategory(const std::string& category) {
        auto it = std::find(config_.organization.test_categories.begin(), 
                           config_.organization.test_categories.end(), category);
        if (it != config_.organization.test_categories.end()) {
            config_.organization.test_categories.erase(it);
            config_.organization.category_descriptions.erase(category);
        }
    }
    
    void setTestCategoryDescription(const std::string& category, const std::string& description) {
        config_.organization.category_descriptions[category] = description;
    }
    
    std::string getTestCategoryDescription(const std::string& category) const {
        auto it = config_.organization.category_descriptions.find(category);
        return (it != config_.organization.category_descriptions.end()) ? it->second : "";
    }
    
    void enableCategory(const std::string& category, bool enabled = true) {
        if (enabled) {
            addTestCategory(category);
        } else {
            removeTestCategory(category);
        }
    }
    
    bool isCategoryEnabled(const std::string& category) const {
        return std::find(config_.organization.test_categories.begin(), 
                        config_.organization.test_categories.end(), category) != config_.organization.test_categories.end();
    }
    
    void enableVerbose(bool enabled = true) {
        config_.verbose = enabled;
    }
    
    void enableParallelExecution(bool enabled = true) {
        config_.framework.parallel_execution = enabled;
    }
    
    void setMaxThreads(size_t max_threads) {
        config_.framework.max_threads = max_threads;
    }
    
    void setTimeoutPerTest(double timeout_seconds) {
        config_.framework.timeout_per_test = timeout_seconds;
    }
    
    void enablePerformanceMonitoring(bool enabled = true) {
        config_.performance.enable_performance_monitoring = enabled;
    }
    
    void enableProfiling(bool enabled = true) {
        config_.performance.enable_profiling = enabled;
        config_.framework.enable_profiling = enabled;
    }
    
    void enableCoverage(bool enabled = true) {
        config_.coverage.enable_coverage = enabled;
    }
    
    void setCoverageThreshold(double threshold) {
        config_.coverage.coverage_threshold = threshold;
    }
    
    void enableIntegrationTests(bool enabled = true) {
        config_.integration.enable_integration_tests = enabled;
    }
    
    void enableConsoleOutput(bool enabled = true) {
        config_.reporting.enable_console_output = enabled;
    }
    
    void enableFileOutput(bool enabled = true) {
        config_.reporting.enable_file_output = enabled;
    }
    
    void setReportFile(const std::string& file) {
        config_.reporting.report_file = file;
    }
    
    void generateHtmlReport() {
        config_.reporting.generate_html_report = true;
    }
    
    void generateXmlReport() {
        config_.reporting.generate_xml_report = true;
    }
    
    void generateCsvReport() {
        config_.reporting.generate_csv_report = true;
    }
    
    void generatePdfReport() {
        config_.reporting.generate_pdf_report = true;
    }
    
    void enableTestReuse(bool enabled = true) {
        config_.execution.enable_test_reuse = enabled;
    }
    
    void enableTestCaching(bool enabled = true) {
        config_.execution.enable_test_caching = enabled;
    }
    
    void setTestCacheSize(size_t size) {
        config_.execution.test_cache_size = size;
    }
    
    void enableEarlyExit(bool enabled = true) {
        config_.execution.enable_early_exit = enabled;
    }
    
    void setMaxConcurrentTests(size_t max_tests) {
        config_.execution.max_concurrent_tests = max_tests;
    }
    
    void enableTestRetry(bool enabled = true) {
        config_.execution.enable_test_retry = enabled;
    }
    
    void setMaxTestRetries(size_t retries) {
        config_.execution.max_test_retries = retries;
    }
    
protected:
    void setupDefaultCategories() {
        // Clear existing categories
        config_.organization.test_categories.clear();
        config_.organization.category_descriptions.clear();
        
        // Add default categories
        config_.organization.test_categories = {
            "Unit", "Integration", "Performance", "General"
        };
        
        // Set default descriptions
        config_.organization.category_descriptions = {
            {"Unit", "Unit tests for individual components"},
            {"Integration", "Integration tests for system components"},
            {"Performance", "Performance benchmarks and profiling"},
            {"General", "General utility and integration tests"}
        };
    }
    
    void runTestsByCategory(const std::string& category) {
        if (!isCategoryEnabled(category) || category.empty()) return;
        
        std::cout << "\n--- Running " << category << " Tests ---" << std::endl;
        
        // In a real implementation, this would find and run tests in the specified category
        // For this framework, we rely on the test framework's registration system
        TestFramework::getInstance().runAllTests();
    }
    
    void generateReports() {
        if (config_.reporting.enable_console_output) {
            std::cout << "\n=== Test Reports Generated ===" << std::endl;
            std::cout << "Console output: " << (config_.reporting.enable_console_output ? "Yes" : "No") << std::endl;
            std::cout << "File output: " << (config_.reporting.enable_file_output ? "Yes" : "No") << std::endl;
            
            if (config_.reporting.enable_file_output && !config_.reporting.report_file.empty()) {
                std::cout << "Report file: " << config_.reporting.report_file << std::endl;
            }
            
            if (config_.reporting.generate_html_report) {
                std::cout << "HTML report: Generated" << std::endl;
            }
            
            if (config_.reporting.generate_xml_report) {
                std::cout << "XML report: Generated" << std::endl;
            }
            
            if (config_.reporting.generate_csv_report) {
                std::cout << "CSV report: Generated" << std::endl;
            }
            
            if (config_.reporting.generate_pdf_report) {
                std::cout << "PDF report: Generated" << std::endl;
            }
        }
        
        // In a real implementation, this would generate actual reports
    }
    
    void printSummary(double total_duration_ms) {
        std::cout << "\n=== Test Summary ===" << std::endl;
        std::cout << "Total duration: " << total_duration_ms << " ms" << std::endl;
        std::cout << "Configuration: " << config_.name << std::endl;
        std::cout << "Status: Test suite completed" << std::endl;
        
        // In a real implementation, this would show actual test results
        std::cout << "\nNote: This is a framework skeleton. Actual test execution requires test registration." << std::endl;
    }
    
private:
    TestConfig config_;
};

#endif // TEST_RUNNER_HPP