#pragma once

/**
 * Profiler report generator for NLM (Neural Learning Machine)
 * Report generation and analysis for profiling data
 */

#include <memory>
#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <sstream>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <atomic>
#include <algorithm>
#include <numeric>
#include <unordered_map>
#include <functional>
#include <set>
#include <cmath>

namespace nlm {

// Forward declarations
class Profiler;
class TimingProfiler;
class MemoryProfiler;
class CPUMonitor;
class IORMonitor;
class EventTracer;
class BenchmarkSuite;

/**
 * Performance analysis result
 */
struct PerformanceAnalysis {
    std::string analysisType;
    std::string analysisName;
    std::chrono::steady_clock::time_point timestamp;
    std::map<std::string, double> metrics; // Key performance metrics
    std::vector<std::string> recommendations; // Optimization recommendations
    std::vector<std::string> warnings; // Performance warnings
    double overallScore; // Overall performance score (0-1)
    bool hasIssues; // Whether any issues were found
    
    PerformanceAnalysis(const std::string& type, const std::string& name)
        : analysisType(type), analysisName(name), overallScore(0.0), hasIssues(false) {
        timestamp = std::chrono::steady_clock::now();
    }
};

/**
 * Optimization suggestion
 */
struct OptimizationSuggestion {
    std::string suggestionId;
    std::string category; // "cpu", "memory", "io", "event", "system"
    std::string description;
    double potentialImprovement; // Percentage improvement potential
    double effortRequired; // Effort required (1-10, where 10 is high effort)
    double priority; // Priority (1-10, where 10 is high priority)
    std::string implementationSteps;
    std::string expectedResults;
    
    OptimizationSuggestion(const std::string& id, const std::string& category,
                          const std::string& desc, double improvement, double effort)
        : suggestionId(id), category(category), description(desc),
          potentialImprovement(improvement), effortRequired(effort),
          priority(0.0) {
        priority = (improvement * 10.0) / effortRequired; // Simple priority calculation
    }
};

/**
 * Profiler report configuration
 */
struct ProfilerReportConfig {
    // Report generation
    bool enablePerformanceReports = true;
    bool enableMemoryReports = true;
    bool enableCPUMonitoringReports = true;
    bool enableIORMonitoringReports = true;
    bool enableEventReports = true;
    bool enableBenchmarkReports = true;
    
    // Report formats
    bool generateTextReports = true;
    bool generateJSONReports = true;
    bool generateHTMLReports = true;
    bool generateCSVReports = true;
    bool generateExcelReports = false;
    
    // Analysis options
    bool enablePerformanceAnalysis = true;
    bool enableTrendAnalysis = true;
    bool enableCorrelationAnalysis = true;
    bool enablePatternDetection = true;
    bool enableOptimizationSuggestions = true;
    
    // Report content
    bool includeRawData = false;
    bool includeStatisticalAnalysis = true;
    bool includeComparisons = true;
    bool includeRecommendations = true;
    
    // Integration
    bool integrateWithProfiler = true;
    bool integrateWithPhase6 = true;
    
    ProfilerReportConfig() = default;
};

/**
 * Profiler report generator class
 * Report generation and analysis for profiling data
 */
class ProfilerReport {
public:
    ProfilerReport(const ProfilerReportConfig& config = ProfilerReportConfig());
    ~ProfilerReport();
    
    // Report generation
    bool generatePerformanceReports(const std::string& sessionName = "") const;
    bool generateMemoryReports(const std::string& sessionName = "") const;
    bool generateCPUMonitoringReports(const std::string& sessionName = "") const;
    bool generateIORMonitoringReports(const std::string& sessionName = "") const;
    bool generateEventReports(const std::string& sessionName = "") const;
    bool generateBenchmarkReports(const std::string& sessionName = "") const;
    
    bool generateComprehensiveReport(const std::string& filename = "") const;
    bool generateExecutiveSummary(const std::string& filename = "") const;
    bool generateDetailedAnalysisReport(const std::string& filename = "") const;
    bool generateOptimizationReport(const std::string& filename = "") const;
    
    // Analysis
    std::vector<PerformanceAnalysis> analyzePerformance(const std::string& sessionName = "") const;
    std::vector<OptimizationSuggestion> analyzeOptimizationOpportunities(const std::string& sessionName = "") const;
    bool performTrendAnalysis(const std::string& sessionName = "") const;
    bool performCorrelationAnalysis(const std::string& sessionName = "") const;
    
    // File operations
    bool exportToJSON(const std::string& filename = "") const;
    bool exportToCSV(const std::string& filename = "") const;
    bool exportToText(const std::string& filename = "") const;
    bool exportToHTML(const std::string& filename = "") const;
    
    // Integration
    void setProfiler(Profiler* profiler) { parentProfiler = profiler; }
    
    // Prevent copy construction and assignment
    ProfilerReport(const ProfilerReport&) = delete;
    ProfilerReport& operator=(const ProfilerReport&) = delete;
    
private:
    // Configuration
    ProfilerReportConfig config_;
    
    // Session management
    std::string currentSessionName;
    
    // Parent profiler reference
    Profiler* parentProfiler;
    
    // Thread safety
    mutable std::mutex mutex_;
    
    // File and directory management
    std::string outputDir;
    
    // Internal helper methods
    void initializeReportDirectory();
    std::string getDefaultFilename(const std::string& extension, const std::string& sessionName = "") const;
    bool validateConfiguration() const;
    void generatePerformanceReport(const std::string& filename, const std::string& sessionName = "") const;
    void generateMemoryReport(const std::string& filename, const std::string& sessionName = "") const;
    void generateCPUMonitoringReport(const std::string& filename, const std::string& sessionName = "") const;
    void generateIORMonitoringReport(const std::string& filename, const std::string& sessionName = "") const;
    void generateEventReport(const std::string& filename, const std::string& sessionName = "") const;
    void generateBenchmarkReport(const std::string& filename, const std::string& sessionName = "") const;
    void generateExecutiveSummary(const std::string& filename) const;
    void generateDetailedAnalysisReport(const std::string& filename) const;
    void generateOptimizationReport(const std::string& filename) const;
    double calculatePerformanceScore(const std::map<std::string, double>& metrics) const;
    std::string formatRecommendation(const OptimizationSuggestion& suggestion) const;
    bool detectPerformanceTrends(const std::string& sessionName) const;
    bool analyzeEventCorrelation(const std::string& sessionName) const;
    std::vector<OptimizationSuggestion> generateOptimizationSuggestions(const std::string& sessionName) const;
    std::vector<std::string> analyzePerformanceBottlenecks(const std::string& sessionName) const;
};

} // namespace nlm
