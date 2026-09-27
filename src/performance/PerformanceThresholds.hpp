#pragma once

/**
 * Performance Thresholds
 * 
 * Performance threshold configuration for the NLM brain simulation project.
 * 
 * Features:
 * - Configurable performance thresholds
 * - Component-specific thresholds
 * * Threshold violation tracking
 * - Rolling threshold adaptation
 * - Alert threshold configuration
 */

#include <string>
#include <unordered_map>
#include <vector>

namespace nlm {

class PerformanceThresholds {
public:
    /**
     * Performance threshold configuration
     */
    struct ThresholdConfig {
        std::string component_name;
        double cpu_threshold; // Percentage (0-100)
        double memory_threshold; // Percentage (0-100)
        double disk_io_threshold; // MB/s
        double network_io_threshold; // MB/s
        double response_time_threshold; // seconds
        double memory_fragmentation_threshold; // 0.0 to 1.0
        bool enabled; // Whether thresholds are active
    };

    PerformanceThresholds();
    ~PerformanceThresholds();
    
    /** Add threshold configuration */
    void addThreshold(const ThresholdConfig& config);
    
    /** Get threshold configuration for component */
    const ThresholdConfig* getThreshold(const std::string& component_name) const;
    
    /** Check if component is within thresholds */
    bool isWithinThresholds(const std::string& component_name, double cpu_usage,
                           double memory_usage, double disk_io, double network_io,
                           double response_time, double memory_fragmentation) const;
    
    /** Get all threshold configurations */
    const std::vector<ThresholdConfig>& getAllThresholds() const { return thresholds_; }
    
    /** Clear all thresholds */
    void clear();
    
    /** Set default thresholds for component */
    void setDefaultThresholds(const std::string& component_name, double cpu_threshold,
                             double memory_threshold, double disk_io_threshold,
                             double network_io_threshold, double response_time_threshold,
                             double memory_fragmentation_threshold);
    
    /** Get threshold violation count */
    int getThresholdViolationCount(const std::string& component_name) const;
    
    /** Get total threshold violation count */
    int getTotalThresholdViolationCount() const { return total_violations_; }
    
private:
    std::vector<ThresholdConfig> thresholds_;
    std::unordered_map<std::string, int> violation_counts_;
    int total_violations_;
};

} // namespace nlm
