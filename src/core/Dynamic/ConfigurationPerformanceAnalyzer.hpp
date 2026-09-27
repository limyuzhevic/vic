#pragma once

/**
 * Configuration Performance Analyzer
 * 
 * Configuration parsing and application performance analysis for the NLM brain simulation project.
 * 
 * Features:
 * - Configuration parsing performance analysis
 * - Configuration application performance analysis
 * - Configuration validation performance analysis
 * - Configuration caching performance analysis
 * - Configuration optimization recommendations
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>

namespace nlm {

class ConfigurationPerformanceAnalyzer {
public:
    /**
     * Configuration performance metrics
     */
    struct ConfigurationMetrics {
        std::string configuration_name;
        double parsing_time; // seconds
        double application_time; // seconds
        double validation_time; // seconds
        double caching_time; // seconds
        size_t configuration_size;
        int parameter_count;
        double total_configuration_efficiency; // 0.0 to 1.0
    };

    /**
     * Configuration system performance summary
     */
    struct ConfigurationSystemSummary {
        double total_configurations;
        double total_parsing_time;
        double average_parsing_time;
        double total_application_time;
        double average_application_time;
        double total_validation_time;
        double average_validation_time;
        double overall_system_efficiency;
    };

    ConfigurationPerformanceAnalyzer();
    ~ConfigurationPerformanceAnalyzer();
    
    /** Record configuration performance metrics */
    void recordConfigurationMetrics(const ConfigurationMetrics& metrics);
    
    /** Get all configuration metrics */
    const std::vector<ConfigurationMetrics>& getConfigurationMetrics() const { return configuration_metrics_; }
    
    /** Get configuration system summary */
    const ConfigurationSystemSummary& getConfigurationSystemSummary() const { return system_summary_; }
    
    /** Calculate configuration performance score */
    double calculateConfigurationPerformanceScore(const ConfigurationMetrics& metrics) const;
    
    /** Get configurations by performance */
    std::vector<std::string> getConfigurationsByPerformance(const std::string& sort_order = "best") const;
    
    /** Generate configuration performance recommendations */
    std::vector<std::string> generateConfigurationPerformanceRecommendations() const;
    
    /** Calculate configuration parsing efficiency */
    double calculateConfigurationParsingEfficiency(const ConfigurationMetrics& metrics) const;
    
    /** Clear configuration metrics */
    void clear();
    
    /** Enable/disable parsing performance analysis */
    void enableParsingAnalysis(bool enable) { analyze_parsing_ = enable; }
    
    /** Enable/disable validation performance analysis */
    void enableValidationAnalysis(bool enable) { analyze_validation_ = enable; }
    
private:
    std::vector<ConfigurationMetrics> configuration_metrics_;
    ConfigurationSystemSummary system_summary_;
    bool analyze_parsing_;
    bool analyze_validation_;
    size_t max_configuration_metrics_;
};

} // namespace nlm
