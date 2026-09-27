#pragma once

/**
 * Plugin Performance Analyzer
 * 
 * Plugin system performance analysis for the NLM brain simulation project.
 * 
 * Features:
 * - Plugin loading performance analysis
 * * Plugin execution performance analysis
 * - Plugin resource usage analysis
 * - Plugin integration performance analysis
 * - Plugin optimization recommendations
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class PluginPerformanceAnalyzer {
public:
    /**
     * Plugin performance metrics
     */
    struct PluginMetrics {
        std::string plugin_name;
        double load_time; // seconds
        double unload_time; // seconds
        double execution_time; // seconds
        double resource_usage; // memory usage in MB
        double cpu_usage; // percentage (0-100)
        int thread_count;
        double integration_efficiency; // 0.0 to 1.0
    };

    /**
     * Plugin system performance summary
     */
    struct PluginSystemSummary {
        double total_plugins;
        double total_load_time;
        double average_load_time;
        double total_execution_time;
        double average_execution_time;
        double average_resource_usage;
        double overall_system_efficiency;
    };

    PluginPerformanceAnalyzer();
    ~PluginPerformanceAnalyzer();
    
    /** Record plugin performance metrics */
    void recordPluginMetrics(const PluginMetrics& metrics);
    
    /** Get all plugin metrics */
    const std::vector<PluginMetrics>& getPluginMetrics() const { return plugin_metrics_; }
    
    /** Get plugin system summary */
    const PluginSystemSummary& getPluginSystemSummary() const { return system_summary_; }
    
    /** Calculate plugin performance score */
    double calculatePluginPerformanceScore(const PluginMetrics& metrics) const;
    
    /** Get plugins by performance */
    std::vector<std::string> getPluginsByPerformance(const std::string& sort_order = "best") const;
    
    /** Generate plugin performance recommendations */
    std::vector<std::string> generatePluginPerformanceRecommendations() const;
    
    /** Calculate plugin resource efficiency */
    double calculatePluginResourceEfficiency(const PluginMetrics& metrics) const;
    
    /** Clear plugin metrics */
    void clear();
    
    /** Enable/disable plugin loading analysis */
    void enablePluginLoadingAnalysis(bool enable) { analyze_plugin_loading_ = enable; }
    
    /** Enable/disable plugin execution analysis */
    void enablePluginExecutionAnalysis(bool enable) { analyze_plugin_execution_ = enable; }
    
private:
    std::vector<PluginMetrics> plugin_metrics_;
    PluginSystemSummary system_summary_;
    bool analyze_plugin_loading_;
    bool analyze_plugin_execution_;
    size_t max_plugin_metrics_;
};

} // namespace nlm
