// Metric Monitor Header
#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <thread>

namespace nlm {

/**
 * Metric monitor for NLM
 */
class MetricMonitor {
public:
    struct MetricData {
        std::string name;
        double value;
        std::string category;
        std::chrono::high_resolution_clock::time_point timestamp;
        std::thread::id threadId;
    };
    
    struct MetricStats {
        double value;
        size_t count;
        std::chrono::high_resolution_clock::time_point lastUpdate;
    };
    
    MetricMonitor();
    ~MetricMonitor();
    
    /**
     * Add metric
     */
    void addMetric(const std::string& name, double value, const std::string& category = "");
    
    /**
     * Update existing metric
     */
    void updateMetric(const std::string& name, double value);
    
    /**
     * Get all metrics
     */
    const std::vector<MetricData>& getMetrics() const;
    
    /**
     * Get metric statistics
     */
    const std::map<std::string, MetricStats>& getMetricStats() const;
    
    /**
     * Generate report
     */
    void generateReport(const std::string& filePath);
    
private:
    static std::string getCurrentTimestamp();
    std::string getTimestampString(const std::chrono::high_resolution_clock::time_point& timestamp);
    
    mutable std::mutex _mutex;
    std::vector<MetricData> _metrics;
    std::map<std::string, MetricStats> _metricStats;
};

} // namespace nlm