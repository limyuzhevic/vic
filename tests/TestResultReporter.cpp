// Test Result Reporter Implementation
// Comprehensive test result reporting and analysis

#include "TestResultReporter.hpp"
#include "AdvancedTestFramework.hpp"
#include <algorithm>
#include <cmath>
#include <ctime>
#include <filesystem>

namespace nlm {
namespace tests {

struct TestResultReporter::Impl {
    std::string outputDirectory;
    std::string defaultFormat;
    bool detailedReportingEnabled;
    bool performanceReportingEnabled;
    bool trendAnalysisEnabled;
    
    Impl() {
        outputDirectory = "test_results";
        defaultFormat = "html";
        detailedReportingEnabled = true;
        performanceReportingEnabled = true;
        trendAnalysisEnabled = true;
        
        // Create output directory if it doesn't exist
        std::filesystem::create_directories(outputDirectory);
    }
};

TestResultReporter::TestResultReporter() : pImpl(std::make_unique<Impl>()) {}
TestResultReporter::~TestResultReporter() = default;

void TestResultReporter::setOutputDirectory(const std::string& directory) {
    pImpl->outputDirectory = directory;
    std::filesystem::create_directories(directory);
}

void TestResultReporter::setDefaultReportFormat(const std::string& format) {
    pImpl->defaultFormat = format;
}

void TestResultReporter::enableDetailedReporting(bool enable) {
    pImpl->detailedReportingEnabled = enable;
}

void TestResultReporter::enablePerformanceReporting(bool enable) {
    pImpl->performanceReportingEnabled = enable;
}

void TestResultReporter::enableTrendAnalysis(bool enable) {
    pImpl->trendAnalysisEnabled = enable;
}

std::string TestResultReporter::generateSummaryReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "NLM Test Summary Report\n";
    ss << "======================\n\n";
    
    if (results.empty()) {
        ss << "No test results to report.\n";
        return ss.str();
    }
    
    TestStatistics stats = calculateStatistics(results);
    
    ss << "Overall Statistics:\n";
    ss << "  Total Tests: " << stats.totalTests << "\n";
    ss << "  Passed Tests: " << stats.passedTests << "\n";
    ss << "  Failed Tests: " << stats.failedTests << "\n";
    ss << "  Pass Rate: " << formatPercentage(stats.passRate) << "\n";
    ss << "  Total Execution Time: " << formatDuration(stats.totalExecutionTime) << "\n";
    ss << "  Average Execution Time: " << formatDuration(stats.averageExecutionTime) << "\n";
    ss << "  Min Execution Time: " << formatDuration(stats.minExecutionTime) << "\n";
    ss << "  Max Execution Time: " << formatDuration(stats.maxExecutionTime) << "\n\n";
    
    // Group by suite
    std::map<std::string, std::vector<TestResult>> resultsBySuite;
    for (const auto& result : results) {
        resultsBySuite[result.testSuite].push_back(result);
    }
    
    ss << "Detailed Results by Suite:\n";
    for (const auto& pair : resultsBySuite) {
        const std::string& suite = pair.first;
        const std::vector<TestResult>& suiteResults = pair.second;
        
        size_t suiteTotal = suiteResults.size();
        size_t suitePassed = 0;
        for (const auto& result : suiteResults) {
            if (result.passed) suitePassed++;
        }
        
        ss << "  " << suite << ": " << suitePassed << "/" << suiteTotal << " ("
           << formatPercentage(static_cast<double>(suitePassed) / suiteTotal * 100.0) << ")\n";
    }
    
    return ss.str();
}

std::string TestResultReporter::generateDetailedReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "NLM Detailed Test Report\n";
    ss << "========================\n\n";
    
    if (results.empty()) {
        ss << "No test results to report.\n";
        return ss.str();
    }
    
    // Sort by suite and then by name
    std::vector<TestResult> sortedResults = results;
    std::sort(sortedResults.begin(), sortedResults.end(),
              [](const TestResult& a, const TestResult& b) {
                  if (a.testSuite != b.testSuite) return a.testSuite < b.testSuite;
                  return a.testName < b.testName;
              });
    
    std::string currentSuite;
    for (const auto& result : sortedResults) {
        if (pImpl->detailedReportingEnabled && result.testSuite != currentSuite) {
            currentSuite = result.testSuite;
            ss << "=== " << currentSuite << " ===\n";
        }
        
        ss << "Test: " << result.testName << "\n";
        ss << "  Status: " << (result.passed ? "PASSED" : "FAILED") << "\n";
        ss << "  Execution Time: " << formatDuration(result.executionTime) << "\n";
        ss << "  Timeout Occurred: " << (result.timeoutOccurred ? "YES" : "NO") << "\n";
        
        if (!result.errorMessage.empty()) {
            ss << "  Error: " << result.errorMessage << "\n";
        }
        
        if (!result.metrics.empty() && pImpl->performanceReportingEnabled) {
            ss << "  Performance Metrics:\n";
            for (const auto& metric : result.metrics) {
                ss << "    " << metric.first << ": " << metric.second << "\n";
            }
        }
        
        ss << "\n";
    }
    
    return ss.str();
}

std::string TestResultReporter::generateHTMLReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "<!DOCTYPE html>\n";
    ss << "<html>\n";
    ss << "<head>\n";
    ss << "  <title>NLM Test Results Report</title>\n";
    ss << "  <style>\n";
    ss << "    body { font-family: Arial, sans-serif; margin: 20px; }\n";
    ss << "    h1 { color: #333; }\n";
    ss << "    h2 { color: #666; border-bottom: 2px solid #ccc; padding-bottom: 5px; }\n";
    ss << "    .passed { background-color: #d4edda; color: #155724; padding: 5px; border-radius: 3px; }\n";
    ss << "    .failed { background-color: #f8d7da; color: #721c24; padding: 5px; border-radius: 3px; }\n";
    ss << "    .suite { font-weight: bold; margin-top: 20px; }\n";
    ss << "    .test { margin-left: 20px; padding: 5px; border-left: 1px solid #ddd; }\n";
    ss << "    .metric { margin-left: 40px; color: #666; font-size: 0.9em; }\n";
    ss << "  </style>\n";
    ss << "</head>\n";
    ss << "<body>\n";
    ss << "  <h1>NLM Test Results Report</h1>\n";
    ss << "  <p>Generated on: " << getCurrentTimestamp() << "</p>\n";
    
    TestStatistics stats = calculateStatistics(results);
    
    ss << "  <h2>Summary</h2>\n";
    ss << "  <table border=\"1\" cellpadding=\"5\" cellspacing=\"0\">\n";
    ss << "    <tr><th>Metric</th><th>Value</th></tr>\n";
    ss << "    <tr><td>Total Tests</td><td>" << stats.totalTests << "</td></tr>\n";
    ss << "    <tr><td>Passed Tests</td><td class=\"passed\">" << stats.passedTests << "</td></tr>\n";
    ss << "    <tr><td>Failed Tests</td><td class=\"failed\">" << stats.failedTests << "</td></tr>\n";
    ss << "    <tr><td>Pass Rate</td><td>" << formatPercentage(stats.passRate) << "</td></tr>\n";
    ss << "    <tr><td>Total Execution Time</td><td>" << formatDuration(stats.totalExecutionTime) << "</td></tr>\n";
    ss << "    <tr><td>Average Execution Time</td><td>" << formatDuration(stats.averageExecutionTime) << "</td></tr>\n";
    ss << "  </table>\n";
    
    // Group by suite
    std::map<std::string, std::vector<TestResult>> resultsBySuite;
    for (const auto& result : results) {
        resultsBySuite[result.testSuite].push_back(result);
    }
    
    ss << "  <h2>Detailed Results by Suite</h2>\n";
    for (const auto& pair : resultsBySuite) {
        const std::string& suite = pair.first;
        const std::vector<TestResult>& suiteResults = pair.second;
        
        ss << "  <h3 class=\"suite\">" << suite << "</h3>\n";
        
        ss << "  <table border=\"1\" cellpadding=\"5\" cellspacing=\"0\">\n";
        ss << "    <tr><th>Test Name</th><th>Status</th><th>Execution Time</th><th>Timeout</th><th>Error</th></tr>\n";
        
        for (const auto& result : suiteResults) {
            ss << "    <tr class=\"test\">\n";
            ss << "      <td>" << escapeHTML(result.testName) << "</td>\n";
            ss << "      <td class=\"" << (result.passed ? "passed" : "failed") << "\">"
               << (result.passed ? "PASSED" : "FAILED") << "</td>\n";
            ss << "      <td>" << formatDuration(result.executionTime) << "</td>\n";
            ss << "      <td>" << (result.timeoutOccurred ? "YES" : "NO") << "</td>\n";
            ss << "      <td>" << escapeHTML(result.errorMessage) << "</td>\n";
            ss << "    </tr>\n";
            
            if (!result.metrics.empty() && pImpl->performanceReportingEnabled) {
                ss << "    <tr><td colspan=\"5\">\n";
                ss << "      <div class=\"metrics\">\n";
                for (const auto& metric : result.metrics) {
                    ss << "        <div class=\"metric\">" << metric.first << ": " << metric.second << "</div>\n";
                }
                ss << "      </div>\n";
                ss << "    </td></tr>\n";
            }
        }
        
        ss << "  </table>\n";
    }
    
    ss << "</body>\n";
    ss << "</html>\n";
    
    return ss.str();
}

std::string TestResultReporter::generateJSONReport(const std::vector<TestResult>& results) const {
    nlohmann::json j;
    
    j["report_metadata"]["generated_at"] = getCurrentTimestamp();
    j["report_metadata"]["format"] = "json";
    
    TestStatistics stats = calculateStatistics(results);
    
    j["summary"]["total_tests"] = stats.totalTests;
    j["summary"]["passed_tests"] = stats.passedTests;
    j["summary"]["failed_tests"] = stats.failedTests;
    j["summary"]["pass_rate"] = stats.passRate;
    j["summary"]["total_execution_time_ms"] = stats.totalExecutionTime.count();
    j["summary"]["average_execution_time_ms"] = stats.averageExecutionTime.count();
    
    // Group by suite
    std::map<std::string, std::vector<TestResult>> resultsBySuite;
    for (const auto& result : results) {
        resultsBySuite[result.testSuite].push_back(result);
    }
    
    j["suites"] = nlohmann::json::array();
    for (const auto& pair : resultsBySuite) {
        nlohmann::json suiteJson;
        suiteJson["name"] = pair.first;
        
        suiteJson["tests"] = nlohmann::json::array();
        for (const auto& result : pair.second) {
            nlohmann::json testJson;
            testJson["test_name"] = result.testName;
            testJson["passed"] = result.passed;
            testJson["execution_time_ms"] = result.executionTime.count();
            testJson["timeout_occurred"] = result.timeoutOccurred;
            testJson["error_message"] = result.errorMessage;
            testJson["metrics"] = result.metrics;
            
            suiteJson["tests"].push_back(testJson);
        }
        
        j["suites"].push_back(suiteJson);
    }
    
    return j.dump(2);
}

std::string TestResultReporter::generateXMLReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<test_results generated_at=\"" << getCurrentTimestamp() << "\">\n";
    
    TestStatistics stats = calculateStatistics(results);
    
    ss << "  <summary>\n";
    ss << "    <total_tests>" << stats.totalTests << "</total_tests>\n";
    ss << "    <passed_tests>" << stats.passedTests << "</passed_tests>\n";
    ss << "    <failed_tests>" << stats.failedTests << "</failed_tests>\n";
    ss << "    <pass_rate>" << stats.passRate << "</pass_rate>\n";
    ss << "    <total_execution_time_ms>" << stats.totalExecutionTime.count() << "</total_execution_time_ms>\n";
    ss << "  </summary>\n";
    
    // Group by suite
    std::map<std::string, std::vector<TestResult>> resultsBySuite;
    for (const auto& result : results) {
        resultsBySuite[result.testSuite].push_back(result);
    }
    
    ss << "  <suites>\n";
    for (const auto& pair : resultsBySuite) {
        const std::string& suite = pair.first;
        ss << "    <suite name=\"" << escapeXML(suite) << "\">\n";
        
        for (const auto& result : pair.second) {
            ss << "      <test name=\"" << escapeXML(result.testName) << "\" passed=\""
               << (result.passed ? "true" : "false") << "\" timeout_occurred=\""
               << (result.timeoutOccurred ? "true" : "false") << "\">\n";
            ss << "        <execution_time_ms>" << result.executionTime.count() << "</execution_time_ms>\n";
            if (!result.errorMessage.empty()) {
                ss << "        <error>" << escapeXML(result.errorMessage) << "</error>\n";
            }
            ss << "      </test>\n";
        }
        
        ss << "    </suite>\n";
    }
    
    ss << "  </suites>\n";
    ss << "</test_results>\n";
    
    return ss.str();
}

std::string TestResultReporter::generateCSVReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "TestSuite,TestName,Passed,ExecutionTimeMs,TimeoutOccurred,ErrorMessage\n";
    
    for (const auto& result : results) {
        std::string suite = result.testSuite;
        std::string name = result.testName;
        std::string error = result.errorMessage;
        
        // Escape quotes and newlines
        auto escape = [](std::string& str) {
            std::replace(str.begin(), str.end(), '"', '"');
            std::replace(str.begin(), str.end(), '\n', ' ');
            std::replace(str.begin(), str.end(), '\r', ' ');
        };
        
        escape(suite);
        escape(name);
        escape(error);
        
        ss << "\"" << suite << "\",\"" << name << "\","
           << (result.passed ? "true" : "false") << "," << result.executionTime.count() << "," 
           << (result.timeoutOccurred ? "true" : "false") << ",\"" << error << "\"\n";
    }
    
    return ss.str();
}

bool TestResultReporter::exportToFile(const std::vector<TestResult>& results, const std::string& filename,
                                      const std::string& format) {
    std::string actualFormat = format;
    if (actualFormat == "auto") {
        // Determine format from file extension
        if (filename.size() >= 5 && filename.substr(filename.size() - 5) == ".html") {
            actualFormat = "html";
        } else if (filename.size() >= 5 && filename.substr(filename.size() - 5) == ".json") {
            actualFormat = "json";
        } else if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".xml") {
            actualFormat = "xml";
        } else if (filename.size() >= 4 && filename.substr(filename.size() - 4) == ".csv") {
            actualFormat = "csv";
        } else {
            actualFormat = pImpl->defaultFormat;
        }
    }
    
    if (actualFormat == "html") {
        return exportToHTML(results, filename);
    } else if (actualFormat == "json") {
        return exportToJSON(results, filename);
    } else if (actualFormat == "xml") {
        return exportToXML(results, filename);
    } else if (actualFormat == "csv") {
        return exportToCSV(results, filename);
    }
    
    return false;
}

bool TestResultReporter::exportToHTML(const std::vector<TestResult>& results, const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateHTMLReport(results);
        file.close();
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool TestResultReporter::exportToJSON(const std::vector<TestResult>& results, const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateJSONReport(results);
        file.close();
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool TestResultReporter::exportToXML(const std::vector<TestResult>& results, const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateXMLReport(results);
        file.close();
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

bool TestResultReporter::exportToCSV(const std::vector<TestResult>& results, const std::string& filename) {
    try {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateCSVReport(results);
        file.close();
        return true;
        
    } catch (const std::exception&) {
        return false;
    }
}

TestStatistics TestResultReporter::calculateStatistics(const std::vector<TestResult>& results) const {
    TestStatistics stats;
    
    if (results.empty()) {
        return stats;
    }
    
    stats.totalTests = results.size();
    stats.passedTests = 0;
    stats.failedTests = 0;
    stats.totalExecutionTime = std::chrono::milliseconds(0);
    stats.minExecutionTime = std::chrono::milliseconds(std::numeric_limits<long long>::max());
    stats.maxExecutionTime = std::chrono::milliseconds(0);
    
    for (const auto& result : results) {
        if (result.passed) {
            stats.passedTests++;
        } else {
            stats.failedTests++;
        }
        
        stats.totalExecutionTime += result.executionTime;
        
        if (result.executionTime < stats.minExecutionTime) {
            stats.minExecutionTime = result.executionTime;
        }
        
        if (result.executionTime > stats.maxExecutionTime) {
            stats.maxExecutionTime = result.executionTime;
        }
    }
    
    if (stats.totalTests > 0) {
        stats.passRate = static_cast<double>(stats.passedTests) / stats.totalTests * 100.0;
        stats.averageExecutionTime = stats.totalExecutionTime / stats.totalTests;
    }
    
    return stats;
}

std::map<std::string, TestTrend> TestResultReporter::analyzeTrends(const std::vector<TestResult>& results) const {
    std::map<std::string, TestTrend> trends;
    
    if (!pImpl->trendAnalysisEnabled) {
        return trends;
    }
    
    // Group by suite
    std::map<std::string, std::vector<TestResult>> resultsBySuite;
    for (const auto& result : results) {
        resultsBySuite[result.testSuite].push_back(result);
    }
    
    for (const auto& pair : resultsBySuite) {
        const std::string& suite = pair.first;
        const std::vector<TestResult>& suiteResults = pair.second;
        
        // Analyze trend for this suite
        if (suiteResults.size() < 2) {
            trends[suite] = TestTrend::UNKNOWN;
            continue;
        }
        
        // Calculate pass rate trend
        std::vector<double> passRates;
        size_t window = std::min<size_t>(3, suiteResults.size());
        
        for (size_t i = 0; i <= suiteResults.size() - window; ++i) {
            size_t passed = 0;
            for (size_t j = 0; j < window; ++j) {
                if (i + j < suiteResults.size() && suiteResults[i + j].passed) {
                    passed++;
                }
            }
            passRates.push_back(static_cast<double>(passed) / window * 100.0);
        }
        
        if (passRates.size() < 2) {
            trends[suite] = TestTrend::UNKNOWN;
            continue;
        }
        
        // Determine trend
        bool increasing = true;
        bool decreasing = true;
        
        for (size_t i = 1; i < passRates.size(); ++i) {
            if (passRates[i] > passRates[i - 1]) {
                decreasing = false;
            } else if (passRates[i] < passRates[i - 1]) {
                increasing = false;
            } else {
                increasing = false;
                decreasing = false;
            }
        }
        
        if (increasing) {
            trends[suite] = TestTrend::INCREASING;
        } else if (decreasing) {
            trends[suite] = TestTrend::DECREASING;
        } else if (!increasing && !decreasing) {
            trends[suite] = TestTrend::FLUCTUATING;
        } else {
            trends[suite] = TestTrend::STABLE;
        }
    }
    
    return trends;
}

std::vector<PerformanceMetrics> TestResultReporter::extractPerformanceMetrics(const std::vector<TestResult>& results) const {
    std::vector<PerformanceMetrics> metrics;
    
    for (const auto& result : results) {
        PerformanceMetrics metric(result.testName, result.testSuite, result.executionTime);
        
        // Copy custom metrics
        metric.customMetrics = result.metrics;
        
        metrics.push_back(metric);
    }
    
    return metrics;
}

std::map<std::string, double> TestResultReporter::calculateAverageTimes(const std::vector<TestResult>& results) const {
    std::map<std::string, double> averages;
    
    // Group by suite
    std::map<std::string, std::vector<std::chrono::milliseconds>> timesBySuite;
    for (const auto& result : results) {
        timesBySuite[result.testSuite].push_back(result.executionTime);
    }
    
    for (const auto& pair : timesBySuite) {
        const std::string& suite = pair.first;
        const std::vector<std::chrono::milliseconds>& times = pair.second;
        
        if (times.empty()) {
            continue;
        }
        
        double sum = 0.0;
        for (const auto& time : times) {
            sum += time.count();
        }
        
        averages[suite] = sum / times.size();
    }
    
    return averages;
}

double TestResultReporter::calculateCodeCoverage(const std::vector<TestResult>& results) const {
    // Simplified code coverage calculation
    // In real implementation, this would analyze actual code coverage data
    size_t totalTests = results.size();
    if (totalTests == 0) return 0.0;
    
    // Estimate based on pass rate and test coverage of components
    double baseCoverage = getOverallPassRate() * 0.8; // Assume 80% coverage of passed tests
    
    // Add additional coverage based on suite diversity
    std::set<std::string> suites;
    for (const auto& result : results) {
        suites.insert(result.testSuite);
    }
    
    double suiteCoverage = std::min(1.0, static_cast<double>(suites.size()) / 10.0); // Assume 10 major suites
    
    return (baseCoverage + suiteCoverage) / 2.0 * 100.0;
}

double TestResultReporter::calculateReliabilityScore(const std::vector<TestResult>& results) const {
    if (results.empty()) return 0.0;
    
    TestStatistics stats = calculateStatistics(results);
    
    double passRateScore = stats.passRate;
    
    // Reliability based on consistency (low variance in execution time)
    std::map<std::string, double> averages = calculateAverageTimes(results);
    double timeConsistencyScore = 100.0; // Simplified
    
    // Reliability based on error patterns
    size_t criticalFailures = 0;
    for (const auto& result : results) {
        if (!result.passed && (result.timeoutOccurred || !result.errorMessage.empty())) {
            criticalFailures++;
        }
    }
    
    double errorRateScore = 100.0 - (static_cast<double>(criticalFailures) / stats.totalTests * 100.0);
    
    return (passRateScore + timeConsistencyScore + errorRateScore) / 3.0;
}

double TestResultReporter::calculatePerformanceScore(const std::vector<TestResult>& results) const {
    if (results.empty()) return 0.0;
    
    std::map<std::string, double> averages = calculateAverageTimes(results);
    
    if (averages.empty()) return 0.0;
    
    double totalAverage = 0.0;
    size_t count = 0;
    
    for (const auto& pair : averages) {
        totalAverage += pair.second;
        count++;
    }
    
    totalAverage /= count;
    
    // Performance score based on execution time (inverse relationship)
    double performanceScore = 100.0 / (1.0 + totalAverage / 1000.0); // Normalize to 1000ms
    
    return performanceScore;
}

bool TestResultReporter::validateReportData(const std::vector<TestResult>& results) const {
    for (const auto& result : results) {
        if (result.testName.empty() || result.testSuite.empty()) {
            return false;
        }
        
        if (result.executionTime.count() < 0) {
            return false;
        }
    }
    
    return true;
}

bool TestResultReporter::validateReportFormat(const std::string& format) const {
    return format == "html" || format == "json" || format == "xml" || format == "csv";
}

std::string TestResultReporter::formatDuration(std::chrono::milliseconds duration) const {
    auto total_ms = duration.count();
    
    if (total_ms < 1000) {
        return std::to_string(total_ms) + " ms";
    } else if (total_ms < 60000) {
        return std::to_string(total_ms / 1000.0) + " s";
    } else {
        auto minutes = total_ms / 60000;
        auto seconds = (total_ms % 60000) / 1000;
        return std::to_string(minutes) + "m " + std::to_string(seconds) + "s";
    }
}

std::string TestResultReporter::formatPercentage(double value) const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << value << "%";
    return ss.str();
}

std::string TestResultReporter::escapeHTML(const std::string& text) const {
    std::string escaped = text;
    std::replace(escaped.begin(), escaped.end(), '&', "&amp;");
    std::replace(escaped.begin(), escaped.end(), '<', "&lt;");
    std::replace(escaped.begin(), escaped.end(), '>', "&gt;");
    std::replace(escaped.begin(), escaped.end(), '"', "&quot;");
    std::replace(escaped.begin(), escaped.end(), '\'', "&apos;");
    return escaped;
}

std::string TestResultReporter::escapeJSON(const std::string& text) const {
    std::string escaped = text;
    std::replace(escaped.begin(), escaped.end(), '\\', "\\\\");
    std::replace(escaped.begin(), escaped.end(), '"', "\\\"");
    std::replace(escaped.begin(), escaped.end(), '/', "\\/");
    std::replace(escaped.begin(), escaped.end(), '\b', "\\b");
    std::replace(escaped.begin(), escaped.end(), '\f', "\\f");
    std::replace(escaped.begin(), escaped.end(), '\n', "\\n");
    std::replace(escaped.begin(), escaped.end(), '\r', "\\r");
    std::replace(escaped.begin(), escaped.end(), '\t', "\\t");
    return escaped;
}

std::string TestResultReporter::escapeXML(const std::string& text) const {
    std::string escaped = text;
    std::replace(escaped.begin(), escaped.end(), '&', "&amp;");
    std::replace(escaped.begin(), escaped.end(), '<', "&lt;");
    std::replace(escaped.begin(), escaped.end(), '>', "&gt;");
    std::replace(escaped.begin(), escaped.end(), '"', "&quot;");
    std::replace(escaped.begin(), escaped.end(), '\'', "&apos;");
    return escaped;
}

std::string TestResultReporter::getCurrentTimestamp() const {
    std::time_t now = std::time(nullptr);
    std::tm* timeinfo = std::gmtime(&now);
    
    std::stringstream ss;
    ss << std::put_time(timeinfo, "%Y-%m-%d %H:%M:%S UTC");
    return ss.str();
}

std::string TestResultReporter::generatePluginTestReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "Plugin System Test Report\n";
    ss << "==========================\n\n";
    
    TestStatistics stats = calculateStatistics(results);
    
    ss << "Plugin System Performance:\n";
    ss << "  Pass Rate: " << formatPercentage(stats.passRate) << "\n";
    ss << "  Total Tests: " << stats.totalTests << "\n";
    ss << "  Average Execution Time: " << formatDuration(stats.averageExecutionTime) << "\n";
    
    ss << "\nKey Metrics:\n";
    
    // Extract plugin-specific metrics
    std::map<std::string, double> averages = calculateAverageTimes(results);
    for (const auto& pair : averages) {
        ss << "  " << pair.first << ": " << formatDuration(std::chrono::milliseconds(static_cast<long long>(pair.second))) << "\n";
    }
    
    return ss.str();
}

std::string TestResultReporter::generateCLITestReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "CLI Interface Test Report\n";
    ss << "==========================\n\n";
    
    TestStatistics stats = calculateStatistics(results);
    
    ss << "CLI Interface Performance:\n";
    ss << "  Pass Rate: " << formatPercentage(stats.passRate) << "\n";
    ss << "  Total Tests: " << stats.totalTests << "\n";
    ss << "  Average Execution Time: " << formatDuration(stats.averageExecutionTime) << "\n";
    
    return ss.str();
}

std::string TestResultReporter::generateIntegrationTestReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "Integration Test Report\n";
    ss << "=========================\n\n";
    
    TestStatistics stats = calculateStatistics(results);
    
    ss << "Integration Performance:\n";
    ss << "  Pass Rate: " << formatPercentage(stats.passRate) << "\n";
    ss << "  Total Tests: " << stats.totalTests << "\n";
    ss << "  Average Execution Time: " << formatDuration(stats.averageExecutionTime) << "\n";
    
    // Analyze integration issues
    size_t integrationFailures = 0;
    for (const auto& result : results) {
        if (!result.passed) {
            integrationFailures++;
        }
    }
    
    ss << "  Integration Failures: " << integrationFailures << "\n";
    
    return ss.str();
}

std::string TestResultReporter::generateStressTestReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "Stress Test Report\n";
    ss << "===================\n\n";
    
    TestStatistics stats = calculateStatistics(results);
    
    ss << "Stress Test Performance:\n";
    ss << "  Pass Rate: " << formatPercentage(stats.passRate) << "\n";
    ss << "  Total Tests: " << stats.totalTests << "\n";
    ss << "  Average Execution Time: " << formatDuration(stats.averageExecutionTime) << "\n";
    
    // Analyze stress test results
    size_t memoryStressFailures = 0;
    size_t plasticityStressFailures = 0;
    size_t integrationStressFailures = 0;
    
    for (const auto& result : results) {
        if (!result.passed) {
            if (result.testName.find("Memory") != std::string::npos) {
                memoryStressFailures++;
            } else if (result.testName.find("Plasticity") != std::string::npos) {
                plasticityStressFailures++;
            } else if (result.testName.find("Integration") != std::string::npos) {
                integrationStressFailures++;
            }
        }
    }
    
    ss << "  Memory Stress Failures: " << memoryStressFailures << "\n";
    ss << "  Plasticity Stress Failures: " << plasticityStressFailures << "\n";
    ss << "  Integration Stress Failures: " << integrationStressFailures << "\n";
    
    return ss.str();
}

std::string TestResultReporter::generatePerformanceTestReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "Performance Test Report\n";
    ss << "========================\n\n";
    
    TestStatistics stats = calculateStatistics(results);
    
    ss << "Performance Characteristics:\n";
    ss << "  Pass Rate: " << formatPercentage(stats.passRate) << "\n";
    ss << "  Total Tests: " << stats.totalTests << "\n";
    ss << "  Average Execution Time: " << formatDuration(stats.averageExecutionTime) << "\n";
    ss << "  Min Execution Time: " << formatDuration(stats.minExecutionTime) << "\n";
    ss << "  Max Execution Time: " << formatDuration(stats.maxExecutionTime) << "\n";
    
    // Extract performance metrics
    std::vector<PerformanceMetrics> metrics = extractPerformanceMetrics(results);
    if (!metrics.empty()) {
        ss << "\nPerformance Metrics:\n";
        double totalTime = 0.0;
        size_t count = 0;
        
        for (const auto& metric : metrics) {
            totalTime += metric.executionTime.count();
            count++;
            ss << "  " << metric.testSuite << " - " << metric.testName << ": " 
               << formatDuration(metric.executionTime) << "\n";
        }
        
        if (count > 0) {
            ss << "  Average: " << formatDuration(std::chrono::milliseconds(static_cast<long long>(totalTime / count))) << "\n";
        }
    }
    
    return ss.str();
}

std::string TestResultReporter::generateBenchmarkTestReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "Benchmark Test Report\n";
    ss << "======================\n\n";
    
    TestStatistics stats = calculateStatistics(results);
    
    ss << "Benchmark Performance:\n";
    ss << "  Pass Rate: " << formatPercentage(stats.passRate) << "\n";
    ss << "  Total Tests: " << stats.totalTests << "\n";
    ss << "  Average Execution Time: " << formatDuration(stats.averageExecutionTime) << "\n";
    
    // Extract benchmark metrics
    std::vector<PerformanceMetrics> metrics = extractPerformanceMetrics(results);
    if (!metrics.empty()) {
        ss << "\nBenchmark Results:\n";
        
        // Group by benchmark type
        std::map<std::string, std::vector<PerformanceMetrics>> benchmarks;
        for (const auto& metric : metrics) {
            if (metric.testName.find("Baseline") != std::string::npos) {
                benchmarks["Baseline"].push_back(metric);
            } else if (metric.testName.find("PluginLoading") != std::string::npos) {
                benchmarks["Plugin Loading"].push_back(metric);
            } else if (metric.testName.find("Simulation") != std::string::npos) {
                benchmarks["Simulation"].push_back(metric);
            } else if (metric.testName.find("Memory") != std::string::npos) {
                benchmarks["Memory"].push_back(metric);
            } else if (metric.testName.find("Network") != std::string::npos) {
                benchmarks["Network"].push_back(metric);
            }
        }
        
        for (const auto& pair : benchmarks) {
            ss << "  " << pair.first << ":\n";
            for (const auto& metric : pair.second) {
                ss << "    " << metric.testName << ": " << formatDuration(metric.executionTime) << "\n";
            }
        }
    }
    
    return ss.str();
}

std::string TestResultReporter::generateResearchScenarioReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "Research Scenario Test Report\n";
    ss << "==============================\n\n";
    
    TestStatistics stats = calculateStatistics(results);
    
    ss << "Research Scenario Performance:\n";
    ss << "  Pass Rate: " << formatPercentage(stats.passRate) << "\n";
    ss << "  Total Tests: " << stats.totalTests << "\n";
    ss << "  Average Execution Time: " << formatDuration(stats.averageExecutionTime) << "\n";
    
    // Analyze research scenario metrics
    size_t researchFailures = 0;
    for (const auto& result : results) {
        if (!result.passed) {
            if (result.testName.find("Research") != std::string::npos) {
                researchFailures++;
            }
        }
    }
    
    ss << "  Research Failures: " << researchFailures << "\n";
    
    return ss.str();
}

std::string TestResultReporter::generateProductionScenarioReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "Production Scenario Test Report\n";
    ss << "================================\n\n";
    
    TestStatistics stats = calculateStatistics(results);
    
    ss << "Production Scenario Performance:\n";
    ss << "  Pass Rate: " << formatPercentage(stats.passRate) << "\n";
    ss << "  Total Tests: " << stats.totalTests << "\n";
    ss << "  Average Execution Time: " << formatDuration(stats.averageExecutionTime) << "\n";
    
    // Analyze production scenario metrics
    size_t productionFailures = 0;
    for (const auto& result : results) {
        if (!result.passed) {
            if (result.testName.find("Production") != std::string::npos) {
                productionFailures++;
            }
        }
    }
    
    ss << "  Production Failures: " << productionFailures << "\n";
    
    return ss.str();
}

std::string TestResultReporter::generateDevelopmentScenarioReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "Development Scenario Test Report\n";
    ss << "=================================\n\n";
    
    TestStatistics stats = calculateStatistics(results);
    
    ss << "Development Scenario Performance:\n";
    ss << "  Pass Rate: " << formatPercentage(stats.passRate) << "\n";
    ss << "  Total Tests: " << stats.totalTests << "\n";
    ss << "  Average Execution Time: " << formatDuration(stats.averageExecutionTime) << "\n";
    
    return ss.str();
}

std::string TestResultReporter::generateDeploymentScenarioReport(const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << "Deployment Scenario Test Report\n";
    ss << "================================\n\n";
    
    TestStatistics stats = calculateStatistics(results);
    
    ss << "Deployment Scenario Performance:\n";
    ss << "  Pass Rate: " << formatPercentage(stats.passRate) << "\n";
    ss << "  Total Tests: " << stats.totalTests << "\n";
    ss << "  Average Execution Time: " << formatDuration(stats.averageExecutionTime) << "\n";
    
    return ss.str();
}

bool TestResultReporter::generateCIReadyReport(const std::vector<TestResult>& results, const std::string& ciFormat) {
    if (ciFormat == "github_actions") {
        return generateGitHubActionsReport(results);
    } else if (ciFormat == "jenkins") {
        return generateJenkinsReport(results);
    } else {
        return exportToFile(results, "ci_report.txt", "csv");
    }
}

bool TestResultReporter::generateGitHubActionsReport(const std::vector<TestResult>& results) {
    // Generate GitHub Actions compatible test report
    nlohmann::json j;
    
    j["summary"]["total_tests"] = results.size();
    j["summary"]["passed_tests"] = 0;
    j["summary"]["failed_tests"] = 0;
    
    for (const auto& result : results) {
        if (result.passed) {
            j["summary"]["passed_tests"]++;
        } else {
            j["summary"]["failed_tests"]++;
        }
    }
    
    // Create test matrix for GitHub Actions
    j["test_matrix"] = nlohmann::json::array();
    for (size_t i = 0; i < results.size(); ++i) {
        nlohmann::json test;
        test["name"] = results[i].testName;
        test["suite"] = results[i].testSuite;
        test["status"] = results[i].passed ? "passed" : "failed";
        test["time_ms"] = results[i].executionTime.count();
        
        if (!results[i].errorMessage.empty()) {
            test["error"] = results[i].errorMessage;
        }
        
        j["test_matrix"].push_back(test);
    }
    
    // Export to .github directory for GitHub Actions
    std::filesystem::create_directories(".github/workflows");
    std::ofstream file(".github/test_results.json");
    if (!file.is_open()) {
        return false;
    }
    
    file << j.dump(2);
    file.close();
    
    return true;
}

bool TestResultReporter::generateJenkinsReport(const std::vector<TestResult>& results) {
    // Generate Jenkins JUnit XML format
    std::stringstream ss;
    ss << "<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n";
    ss << "<testsuites>\n";
    ss << "  <testsuite name=\"NLM All Tests\" tests=\"" << results.size() << "\" "
       << "failures=\"" << std::count_if(results.begin(), results.end(),
                                          [](const TestResult& r) { return !r.passed; }) << "\" "
       << "time=\"" << calculateStatistics(results).totalExecutionTime.count() / 1000.0 << "\">\n";
    
    for (const auto& result : results) {
        ss << "    <testcase name=\"" << escapeXML(result.testName) << "\" "
           << "classname=\"" << escapeXML(result.testSuite) << "\" "
           << "time=\"" << result.executionTime.count() / 1000.0 << "\">\n";
        
        if (!result.passed) {
            ss << "      <failure message=\"" << escapeXML(result.errorMessage) << "\"/>\n";
        }
        
        ss << "    </testcase>\n";
    }
    
    ss << "  </testsuite>\n";
    ss << "</testsuites>\n";
    
    std::ofstream file("jenkins-test-results.xml");
    if (!file.is_open()) {
        return false;
    }
    
    file << ss.str();
    file.close();
    
    return true;
}

} // namespace tests
} // namespace nlm
