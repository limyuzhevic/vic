#pragma once

/**
 * Anomaly Detector
 * 
 * Automatic anomaly detection for the NLM brain simulation project.
 * 
 * Features:
 * - Statistical anomaly detection
 * - Pattern-based anomaly detection
 * - Anomaly trend analysis
 * - Anomaly severity classification
 * - Anomaly alerting
 */

#include <chrono>
#include <string>
#include <vector>
#include <unordered_map>
#include <numeric>
#include <algorithm>

namespace nlm {

class AnomalyDetector {
public:
    /**
     * Anomaly record
     */
    struct Anomaly {
        std::string anomaly_type; // "statistical", "pattern", "trend"
        std::string component_name;
        double anomaly_score; // 0.0 to 1.0
        double deviation_from_mean; // Standard deviations from mean
        std::chrono::high_resolution_clock::time_point timestamp;
        std::string description;
        bool is_resolved;
        double severity; // 0.0 to 1.0
    };

    /**
     * Statistical metrics for anomaly detection
     */
    struct StatisticalMetrics {
        double mean;
        double standard_deviation;
        double minimum;
        double maximum;
        double median;
        double variance;
    };

    AnomalyDetector();
    ~AnomalyDetector();
    
    /** Analyze data for anomalies */
    void analyzeForAnomalies(const std::string& component_name, 
                            const std::vector<double>& data_points,
                            const StatisticalMetrics& metrics);
    
    /** Detect statistical anomalies */
    std::vector<Anomaly> detectStatisticalAnomalies(const std::string& component_name,
                                                    const std::vector<double>& data_points,
                                                    const StatisticalMetrics& metrics,
                                                    double threshold = 2.0) const;
    
    /** Detect pattern anomalies */
    std::vector<Anomaly> detectPatternAnomalies(const std::string& component_name,
                                                const std::vector<std::vector<double>>& patterns,
                                                double threshold = 0.8) const;
    
    /** Get all detected anomalies */
    const std::vector<Anomaly>& getAnomalies() const { return anomalies_; }
    
    /** Get anomalies by component */
    std::vector<Anomaly> getAnomaliesForComponent(const std::string& component_name) const;
    
    /** Get anomalies by type */
    std::vector<Anomaly> getAnomaliesByType(const std::string& anomaly_type) const;
    
    /** Mark anomaly as resolved */
    void markAnomalyResolved(const std::string& component_name, const std::string& description);
    
    /** Get unresolved anomaly count */
    int getUnresolvedAnomalyCount() const;
    
    /** Calculate anomaly severity */
    double calculateAnomalySeverity(double deviation, double threshold) const;
    
    /** Clear all anomalies */
    void clear();
    
    /** Enable/disable anomaly detection */
    void setEnabled(bool enabled) { enabled_ = enabled; }
    
    /** Check if anomaly detection is enabled */
    bool isEnabled() const { return enabled_; }
    
private:
    std::vector<Anomaly> anomalies_;
    bool enabled_;
    std::unordered_map<std::string, StatisticalMetrics> component_metrics_;
    size_t max_anomalies_;
};

} // namespace nlm
