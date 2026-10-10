#pragma once

#include <string>
#include <vector>
#include <chrono>
#include <map>
#include <algorithm>
#include <numeric>
#include <iomanip>
#include <sstream>

namespace nlm {

// Performance analysis structures
struct PerformanceAnalysis {
    double averageLatencyMs;
    double p95LatencyMs;
    double p99LatencyMs;
    double minLatencyMs;
    double maxLatencyMs;
    double throughputOpsPerSecond;
    double efficiencyScore;
    std::vector<double> recentTrends;
    std::vector<std::string> bottlenecks;
};

struct PerformanceReport {
    std::string title;
    std::string timestamp;
    double durationSeconds;
    PerformanceAnalysis systemAnalysis;
    PerformanceAnalysis brainAnalysis;
    PerformanceAnalysis integrationAnalysis;
    PerformanceAnalysis plasticityAnalysis;
    std::vector<std::string> recommendations;
    std::string summary;
    
    std::string generateTextReport() const;
    std::string generateJSONReport() const;
};

class PerformanceReporter {
public:
    PerformanceReporter();
    ~PerformanceReporter();
    
    // Generate comprehensive performance report
    PerformanceReport generateReport(const std::string& title = "Performance Report",
                                     const std::string& format = "text") const;
    
    // Generate real-time dashboard
    std::string generateDashboard(const std::string& format = "text") const;
    
    // Save report to file
    bool saveReport(const PerformanceReport& report, const std::string& filepath) const;
    
    // Configure reporter
    void setReportFormat(const std::string& format);
    void setIncludeHistoricalData(bool include);
    void setMaxHistoricalEntries(size_t maxEntries);
    
    // Alert and bottleneck analysis
    std::vector<std::string> analyzeBottlenecks(const PerformanceReport& report) const;
    std::vector<std::string> generateOptimizationRecommendations(const PerformanceReport& report) const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm