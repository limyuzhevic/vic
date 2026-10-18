#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <unordered_map>
#include <sstream>
#include <iomanip>

namespace nlm {

struct PerformanceMetric {
    std::string name;
    double value;
    std::string unit;
    std::chrono::system_clock::time_point timestamp;
    std::string category; // timing, memory, cpu, io, events
    std::string operation; // specific operation being measured
};

struct PerformanceReport {
    std::string title;
    std::string description;
    std::chrono::system_clock::time_point generationTime;
    std::vector<PerformanceMetric> metrics;
    std::unordered_map<std::string, double> summaryStats;
    std::vector<std::string> recommendations;
};

class PerformanceReportGenerator {
public:
    PerformanceReportGenerator();
    virtual ~PerformanceReportGenerator();
    
    // Report generation
    void addMetric(const std::string& name, double value, const std::string& unit = "",
                  const std::string& category = "general",
                  const std::string& operation = "");
    void addTimeMeasurement(const std::string& operation, double time);
    void addMemoryMeasurement(const std::string& operation, size_t memory);
    void addCPUMeasurement(const std::string& operation, double cpuTime);
    void addEventCount(const std::string& eventName, size_t count);
    
    // Statistical analysis
    double calculateMean(const std::string& metricName) const;
    double calculateStandardDeviation(const std::string& metricName) const;
    double calculateMedian(const std::string& metricName) const;
    double calculatePercentile(const std::string& metricName, double percentile) const;
    
    // Report output
    void generateTextReport(std::ostream& output) const;
    void generateJSONReport(std::ostream& output) const;
    void generateHTMLReport(std::ostream& output) const;
    void generateCSVReport(std::ostream& output) const;
    
    // File output
    bool generateTextReport(const std::string& filename) const;
    bool generateJSONReport(const std::string& filename) const;
    bool generateHTMLReport(const std::string& filename) const;
    bool generateCSVReport(const std::string& filename) const;
    
    // Performance analysis
    std::vector<std::string> identifyPerformanceTrends() const;
    std::vector<std::string> detectAnomalies() const;
    std::vector<std::string> generateOptimizationRecommendations() const;
    std::string getPerformanceSummary() const;
    
    // Report metadata
    void setTitle(const std::string& title);
    void setDescription(const std::string& description);
    std::string getTitle() const { return title; }
    std::string getDescription() const { return description; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string title;
    std::string description;
    mutable std::mutex mutex_;
};

} // namespace nlm
