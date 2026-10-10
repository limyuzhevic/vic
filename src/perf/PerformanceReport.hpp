#pragma once

#include "PerformanceProfiler.hpp"
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <fstream>
#include <iomanip>
#include <sstream>
#include <memory>

namespace nlm {

class PerformanceReport {
private:
    std::string reportTitle;
    std::chrono::high_resolution_clock::time_point generationTime;
    std::vector<PerformanceMetrics> performanceData;
    std::map<std::string, std::vector<double>> benchmarkData;
    std::map<std::string, std::string> metadata;
    
public:
    PerformanceReport(const std::string& title = "NLM Performance Report");
    ~PerformanceReport() = default;
    
    // Report generation
    void addPerformanceMetrics(const PerformanceMetrics& metrics);
    void addBenchmarkData(const std::string& benchmarkName, const std::vector<double>& data);
    void setMetadata(const std::string& key, const std::string& value);
    
    // Report export
    void exportToJSON(const std::string& filename) const;
    void exportToCSV(const std::string& filename) const;
    void exportToHTML(const std::string& filename) const;
    void exportToText(const std::string& filename) const;
    void exportToXML(const std::string& filename) const;
    
    // Report analysis
    double calculateAverageCpuUsage() const;
    double calculatePeakMemoryUsage() const;
    double calculateAverageStepTime() const;
    double calculatePerformanceTrend() const;
    bool detectPerformanceRegression(double threshold) const;
    
    // Report formatting
    std::string formatMetricsAsText(const PerformanceMetrics& metrics) const;
    std::string formatBenchmarkResults(const std::string& benchmarkName) const;
    std::string generateSummary() const;
    
    // Report utilities
    void clear();
    bool isEmpty() const;
    size_t getDataCount() const;
    std::chrono::high_resolution_clock::time_point getGenerationTime() const;
    std::string getReportTitle() const;
    
    // Statistical analysis
    double calculateStandardDeviation(const std::vector<double>& data) const;
    double calculateMean(const std::vector<double>& data) const;
    double calculateMedian(const std::vector<double>& data) const;
    double calculatePercentile(const std::vector<double>& data, double percentile) const;
    
    // Performance comparison
    void compareWithPreviousReport(const PerformanceReport& previousReport) const;
    void identifyPerformanceTrends() const;
    void generatePerformanceAlerts() const;
    
    // Dashboard generation
    void generateDashboardHTML(const std::string& filename) const;
    void generateMetricsDashboard(const std::string& filename) const;
    
private:
    std::string escapeXML(const std::string& input) const;
    std::string escapeJSON(const std::string& input) const;
    std::string formatDuration(double seconds) const;
    std::string formatBytes(size_t bytes) const;
    std::string formatPercentage(double value) const;
};

} // namespace nlm