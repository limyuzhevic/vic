#pragma once

#include <chrono>
#include <string>
#include <vector>
#include <map>
#include <memory>
#include <mutex>
#include <atomic>
#include <unordered_map>
#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <numeric>
#include <set>
#include <fstream>
#include <iostream>

namespace nlm {

// Performance comparison data structures
struct PerformanceComparisonResult {
    std::string comparison_id;
    std::chrono::steady_clock::time_point timestamp;
    std::map<std::string, std::vector<double>> configuration_metrics;
    std::map<std::string, double> configuration_scores;
    std::map<std::string, std::map<std::string, double>> metric_improvements;
    std::map<std::string, std::map<std::string, double>> metric_degradations;
    std::string best_configuration;
    std::string recommendation;
    double overall_confidence;
};

struct MemoryUsageAnalysis {
    std::string analysis_id;
    std::chrono::steady_clock::time_point timestamp;
    std::vector<double> memory_usage_over_time;
    double peak_memory_mb;
    double average_memory_mb;
    double memory_growth_rate_mb_per_step;
    bool memory_leak_detected;
    double memory_leak_rate;
    std::map<std::string, double> memory_patterns;
    std::string leak_source_function;
    std::string memory_optimization_suggestion;
};

struct SpikeEventAnalysis {
    std::string analysis_id;
    std::chrono::steady_clock::time_point timestamp;
    std::vector<uint64_t> spike_timestamps;
    std::vector<double> spike_amplitudes;
    std::vector<double> inter_spike_intervals;
    double spike_frequency_hz;
    double average_spike_amplitude;
    double spike_amplitude_std;
    std::string spike_pattern_type;  // "regular", "burst", "irregular", "pause"
    double burst_detection_score;
    std::vector<std::pair<uint64_t, double>> region_spike_analysis;
    std::string pattern_classification_notes;
    std::string spike_event_optimization_suggestion;
};

struct SystemScalingAnalysis {
    std::string analysis_id;
    std::chrono::steady_clock::time_point timestamp;
    std::vector<size_t> neuron_counts;
    std::vector<double> performance_scales;
    std::vector<double> memory_scales;
    double scaling_efficiency;
    double scalability_score;
    std::map<std::string, double> scaling_factors;
    std::string scaling_recommendation;
    std::string hardware_scaling_guidance;
};

struct ProfilingReport {
    std::string report_id;
    std::chrono::steady_clock::time_point timestamp;
    std::string report_title;
    std::string report_type;  // "performance", "memory", "spike", "system"
    std::string analysis_scope;
    std::string executive_summary;
    std::string key_findings;
    std::string methodology;
    std::map<std::string, std::string> analysis_results;
    std::vector<std::string> recommendations;
    std::string generated_by;
    double confidence_level;
};

struct PerformanceProfiler {
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Disable copying and moving
    PerformanceProfiler(const PerformanceProfiler&) = delete;
    PerformanceProfiler& operator=(const PerformanceProfiler&) = delete;
    PerformanceProfiler(PerformanceProfiler&&) = delete;
    PerformanceProfiler& operator=(PerformanceProfiler&&) = delete;
    
    // Configuration comparison
    PerformanceComparisonResult compareConfigurations(
        const std::vector<std::vector<PerformanceMetrics>>& datasets,
        const std::vector<std::string>& config_names,
        const std::vector<std::string>& metric_names = {});
    
    PerformanceComparisonResult compareConfigurationsWithAdvancedAnalysis(
        const std::vector<std::vector<PerformanceMetrics>>& datasets,
        const std::vector<std::string>& config_names,
        const std::map<std::string, std::function<double(const PerformanceMetrics&)>>& custom_metrics);
    
    // Memory analysis
    MemoryUsageAnalysis analyzeMemoryUsage(const std::vector<PerformanceMetrics>& metrics,
                                          uint64_t analysis_steps = 1000);
    
    MemoryUsageAnalysis analyzeMemoryUsageWithDetails(
        const std::vector<PerformanceMetrics>& metrics,
        const std::map<std::string, std::function<double(const PerformanceMetrics&)>>& detailed_metrics);
    
    // Spike event analysis
    SpikeEventAnalysis analyzeSpikeEvents(const std::vector<PerformanceMetrics>& metrics,
                                         uint64_t analysis_steps = 1000);
    
    SpikeEventAnalysis analyzeSpikeEventsWithPatternDetection(
        const std::vector<PerformanceMetrics>& metrics,
        const std::map<std::string, std::function<double(const PerformanceMetrics&)>>& spike_metrics);
    
    // System scaling analysis
    SystemScalingAnalysis analyzeSystemScaling(
        const std::vector<std::pair<size_t, PerformanceMetrics>>& size_metric_pairs);
    
    SystemScalingAnalysis analyzeSystemScalingWithHardwareModel(
        const std::vector<std::pair<size_t, PerformanceMetrics>>& size_metric_pairs,
        const std::map<std::string, double>& hardware_characteristics);
    
    // Report generation
    ProfilingReport generatePerformanceReport(const PerformanceComparisonResult& comparison,
                                              const std::string& title = "Performance Comparison");
    
    ProfilingReport generateMemoryUsageReport(const MemoryUsageAnalysis& analysis,
                                              const std::string& title = "Memory Usage Analysis");
    
    ProfilingReport generateSpikeEventReport(const SpikeEventAnalysis& analysis,
                                             const std::string& title = "Spike Event Analysis");
    
    ProfilingReport generateSystemScalingReport(const SystemScalingAnalysis& analysis,
                                                const std::string& title = "System Scaling Analysis");
    
    // Data export
    bool exportComparisonReport(const PerformanceComparisonResult& result, const std::string& filepath) const;
    bool exportMemoryAnalysisReport(const MemoryUsageAnalysis& analysis, const std::string& filepath) const;
    bool exportSpikeEventReport(const SpikeEventAnalysis& analysis, const std::string& filepath) const;
    bool exportScalingReport(const SystemScalingAnalysis& analysis, const std::string& filepath) const;
    
    // Batch analysis
    std::vector<PerformanceComparisonResult> compareMultipleConfigurationsBatch(
        const std::vector<std::vector<std::vector<PerformanceMetrics>>>& all_datasets,
        const std::vector<std::vector<std::string>>& config_names_list);
    
    std::vector<MemoryUsageAnalysis> analyzeMemoryUsageBatch(
        const std::vector<std::vector<PerformanceMetrics>>& all_metrics,
        uint64_t analysis_steps = 1000);
    
    std::vector<SpikeEventAnalysis> analyzeSpikeEventsBatch(
        const std::vector<std::vector<PerformanceMetrics>>& all_metrics,
        uint64_t analysis_steps = 1000);
    
    // Statistical analysis
    struct StatisticalSummary {
        double mean = 0.0;
        double median = 0.0;
        double std_dev = 0.0;
        double min = 0.0;
        double max = 0.0;
        double variance = 0.0;
        std::vector<double> percentiles;  // 25th, 50th, 75th percentiles
        std::vector<double> quartiles;
    };
    
    StatisticalSummary calculateStatistics(const std::vector<double>& data) const;
    
    double calculateTrend(const std::vector<std::pair<uint64_t, double>>& data_points) const;
    double calculateCorrelation(const std::vector<double>& x, const std::vector<double>& y) const;
    
    // Anomaly detection
    struct AnomalyDetection {
        bool anomaly_detected = false;
        std::vector<size_t> anomaly_indices;
        std::vector<double> anomaly_scores;
        std::string anomaly_type;
        std::string anomaly_description;
        std::string suggested_action;
    };
    
    AnomalyDetection detectAnomalies(const std::vector<double>& data, double threshold = 2.0) const;
    
    // Pattern analysis
    struct PatternAnalysis {
        std::string pattern_type;
        double pattern_confidence;
        std::vector<size_t> pattern_indices;
        std::map<std::string, double> pattern_parameters;
        std::string pattern_interpretation;
    };
    
    PatternAnalysis analyzePatterns(const std::vector<double>& data,
                                   const std::vector<std::string>& pattern_types = {});
    
    // Configuration recommendations
    std::vector<std::string> generateConfigurationRecommendations(
        const PerformanceComparisonResult& comparison) const;
    
    std::vector<std::string> generateMemoryOptimizationRecommendations(
        const MemoryUsageAnalysis& analysis) const;
    
    std::vector<std::string> generateSpikeEventRecommendations(
        const SpikeEventAnalysis& analysis) const;
    
    std::vector<std::string> generateScalingRecommendations(
        const SystemScalingAnalysis& analysis) const;
    
    // Validation and verification
    bool validateAnalysisResults(const PerformanceComparisonResult& result) const;
    bool validateAnalysisResults(const MemoryUsageAnalysis& result) const;
    bool validateAnalysisResults(const SpikeEventAnalysis& result) const;
    bool validateAnalysisResults(const SystemScalingAnalysis& result) const;
    
    // Statistics helpers
    static double calculateMean(const std::vector<double>& data);
    static double calculateMedian(const std::vector<double>& data);
    static double calculateStandardDeviation(const std::vector<double>& data, double mean);
    static double calculateVariance(const std::vector<double>& data, double mean);
    static std::vector<double> calculatePercentiles(const std::vector<double>& data,
                                                   const std::vector<double>& percentiles);
    
    // Export helpers
    static std::string performanceComparisonResultToJSON(const PerformanceComparisonResult& result);
    static std::string memoryUsageAnalysisToJSON(const MemoryUsageAnalysis& analysis);
    static std::string spikeEventAnalysisToJSON(const SpikeEventAnalysis& analysis);
    static std::string systemScalingAnalysisToJSON(const SystemScalingAnalysis& analysis);
    
    // Configuration
    void setConfidenceLevel(double level) { confidence_level_ = level; }
    void setThresholdMultiplier(double multiplier) { threshold_multiplier_ = multiplier; }
    void enableAdvancedAnalysis(bool enable) { advanced_analysis_ = enable; }
    void setMaxReportSizeMB(size_t size_mb) { max_report_size_mb_ = size_mb; }
    
    // Statistics
    size_t getAnalysisCount() const { return analysis_history_.size(); }
    PerformanceComparisonResult getLastComparison() const;
    MemoryUsageAnalysis getLastMemoryAnalysis() const;
    SpikeEventAnalysis getLastSpikeAnalysis() const;
    SystemScalingAnalysis getLastScalingAnalysis() const;
    
private:
    mutable std::mutex analysis_mutex_;
    std::vector<PerformanceComparisonResult> comparison_history_;
    std::vector<MemoryUsageAnalysis> memory_analysis_history_;
    std::vector<SpikeEventAnalysis> spike_analysis_history_;
    std::vector<SystemScalingAnalysis> scaling_analysis_history_;
    std::atomic<uint64_t> next_analysis_id_ = 0;
    
    double confidence_level_ = 0.95;
    double threshold_multiplier_ = 2.0;
    bool advanced_analysis_ = false;
    size_t max_report_size_mb_ = 100;
    
    // Analysis algorithms
    PerformanceComparisonResult performAdvancedConfigurationComparison(
        const std::vector<std::vector<PerformanceMetrics>>& datasets,
        const std::vector<std::string>& config_names,
        const std::vector<std::string>& metric_names);
    
    MemoryUsageAnalysis performDeepMemoryAnalysis(
        const std::vector<PerformanceMetrics>& metrics,
        const std::map<std::string, std::function<double(const PerformanceMetrics&)>>& detailed_metrics);
    
    SpikeEventAnalysis performAdvancedSpikeAnalysis(
        const std::vector<PerformanceMetrics>& metrics,
        const std::map<std::string, std::function<double(const PerformanceMetrics&)>>& spike_metrics);
    
    SystemScalingAnalysis performHardwareAwareScalingAnalysis(
        const std::vector<std::pair<size_t, PerformanceMetrics>>& size_metric_pairs,
        const std::map<std::string, double>& hardware_characteristics);
    
    // Statistical analysis helpers
    static std::vector<double> calculateQuartiles(const std::vector<double>& data);
    static double calculateInterquartileRange(const std::vector<double>& data);
    static std::vector<size_t> findOutliers(const std::vector<double>& data, double iqr_threshold);
    
    // Pattern analysis helpers
    static std::string detectPeriodicity(const std::vector<double>& data, double tolerance = 0.1);
    static std::string detectTrend(const std::vector<double>& data);
    static std::string detectSeasonality(const std::vector<double>& data, size_t period);
    
    // Export helpers
    static std::string profilingReportToJSON(const ProfilingReport& report);
};

} // namespace nlm
