#pragma once

#include "TestConfiguration.hpp"
#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <cmath>

namespace nlm {

struct TestStatistics {
    int totalTests;
    int passedTests;
    int failedTests;
    int skippedTests;
    int errorTests;
    int timeoutTests;
    double totalDuration;
    double averageDuration;
    double medianDuration;
    double standardDeviation;
    
    TestStatistics() : totalTests(0), passedTests(0), failedTests(0), skippedTests(0),
                       errorTests(0), timeoutTests(0), totalDuration(0.0),
                       averageDuration(0.0), medianDuration(0.0), standardDeviation(0.0) {}
    
    double getPassRate() const {
        return totalTests > 0 ? (passedTests * 100.0) / totalTests : 0.0;
    }
    
    double getFailureRate() const {
        return totalTests > 0 ? (failedTests * 100.0) / totalTests : 0.0;
    }
    
    double getErrorRate() const {
        return totalTests > 0 ? ((errorTests + timeoutTests) * 100.0) / totalTests : 0.0;
    }
};

struct CategoryStatistics {
    std::string categoryName;
    int testCount;
    int passedCount;
    double totalDuration;
    double passRate;
    
    CategoryStatistics(const std::string& name) : categoryName(name), testCount(0), 
                                                  passedCount(0), totalDuration(0.0), passRate(0.0) {}
};

struct LevelStatistics {
    std::string levelName;
    int testCount;
    int passedCount;
    double totalDuration;
    double passRate;
    
    LevelStatistics(const std::string& name) : levelName(name), testCount(0), 
                                              passedCount(0), totalDuration(0.0), passRate(0.0) {}
};

struct PerformanceMetrics {
    std::string metricName;
    double value;
    std::string unit;
    std::chrono::duration<double> duration;
    
    PerformanceMetrics(const std::string& name, double val, const std::string& unit = "")
        : metricName(name), value(val), unit(unit), duration(0.0) {}
};

struct TrendData {
    std::chrono::system_clock::time_point timestamp;
    double passRate;
    double averageDuration;
    int totalTests;
    int failedTests;
};

struct FlakeData {
    std::string testName;
    int failureCount;
    int totalRuns;
    double failureRate;
    std::vector<std::chrono::system_clock::time_point> failureTimes;
    std::vector<TestStatus> failureStatus;
    
    FlakeData(const std::string& name) : testName(name), failureCount(0), totalRuns(0),
                                        failureRate(0.0) {}
};

class TestReporter {
public:
    enum class OutputFormat {
        CONSOLE,
        JSON,
        XML,
        CSV,
        HTML
    };
    
    enum class ReportLevel {
        SUMMARY,
        DETAILED,
        VERBOSE
    };
    
    TestReporter();
    ~TestReporter() = default;
    
    static TestReporter& getInstance() {
        static TestReporter instance;
        return instance;
    }
    
    void setOutputFormat(OutputFormat format) { outputFormat = format; }
    void setReportLevel(ReportLevel level) { reportLevel = level; }
    void setOutputFile(const std::string& filename) { outputFile = filename; }
    
    void addTestResult(const TestResult& result);
    void addBenchmarkResult(const BenchmarkResult& result);
    
    void generateReport(const std::string& title = "NLM Test Report");
    void generateAnalyticsReport(const std::string& title = "NLM Analytics Report");
    void generatePerformanceReport(const std::string& title = "NLM Performance Report");
    
    void generateTrendReport(const std::vector<TrendData>& trendData, const std::string& title = "NLM Trend Report");
    void generateFlakeReport(const std::vector<FlakeData>& flakeData, const std::string& title = "NLM Flake Report");
    
    TestStatistics calculateStatistics(const std::vector<TestResult>& results) const;
    std::vector<CategoryStatistics> calculateCategoryStatistics(const std::vector<TestResult>& results) const;
    std::vector<LevelStatistics> calculateLevelStatistics(const std::vector<TestResult>& results) const;
    std::vector<PerformanceMetrics> calculatePerformanceMetrics(const std::vector<BenchmarkResult>& results) const;
    
    std::string formatAsJSON(const TestResult& result) const;
    std::string formatAsXML(const TestResult& result) const;
    std::string formatAsCSV(const TestResult& result) const;
    std::string formatAsHTML(const TestResult& result) const;
    
    std::string formatSummary(const TestStatistics& stats) const;
    std::string formatDetailed(const TestStatistics& stats, const std::vector<CategoryStatistics>& categoryStats) const;
    std::string formatVerbose(const TestStatistics& stats, const std::vector<TestResult>& results) const;
    
    std::vector<TrendData> collectTrendData(const std::vector<TestResult>& results) const;
    std::vector<FlakeData> detectFlakes(const std::vector<TestResult>& results) const;
    
    void generateHTMLReport(const std::string& title, const std::vector<TestResult>& results);
    void generateJSONReport(const std::string& title, const std::vector<TestResult>& results);
    void generateXMLReport(const std::string& title, const std::vector<TestResult>& results);
    void generateCSVReport(const std::string& title, const std::vector<TestResult>& results);
    
    std::string calculatePercentile(const std::vector<double>& values, double percentile) const;
    double calculateMean(const std::vector<double>& values) const;
    double calculateStandardDeviation(const std::vector<double>& values, double mean) const;
    
    void exportResults(const std::string& filename, const std::vector<TestResult>& results);
    void importResults(const std::string& filename, std::vector<TestResult>& results);
    
private:
    OutputFormat outputFormat;
    ReportLevel reportLevel;
    std::string outputFile;
    std::vector<TestResult> testResults;
    std::vector<BenchmarkResult> benchmarkResults;
    
    std::string escapeJSON(const std::string& str) const;
    std::string escapeHTML(const std::string& str) const;
    std::string currentTimestamp() const;
    
    void writeToFile(const std::string& content) const;
    void writeReportHeader(std::stringstream& ss, const std::string& title) const;
    void writeReportFooter(std::stringstream& ss) const;
};

} // namespace nlm
