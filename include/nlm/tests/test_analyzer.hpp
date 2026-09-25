// Test analyzer and statistical analysis
// Advanced analysis of test results and performance data
#pragma once

#include <vector>
#include <string>
#include <map>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>
#include <fstream>
#include <set>
#include <queue>
#include <functional>
#include "test_framework.hpp"

namespace nlm {
namespace test {

// Statistical analysis utilities for test data
class StatisticalAnalyzer {
public:
    // Statistical distribution analysis
    struct DistributionAnalysis {
        double mean;
        double median;
        double mode;
        double variance;
        double stdDev;
        double skewness;
        double kurtosis;
        double min;
        double max;
        double range;
        double iqr; // Interquartile range
        std::vector<double> percentiles;
    };
    
    // Calculate basic statistics
    static DistributionAnalysis analyzeDistribution(const std::vector<double>& data) {
        DistributionAnalysis result;
        
        if (data.empty()) {
            return result;
        }
        
        result.mean = std::accumulate(data.begin(), data.end(), 0.0) / data.size();
        result.min = *std::min_element(data.begin(), data.end());
        result.max = *std::max_element(data.begin(), data.end());
        result.range = result.max - result.min;
        
        // Median
        std::vector<double> sorted = data;
        std::sort(sorted.begin(), sorted.end());
        result.median = (sorted.size() % 2 == 0) ? 
            (sorted[sorted.size()/2 - 1] + sorted[sorted.size()/2]) / 2.0 :
            sorted[sorted.size()/2];
        
        // Mode
        std::unordered_map<double, int> frequency;
        for (double value : data) {
            frequency[value]++;
        }
        double modeValue = data[0];
        int maxCount = 0;
        for (const auto& pair : frequency) {
            if (pair.second > maxCount) {
                modeValue = pair.first;
                maxCount = pair.second;
            }
        }
        result.mode = modeValue;
        
        // Variance and standard deviation
        double sumSquares = 0.0;
        for (double value : data) {
            double diff = value - result.mean;
            sumSquares += diff * diff;
        }
        result.variance = (data.size() > 1) ? sumSquares / (data.size() - 1) : 0.0;
        result.stdDev = std::sqrt(result.variance);
        
        // Skewness and kurtosis
        if (result.stdDev > 1e-12) {
            double sumSkew = 0.0;
            double sumKurt = 0.0;
            for (double value : data) {
                double z = (value - result.mean) / result.stdDev;
                sumSkew += z * z * z;
                sumKurt += z * z * z * z;
            }
            result.skewness = (data.size() > 2) ? sumSkew / data.size() : 0.0;
            result.kurtosis = (data.size() > 3) ? (sumKurt / data.size() - 3.0) : 0.0;
        }
        
        // Interquartile range
        size_t q1Index = data.size() / 4;
        size_t q3Index = (3 * data.size()) / 4;
        result.iqr = sorted[q3Index] - sorted[q1Index];
        
        // Percentiles
        result.percentiles.resize(10); // 0th, 10th, ..., 100th percentiles
        for (int i = 0; i <= 10; ++i) {
            size_t index = static_cast<size_t>((i * data.size()) / 10);
            result.percentiles[i] = sorted[std::min(index, data.size() - 1)];
        }
        
        return result;
    }
    
    // Analyze correlation between test metrics
    static double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) {
        if (x.size() != y.size() || x.size() < 2) return 0.0;
        
        double xMean = std::accumulate(x.begin(), x.end(), 0.0) / x.size();
        double yMean = std::accumulate(y.begin(), y.end(), 0.0) / y.size();
        
        double numerator = 0.0, xDenom = 0.0, yDenom = 0.0;
        
        for (size_t i = 0; i < x.size(); ++i) {
            double xDiff = x[i] - xMean;
            double yDiff = y[i] - yMean;
            numerator += xDiff * yDiff;
            xDenom += xDiff * xDiff;
            yDenom += yDiff * yDiff;
        }
        
        double denominator = std::sqrt(xDenom * yDenom);
        return (denominator > 1e-12) ? numerator / denominator : 0.0;
    }
    
    // Time series analysis
    struct TimeSeriesAnalysis {
        std::vector<double> values;
        std::vector<double> movingAverage;
        double trend;
        double seasonality;
        double volatility;
        double autocorrelation;
    };
    
    static TimeSeriesAnalysis analyzeTimeSeries(const std::vector<double>& data,
                                               size_t windowSize = 10) {
        TimeSeriesAnalysis result;
        result.values = data;
        
        if (data.size() >= windowSize) {
            // Moving average
            result.movingAverage.resize(data.size() - windowSize + 1);
            double windowSum = std::accumulate(data.begin(), data.begin() + windowSize, 0.0);
            result.movingAverage[0] = windowSum / windowSize;
            
            for (size_t i = windowSize; i < data.size(); ++i) {
                windowSum += data[i] - data[i - windowSize];
                result.movingAverage[i - windowSize + 1] = windowSum / windowSize;
            }
        }
        
        // Simple trend calculation (linear regression)
        if (data.size() >= 2) {
            double xSum = 0.0, ySum = 0.0, xySum = 0.0, x2Sum = 0.0;
            for (size_t i = 0; i < data.size(); ++i) {
                double x = static_cast<double>(i);
                double y = data[i];
                xSum += x;
                ySum += y;
                xySum += x * y;
                x2Sum += x * x;
            }
            
            double n = static_cast<double>(data.size());
            result.trend = (n * xySum - xSum * ySum) / (n * x2Sum - xSum * xSum);
        }
        
        // Volatility (standard deviation of changes)
        if (data.size() >= 2) {
            std::vector<double> changes;
            for (size_t i = 1; i < data.size(); ++i) {
                changes.push_back(data[i] - data[i - 1]);
            }
            
            DistributionAnalysis changeStats = analyzeDistribution(changes);
            result.volatility = changeStats.stdDev;
        }
        
        // Autocorrelation (lag-1)
        if (data.size() >= 3) {
            double sumXY = 0.0, sumX = 0.0, sumY = 0.0, sumX2 = 0.0;
            for (size_t i = 0; i < data.size() - 1; ++i) {
                double x = data[i];
                double y = data[i + 1];
                sumXY += x * y;
                sumX += x;
                sumY += y;
                sumX2 += x * x;
            }
            
            double denominator = std::sqrt(sumX2 * (sumY * data.size() - sumY));
            result.autocorrelation = (denominator > 1e-12) ? (sumXY * data.size() - sumX * sumY) / denominator : 0.0;
        }
        
        return result;
    }
};

// Test result analyzer for comprehensive analysis
class TestAnalyzer {
public:
    struct TestSuiteAnalysis {
        std::string name;
        size_t totalTests;
        size_t passedTests;
        size_t failedTests;
        double passRate;
        double averageExecutionTime;
        double medianExecutionTime;
        double totalSetupTime;
        double totalTeardownTime;
        size_t memoryLeaks;
        double memoryUsage;
        
        // Category breakdown
        std::map<TestCategory, size_t> testsByCategory;
        std::map<TestCategory, double> passRateByCategory;
        
        // Time series of test results
        std::vector<double> executionTimes;
        std::vector<std::string> failures;
    };
    
    // Analyze a collection of test results
    static TestSuiteAnalysis analyzeTests(const std::vector<TestResult>& results) {
        TestSuiteAnalysis analysis;
        
        if (results.empty()) return analysis;
        
        analysis.totalTests = results.size();
        
        for (const auto& result : results) {
            switch (result.status) {
                case TestStatus::Passed: analysis.passedTests++; break;
                case TestStatus::Failed: analysis.failedTests++; analysis.failures.push_back(result.name); break;
                case TestStatus::Skipped: analysis.totalTests--; break; // Skipped tests don't count in total
                default: break;
            }
            
            analysis.executionTimes.push_back(result.executionTime.count() * 1e-9); // Convert to seconds
            analysis.testsByCategory[result.category]++;
        }
        
        analysis.passRate = (analysis.totalTests > 0) ? 
            (static_cast<double>(analysis.passedTests) / analysis.totalTests) * 100.0 : 0.0;
        
        // Calculate statistics for execution times
        if (!analysis.executionTimes.empty()) {
            DistributionAnalysis timeStats = StatisticalAnalyzer::analyzeDistribution(analysis.executionTimes);
            analysis.averageExecutionTime = timeStats.mean;
            analysis.medianExecutionTime = timeStats.median;
        }
        
        return analysis;
    }
    
    // Generate comprehensive test report
    static std::string generateTestReport(const TestSuiteAnalysis& analysis) {
        std::stringstream ss;
        ss << "Test Suite Analysis Report" << std::endl;
        ss << "==========================" << std::endl;
        ss << "Suite: " << analysis.name << std::endl;
        ss << "Total Tests: " << analysis.totalTests << std::endl;
        ss << "Passed: " << analysis.passedTests << std::endl;
        ss << "Failed: " << analysis.failedTests << std::endl;
        ss << std::fixed << std::setprecision(2);
        ss << "Pass Rate: " << analysis.passRate << "%" << std::endl;
        ss << "Average Execution Time: " << analysis.averageExecutionTime * 1000 << "ms" << std::endl;
        ss << "Median Execution Time: " << analysis.medianExecutionTime * 1000 << "ms" << std::endl;
        ss << "Total Setup Time: " << analysis.totalSetupTime << std::endl;
        ss << "Total Teardown Time: " << analysis.totalTeardownTime << std::endl;
        ss << "Memory Leaks Detected: " << analysis.memoryLeaks << std::endl;
        ss << "Memory Usage: " << analysis.memoryUsage << "MB" << std::endl;
        ss << std::endl;
        
        ss << "Category Breakdown:" << std::endl;
        for (const auto& pair : analysis.testsByCategory) {
            double categoryRate = (pair.second > 0) ? 
                (static_cast<double>(analysis.passedTests) / pair.second) * 100.0 : 0.0;
            ss << "  " << static_cast<int>(pair.first) << ": " << pair.second 
               << " tests (" << categoryRate << "% pass rate)" << std::endl;
        }
        
        if (!analysis.failures.empty()) {
            ss << std::endl << "Failed Tests:" << std::endl;
            for (const auto& failure : analysis.failures) {
                ss << "  - " << failure << std::endl;
            }
        }
        
        return ss.str();
    }
    
    // Identify performance bottlenecks
    static std::vector<std::string> identifyPerformanceIssues(const TestSuiteAnalysis& analysis) {
        std::vector<std::string> issues;
        
        if (analysis.averageExecutionTime > 1.0) { // More than 1 second
            issues.push_back("High average execution time: " + 
                           std::to_string(analysis.averageExecutionTime * 1000) + "ms");
        }
        
        if (analysis.memoryUsage > 100) { // More than 100MB
            issues.push_back("High memory usage: " + std::to_string(analysis.memoryUsage) + "MB");
        }
        
        if (analysis.memoryLeaks > 0) {
            issues.push_back(std::to_string(analysis.memoryLeaks) + " memory leaks detected");
        }
        
        // Check category-specific issues
        for (const auto& pair : analysis.testsByCategory) {
            if (pair.second >= 5) { // 5 or more tests in category
                double categoryRate = (static_cast<double>(analysis.passedTests) / pair.second) * 100.0;
                if (categoryRate < 50.0) {
                    issues.push_back("Low pass rate in category " + 
                                   std::to_string(static_cast<int>(pair.first)) + 
                                   ": " + std::to_string(categoryRate) + "%");
                }
            }
        }
        
        return issues;
    }
    
    // Generate visualization data (CSV format)
    static bool generateVisualizationData(const TestSuiteAnalysis& analysis, 
                                          const std::string& filename) {
        std::ofstream file(filename);
        if (!file) return false;
        
        file << "Metric,Value\n";
        file << "total_tests," << analysis.totalTests << "\n";
        file << "passed_tests," << analysis.passedTests << "\n";
        file << "failed_tests," << analysis.failedTests << "\n";
        file << "pass_rate," << analysis.passRate << "\n";
        file << "avg_execution_time_ms," << analysis.averageExecutionTime * 1000 << "\n";
        file << "median_execution_time_ms," << analysis.medianExecutionTime * 1000 << "\n";
        file << "total_setup_time_ms," << analysis.totalSetupTime << "\n";
        file << "total_teardown_time_ms," << analysis.totalTeardownTime << "\n";
        file << "memory_leaks," << analysis.memoryLeaks << "\n";
        file << "memory_usage_mb," << analysis.memoryUsage << "\n";
        
        return file.good();
    }
};

// Test comparison utilities for regression testing
class RegressionAnalyzer {
public:
    struct RegressionReport {
        std::string testName;
        std::chrono::nanoseconds baselineTime;
        std::chrono::nanoseconds currentTime;
        double timeDifferenceRatio;
        bool isRegressed;
        std::string regressionDetails;
    };
    
    // Compare test results against baseline
    static RegressionReport compareWithBaseline(const TestResult& current,
                                                const TestResult& baseline,
                                                double regressionThreshold = 1.2) {
        RegressionReport report;
        report.testName = current.name;
        report.baselineTime = baseline.executionTime;
        report.currentTime = current.executionTime;
        
        if (baseline.executionTime.count() > 0) {
            report.timeDifferenceRatio = static_cast<double>(current.executionTime.count()) /
                                        static_cast<double>(baseline.executionTime.count());
        } else {
            report.timeDifferenceRatio = 1.0;
        }
        
        report.isRegressed = report.timeDifferenceRatio > regressionThreshold;
        
        std::stringstream ss;
        ss << "Baseline: " << baseline.executionTime.count() << "ns\n"
           << "Current: " << current.executionTime.count() << "ns\n"
           << "Ratio: " << std::fixed << std::setprecision(2) << report.timeDifferenceRatio << "x\n"
           << "Regression Threshold: " << regressionThreshold << "x";
        
        if (report.isRegressed) {
            ss << " - REGRESSION DETECTED";
        } else {
            ss << " - No regression";
        }
        
        report.regressionDetails = ss.str();
        return report;
    }
    
    // Generate regression report
    static std::string generateRegressionReport(const std::vector<RegressionReport>& reports) {
        std::stringstream ss;
        ss << "Performance Regression Analysis Report" << std::endl;
        ss << "=====================================" << std::endl;
        ss << "Generated at: " << std::chrono::system_clock::now() << std::endl;
        ss << std::endl;
        
        int totalTests = reports.size();
        int regressedTests = 0;
        double totalRegressionRatio = 0.0;
        
        for (const auto& report : reports) {
            ss << "Test: " << report.testName << std::endl;
            ss << "  Baseline Time: " << report.baselineTime.count() << "ns" << std::endl;
            ss << "  Current Time: " << report.currentTime.count() << "ns" << std::endl;
            ss << "  Ratio: " << std::fixed << std::setprecision(2) << report.timeDifferenceRatio << "x" << std::endl;
            ss << "  Status: " << (report.isRegressed ? "REGRESSED" : "No Regression") << std::endl;
            ss << std::endl;
            
            if (report.isRegressed) {
                regressedTests++;
                totalRegressionRatio += report.timeDifferenceRatio;
            }
        }
        
        ss << "Summary:" << std::endl;
        ss << "  Total Tests: " << totalTests << std::endl;
        ss << "  Regressed Tests: " << regressedTests << std::endl;
        ss << "  Regression Rate: " << std::fixed << std::setprecision(2) << 
           (totalTests > 0 ? (static_cast<double>(regressedTests) / totalTests) * 100.0 : 0.0) << "%" << std::endl;
        ss << "  Average Regression Ratio: " << std::fixed << std::setprecision(2) << 
           (regressedTests > 0 ? totalRegressionRatio / regressedTests : 0.0) << "x" << std::endl;
        
        return ss.str();
    }
};

}}} // namespace nlm::test
