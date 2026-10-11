#include "TestReporting.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <ctime>

namespace nlm {

TestReporter::TestReporter() : outputFormat(OutputFormat::CONSOLE), reportLevel(ReportLevel::SUMMARY) {}

void TestReporter::addTestResult(const TestResult& result) {
    testResults.push_back(result);
}

void TestReporter::addBenchmarkResult(const BenchmarkResult& result) {
    benchmarkResults.push_back(result);
}

void TestReporter::generateReport(const std::string& title) {
    std::stringstream ss;
    writeReportHeader(ss, title);
    
    TestStatistics stats = calculateStatistics(testResults);
    
    switch (reportLevel) {
        case ReportLevel::SUMMARY:
            ss << formatSummary(stats) << std::endl;
            break;
        case ReportLevel::DETAILED:
            {
                auto categoryStats = calculateCategoryStatistics(testResults);
                ss << formatDetailed(stats, categoryStats) << std::endl;
            }
            break;
        case ReportLevel::VERBOSE:
            ss << formatVerbose(stats, testResults) << std::endl;
            break;
    }
    
    writeReportFooter(ss);
    
    if (outputFile.empty()) {
        std::cout << ss.str();
    } else {
        writeToFile(ss.str());
    }
}

void TestReporter::generateAnalyticsReport(const std::string& title) {
    std::stringstream ss;
    writeReportHeader(ss, "Analytics: " + title);
    
    auto stats = calculateStatistics(testResults);
    auto categoryStats = calculateCategoryStatistics(testResults);
    auto levelStats = calculateLevelStatistics(testResults);
    auto performanceMetrics = calculatePerformanceMetrics(benchmarkResults);
    auto trendData = collectTrendData(testResults);
    auto flakeData = detectFlakes(testResults);
    
    ss << "=== Analytics Report ===" << std::endl;
    ss << "Total Tests: " << stats.totalTests << std::endl;
    ss << "Pass Rate: " << stats.getPassRate() << "%" << std::endl;
    ss << "Average Duration: " << stats.averageDuration << "s" << std::endl;
    ss << std::endl;
    
    ss << "--- Category Performance ---" << std::endl;
    for (const auto& cat : categoryStats) {
        ss << "  " << cat.categoryName << ": " << cat.passRate << "% pass rate (" 
           << cat.testCount << " tests, " << cat.totalDuration << "s total)" << std::endl;
    }
    
    ss << std::endl << "--- Level Performance ---" << std::endl;
    for (const auto& lvl : levelStats) {
        ss << "  " << lvl.levelName << ": " << lvl.passRate << "% pass rate (" 
           << lvl.testCount << " tests, " << lvl.totalDuration << "s total)" << std::endl;
    }
    
    ss << std::endl << "--- Performance Metrics ---" << std::endl;
    for (const auto& metric : performanceMetrics) {
        ss << "  " << metric.metricName << ": " << metric.value << " " << metric.unit 
           << " (" << metric.duration.count() << "s)" << std::endl;
    }
    
    ss << std::endl << "--- Flake Analysis ---" << std::endl;
    if (flakeData.empty()) {
        ss << "  No flaky tests detected." << std::endl;
    } else {
        ss << "  Flaky tests (failure rate > 10%):" << std::endl;
        for (const auto& flake : flakeData) {
            ss << "    " << flake.testName << ": " << flake.failureRate << "% failure rate ("
               << flake.failureCount << "/" << flake.totalRuns << " failures)" << std::endl;
        }
    }
    
    ss << std::endl << "--- Trends ---" << std::endl;
    if (trendData.size() > 1) {
        double trend = (trendData.back().passRate - trendData.front().passRate) / trendData.size();
        ss << "  Pass rate trend: " << trend << "% per test run" << std::endl;
        ss << "  Duration trend: " << (trendData.back().averageDuration - trendData.front().averageDuration) << "s per test run" << std::endl;
    }
    
    writeReportFooter(ss);
    
    if (outputFile.empty()) {
        std::cout << ss.str();
    } else {
        writeToFile(ss.str());
    }
}

void TestReporter::generatePerformanceReport(const std::string& title) {
    std::stringstream ss;
    writeReportHeader(ss, "Performance: " + title);
    
    ss << "=== Performance Report ===" << std::endl;
    ss << "Benchmarks executed: " << benchmarkResults.size() << std::endl;
    ss << std::endl;
    
    for (const auto& bench : benchmarkResults) {
        ss << formatBenchmarkResult(bench) << std::endl;
    }
    
    auto performanceMetrics = calculatePerformanceMetrics(benchmarkResults);
    
    ss << "=== Performance Metrics Summary ===" << std::endl;
    for (const auto& metric : performanceMetrics) {
        ss << "  " << metric.metricName << ": " << metric.value << " " << metric.unit 
           << " (target: " << (metric.metricName.find("Time") != std::string::npos ? "<1.0s" : "<100MB") << ")" << std::endl;
    }
    
    writeReportFooter(ss);
    
    if (outputFile.empty()) {
        std::cout << ss.str();
    } else {
        writeToFile(ss.str());
    }
}

void TestReporter::generateTrendReport(const std::vector<TrendData>& trendData, const std::string& title) {
    std::stringstream ss;
    writeReportHeader(ss, title);
    
    ss << "=== Trend Report ===" << std::endl;
    ss << "Number of data points: " << trendData.size() << std::endl;
    ss << std::endl;
    
    ss << "Timestamp, PassRate, AverageDuration, TotalTests, FailedTests" << std::endl;
    for (const auto& point : trendData) {
        auto time = std::chrono::system_clock::to_time_t(point.timestamp);
        char timeStr[100];
        std::strftime(timeStr, sizeof(timeStr), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
        ss << timeStr << ", " << point.passRate << "%, " << point.averageDuration.count() 
           << "s, " << point.totalTests << ", " << point.failedTests << std::endl;
    }
    
    writeReportFooter(ss);
    
    if (outputFile.empty()) {
        std::cout << ss.str();
    } else {
        writeToFile(ss.str());
    }
}

void TestReporter::generateFlakeReport(const std::vector<FlakeData>& flakeData, const std::string& title) {
    std::stringstream ss;
    writeReportHeader(ss, title);
    
    ss << "=== Flake Report ===" << std::endl;
    ss << "Total tests analyzed: " << (testResults.empty() ? 0 : testResults.size()) << std::endl;
    ss << "Flaky tests detected: " << flakeData.size() << std::endl;
    ss << std::endl;
    
    if (flakeData.empty()) {
        ss << "No flaky tests detected." << std::endl;
    } else {
        ss << "Flaky tests (failure rate > 10%):" << std::endl;
        ss << std::left << std::setw(40) << "Test Name" 
           << std::setw(15) << "Failure Rate" 
           << std::setw(15) << "Failures/Run" 
           << std::setw(10) << "Total Runs" << std::endl;
        ss << std::string(80, '-') << std::endl;
        
        for (const auto& flake : flakeData) {
            ss << std::left << std::setw(40) << flake.testName
               << std::setw(15) << (flake.failureRate * 100) << "%"
               << std::setw(15) << flake.failureCount << "/" << flake.totalRuns
               << std::setw(10) << flake.totalRuns << std::endl;
        }
    }
    
    writeReportFooter(ss);
    
    if (outputFile.empty()) {
        std::cout << ss.str();
    } else {
        writeToFile(ss.str());
    }
}

TestStatistics TestReporter::calculateStatistics(const std::vector<TestResult>& results) const {
    TestStatistics stats;
    stats.totalTests = results.size();
    
    std::vector<double> durations;
    
    for (const auto& result : results) {
        switch (result.status) {
            case TestStatus::PASSED: stats.passedTests++; break;
            case TestStatus::FAILED: stats.failedTests++; break;
            case TestStatus::SKIPPED: stats.skippedTests++; break;
            case TestStatus::ERROR: stats.errorTests++; break;
            case TestStatus::TIMEOUT: stats.timeoutTests++; break;
        }
        
        stats.totalDuration += result.duration.count();
        durations.push_back(result.duration.count());
    }
    
    if (stats.totalTests > 0) {
        stats.averageDuration = stats.totalDuration / stats.totalTests;
        stats.medianDuration = calculatePercentile(durations, 50.0);
        stats.standardDeviation = calculateStandardDeviation(durations, stats.averageDuration);
    }
    
    return stats;
}

std::vector<CategoryStatistics> TestReporter::calculateCategoryStatistics(const std::vector<TestResult>& results) const {
    std::unordered_map<std::string, CategoryStatistics> categoryMap;
    
    for (const auto& result : results) {
        std::string categoryName;
        switch (result.params.category) {
            case TestCategory::NEURAL_DYNAMICS: categoryName = "Neural Dynamics"; break;
            case TestCategory::PLASTICITY: categoryName = "Plasticity"; break;
            case TestCategory::MEMORY_SYSTEM: categoryName = "Memory System"; break;
            case TestCategory::PREDICTION: categoryName = "Prediction"; break;
            case TestCategory::COGNITION: categoryName = "Cognition"; break;
            case TestCategory::NEUROMODULATION: categoryName = "Neuromodulation"; break;
            case TestCategory::DEVELOPMENT: categoryName = "Development"; break;
            case TestCategory::MULTI_AGENT: categoryName = "Multi-Agent"; break;
            case TestCategory::PERFORMANCE: categoryName = "Performance"; break;
            case TestCategory::MEMORY_LEAK: categoryName = "Memory Leak"; break;
            case TestCategory::INTEGRATION: categoryName = "Integration"; break;
            case TestCategory::REGRESSION: categoryName = "Regression"; break;
        }
        
        if (categoryMap.find(categoryName) == categoryMap.end()) {
            categoryMap[categoryName] = CategoryStatistics(categoryName);
        }
        
        auto& catStats = categoryMap[categoryName];
        catStats.testCount++;
        if (result.status == TestStatus::PASSED) catStats.passedCount++;
        catStats.totalDuration += result.duration.count();
    }
    
    std::vector<CategoryStatistics> categoryStats;
    for (auto& pair : categoryMap) {
        if (pair.second.testCount > 0) {
            pair.second.passRate = (pair.second.passedCount * 100.0) / pair.second.testCount;
        }
        categoryStats.push_back(pair.second);
    }
    
    return categoryStats;
}

std::vector<LevelStatistics> TestReporter::calculateLevelStatistics(const std::vector<TestResult>& results) const {
    std::unordered_map<std::string, LevelStatistics> levelMap;
    
    for (const auto& result : results) {
        std::string levelName;
        switch (result.params.level) {
            case TestLevel::UNIT: levelName = "Unit"; break;
            case TestLevel::INTEGRATION: levelName = "Integration"; break;
            case TestLevel::PERFORMANCE: levelName = "Performance"; break;
            case TestLevel::STRESS: levelName = "Stress"; break;
            case TestLevel::MEMORY: levelName = "Memory"; break;
        }
        
        if (levelMap.find(levelName) == levelMap.end()) {
            levelMap[levelName] = LevelStatistics(levelName);
        }
        
        auto& lvlStats = levelMap[levelName];
        lvlStats.testCount++;
        if (result.status == TestStatus::PASSED) lvlStats.passedCount++;
        lvlStats.totalDuration += result.duration.count();
    }
    
    std::vector<LevelStatistics> levelStats;
    for (auto& pair : levelMap) {
        if (pair.second.testCount > 0) {
            pair.second.passRate = (pair.second.passedCount * 100.0) / pair.second.testCount;
        }
        levelStats.push_back(pair.second);
    }
    
    return levelStats;
}

std::vector<PerformanceMetrics> TestReporter::calculatePerformanceMetrics(const std::vector<BenchmarkResult>& results) const {
    std::vector<PerformanceMetrics> metrics;
    
    double totalTime = 0.0;
    double totalMemory = 0.0;
    double totalCPU = 0.0;
    int count = 0;
    
    for (const auto& bench : results) {
        totalTime += bench.executionTime;
        totalMemory += bench.memoryUsage;
        totalCPU += bench.cpuUsage;
        count++;
    }
    
    if (count > 0) {
        metrics.push_back(PerformanceMetrics("Average Execution Time", totalTime / count, "s"));
        metrics.push_back(PerformanceMetrics("Average Memory Usage", totalMemory / count, "MB"));
        metrics.push_back(PerformanceMetrics("Average CPU Usage", totalCPU / count, "%"));
    }
    
    return metrics;
}

std::vector<TrendData> TestReporter::collectTrendData(const std::vector<TestResult>& results) const {
    std::vector<TrendData> trends;
    
    if (results.empty()) return trends;
    
    TestStatistics stats = calculateStatistics(results);
    TrendData trend;
    trend.timestamp = std::chrono::system_clock::now();
    trend.passRate = stats.getPassRate();
    trend.averageDuration = stats.averageDuration;
    trend.totalTests = stats.totalTests;
    trend.failedTests = stats.failedTests;
    
    trends.push_back(trend);
    
    return trends;
}

std::vector<FlakeData> TestReporter::detectFlakes(const std::vector<TestResult>& results) const {
    std::unordered_map<std::string, FlakeData> flakeMap;
    
    for (const auto& result : results) {
        if (result.status == TestStatus::FAILED && result.params.enabled) {
            std::string testName = result.testName;
            
            if (flakeMap.find(testName) == flakeMap.end()) {
                flakeMap[testName] = FlakeData(testName);
            }
            
            auto& flake = flakeMap[testName];
            flake.failureCount++;
            flake.totalRuns++;
            flake.failureStatus.push_back(result.status);
            flake.failureTimes.push_back(result.startTime);
        } else if (result.status != TestStatus::SKIPPED && result.params.enabled) {
            std::string testName = result.testName;
            
            if (flakeMap.find(testName) == flakeMap.end()) {
                flakeMap[testName] = FlakeData(testName);
            }
            
            flakeMap[testName].totalRuns++;
        }
    }
    
    std::vector<FlakeData> flakeData;
    for (auto& pair : flakeMap) {
        if (pair.second.totalRuns > 0) {
            pair.second.failureRate = (pair.second.failureCount * 1.0) / pair.second.totalRuns;
            if (pair.second.failureRate > 0.1) {
                flakeData.push_back(pair.second);
            }
        }
    }
    
    return flakeData;
}

std::string TestReporter::formatAsJSON(const TestResult& result) const {
    std::stringstream ss;
    ss << "{\n";
    ss << "  \"testName\": \"" << result.testName << "\",\n";
    ss << "  \"status\": " << static_cast<int>(result.status) << ",\n";
    ss << "  \"duration\": " << result.duration.count() << ",\n";
    ss << "  \"failureMessage\": \"" << escapeJSON(result.failureMessage) << "\",\n";
    ss << "  \"errorMessage\": \"" << escapeJSON(result.errorMessage) << "\"\n";
    ss << "}";
    return ss.str();
}

std::string TestReporter::formatAsXML(const TestResult& result) const {
    std::stringstream ss;
    ss << "<TestResult>\n";
    ss << "  <testName>" << escapeHTML(result.testName) << "</testName>\n";
    ss << "  <status>" << static_cast<int>(result.status) << "</status>\n";
    ss << "  <duration>" << result.duration.count() << "</duration>\n";
    ss << "  <failureMessage>" << escapeHTML(result.failureMessage) << "</failureMessage>\n";
    ss << "  <errorMessage>" << escapeHTML(result.errorMessage) << "</errorMessage>\n";
    ss << "</TestResult>";
    return ss.str();
}

std::string TestReporter::formatAsCSV(const TestResult& result) const {
    std::stringstream ss;
    ss << "\"" << result.testName << "\"," << static_cast<int>(result.status) << 
       "\"" << result.duration.count() << ",\"" << escapeJSON(result.failureMessage) 
       << "\",\"" << escapeJSON(result.errorMessage) << "\"";
    return ss.str();
}

std::string TestReporter::formatAsHTML(const TestResult& result) const {
    std::stringstream ss;
    ss << "<div class=\"test-result\">\n";
    ss << "  <h3>" << escapeHTML(result.testName) << "</h3>\n";
    ss << "  <p>Status: " << static_cast<int>(result.status) << "</p>\n";
    ss << "  <p>Duration: " << result.duration.count() << "s</p>\n";
    if (!result.failureMessage.empty()) {
        ss << "  <p class=\"failure\">Failure: " << escapeHTML(result.failureMessage) << "</p>\n";
    }
    if (!result.errorMessage.empty()) {
        ss << "  <p class=\"error\">Error: " << escapeHTML(result.errorMessage) << "</p>\n";
    }
    ss << "</div>\n";
    return ss.str();
}

std::string TestReporter::formatSummary(const TestStatistics& stats) const {
    std::stringstream ss;
    ss << "=== Test Summary ===" << std::endl;
    ss << "Total Tests: " << stats.totalTests << std::endl;
    ss << "Passed: " << stats.passedTests << " (" << stats.getPassRate() << "%)" << std::endl;
    ss << "Failed: " << stats.failedTests << " (" << stats.getFailureRate() << "%)" << std::endl;
    ss << "Errors: " << stats.errorTests << " + " << stats.timeoutTests << " Timeouts" << std::endl;
    ss << "Skipped: " << stats.skippedTests << std::endl;
    ss << "Average Duration: " << stats.averageDuration << "s" << std::endl;
    ss << "Median Duration: " << stats.medianDuration << "s" << std::endl;
    ss << "Standard Deviation: " << stats.standardDeviation << "s" << std::endl;
    return ss.str();
}

std::string TestReporter::formatDetailed(const TestStatistics& stats, const std::vector<CategoryStatistics>& categoryStats) const {
    std::stringstream ss;
    ss << formatSummary(stats) << std::endl;
    ss << "\n=== Category Performance ===" << std::endl;
    ss << std::left << std::setw(30) << "Category" 
       << std::setw(10) << "Tests" 
       << std::setw(10) << "Passed" 
       << std::setw(15) << "Pass Rate" 
       << std::setw(15) << "Total Duration" << std::endl;
    ss << std::string(80, '-') << std::endl;
    
    for (const auto& cat : categoryStats) {
        ss << std::left << std::setw(30) << cat.categoryName
           << std::setw(10) << cat.testCount
           << std::setw(10) << cat.passedCount
           << std::setw(15) << cat.passRate << "%"
           << std::setw(15) << cat.totalDuration << "s" << std::endl;
    }
    
    return ss.str();
}

std::string TestReporter::formatVerbose(const TestStatistics& stats, const std::vector<TestResult>& results) const {
    std::stringstream ss;
    ss << formatDetailed(stats, calculateCategoryStatistics(results)) << std::endl;
    ss << "\n=== Test Results Detail ===" << std::endl;
    
    for (const auto& result : results) {
        ss << formatTestResult(result) << std::endl;
    }
    
    return ss.str();
}

std::string TestReporter::calculatePercentile(const std::vector<double>& values, double percentile) const {
    if (values.empty()) return 0.0;
    
    std::vector<double> sortedValues = values;
    std::sort(sortedValues.begin(), sortedValues.end());
    
    double rank = (percentile / 100.0) * (sortedValues.size() - 1) + 1;
    int index = static_cast<int>(std::floor(rank)) - 1;
    
    if (index >= sortedValues.size() - 1) return sortedValues.back();
    
    double fraction = rank - static_cast<int>(rank);
    return sortedValues[index] + fraction * (sortedValues[index + 1] - sortedValues[index]);
}

double TestReporter::calculateMean(const std::vector<double>& values) const {
    if (values.empty()) return 0.0;
    
    double sum = std::accumulate(values.begin(), values.end(), 0.0);
    return sum / values.size();
}

double TestReporter::calculateStandardDeviation(const std::vector<double>& values, double mean) const {
    if (values.empty()) return 0.0;
    
    double sum = 0.0;
    for (const auto& val : values) {
        sum += (val - mean) * (val - mean);
    }
    
    return std::sqrt(sum / values.size());
}

void TestReporter::generateHTMLReport(const std::string& title, const std::vector<TestResult>& results) {
    outputFormat = OutputFormat::HTML;
    generateReport(title);
}

void TestReporter::generateJSONReport(const std::string& title, const std::vector<TestResult>& results) {
    outputFormat = OutputFormat::JSON;
    generateReport(title);
}

void TestReporter::generateXMLReport(const std::string& title, const std::vector<TestResult>& results) {
    outputFormat = OutputFormat::XML;
    generateReport(title);
}

void TestReporter::generateCSVReport(const std::string& title, const std::vector<TestResult>& results) {
    outputFormat = OutputFormat::CSV;
    generateReport(title);
}

void TestReporter::exportResults(const std::string& filename, const std::vector<TestResult>& results) {
    testResults = results;
    outputFile = filename;
    generateReport("Exported Results");
    outputFile.clear();
}

void TestReporter::importResults(const std::string& filename, std::vector<TestResult>& results) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file for import: " + filename);
    }
    
    // Parse file based on format (simplified implementation)
    // In a real implementation, this would properly parse the report format
    results.clear();
    file.close();
}

std::string TestReporter::escapeJSON(const std::string& str) const {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '\\': escaped += "\\\\"; break;
            case '\"': escaped += "\\\""; break;
            case '\n': escaped += "\\n"; break;
            case '\r': escaped += "\\r"; break;
            case '\t': escaped += "\\t"; break;
            default: escaped += c;
        }
    }
    return escaped;
}

std::string TestReporter::escapeHTML(const std::string& str) const {
    std::string escaped;
    for (char c : str) {
        switch (c) {
            case '<': escaped += "&lt;"; break;
            case '>': escaped += "&gt;"; break;
            case '&': escaped += "&amp;"; break;
            case '"': escaped += "&quot;"; break;
            case '\'': escaped += "&#39;"; break;
            default: escaped += c;
        }
    }
    return escaped;
}

std::string TestReporter::currentTimestamp() const {
    auto now = std::chrono::system_clock::now();
    auto time = std::chrono::system_clock::to_time_t(now);
    char buffer[100];
    std::strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", std::localtime(&time));
    return std::string(buffer);
}

void TestReporter::writeToFile(const std::string& content) const {
    std::ofstream file(outputFile);
    if (file.is_open()) {
        file << content;
        file.close();
    }
}

void TestReporter::writeReportHeader(std::stringstream& ss, const std::string& title) const {
    ss << "========================================" << std::endl;
    ss << "           " << title << "           " << std::endl;
    ss << "========================================" << std::endl;
    ss << "Generated: " << currentTimestamp() << std::endl;
    ss << std::endl;
}

void TestReporter::writeReportFooter(std::stringstream& ss) const {
    ss << "========================================" << std::endl;
    ss << "End of Report" << std::endl;
    ss << "========================================" << std::endl;
}

} // namespace nlm
