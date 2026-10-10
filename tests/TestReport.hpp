// TestReport.hpp - Test reporting and analysis
#pragma once

#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <iomanip>
#include <chrono>
#include <ctime>
#include <sstream>
#include <algorithm>

// Test report data structures
struct TestReport {
    std::string timestamp;
    std::string testSuite;
    std::string testRunId;
    std::string environmentInfo;
    
    struct TestCaseReport {
        std::string name;
        std::string category;
        std::string status;
        std::string errorMessage;
        double executionTimeMs;
        size_t memoryUsageBytes;
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        bool isCritical;
        
        TestCaseReport(const std::string& n, const std::string& cat) 
            : name(n), category(cat), status("Pending"), executionTimeMs(0.0), 
              memoryUsageBytes(0), isCritical(false) {}
    };
    
    struct Summary {
        int totalTests = 0;
        int passed = 0;
        int failed = 0;
        int skipped = 0;
        int blocked = 0;
        double totalTimeMs = 0.0;
        size_t totalMemoryUsage = 0;
        int criticalFailed = 0;
        int nonCriticalFailed = 0;
        
        double getPassRate() const {
            return totalTests > 0 ? (static_cast<double>(passed) / totalTests) * 100.0 : 0.0;
        }
        
        std::string getPassRateString() const {
            std::stringstream ss;
            ss << std::fixed << std::setprecision(2) << getPassRate();
            return ss.str();
        }
    };
    
    TestReport(const std::string& suite, const std::string& runId)
        : testSuite(suite), testRunId(runId) {
        auto now = std::chrono::system_clock::now();
        auto time_t_now = std::chrono::system_clock::to_time_t(now);
        timestamp = std::ctime(&time_t_now);
        timestamp.erase(timestamp.find_last_not_of("\n\r") + 1);
        
        // Get environment info
        std::stringstream envInfo;
        envInfo << "NLM Version: 0.1.0\n";
        envInfo << "Compiler: " << __VERSION__ << "\n";
        envInfo << "Standard: C++" << __cplusplus << "\n";
        environmentInfo = envInfo.str();
    }
    
    // Add test case to report
    void addTestCase(const TestCaseReport& testCase) {
        testCases.push_back(testCase);
    }
    
    // Generate summary statistics
    void generateSummary() {
        summary.totalTests = testCases.size();
        for (const auto& testCase : testCases) {
            if (testCase.status == "Passed") {
                summary.passed++;
            } else if (testCase.status == "Failed") {
                summary.failed++;
                if (testCase.isCritical) {
                    summary.criticalFailed++;
                } else {
                    summary.nonCriticalFailed++;
                }
            } else if (testCase.status == "Skipped") {
                summary.skipped++;
            } else if (testCase.status == "Blocked") {
                summary.blocked++;
            }
            
            summary.totalTimeMs += testCase.executionTimeMs;
            summary.totalMemoryUsage += testCase.memoryUsageBytes;
        }
    }
    
    // Generate HTML report
    std::string generateHTMLReport() const {
        std::stringstream html;
        
        html << "<!DOCTYPE html>\n";
        html << "<html><head><title>NLM Test Report</title></head><body>\n";
        html << "<h1>NLM Test Report</h1>\n";
        html << "<h2>Summary</h2>\n";
        html << "<p>Total Tests: " << summary.totalTests << "</p>\n";
        html << "<p>Passed: " << summary.passed << "</p>\n";
        html << "<p>Failed: " << summary.failed << "</p>\n";
        html << "<p>Skipped: " << summary.skipped << "</p>\n";
        html << "<p>Blocked: " << summary.blocked << "</p>\n";
        html << "<p>Pass Rate: " << summary.getPassRateString() << "%</p>\n";
        html << "<p>Total Time: " << summary.totalTimeMs << " ms</p>\n";
        html << "<p>Total Memory Usage: " << summary.totalMemoryUsage << " bytes</p>\n";
        
        html << "<h2>Test Cases</h2>\n";
        html << "<table border='1'>\n";
        html << "<tr><th>Name</th><th>Category</th><th>Status</th><th>Execution Time (ms)</th><th>Memory (bytes)</th><th>Error</th></tr>\n";
        
        for (const auto& testCase : testCases) {
            html << "<tr>";
            html << "<td>" << testCase.name << "</td>";
            html << "<td>" << testCase.category << "</td>";
            html << "<td>" << testCase.status << "</td>";
            html << "<td>" << testCase.executionTimeMs << "</td>";
            html << "<td>" << testCase.memoryUsageBytes << "</td>";
            html << "<td>" << testCase.errorMessage << "</td>";
            html << "</tr>\n";
        }
        
        html << "</table>\n";
        html << "<h2>Environment Information</h2>\n";
        html << "<pre>" << environmentInfo << "</pre>\n";
        html << "</body></html>\n";
        
        return html.str();
    }
    
    // Generate XML report
    std::string generateXMLReport() const {
        std::stringstream xml;
        
        xml << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
        xml << "<test-report>\n";
        xml << "<timestamp>" << timestamp << "</timestamp>\n";
        xml << "<test-suite>" << testSuite << "</test-suite>\n";
        xml << "<test-run-id>" << testRunId << "</test-run-id>\n";
        xml << "<environment>" << environmentInfo << "</environment>\n";
        
        xml << "<summary>\n";
        xml << "<total-tests>" << summary.totalTests << "</total-tests>\n";
        xml << "<passed>" << summary.passed << "</passed>\n";
        xml << "<failed>" << summary.failed << "</failed>\n";
        xml << "<skipped>" << summary.skipped << "</skipped>\n";
        xml << "<blocked>" << summary.blocked << "</blocked>\n";
        xml << "<total-time-ms>" << summary.totalTimeMs << "</total-time-ms>\n";
        xml << "<total-memory-bytes>" << summary.totalMemoryUsage << "</total-memory-bytes>\n";
        xml << "<critical-failed>" << summary.criticalFailed << "</critical-failed>\n";
        xml << "<non-critical-failed>" << summary.nonCriticalFailed << "</non-critical-failed>\n";
        xml << "<pass-rate>" << summary.getPassRateString() << "</pass-rate>\n";
        xml << "</summary>\n";
        
        xml << "<test-cases>\n";
        for (const auto& testCase : testCases) {
            xml << "<test-case>\n";
            xml << "<name>" << testCase.name << "</name>\n";
            xml << "<category>" << testCase.category << "</category>\n";
            xml << "<status>" << testCase.status << "</status>\n";
            xml << "<execution-time-ms>" << testCase.executionTimeMs << "</execution-time-ms>\n";
            xml << "<memory-bytes>" << testCase.memoryUsageBytes << "</memory-bytes>\n";
            xml << "<error>" << testCase.errorMessage << "</error>\n";
            xml << "<critical>" << (testCase.isCritical ? "true" : "false") << "</critical>\n";
            xml << "</test-case>\n";
        }
        
        xml << "</test-cases>\n";
        xml << "</test-report>\n";
        
        return xml.str();
    }
    
    // Write report to file
    bool writeReport(const std::string& filename, const std::string& format = "html") const {
        std::ofstream file(filename);
        if (!file) {
            return false;
        }
        
        if (format == "html") {
            file << generateHTMLReport();
        } else if (format == "xml") {
            file << generateXMLReport();
        } else {
            file << "Unsupported format: " << format << std::endl;
            return false;
        }
        
        file.close();
        return true;
    }
    
    // Generate performance analysis
    std::string generatePerformanceAnalysis() const {
        std::stringstream analysis;
        analysis << "Performance Analysis Report\n";
        analysis << "===========================\n\n";
        
        if (testCases.empty()) {
            analysis << "No test cases to analyze.\n";
            return analysis.str();
        }
        
        analysis << "Slowest tests (top 10):\n";
        std::vector<const TestCaseReport*> sortedTests;
        for (const auto& testCase : testCases) {
            sortedTests.push_back(&testCase);
        }
        
        std::sort(sortedTests.begin(), sortedTests.end(),
                  [](const TestCaseReport* a, const TestCaseReport* b) {
                      return a->executionTimeMs > b->executionTimeMs;
                  });
        
        int count = 0;
        for (const auto* testCase : sortedTests) {
            if (count >= 10) break;
            analysis << testCase->name << ": " << testCase->executionTimeMs << " ms\n";
            count++;
        }
        
        analysis << "\n\nFastest tests (top 10):\n";
        std::vector<const TestCaseReport*> fastestTests = sortedTests;
        std::reverse(fastestTests.begin(), fastestTests.end());
        count = 0;
        for (const auto* testCase : fastestTests) {
            if (count >= 10) break;
            analysis << testCase->name << ": " << testCase->executionTimeMs << " ms\n";
            count++;
        }
        
        analysis << "\n\nMemory usage analysis:\n";
        std::map<std::string, size_t> memoryByCategory;
        for (const auto& testCase : testCases) {
            memoryByCategory[testCase.category] += testCase.memoryUsageBytes;
        }
        
        for (const auto& pair : memoryByCategory) {
            analysis << pair.first << ": " << pair.second << " bytes\n";
        }
        
        return analysis.str();
    }
    
    // Get list of failed tests
    std::vector<std::string> getFailedTests() const {
        std::vector<std::string> failed;
        for (const auto& testCase : testCases) {
            if (testCase.status == "Failed") {
                failed.push_back(testCase.name);
            }
        }
        return failed;
    }
    
    // Get list of critical failed tests
    std::vector<std::string> getCriticalFailedTests() const {
        std::vector<std::string> criticalFailed;
        for (const auto& testCase : testCases) {
            if (testCase.status == "Failed" && testCase.isCritical) {
                criticalFailed.push_back(testCase.name);
            }
        }
        return criticalFailed;
    }
    
    // Get statistics by category
    std::map<std::string, Summary> getStatisticsByCategory() const {
        std::map<std::string, Summary> stats;
        for (const auto& testCase : testCases) {
            auto it = stats.find(testCase.category);
            if (it == stats.end()) {
                it = stats.insert(std::make_pair(testCase.category, Summary())).first;
            }
            
            auto& catSummary = it->second;
            catSummary.totalTests++;
            
            if (testCase.status == "Passed") {
                catSummary.passed++;
            } else if (testCase.status == "Failed") {
                catSummary.failed++;
            } else if (testCase.status == "Skipped") {
                catSummary.skipped++;
            } else if (testCase.status == "Blocked") {
                catSummary.blocked++;
            }
        }
        
        return stats;
    }
    
private:
    std::vector<TestCaseReport> testCases;
    Summary summary;
};