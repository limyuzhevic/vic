// NLM Testing Framework
// Test Report Generator

#ifndef TEST_REPORT_GENERATOR_HPP
#define TEST_REPORT_GENERATOR_HPP

#include "TestFramework.hpp"
#include <fstream>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>
#include <map>
#include <vector>

class TestReportGenerator {
public:
    enum class ReportFormat {
        TEXT,
        HTML,
        JSON,
        XML,
        CSV
    };
    
    enum class ReportLevel {
        SUMMARY,
        DETAILED,
        EXTENSIVE
    };
    
    struct ReportStats {
        size_t total_tests;
        size_t passed_tests;
        size_t failed_tests;
        size_t skipped_tests;
        double success_rate;
        Timestamp generation_time;
        Timestamp total_test_time;
        std::map<std::string, size_t> test_type_counts;
        std::map<std::string, double> average_performance_metrics;
    };
    
    static std::string generateReport(const TestFramework& framework, 
                                      ReportFormat format = ReportFormat::TEXT,
                                      ReportLevel level = ReportLevel::DETAILED) {
        ReportStats stats = calculateStats(framework);
        
        switch (format) {
            case ReportFormat::TEXT:
                return generateTextReport(framework, stats, level);
            case ReportFormat::HTML:
                return generateHTMLReport(framework, stats, level);
            case ReportFormat::JSON:
                return generateJSONReport(framework, stats, level);
            case ReportFormat::XML:
                return generateXMLReport(framework, stats, level);
            case ReportFormat::CSV:
                return generateCSVReport(framework, stats, level);
            default:
                return generateTextReport(framework, stats, level);
        }
    }
    
private:
    static ReportStats calculateStats(const TestFramework& framework) {
        ReportStats stats;
        stats.total_tests = framework.getTotalTests();
        stats.passed_tests = framework.getPassedTests();
        stats.failed_tests = framework.getFailedTests();
        stats.skipped_tests = framework.getSkippedTests();
        
        if (stats.total_tests > 0) {
            stats.success_rate = (stats.passed_tests * 100.0) / stats.total_tests;
        } else {
            stats.success_rate = 0.0;
        }
        
        stats.generation_time = timestamp();
        stats.total_test_time = 0; // Would need to track this in framework
        
        return stats;
    }
    
    static std::string generateTextReport(const TestFramework& framework, 
                                         const ReportStats& stats, ReportLevel level) {
        std::stringstream ss;
        ss << "========================================" << std::endl;
        ss << "          NLM COMPREHENSIVE TEST REPORT" << std::endl;
        ss << "========================================" << std::endl;
        ss << "Generated: " << getCurrentTimestamp() << std::endl;
        ss << "Framework Version: 3.0.0" << std::endl;
        ss << std::endl;
        
        ss << "=== EXECUTION SUMMARY ===" << std::endl;
        ss << "Total Tests: " << stats.total_tests << std::endl;
        ss << "Passed: " << stats.passed_tests << std::endl;
        ss << "Failed: " << stats.failed_tests << std::endl;
        ss << "Skipped: " << stats.skipped_tests << std::endl;
        ss << std::fixed << std::setprecision(2);
        ss << "Success Rate: " << stats.success_rate << "%" << std::endl;
        ss << std::endl;
        
        if (level >= ReportLevel::DETAILED) {
            ss << "=== DETAILED RESULTS ===" << std::endl;
            // Would need access to framework's test results to provide details
            ss << "(Detailed test results would appear here)" << std::endl;
            ss << std::endl;
        }
        
        if (level >= ReportLevel::EXTENSIVE) {
            ss << "=== PERFORMANCE METRICS ===" << std::endl;
            // Would need performance metrics from framework
            ss << "(Performance metrics would appear here)" << std::endl;
            ss << std::endl;
        }
        
        ss << "=== TEST TYPE BREAKDOWN ===" << std::endl;
        // Would need test type counts from framework
        ss << "(Test type breakdown would appear here)" << std::endl;
        ss << std::endl;
        
        ss << "========================================" << std::endl;
        ss << "              REPORT END" << std::endl;
        ss << "========================================" << std::endl;
        
        return ss.str();
    }
    
    static std::string generateHTMLReport(const TestFramework& framework, 
                                         const ReportStats& stats, ReportLevel level) {
        std::stringstream ss;
        ss << "<!DOCTYPE html>" << std::endl;
        ss << "<html>" << std::endl;
        ss << "<head>" << std::endl;
        ss << "    <title>NLM Comprehensive Test Report</title>" << std::endl;
        ss << "    <style>" << std::endl;
        ss << "        body { font-family: Arial, sans-serif; margin: 20px; }" << std::endl;
        ss << "        .header { background-color: #2c3e50; color: white; padding: 20px; border-radius: 5px; }" << std::endl;
        ss << "        .section { margin: 20px 0; padding: 15px; border: 1px solid #ddd; border-radius: 5px; }" << std::endl;
        ss << "        .passed { background-color: #d4edda; color: #155724; }" << std::endl;
        ss << "        .failed { background-color: #f8d7da; color: #721c24; }" << std::endl;
        ss << "        .skipped { background-color: #fff3cd; color: #856404; }" << std::endl;
        ss << "        table { border-collapse: collapse; width: 100%; }" << std::endl;
        ss << "        th, td { border: 1px solid #ddd; padding: 8px; text-align: left; }" << std::endl;
        ss << "        th { background-color: #f2f2f2; }" << std::endl;
        ss << "    </style>" << std::endl;
        ss << "</head>" << std::endl;
        ss << "<body>" << std::endl;
        
        ss << "<div class=\"header\">" << std::endl;
        ss << "    <h1>NLM Comprehensive Test Report</h1>" << std::endl;
        ss << "    <p>Generated: " << getCurrentTimestamp() << "</p>" << std::endl;
        ss << "    <p>Framework Version: 3.0.0</p>" << std::endl;
        ss << "</div>" << std::endl;
        
        ss << "<div class=\"section passed\">" << std::endl;
        ss << "    <h2>Execution Summary</h2>" << std::endl;
        ss << "    <table>" << std::endl;
        ss << "        <tr><th>Metric</th><th>Value</th></tr>" << std::endl;
        ss << "        <tr><td>Total Tests</td><td>" << stats.total_tests << "</td></tr>" << std::endl;
        ss << "        <tr><td>Passed</td><td>" << stats.passed_tests << "</td></tr>" << std::endl;
        ss << "        <tr><td>Failed</td><td>" << stats.failed_tests << "</td></tr>" << std::endl;
        ss << "        <tr><td>Skipped</td><td>" << stats.skipped_tests << "</td></tr>" << std::endl;
        ss << "        <tr><td>Success Rate</td><td>" << std::fixed << std::setprecision(2) 
           << stats.success_rate << "%</td></tr>" << std::endl;
        ss << "    </table>" << std::endl;
        ss << "</div>" << std::endl;
        
        ss << "</body>" << std::endl;
        ss << "</html>" << std::endl;
        
        return ss.str();
    }
    
    static std::string generateJSONReport(const TestFramework& framework, 
                                         const ReportStats& stats, ReportLevel level) {
        std::stringstream ss;
        ss << "{" << std::endl;
        ss << "  \"report\": {" << std::endl;
        ss << "    \"metadata\": {" << std::endl;
        ss << "      \"generated\": \"" << getCurrentTimestamp() << "\"," << std::endl;
        ss << "      \"framework_version\": \"3.0.0\"" << std::endl;
        ss << "    }," << std::endl;
        ss << "    \"summary\": {" << std::endl;
        ss << "      \"total_tests\": " << stats.total_tests << "," << std::endl;
        ss << "      \"passed_tests\": " << stats.passed_tests << "," << std::endl;
        ss << "      \"failed_tests\": " << stats.failed_tests << "," << std::endl;
        ss << "      \"skipped_tests\": " << stats.skipped_tests << "," << std::endl;
        ss << "      \"success_rate\": " << std::fixed << std::setprecision(2) 
           << stats.success_rate << std::endl;
        ss << "    }" << std::endl;
        ss << "  }" << std::endl;
        ss << "}" << std::endl;
        
        return ss.str();
    }
    
    static std::string generateXMLReport(const TestFramework& framework, 
                                         const ReportStats& stats, ReportLevel level) {
        std::stringstream ss;
        ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>" << std::endl;
        ss << "<test_report>" << std::endl;
        ss << "  <metadata>" << std::endl;
        ss << "    <generated>" << getCurrentTimestamp() << "</generated>" << std::endl;
        ss << "    <framework_version>3.0.0</framework_version>" << std::endl;
        ss << "  </metadata>" << std::endl;
        ss << "  <summary>" << std::endl;
        ss << "    <total_tests>" << stats.total_tests << "</total_tests>" << std::endl;
        ss << "    <passed_tests>" << stats.passed_tests << "</passed_tests>" << std::endl;
        ss << "    <failed_tests>" << stats.failed_tests << "</failed_tests>" << std::endl;
        ss << "    <skipped_tests>" << stats.skipped_tests << "</skipped_tests>" << std::endl;
        ss << "    <success_rate>" << std::fixed << std::setprecision(2) 
           << stats.success_rate << "</success_rate>" << std::endl;
        ss << "  </summary>" << std::endl;
        ss << "</test_report>" << std::endl;
        
        return ss.str();
    }
    
    static std::string generateCSVReport(const TestFramework& framework, 
                                         const ReportStats& stats, ReportLevel level) {
        std::stringstream ss;
        ss << "Metric,Value" << std::endl;
        ss << "Total_Tests," << stats.total_tests << std::endl;
        ss << "Passed_Tests," << stats.passed_tests << std::endl;
        ss << "Failed_Tests," << stats.failed_tests << std::endl;
        ss << "Skipped_Tests," << stats.skipped_tests << std::endl;
        ss << "Success_Rate," << std::fixed << std::setprecision(2) 
           << stats.success_rate << std::endl;
        
        return ss.str();
    }
    
    static std::string getCurrentTimestamp() {
        auto now = std::chrono::system_clock::now();
        auto time = std::chrono::system_clock::to_time_t(now);
        std::stringstream ss;
        ss << std::put_time(std::localtime(&time), "%Y-%m-%d %H:%M:%S");
        return ss.str();
    }
};

#endif // TEST_REPORT_GENERATOR_HPP