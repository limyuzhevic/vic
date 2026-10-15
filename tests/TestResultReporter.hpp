// Test Result Reporter
// Comprehensive test result reporting and analysis

#ifndef TEST_RESULT_REPORTER_HPP
#define TEST_RESULT_REPORTER_HPP

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <algorithm>
#include <cmath>

namespace nlm {
namespace tests {

struct TestResult;
struct AdvancedTestFramework;

// Test result statistics
struct TestStatistics {
    size_t totalTests;
    size_t passedTests;
    size_t failedTests;
    double passRate;
    std::chrono::milliseconds totalExecutionTime;
    std::chrono::milliseconds averageExecutionTime;
    std::chrono::milliseconds minExecutionTime;
    std::chrono::milliseconds maxExecutionTime;
    
    TestStatistics()
        : totalTests(0), passedTests(0), failedTests(0), passRate(0.0),
          totalExecutionTime(0), averageExecutionTime(0), minExecutionTime(0), maxExecutionTime(0) {}
};

// Test result trend
enum class TestTrend {
    STABLE,
    DECREASING,
    INCREASING,
    FLUCTUATING,
    UNKNOWN
};

// Performance metrics
struct PerformanceMetrics {
    std::string testName;
    std::string testSuite;
    std::chrono::milliseconds executionTime;
    std::map<std::string, double> customMetrics;
    
    PerformanceMetrics(const std::string& name, const std::string& suite,
                       std::chrono::milliseconds time)
        : testName(name), testSuite(suite), executionTime(time) {}
};

// Test result reporter
class TestResultReporter {
public:
    TestResultReporter();
    ~TestResultReporter();
    
    // Configuration
    void setOutputDirectory(const std::string& directory);
    void setDefaultReportFormat(const std::string& format);
    void enableDetailedReporting(bool enable);
    void enablePerformanceReporting(bool enable);
    void enableTrendAnalysis(bool enable);
    
    // Reporting
    std::string generateSummaryReport(const std::vector<TestResult>& results) const;
    std::string generateDetailedReport(const std::vector<TestResult>& results) const;
    std::string generateHTMLReport(const std::vector<TestResult>& results) const;
    std::string generateJSONReport(const std::vector<TestResult>& results) const;
    std::string generateXMLReport(const std::vector<TestResult>& results) const;
    std::string generateCSVReport(const std::vector<TestResult>& results) const;
    
    // File reporting
    bool exportToFile(const std::vector<TestResult>& results, const std::string& filename,
                     const std::string& format = "auto");
    bool exportToHTML(const std::vector<TestResult>& results, const std::string& filename);
    bool exportToJSON(const std::vector<TestResult>& results, const std::string& filename);
    bool exportToXML(const std::vector<TestResult>& results, const std::string& filename);
    bool exportToCSV(const std::vector<TestResult>& results, const std::string& filename);
    
    // Analysis and statistics
    TestStatistics calculateStatistics(const std::vector<TestResult>& results) const;
    std::map<std::string, TestTrend> analyzeTrends(const std::vector<TestResult>& results) const;
    std::vector<PerformanceMetrics> extractPerformanceMetrics(const std::vector<TestResult>& results) const;
    std::map<std::string, double> calculateAverageTimes(const std::vector<TestResult>& results) const;
    
    // Quality metrics
    double calculateCodeCoverage(const std::vector<TestResult>& results) const;
    double calculateReliabilityScore(const std::vector<TestResult>& results) const;
    double calculatePerformanceScore(const std::vector<TestResult>& results) const;
    
    // Validation
    bool validateReportData(const std::vector<TestResult>& results) const;
    bool validateReportFormat(const std::string& format) const;
    
    // Utilities
    std::string formatDuration(std::chrono::milliseconds duration) const;
    std::string formatPercentage(double value) const;
    std::string escapeHTML(const std::string& text) const;
    std::string escapeJSON(const std::string& text) const;
    
    // Specialized reporters
    std::string generatePluginTestReport(const std::vector<TestResult>& results) const;
    std::string generateCLITestReport(const std::vector<TestResult>& results) const;
    std::string generateIntegrationTestReport(const std::vector<TestResult>& results) const;
    std::string generateStressTestReport(const std::vector<TestResult>& results) const;
    std::string generatePerformanceTestReport(const std::vector<TestResult>& results) const;
    std::string generateBenchmarkTestReport(const std::vector<TestResult>& results) const;
    
    // Continuous integration support
    bool generateCIReadyReport(const std::vector<TestResult>& results, const std::string& ciFormat);
    bool generateGitHubActionsReport(const std::vector<TestResult>& results);
    bool generateJenkinsReport(const std::vector<TestResult>& results);
    
    // Real-world scenario reports
    std::string generateResearchScenarioReport(const std::vector<TestResult>& results) const;
    std::string generateProductionScenarioReport(const std::vector<TestResult>& results) const;
    std::string generateDevelopmentScenarioReport(const std::vector<TestResult>& results) const;
    std::string generateDeploymentScenarioReport(const std::vector<TestResult>& results) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private constructor and destructor
    TestResultReporter(const TestResultReporter&) = delete;
    TestResultReporter& operator=(const TestResultReporter&) = delete;
};

} // namespace tests
} // namespace nlm

#endif // TEST_RESULT_REPORTER_HPP
