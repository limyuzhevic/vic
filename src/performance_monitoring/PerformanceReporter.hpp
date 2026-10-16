#pragma once

/**
 * Performance Reporting System for NLM Framework
 * 
 * This system provides comprehensive performance reporting:
 * - Performance reporting and statistics
 * - Performance comparison tools
 * - Performance regression detection
 * - Performance optimization recommendations
 * - Performance baseline establishment
 * - Comparative analysis between different configurations
 * - Multiple output formats (text, JSON, CSV, HTML)
 * - Performance threshold alerts and warnings
 * - Performance optimization suggestions
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <fstream>
#include <thread>
#include <optional>
#include <functional>
#include <map>
#include "../core/Config/Config.hpp"
#include "PerformanceMonitor.hpp"
#include "PerformanceMetrics.hpp"
#include "PerformanceAnalysis.hpp"

namespace nlm {

/**
 * Performance report configuration
 */
struct ReportConfig {
    std::string title;
    std::string description;
    OutputFormat format;
    bool includePerformanceMetrics;
    bool includePerformanceProfile;
    bool includeAlerts;
    bool includeRecommendations;
    bool includeComparison;
    std::string baselineName;
    std::chrono::microseconds startTime;
    std::chrono::microseconds endTime;
    bool generateDetailedReports;
    bool includeFlamesGraphs;
    bool includeBottleneckAnalysis;
    bool includeHotPathAnalysis;
    bool includeMemoryAnalysis;
    bool includeCPUAnalysis;
    bool includeIOAnalysis;
};

/**
 * Performance comparison information
 */
struct PerformanceComparison {
    std::string configName;
    std::string baselineName;
    double performanceScore;
    std::unordered_map<PerformanceCategory, double> categoryScores;
    std::string grade;
    std::vector<std::string> improvements;
    std::vector<std::string> regressions;
    std::chrono::microseconds timestamp;
    std::string comparisonDetails;
};

/**
 * Performance reporting system
 */
class PerformanceReporter {
public:
    PerformanceReporter(std::shared_ptr<PerformanceMonitor> monitor,
                        std::shared_ptr<PerformanceMetrics> metrics,
                        std::shared_ptr<PerformanceAnalysis> analysis);
    ~PerformanceReporter();
    
    // Report generation
    bool generateReport(const std::string& filename, OutputFormat format = OutputFormat::JSON,
                       const std::string& baseline = "") const;
    bool generateSummaryReport(const std::string& filename, OutputFormat format = OutputFormat::Text) const;
    bool generateDetailedReport(const std::string& filename, OutputFormat format = OutputFormat::HTML) const;
    bool generateComparisonReport(const std::string& filename, const std::string& baseline,
                                 OutputFormat format = OutputFormat::Text) const;
    
    // Configuration
    void setConfig(const PerformanceConfig& config);
    PerformanceConfig getConfig() const;
    
    // Report configuration
    void setReportConfig(const ReportConfig& config);
    ReportConfig getReportConfig() const;
    
    // Report management
    void setReportTitle(const std::string& title);
    void setReportDescription(const std::string& description);
    void setIncludePerformanceMetrics(bool include);
    void setIncludePerformanceProfile(bool include);
    void setIncludeAlerts(bool include);
    void setIncludeRecommendations(bool include);
    void setIncludeComparison(bool include);
    void setBaselineName(const std::string& baselineName);
    void setTimeRange(std::chrono::microseconds start, std::chrono::microseconds end);
    
    // Performance comparison
    bool establishBaseline(const std::string& baselineName);
    bool compareToBaseline(const std::string& baselineName) const;
    std::vector<PerformanceComparison> getPerformanceComparisons() const;
    
    // Export/import
    bool exportReport(const std::string& filename, OutputFormat format = OutputFormat::JSON) const;
    bool importReport(const std::string& filename);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Private constructor
    PerformanceReporter();
    
    // Helper functions
    std::string generateTextReport() const;
    std::string generateJSONReport() const;
    std::string generateCSVReport() const;
    std::string generateHTMLReport() const;
    
    std::string generateComparisonReport(const std::string& baselineName) const;
    std::string generateSummaryReport() const;
    std::string generateDetailedReport() const;
    
    std::string generateMetricSection() const;
    std::string generateProfileSection() const;
    std::string generateAlertSection() const;
    std::string generateRecommendationSection() const;
    std::string generateComparisonSection() const;
    std::string generateBottleneckSection() const;
    std::string generateHotPathSection() const;
    std::string generateMemorySection() const;
    std::string generateCPUSection() const;
    std::string generateIOSection() const;
};

} // namespace nlm

// Global performance reporter instance for convenience
nlm::PerformanceReporter& getGlobalPerformanceReporter();