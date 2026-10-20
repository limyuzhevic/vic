#pragma once

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <mutex>
#include <shared_mutex>
#include <unordered_map>
#include <optional>
#include <limits>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations for types used in MetricsRecorder
class Brain;
class OptimizedBrain;
struct DataPoint;
struct MetricRecord;

// Time series data point
struct DataPoint {
    std::chrono::high_resolution_clock::time_point timestamp;
    double value;
    std::string label;
    
    DataPoint(double val, const std::string& lbl = "") : value(val), label(lbl) {
        timestamp = std::chrono::high_resolution_clock::now();
    }
};

// Metric with historical data
struct MetricRecord {
    std::string name;
    std::vector<DataPoint> data;
    double minValue = std::numeric_limits<double>::max();
    double maxValue = std::numeric_limits<double>::lowest();
    double lastValue = 0.0;
    size_t dataPointCount = 0;
    
    void addDataPoint(double value, const std::string& label = "") {
        data.emplace_back(value, label);
        lastValue = value;
        minValue = std::min(minValue, value);
        maxValue = std::max(maxValue, value);
        dataPointCount++;
    }
    
    size_t getDataCount() const { return data.size(); }
    std::optional<double> getLatestValue() const {
        return data.empty() ? std::optional<double>() : std::optional<double>(data.back().value);
    }
};

// Metrics recorder for historical data collection
class MetricsRecorder {
public:
    MetricsRecorder();
    ~MetricsRecorder();
    
    // Disable copying
    MetricsRecorder(const MetricsRecorder&) = delete;
    MetricsRecorder& operator=(const MetricsRecorder&) = delete;
    
    // Record a metric value
    void record(const std::string& name, double value, const std::string& label = "");
    
    // Record neural system metrics
    void recordNeuralMetrics(const Brain& brain, Timestamp currentTime, SimulationStep currentStep);
    
    // Record performance metrics
    void recordPerformanceMetrics(const OptimizedBrain* optimizedBrain = nullptr);
    
    // Record spike metrics
    void recordSpikeMetrics(const std::vector<DetailedSpikeEvent>& spikes);
    
    // Get metric by name
    const MetricRecord* getMetric(const std::string& name) const;
    
    // Get all metric names
    std::vector<std::string> getMetricNames() const;
    
    // Get historical data for a metric
    std::vector<DataPoint> getHistoricalData(const std::string& name, size_t limit = 0) const;
    
    // Get current metric value
    std::optional<double> getCurrentValue(const std::string& name) const;
    
    // Get statistics for a metric
    struct MetricStats {
        double mean = 0.0;
        double stdDev = 0.0;
        double min = 0.0;
        double max = 0.0;
        size_t count = 0;
        double lastValue = 0.0;
        
        std::string toString() const {
            return "mean=" + std::to_string(mean) + 
                   ", stddev=" + std::to_string(stdDev) +
                   ", min=" + std::to_string(min) +
                   ", max=" + std::to_string(max) +
                   ", count=" + std::to_string(count);
        }
    };
    
    MetricStats getStats(const std::string& name) const;
    
    // Clear all data
    void clear();
    
    // Clear old data (older than specified duration)
    void clearOldData(const std::chrono::milliseconds& age);
    
    // Get data summary
    std::string summary() const;
    
    // Enable/disable recording
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    // Get recording interval
    void setRecordingInterval(const std::chrono::milliseconds& interval) { 
        recordingInterval_ = interval; 
    }
    
    std::chrono::milliseconds getRecordingInterval() const { 
        return recordingInterval_; 
    }
    
    // Get the singleton instance
    static MetricsRecorder& getInstance() {
        static MetricsRecorder instance;
        return instance;
    }
    
    // Global recording function
    static void recordGlobal(const std::string& name, double value, const std::string& label = "");
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    bool enabled_;
    std::chrono::milliseconds recordingInterval_;
    mutable std::shared_mutex mutex_;
};

} // namespace nlm

