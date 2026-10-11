// NLM Enhanced Test Reporting System
// Phase 7: Comprehensive Test Suite
// Advanced test reporting and result analysis

#pragma once

#include <string>
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <map>
#include <set>
#include <chrono>
#include <cmath>

// Test result categories
enum class TestResultCategory {
    PASSED,
    FAILED,
    SKIPPED,
    TIMEOUT,
    ERROR,
    MEMORY_LEAK,
    PERFORMANCE,
    INTEGRATION,
    STRESS,
    REGRESSION,
    CROSS_PLATFORM
};

// Test result severity levels
enum class TestSeverity {
    INFO,
    WARNING,
    ERROR,
    CRITICAL
};

// Performance analysis results
struct PerformanceAnalysis {
    std::string test_name;
    double execution_time_ns;
    double execution_time_us;
    double execution_time_ms;
    double execution_time_s;
    size_t memory_used;
    size_t peak_memory;
    double memory_efficiency;
    size_t operations_per_second;
    std::vector<std::string> performance_issues;
    bool performance_passed = true;
    
    PerformanceAnalysis(const std::string& name) : test_name(name) {}
};

// Memory analysis results
struct MemoryAnalysis {
    std::string test_name;
    size_t total_allocated;
    size_t peak_allocated;
    size_t current_allocated;
    size_t leak_count;
    std::vector<std::string> leak_descriptions;
    std::map<std::string, size_t> allocation_patterns;
    bool memory_passed = true;
    
    MemoryAnalysis(const std::string& name) : test_name(name) {}
};

// Integration analysis results
struct IntegrationAnalysis {
    std::string test_name;
    std::vector<std::string> components_tested;
    std::vector<std::string> integration_points;
    std::vector<std::string> failed_integration_points;
    std::chrono::duration<double> average_interaction_time;
    bool integration_passed = true;
    
    IntegrationAnalysis(const std::string& name) : test_name(name) {}
};

// Stress test analysis results
struct StressAnalysis {
    std::string test_name;
    std::chrono::duration<double> test_duration;
    double target_load;
    double actual_load;
    double stability_score;
    std::vector<std::string> stress_issues;
    bool stress_passed = true;
    
    StressAnalysis(const std::string& name) : test_name(name) {}
};

// Regression analysis results
struct RegressionAnalysis {
    std::string test_name;
    std::string baseline_version;
    std::string current_version;
    double tolerance_value;
    std::vector<std::string> regression_detected;
    std::vector<std::string> regression_fixed;
    bool regression_passed = true;
    
    RegressionAnalysis(const std::string& name) : test_name(name) {}
};

// Cross-platform analysis results
struct CrossPlatformAnalysis {
    std::string test_name;
    std::vector<std::string> platforms_tested;
    std::vector<std::string> platforms_failed;
    std::map<std::string, std::string> platform_results;
    bool platform_consistency = true;
    
    CrossPlatformAnalysis(const std::string& name) : test_name(name) {}
};

// Detailed test result with comprehensive analysis
struct DetailedTestResult {
    std::string test_name;
    TestResultCategory category;
    TestSeverity severity;
    TestStatus status;
    std::chrono::duration<double> execution_time;
    size_t memory_used;
    size_t peak_memory;
    std::string description;
    std::string error_message;
    std::vector<std::string> warnings;
    std::vector<std::string> tags;
    std::chrono::time_point<std::chrono::system_clock> start_time;
    std::chrono::time_point<std::chrono::system_clock> end_time;
    
    // Analysis results
    PerformanceAnalysis performance_analysis;
    MemoryAnalysis memory_analysis;
    IntegrationAnalysis integration_analysis;
    StressAnalysis stress_analysis;
    RegressionAnalysis regression_analysis;
    CrossPlatformAnalysis cross_platform_analysis;
    
    DetailedTestResult(const std::string& name) 
        : test_name(name), status(TestStatus::NOT_STARTED),
          performance_analysis(name), memory_analysis(name),
          integration_analysis(name), stress_analysis(name),
          regression_analysis(name), cross_platform_analysis(name) {}
};

// Test suite report with comprehensive analysis
struct TestSuiteReport {
    std::string title;
    std::string description;
    std::string generated_time;
    std::chrono::duration<double> total_execution_time;
    std::chrono::duration<double> setup_time;
    std::chrono::duration<double> teardown_time;
    
    TestExecutionStats execution_stats;
    std::vector<DetailedTestResult> detailed_results;
    
    // Summary statistics
    std::map<TestResultCategory, size_t> category_counts;
    std::map<TestSeverity, size_t> severity_counts;
    std::map<std::string, std::vector<DetailedTestResult>> results_by_tag;
    std::map<std::string, std::vector<DetailedTestResult>> results_by_category;
    std::map<std::string, std::vector<DetailedTestResult>> results_by_platform;
    
    // Performance and analysis summaries
    std::vector<PerformanceAnalysis> performance_summaries;
    std::vector<MemoryAnalysis> memory_summaries;
    std::vector<IntegrationAnalysis> integration_summaries;
    std::vector<StressAnalysis> stress_summaries;
    std::vector<RegressionAnalysis> regression_summaries;
    std::vector<CrossPlatformAnalysis> platform_summaries;
    
    TestSuiteReport(const std::string& report_title, const std::string& report_desc)
        : title(report_title), description(report_desc) {}
};

// Test report generator with advanced features
class TestReportGenerator {
private:
    std::string output_directory;
    bool enable_html_reports;
    bool enable_json_reports;
    bool enable_xml_reports;
    bool enable_csv_reports;
    bool enable_text_reports;
    bool enable_performance_reports;
    bool enable_memory_reports;
    bool enable_integration_reports;
    bool enable_stress_reports;
    bool enable_regression_reports;
    bool enable_platform_reports;
    
public:
    TestReportGenerator() : output_directory("test_reports"),
        enable_html_reports(true), enable_json_reports(true),
        enable_xml_reports(true), enable_csv_reports(true),
        enable_text_reports(true), enable_performance_reports(true),
        enable_memory_reports(true), enable_integration_reports(true),
        enable_stress_reports(true), enable_regression_reports(true),
        enable_platform_reports(true) {}
    
    void set_output_directory(const std::string& directory) {
        output_directory = directory;
    }
    
    void enable_report_format(const std::string& format, bool enabled) {
        if (format == "html") enable_html_reports = enabled;
        else if (format == "json") enable_json_reports = enabled;
        else if (format == "xml") enable_xml_reports = enabled;
        else if (format == "csv") enable_csv_reports = enabled;
        else if (format == "text") enable_text_reports = enabled;
        else if (format == "performance") enable_performance_reports = enabled;
        else if (format == "memory") enable_memory_reports = enabled;
        else if (format == "integration") enable_integration_reports = enabled;
        else if (format == "stress") enable_stress_reports = enabled;
        else if (format == "regression") enable_regression_reports = enabled;
        else if (format == "platform") enable_platform_reports = enabled;
    }
    
    void generate_all_reports(const TestSuiteReport& report) {
        if (enable_text_reports) {
            generate_text_report(report);
        }
        if (enable_json_reports) {
            generate_json_report(report);
        }
        if (enable_xml_reports) {
            generate_xml_report(report);
        }
        if (enable_html_reports) {
            generate_html_report(report);
        }
        if (enable_csv_reports) {
            generate_csv_report(report);
        }
        if (enable_performance_reports) {
            generate_performance_report(report);
        }
        if (enable_memory_reports) {
            generate_memory_report(report);
        }
        if (enable_integration_reports) {
            generate_integration_report(report);
        }
        if (enable_stress_reports) {
            generate_stress_report(report);
        }
        if (enable_regression_reports) {
            generate_regression_report(report);
        }
        if (enable_platform_reports) {
            generate_platform_report(report);
        }
    }
    
    void generate_text_report(const TestSuiteReport& report) {
        std::string filename = output_directory + "/test_report.txt";
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create text report file: " + filename);
        }
        
        file << "=== NLM Enhanced Test Suite Report ===" << std::endl;
        file << "Title: " << report.title << std::endl;
        file << "Description: " << report.description << std::endl;
        file << "Generated: " << report.generated_time << std::endl;
        file << "Total Execution Time: " << std::fixed << std::setprecision(3) 
             << report.total_execution_time.count() << " seconds" << std::endl;
        file << std::endl;
        
        file << "=== Executive Summary ===" << std::endl;
        file << "Total Tests: " << report.execution_stats.total_tests << std::endl;
        file << "Passed: " << report.execution_stats.passed_tests << std::endl;
        file << "Failed: " << report.execution_stats.failed_tests << std::endl;
        file << "Skipped: " << report.execution_stats.skipped_tests << std::endl;
        file << "Timeouts: " << report.execution_stats.timeout_tests << std::endl;
        file << std::endl;
        
        file << "Success Rate: " << std::fixed << std::setprecision(2) 
             << report.execution_stats.get_pass_rate() << "%" << std::endl;
        file << "Failure Rate: " << std::fixed << std::setprecision(2) 
             << report.execution_stats.get_failure_rate() << "%" << std::endl;
        file << "Timeout Rate: " << std::fixed << std::setprecision(2) 
             << report.execution_stats.get_timeout_rate() << "%" << std::endl;
        file << std::endl;
        
        file << "=== Detailed Test Results ===" << std::endl;
        for (const auto& result : report.detailed_results) {
            file << "Test: " << result.test_name << std::endl;
            file << "  Status: " << get_test_status_string(result.status) << std::endl;
            file << "  Category: " << get_test_category_string(result.category) << std::endl;
            file << "  Severity: " << get_test_severity_string(result.severity) << std::endl;
            file << "  Execution Time: " << std::fixed << std::setprecision(3) 
                 << result.execution_time.count() << " seconds" << std::endl;
            file << "  Memory Used: " << result.memory_used << " bytes" << std::endl;
            file << "  Peak Memory: " << result.peak_memory << " bytes" << std::endl;
            
            if (!result.description.empty()) {
                file << "  Description: " << result.description << std::endl;
            }
            
            if (!result.error_message.empty()) {
                file << "  Error: " << result.error_message << std::endl;
            }
            
            if (!result.warnings.empty()) {
                file << "  Warnings:" << std::endl;
                for (const auto& warning : result.warnings) {
                    file << "    - " << warning << std::endl;
                }
            }
            
            if (!result.tags.empty()) {
                file << "  Tags: ";
                for (size_t i = 0; i < result.tags.size(); ++i) {
                    if (i > 0) file << ", ";
                    file << result.tags[i];
                }
                file << std::endl;
            }
            
            file << std::endl;
        }
        
        file.close();
    }
    
    void generate_json_report(const TestSuiteReport& report) {
        std::string filename = output_directory + "/test_report.json";
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create JSON report file: " + filename);
        }
        
        file << "{\n";
        file << "  \"title\": \"" << report.title << "\",\n";
        file << "  \"description\": \"" << report.description << "\",\n";
        file << "  \"generated_time\": \"" << report.generated_time << "\",\n";
        file << "  \"total_execution_time\": " << std::fixed << std::setprecision(3) 
             << report.total_execution_time.count() << ",\n";
        file << "  \"setup_time\": " << std::fixed << std::setprecision(3) 
             << report.setup_time.count() << ",\n";
        file << "  \"teardown_time\": " << std::fixed << std::setprecision(3) 
             << report.teardown_time.count() << ",\n";
        file << "  \"execution_stats\": {\n";
        file << "    \"total_tests\": " << report.execution_stats.total_tests << ",\n";
        file << "    \"passed_tests\": " << report.execution_stats.passed_tests << ",\n";
        file << "    \"failed_tests\": " << report.execution_stats.failed_tests << ",\n";
        file << "    \"skipped_tests\": " << report.execution_stats.skipped_tests << ",\n";
        file << "    \"timeout_tests\": " << report.execution_stats.timeout_tests << ",\n";
        file << "    \"total_execution_time\": " << report.execution_stats.total_execution_time << ",\n";
        file << "    \"total_memory_used\": " << report.execution_stats.total_memory_used << ",\n";
        file << "    \"peak_memory_used\": " << report.execution_stats.peak_memory_used << ",\n";
        file << "    \"pass_rate\": " << report.execution_stats.get_pass_rate() << ",\n";
        file << "    \"failure_rate\": " << report.execution_stats.get_failure_rate() << ",\n";
        file << "    \"timeout_rate\": " << report.execution_stats.get_timeout_rate() << "\n";
        file << "  },\n";
        file << "  \"detailed_results\": [\n";
        
        for (size_t i = 0; i < report.detailed_results.size(); ++i) {
            const auto& result = report.detailed_results[i];
            file << "    {\n";
            file << "      \"test_name\": \"" << result.test_name << "\",\n";
            file << "      \"status\": \"" << get_test_status_string(result.status) << "\",\n";
            file << "      \"category\": \"" << get_test_category_string(result.category) << "\",\n";
            file << "      \"severity\": \"" << get_test_severity_string(result.severity) << "\",\n";
            file << "      \"execution_time\": " << std::fixed << std::setprecision(3) 
                 << result.execution_time.count() << ",\n";
            file << "      \"memory_used\": " << result.memory_used << ",\n";
            file << "      \"peak_memory\": " << result.peak_memory << ",\n";
            if (!result.description.empty()) {
                file << "      \"description\": \"" << escape_json(result.description) << "\",\n";
            }
            if (!result.error_message.empty()) {
                file << "      \"error_message\": \"" << escape_json(result.error_message) << "\",\n";
            }
            if (!result.warnings.empty()) {
                file << "      \"warnings\": [";
                for (size_t j = 0; j < result.warnings.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escape_json(result.warnings[j]) << "\"";
                }
                file << "],\n";
            }
            if (!result.tags.empty()) {
                file << "      \"tags\": [";
                for (size_t j = 0; j < result.tags.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escape_json(result.tags[j]) << "\"";
                }
                file << "],\n";
            }
            file << "      \"start_time\": \"" << get_time_string(result.start_time) << "\",\n";
            file << "      \"end_time\": \"" << get_time_string(result.end_time) << "\"\n";
            
            if (i < report.detailed_results.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ],\n";
        file << "  \"category_counts\": {\n";
        for (const auto& pair : report.category_counts) {
            std::string category_name = get_test_category_string(pair.first);
            file << "    \"" << category_name << "\": " << pair.second << ",\n";
        }
        file << "  },\n";
        file << "  \"severity_counts\": {\n";
        for (const auto& pair : report.severity_counts) {
            std::string severity_name = get_test_severity_string(pair.first);
            file << "    \"" << severity_name << "\": " << pair.second << ",\n";
        }
        file << "  },\n";
        file << "  \"performance_summaries\": [\n";
        
        for (size_t i = 0; i < report.performance_summaries.size(); ++i) {
            const auto& perf = report.performance_summaries[i];
            file << "    {\n";
            file << "      \"test_name\": \"" << perf.test_name << "\",\n";
            file << "      \"execution_time_ns\": " << perf.execution_time_ns << ",\n";
            file << "      \"execution_time_us\": " << perf.execution_time_us << ",\n";
            file << "      \"execution_time_ms\": " << perf.execution_time_ms << ",\n";
            file << "      \"execution_time_s\": " << perf.execution_time_s << ",\n";
            file << "      \"memory_used\": " << perf.memory_used << ",\n";
            file << "      \"operations_per_second\": " << perf.operations_per_second << ",\n";
            file << "      \"memory_efficiency\": " << perf.memory_efficiency << ",\n";
            file << "      \"performance_passed\": " << (perf.performance_passed ? "true" : "false") << "\n";
            
            if (i < report.performance_summaries.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
    }
    
    void generate_xml_report(const TestSuiteReport& report) {
        std::string filename = output_directory + "/test_report.xml";
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create XML report file: " + filename);
        }
        
        file << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        file << "<test_results>\n";
        file << "  <metadata>\n";
        file << "    <title>" << report.title << "</title>\n";
        file << "    <description>" << report.description << "</description>\n";
        file << "    <generated_time>" << report.generated_time << "</generated_time>\n";
        file << "  </metadata>\n";
        file << "  <execution_summary>\n";
        file << "    <total_tests>" << report.execution_stats.total_tests << "</total_tests>\n";
        file << "    <passed_tests>" << report.execution_stats.passed_tests << "</passed_tests>\n";
        file << "    <failed_tests>" << report.execution_stats.failed_tests << "</failed_tests>\n";
        file << "    <skipped_tests>" << report.execution_stats.skipped_tests << "</skipped_tests>\n";
        file << "    <timeout_tests>" << report.execution_stats.timeout_tests << "</timeout_tests>\n";
        file << "    <pass_rate>" << report.execution_stats.get_pass_rate() << "</pass_rate>\n";
        file << "    <failure_rate>" << report.execution_stats.get_failure_rate() << "</failure_rate>\n";
        file << "    <timeout_rate>" << report.execution_stats.get_timeout_rate() << "</timeout_rate>\n";
        file << "  </execution_summary>\n";
        file << "  <test_results>\n";
        
        for (const auto& result : report.detailed_results) {
            file << "    <test_result>\n";
            file << "      <test_name>" << result.test_name << "</test_name>\n";
            file << "      <status>" << get_test_status_string(result.status) << "</status>\n";
            file << "      <category>" << get_test_category_string(result.category) << "</category>\n";
            file << "      <severity>" << get_test_severity_string(result.severity) << "</severity>\n";
            file << "      <execution_time>" << std::fixed << std::setprecision(3) 
                 << result.execution_time.count() << "</execution_time>\n";
            file << "      <memory_used>" << result.memory_used << "</memory_used>\n";
            file << "      <peak_memory>" << result.peak_memory << "</peak_memory>\n";
            if (!result.description.empty()) {
                file << "      <description>" << escape_xml(result.description) << "</description>\n";
            }
            if (!result.error_message.empty()) {
                file << "      <error_message>" << escape_xml(result.error_message) << "</error_message>\n";
            }
            if (!result.warnings.empty()) {
                file << "      <warnings>\n";
                for (const auto& warning : result.warnings) {
                    file << "        <warning>" << escape_xml(warning) << "</warning>\n";
                }
                file << "      </warnings>\n";
            }
            if (!result.tags.empty()) {
                file << "      <tags>\n";
                for (const auto& tag : result.tags) {
                    file << "        <tag>" << escape_xml(tag) << "</tag>\n";
                }
                file << "      </tags>\n";
            }
            file << "      <start_time>" << get_time_string(result.start_time) << "</start_time>\n";
            file << "      <end_time>" << get_time_string(result.end_time) << "</end_time>\n";
            file << "    </test_result>\n";
        }
        
        file << "  </test_results>\n";
        file << "</test_results>\n";
        
        file.close();
    }
    
    void generate_html_report(const TestSuiteReport& report) {
        std::string filename = output_directory + "/test_report.html";
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create HTML report file: " + filename);
        }
        
        file << "<!DOCTYPE html>\n";
        file << "<html>\n";
        file << "<head>\n";
        file << "  <title>" << report.title << "</title>\n";
        file << "  <style>\n";
        file << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
        file << "    h1 { color: #333; }\n";
        file << "    h2 { color: #555; border-bottom: 2px solid #ccc; padding-bottom: 5px; }\n";
        file << "    .passed { background-color: #d4edda; color: #155724; }\n";
        file << "    .failed { background-color: #f8d7da; color: #721c24; }\n";
        file << "    .skipped { background-color: #fff3cd; color: #856404; }\n";
        file << "    .timeout { background-color: #ffeeba; color: #856404; }\n";
        file << "    table { border-collapse: collapse; width: 100%; margin-bottom: 20px; }\n";
        file << "    th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }\n";
        file << "    th { background-color: #f2f2f2; }\n";
        file << "    tr:nth-child(even) { background-color: #f9f9f9; }\n";
        file << "  </style>\n";
        file << "</head>\n";
        file << "<body>\n";
        file << "  <h1>" << report.title << "</h1>\n";
        file << "  <p>" << report.description << "</p>\n";
        file << "  <p>Generated: " << report.generated_time << "</p>\n";
        file << "\n";
        file << "  <h2>Executive Summary</h2>\n";
        file << "  <p>Total Tests: " << report.execution_stats.total_tests << "</p>\n";
        file << "  <p>Passed: " << report.execution_stats.passed_tests << "</p>\n";
        file << "  <p>Failed: " << report.execution_stats.failed_tests << "</p>\n";
        file << "  <p>Skipped: " << report.execution_stats.skipped_tests << "</p>\n";
        file << "  <p>Timeouts: " << report.execution_stats.timeout_tests << "</p>\n";
        file << "  <p>Success Rate: " << std::fixed << std::setprecision(2) 
             << report.execution_stats.get_pass_rate() << "%</p>\n";
        file << "\n";
        file << "  <h2>Detailed Test Results</h2>\n";
        file << "  <table>\n";
        file << "    <thead>\n";
        file << "      <tr>\n";
        file << "        <th>Test Name</th>\n";
        file << "        <th>Status</th>\n";
        file << "        <th>Category</th>\n";
        file << "        <th>Severity</th>\n";
        file << "        <th>Execution Time (s)</th>\n";
        file << "        <th>Memory Used (bytes)</th>\n";
        file << "        <th>Peak Memory (bytes)</th>\n";
        file << "        <th>Description</th>\n";
        file << "      </tr>\n";
        file << "    </thead>\n";
        file << "    <tbody>\n";
        
        for (const auto& result : report.detailed_results) {
            file << "      <tr class=\"" << get_status_class(result.status) << "\">\n";
            file << "        <td>" << result.test_name << "</td>\n";
            file << "        <td>" << get_test_status_string(result.status) << "</td>\n";
            file << "        <td>" << get_test_category_string(result.category) << "</td>\n";
            file << "        <td>" << get_test_severity_string(result.severity) << "</td>\n";
            file << "        <td>" << std::fixed << std::setprecision(3) << result.execution_time.count() << "</td>\n";
            file << "        <td>" << result.memory_used << "</td>\n";
            file << "        <td>" << result.peak_memory << "</td>\n";
            file << "        <td>" << (result.description.empty() ? "-" : result.description) << "</td>\n";
            file << "      </tr>\n";
        }
        
        file << "    </tbody>\n";
        file << "  </table>\n";
        file << "\n";
        file << "  <h2>Analysis Results</h2>\n";
        file << "  <h3>Performance Analysis</h3>\n";
        file << "  <table>\n";
        file << "    <thead>\n";
        file << "      <tr>\n";
        file << "        <th>Test Name</th>\n";
        file << "        <th>Execution Time (ms)</th>\n";
        file << "        <th>Memory Efficiency</th>\n";
        file << "        <th>Operations/sec</th>\n";
        file << "        <th>Passed</th>\n";
        file << "      </tr>\n";
        file << "    </thead>\n";
        file << "    <tbody>\n";
        
        for (const auto& perf : report.performance_summaries) {
            file << "      <tr class=\"" << (perf.performance_passed ? "passed" : "failed") << "\">\n";
            file << "        <td>" << perf.test_name << "</td>\n";
            file << "        <td>" << std::fixed << std::setprecision(3) << perf.execution_time_ms << "</td>\n";
            file << "        <td>" << std::fixed << std::setprecision(2) << perf.memory_efficiency << "%</td>\n";
            file << "        <td>" << perf.operations_per_second << "</td>\n";
            file << "        <td>" << (perf.performance_passed ? "Yes" : "No") << "</td>\n";
            file << "      </tr>\n";
        }
        
        file << "    </tbody>\n";
        file << "  </table>\n";
        file << "\n";
        file << "  <h3>Memory Analysis</h3>\n";
        file << "  <table>\n";
        file << "    <thead>\n";
        file << "      <tr>\n";
        file << "        <th>Test Name</th>\n";
        file << "        <th>Total Allocated</th>\n";
        file << "        <th>Peak Allocated</th>\n";
        file << "        <th>Current Allocated</th>\n";
        file << "        <th>Leak Count</th>\n";
        file << "        <th>Passed</th>\n";
        file << "      </tr>\n";
        file << "    </thead>\n";
        file << "    <tbody>\n";
        
        for (const auto& mem : report.memory_summaries) {
            file << "      <tr class=\"" << (mem.memory_passed ? "passed" : "failed") << "\">\n";
            file << "        <td>" << mem.test_name << "</td>\n";
            file << "        <td>" << mem.total_allocated << "</td>\n";
            file << "        <td>" << mem.peak_allocated << "</td>\n";
            file << "        <td>" << mem.current_allocated << "</td>\n";
            file << "        <td>" << mem.leak_count << "</td>\n";
            file << "        <td>" << (mem.memory_passed ? "Yes" : "No") << "</td>\n";
            file << "      </tr>\n";
        }
        
        file << "    </tbody>\n";
        file << "  </table>\n";
        file << "\n";
        file << "</body>\n";
        file << "</html>\n";
        
        file.close();
    }
    
    void generate_csv_report(const TestSuiteReport& report) {
        std::string filename = output_directory + "/test_report.csv";
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create CSV report file: " + filename);
        }
        
        file << "Test Name,Status,Category,Severity,Execution Time (s),Memory Used (bytes),Peak Memory (bytes),Description,Warnings\n";
        
        for (const auto& result : report.detailed_results) {
            file << "\"" << escape_csv(result.test_name) << "\",";
            file << "\"" << get_test_status_string(result.status) << "\",";
            file << "\"" << get_test_category_string(result.category) << "\",";
            file << "\"" << get_test_severity_string(result.severity) << "\",";
            file << std::fixed << std::setprecision(3) << result.execution_time.count() << ",";
            file << result.memory_used << ",";
            file << result.peak_memory << ",";
            file << "\"" << escape_csv(result.description) << "\",";
            
            if (!result.warnings.empty()) {
                file << "\"";
                for (size_t i = 0; i < result.warnings.size(); ++i) {
                    if (i > 0) file << "; ";
                    file << escape_csv(result.warnings[i]);
                }
                file << "\"";
            }
            file << "\n";
        }
        
        file.close();
    }
    
    void generate_performance_report(const TestSuiteReport& report) {
        std::string filename = output_directory + "/performance_report.json";
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create performance report file: " + filename);
        }
        
        file << "{\n";
        file << "  \"report_type\": \"performance\",\n";
        file << "  \"generated_time\": \"" << report.generated_time << "\",\n";
        file << "  \"test_count\": " << report.performance_summaries.size() << ",\n";
        file << "  \"performance_summary\": [\n";
        
        for (size_t i = 0; i < report.performance_summaries.size(); ++i) {
            const auto& perf = report.performance_summaries[i];
            file << "    {\n";
            file << "      \"test_name\": \"" << perf.test_name << "\",\n";
            file << "      \"execution_time_ms\": " << perf.execution_time_ms << ",\n";
            file << "      \"memory_efficiency\": " << perf.memory_efficiency << ",\n";
            file << "      \"operations_per_second\": " << perf.operations_per_second << ",\n";
            file << "      \"passed\": " << (perf.performance_passed ? "true" : "false") << ",\n";
            if (!perf.performance_issues.empty()) {
                file << "      \"issues\": [";
                for (size_t j = 0; j < perf.performance_issues.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escape_json(perf.performance_issues[j]) << "\"";
                }
                file << "],\n";
            }
            file << "      \"threshold_met\": " << (perf.execution_time_ms < 1000.0 ? "true" : "false") << "\n";
            
            if (i < report.performance_summaries.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
    }
    
    void generate_memory_report(const TestSuiteReport& report) {
        std::string filename = output_directory + "/memory_report.json";
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create memory report file: " + filename);
        }
        
        file << "{\n";
        file << "  \"report_type\": \"memory\",\n";
        file << "  \"generated_time\": \"" << report.generated_time << "\",\n";
        file << "  \"test_count\": " << report.memory_summaries.size() << ",\n";
        file << "  \"memory_summary\": [\n";
        
        for (size_t i = 0; i < report.memory_summaries.size(); ++i) {
            const auto& mem = report.memory_summaries[i];
            file << "    {\n";
            file << "      \"test_name\": \"" << mem.test_name << "\",\n";
            file << "      \"total_allocated\": " << mem.total_allocated << ",\n";
            file << "      \"peak_allocated\": " << mem.peak_allocated << ",\n";
            file << "      \"current_allocated\": " << mem.current_allocated << ",\n";
            file << "      \"leak_count\": " << mem.leak_count << ",\n";
            file << "      \"memory_passed\": " << (mem.memory_passed ? "true" : "false") << ",\n";
            if (!mem.leak_descriptions.empty()) {
                file << "      \"leak_descriptions\": [";
                for (size_t j = 0; j < mem.leak_descriptions.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escape_json(mem.leak_descriptions[j]) << "\"";
                }
                file << "],\n";
            }
            file << "      \"memory_efficiency\": " << (mem.total_allocated > 0 ? 
                (static_cast<double>(mem.current_allocated) / mem.total_allocated * 100.0) : 0.0) << "\n";
            
            if (i < report.memory_summaries.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
    }
    
    void generate_integration_report(const TestSuiteReport& report) {
        std::string filename = output_directory + "/integration_report.json";
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create integration report file: " + filename);
        }
        
        file << "{\n";
        file << "  \"report_type\": \"integration\",\n";
        file << "  \"generated_time\": \"" << report.generated_time << "\",\n";
        file << "  \"test_count\": " << report.integration_summaries.size() << ",\n";
        file << "  \"integration_summary\": [\n";
        
        for (size_t i = 0; i < report.integration_summaries.size(); ++i) {
            const auto& integration = report.integration_summaries[i];
            file << "    {\n";
            file << "      \"test_name\": \"" << integration.test_name << "\",\n";
            file << "      \"components_tested\": [";
            for (size_t j = 0; j < integration.components_tested.size(); ++j) {
                if (j > 0) file << ", ";
                file << "\"" << escape_json(integration.components_tested[j]) << "\"";
            }
            file << "],\n";
            file << "      \"integration_points\": [";
            for (size_t j = 0; j < integration.integration_points.size(); ++j) {
                if (j > 0) file << ", ";
                file << "\"" << escape_json(integration.integration_points[j]) << "\"";
            }
            file << "],\n";
            file << "      \"failed_integration_points\": [";
            for (size_t j = 0; j < integration.failed_integration_points.size(); ++j) {
                if (j > 0) file << ", ";
                file << "\"" << escape_json(integration.failed_integration_points[j]) << "\"";
            }
            file << "],\n";
            file << "      \"average_interaction_time\": " << integration.average_interaction_time << ",\n";
            file << "      \"integration_passed\": " << (integration.integration_passed ? "true" : "false") << "\n";
            
            if (i < report.integration_summaries.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
    }
    
    void generate_stress_report(const TestSuiteReport& report) {
        std::string filename = output_directory + "/stress_report.json";
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create stress report file: " + filename);
        }
        
        file << "{\n";
        file << "  \"report_type\": \"stress\",\n";
        file << "  \"generated_time\": \"" << report.generated_time << "\",\n";
        file << "  \"test_count\": " << report.stress_summaries.size() << ",\n";
        file << "  \"stress_summary\": [\n";
        
        for (size_t i = 0; i < report.stress_summaries.size(); ++i) {
            const auto& stress = report.stress_summaries[i];
            file << "    {\n";
            file << "      \"test_name\": \"" << stress.test_name << "\",\n";
            file << "      \"test_duration_seconds\": " << stress.test_duration.count() << ",\n";
            file << "      \"target_load\": " << stress.target_load << ",\n";
            file << "      \"actual_load\": " << stress.actual_load << ",\n";
            file << "      \"stability_score\": " << stress.stability_score << ",\n";
            file << "      \"stress_passed\": " << (stress.stress_passed ? "true" : "false") << ",\n";
            if (!stress.stress_issues.empty()) {
                file << "      \"issues\": [";
                for (size_t j = 0; j < stress.stress_issues.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escape_json(stress.stress_issues[j]) << "\"";
                }
                file << "],\n";
            }
            file << "      \"load_efficiency\": " << (stress.actual_load / stress.target_load * 100.0) << "\n";
            
            if (i < report.stress_summaries.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
    }
    
    void generate_regression_report(const TestSuiteReport& report) {
        std::string filename = output_directory + "/regression_report.json";
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create regression report file: " + filename);
        }
        
        file << "{\n";
        file << "  \"report_type\": \"regression\",\n";
        file << "  \"generated_time\": \"" << report.generated_time << "\",\n";
        file << "  \"test_count\": " << report.regression_summaries.size() << ",\n";
        file << "  \"regression_summary\": [\n";
        
        for (size_t i = 0; i < report.regression_summaries.size(); ++i) {
            const auto& regression = report.regression_summaries[i];
            file << "    {\n";
            file << "      \"test_name\": \"" << regression.test_name << "\",\n";
            file << "      \"baseline_version\": \"" << regression.baseline_version << "\",\n";
            file << "      \"current_version\": \"" << regression.current_version << "\",\n";
            file << "      \"tolerance_value\": " << regression.tolerance_value << ",\n";
            file << "      \"regression_passed\": " << (regression.regression_passed ? "true" : "false") << ",\n";
            if (!regression.regression_detected.empty()) {
                file << "      \"regression_detected\": [";
                for (size_t j = 0; j < regression.regression_detected.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escape_json(regression.regression_detected[j]) << "\"";
                }
                file << "],\n";
            }
            if (!regression.regression_fixed.empty()) {
                file << "      \"regression_fixed\": [";
                for (size_t j = 0; j < regression.regression_fixed.size(); ++j) {
                    if (j > 0) file << ", ";
                    file << "\"" << escape_json(regression.regression_fixed[j]) << "\"";
                }
                file << "],\n";
            }
            file << "      \"change_percentage\": " << (regression.tolerance_value * 100.0) << "\n";
            
            if (i < report.regression_summaries.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
    }
    
    void generate_platform_report(const TestSuiteReport& report) {
        std::string filename = output_directory + "/platform_report.json";
        std::ofstream file(filename);
        if (!file.is_open()) {
            throw std::runtime_error("Cannot create platform report file: " + filename);
        }
        
        file << "{\n";
        file << "  \"report_type\": \"platform\",\n";
        file << "  \"generated_time\": \"" << report.generated_time << "\",\n";
        file << "  \"test_count\": " << report.platform_summaries.size() << ",\n";
        file << "  \"platform_summary\": [\n";
        
        for (size_t i = 0; i < report.platform_summaries.size(); ++i) {
            const auto& platform = report.platform_summaries[i];
            file << "    {\n";
            file << "      \"test_name\": \"" << platform.test_name << "\",\n";
            file << "      \"platforms_tested\": [";
            for (size_t j = 0; j < platform.platforms_tested.size(); ++j) {
                if (j > 0) file << ", ";
                file << "\"" << escape_json(platform.platforms_tested[j]) << "\"";
            }
            file << "],\n";
            file << "      \"platforms_failed\": [";
            for (size_t j = 0; j < platform.platforms_failed.size(); ++j) {
                if (j > 0) file << ", ";
                file << "\"" << escape_json(platform.platforms_failed[j]) << "\"";
            }
            file << "],\n";
            file << "      \"platform_consistency\": " << (platform.platform_consistency ? "true" : "false") << "\n";
            
            if (i < report.platform_summaries.size() - 1) {
                file << "    },\n";
            } else {
                file << "    }\n";
            }
        }
        
        file << "  ]\n";
        file << "}\n";
        
        file.close();
    }
    
    std::string get_test_status_string(TestStatus status) {
        switch (status) {
            case TestStatus::NOT_STARTED: return "NOT_STARTED";
            case TestStatus::RUNNING: return "RUNNING";
            case TestStatus::PASSED: return "PASSED";
            case TestStatus::FAILED: return "FAILED";
            case TestStatus::SKIPPED: return "SKIPPED";
            case TestStatus::TIMEOUT: return "TIMEOUT";
            case TestStatus::ERROR: return "ERROR";
            default: return "UNKNOWN";
        }
    }
    
    std::string get_test_category_string(TestResultCategory category) {
        switch (category) {
            case TestResultCategory::PASSED: return "PASSED";
            case TestResultCategory::FAILED: return "FAILED";
            case TestResultCategory::SKIPPED: return "SKIPPED";
            case TestResultCategory::TIMEOUT: return "TIMEOUT";
            case TestResultCategory::ERROR: return "ERROR";
            case TestResultCategory::MEMORY_LEAK: return "MEMORY_LEAK";
            case TestResultCategory::PERFORMANCE: return "PERFORMANCE";
            case TestResultCategory::INTEGRATION: return "INTEGRATION";
            case TestResultCategory::STRESS: return "STRESS";
            case TestResultCategory::REGRESSION: return "REGRESSION";
            case TestResultCategory::CROSS_PLATFORM: return "CROSS_PLATFORM";
            default: return "UNKNOWN";
        }
    }
    
    std::string get_test_severity_string(TestSeverity severity) {
        switch (severity) {
            case TestSeverity::INFO: return "INFO";
            case TestSeverity::WARNING: return "WARNING";
            case TestSeverity::ERROR: return "ERROR";
            case TestSeverity::CRITICAL: return "CRITICAL";
            default: return "UNKNOWN";
        }
    }
    
    std::string get_status_class(TestStatus status) {
        switch (status) {
            case TestStatus::PASSED: return "passed";
            case TestStatus::FAILED: return "failed";
            case TestStatus::SKIPPED: return "skipped";
            case TestStatus::TIMEOUT: return "timeout";
            case TestStatus::ERROR: return "error";
            default: return "";
        }
    }
    
    std::string escape_json(const std::string& input) {
        std::string output = input;
        std::replace(output.begin(), output.end(), '\\', "\\\\");
        std::replace(output.begin(), output.end(), '"', "\\\"");
        std::replace(output.begin(), output.end(), '\b', "\\b");
        std::replace(output.begin(), output.end(), '\f', "\\f");
        std::replace(output.begin(), output.end(), '\n', "\\n");
        std::replace(output.begin(), output.end(), '\r', "\\r");
        std::replace(output.begin(), output.end(), '\t', "\\t");
        return output;
    }
    
    std::string escape_xml(const std::string& input) {
        std::string output = input;
        std::replace(output.begin(), output.end(), '&', "&amp;");
        std::replace(output.begin(), output.end(), '<', "&lt;");
        std::replace(output.begin(), output.end(), '>', "&gt;");
        std::replace(output.begin(), output.end(), '"', "&quot;");
        std::replace(output.begin(), output.end(), "'", "&apos;");
        return output;
    }
    
    std::string escape_csv(const std::string& input) {
        std::string output = input;
        if (output.find(',') != std::string::npos || output.find('"') != std::string::npos || output.find('\n') != std::string::npos) {
            output = "\"" + output + "\"";
            std::replace(output.begin(), output.end(), '"', "\"\"");
        }
        return output;
    }
    
    std::string get_time_string(const std::chrono::time_point<std::chrono::system_clock>& time_point) {
        auto time_t = std::chrono::system_clock::to_time_t(time_point);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};