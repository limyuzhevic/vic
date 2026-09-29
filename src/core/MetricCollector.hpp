#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <mutex>
#include <chrono>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace nlm {

class MetricCollector {
private:
    struct TimeSeriesPoint {
        std::chrono::duration<double> timestamp;
        double value;
        std::string component;
        std::unordered_map<std::string, std::string> tags;
    };

    struct MetricStats {
        double min = 0.0;
        double max = 0.0;
        double mean = 0.0;
        double variance = 0.0;
        double stddev = 0.0;
        double median = 0.0;
        int count = 0;
    };

    std::unordered_map<std::string, std::vector<TimeSeriesPoint>> timeSeriesData_;
    mutable std::mutex dataMutex_;
    size_t maxDataPoints_ = 1000000;
    std::chrono::duration<double> retentionPeriod_ = std::chrono::hours(24);
    bool enableRollingWindow_ = true;

public:
    MetricCollector() = default;

    // Core data collection
    void addMetric(const std::string& name, double value,
                  const std::string& component = "core",
                  const std::unordered_map<std::string, std::string>& tags = {}) {
        std::lock_guard<std::mutex> lock(dataMutex_);
        
        TimeSeriesPoint point;
        point.timestamp = std::chrono::high_resolution_clock::now();
        point.value = value;
        point.component = component;
        point.tags = tags;
        
        // Manage data size (rolling window)
        if (enableRollingWindow_ && timeSeriesData_[name].size() >= maxDataPoints_) {
            timeSeriesData_[name].erase(timeSeriesData_[name].begin());
        }
        
        timeSeriesData_[name].push_back(std::move(point));
    }

    // Time-series queries
    std::vector<TimeSeriesPoint> getTimeSeries(const std::string& name,
                                             std::chrono::duration<double> startTime,
                                             std::chrono::duration<double> endTime) {
        std::lock_guard<std::mutex> lock(dataMutex_);
        std::vector<TimeSeriesPoint> result;
        
        if (timeSeriesData_.find(name) == timeSeriesData_.end()) {
            return result;
        }
        
        const auto& data = timeSeriesData_[name];
        for (const auto& point : data) {
            if (point.timestamp >= startTime && point.timestamp <= endTime) {
                result.push_back(point);
            }
        }
        
        return result;
    }

    std::vector<TimeSeriesPoint> getTimeSeries(const std::string& name,
                                             std::chrono::duration<double> range) {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto startTime = endTime - range;
        return getTimeSeries(name, startTime, endTime);
    }

    // Statistical calculations
    MetricStats calculateStats(const std::string& name,
                               std::chrono::duration<double> range) {
        std::lock_guard<std::mutex> lock(dataMutex_);
        MetricStats stats;
        
        auto data = getTimeSeries(name, range);
        if (data.empty()) return stats;
        
        stats.count = data.size();
        
        // Extract values for calculation
        std::vector<double> values;
        for (const auto& point : data) {
            values.push_back(point.value);
        }
        
        // Calculate basic statistics
        stats.min = *std::min_element(values.begin(), values.end());
        stats.max = *std::max_element(values.begin(), values.end());
        
        stats.mean = std::accumulate(values.begin(), values.end(), 0.0) / values.size();
        
        // Calculate variance
        double sumSquares = 0.0;
        for (double value : values) {
            sumSquares += (value - stats.mean) * (value - stats.mean);
        }
        stats.variance = sumSquares / values.size();
        stats.stddev = std::sqrt(stats.variance);
        
        // Calculate median
        std::vector<double> sortedValues = values;
        std::sort(sortedValues.begin(), sortedValues.end());
        if (sortedValues.size() % 2 == 0) {
            stats.median = (sortedValues[sortedValues.size() / 2 - 1] + 
                          sortedValues[sortedValues.size() / 2]) / 2.0;
        } else {
            stats.median = sortedValues[sortedValues.size() / 2];
        }
        
        return stats;
    }

    // Advanced analytics
    struct AnomalyInfo {
        std::string metricName;
        std::chrono::duration<double> timestamp;
        double value;
        double deviation;
        AnomalyType type;
        std::string description;
    };

    std::vector<AnomalyInfo> detectAnomalies(const std::string& name,
                                            double threshold = 2.0,
                                            AnomalyDetectionMethod method = AnomalyDetectionMethod::StandardDeviation) {
        std::vector<AnomalyInfo> anomalies;
        
        auto data = getTimeSeries(name, std::chrono::hours(24)); // Default to 24-hour window
        if (data.size() < 3) return anomalies; // Not enough data for anomaly detection
        
        // Calculate statistics
        double sum = 0.0, sumSquares = 0.0;
        for (const auto& point : data) {
            sum += point.value;
            sumSquares += point.value * point.value;
        }
        
        double mean = sum / data.size();
        double stddev = std::sqrt(sumSquares / data.size() - mean * mean);
        
        // Detect anomalies
        for (const auto& point : data) {
            double deviation = std::abs(point.value - mean);
            
            if (method == AnomalyDetectionMethod::StandardDeviation) {
                if (stddev > 0 && deviation / stddev > threshold) {
                    AnomalyInfo anomaly;
                    anomaly.metricName = name;
                    anomaly.timestamp = point.timestamp;
                    anomaly.value = point.value;
                    anomaly.deviation = deviation;
                    anomaly.type = AnomalyType::StandardDeviationOutlier;
                    anomaly.description = "Value deviates more than " + 
                                        std::to_string(threshold) + " standard deviations from mean";
                    anomalies.push_back(anomaly);
                }
            }
            // Other detection methods can be added here
        }
        
        return anomalies;
    }

    // Data management
    void clearMetric(const std::string& name) {
        std::lock_guard<std::mutex> lock(dataMutex_);
        if (timeSeriesData_.find(name) != timeSeriesData_.end()) {
            timeSeriesData_[name].clear();
        }
    }

    void clearExpiredData() {
        std::lock_guard<std::mutex> lock(dataMutex_);
        auto now = std::chrono::high_resolution_clock::now();
        
        for (auto& [metricName, data] : timeSeriesData_) {
            auto startTime = now - retentionPeriod_;
            auto newEnd = std::remove_if(data.begin(), data.end(),
                                        [startTime](const TimeSeriesPoint& point) {
                                            return point.timestamp < startTime;
                                        });
            data.erase(newEnd, data.end());
        }
    }

    size_t getMetricCount(const std::string& name) const {
        std::lock_guard<std::mutex> lock(dataMutex_);
        if (timeSeriesData_.find(name) != timeSeriesData_.end()) {
            return timeSeriesData_.at(name).size();
        }
        return 0;
    }

    std::vector<std::string> getMetricNames() const {
        std::lock_guard<std::mutex> lock(dataMutex_);
        std::vector<std::string> names;
        names.reserve(timeSeriesData_.size());
        for (const auto& pair : timeSeriesData_) {
            names.push_back(pair.first);
        }
        return names;
    }

    // Configuration
    void setMaxDataPoints(size_t maxPoints) {
        maxDataPoints_ = maxPoints;
    }

    void setRetentionPeriod(std::chrono::duration<double> period) {
        retentionPeriod_ = period;
    }

    void enableRollingWindow(bool enable) {
        enableRollingWindow_ = enable;
    }

    // Reporting
    std::string generateTimeSeriesReport(const std::string& metricName) {
        std::lock_guard<std::mutex> lock(dataMutex_);
        std::stringstream ss;
        
        if (timeSeriesData_.find(metricName) == timeSeriesData_.end()) {
            ss << "Metric \"" << metricName << "\" not found.\n";
            return ss.str();
        }
        
        ss << "Time Series Report for: " << metricName << "\n";
        ss << "==================================\n\n";
        
        const auto& data = timeSeriesData_[metricName];
        ss << "Data points: " << data.size() << "\n";
        
        // Get statistics for last 24 hours
        auto now = std::chrono::high_resolution_clock::now();
        auto yesterday = now - std::chrono::hours(24);
        
        auto stats = calculateStats(metricName, std::chrono::hours(24));
        ss << "\nStatistics (last 24h):\n";
        ss << "  Min: " << stats.min << "\n";
        ss << "  Max: " << stats.max << "\n";
        ss << "  Mean: " << stats.mean << "\n";
        ss << "  Median: " << stats.median << "\n";
        ss << "  Std Dev: " << stats.stddev << "\n";
        ss << "  Count: " << stats.count << "\n";
        
        // Show recent data (last 10 points)
        ss << "\nRecent Data (last 10 points):\n";
        size_t startIndex = std::max<size_t>(0, data.size() - 10);
        for (size_t i = startIndex; i < data.size(); ++i) {
            const auto& point = data[i];
            ss << "  " << std::put_time(std::gmtime(&std::chrono::system_clock::to_time_t(
                        std::chrono::system_clock::now())), "%H:%M:%S")
               << " | " << point.value << " | " << point.component << "\n";
        }
        
        return ss.str();
    }

    std::string generateAggregatedReport() {
        std::lock_guard<std::mutex> lock(dataMutex_);
        std::stringstream ss;
        
        ss << "Aggregated Metrics Report\n";
        ss << "==========================\n\n";
        
        for (const auto& [metricName, data] : timeSeriesData_) {
            if (data.empty()) continue;
            
            ss << "Metric: " << metricName << "\n";
            ss << "  Points: " << data.size() << "\n";
            
            // Calculate recent stats (last 1 hour)
            auto now = std::chrono::high_resolution_clock::now();
            auto oneHourAgo = now - std::chrono::hours(1);
            
            MetricStats recentStats;
            std::vector<double> recentValues;
            for (const auto& point : data) {
                if (point.timestamp >= oneHourAgo) {
                    recentValues.push_back(point.value);
                }
            }
            
            if (!recentValues.empty()) {
                recentStats.count = recentValues.size();
                recentStats.min = *std::min_element(recentValues.begin(), recentValues.end());
                recentStats.max = *std::max_element(recentValues.begin(), recentValues.end());
                recentStats.mean = std::accumulate(recentValues.begin(), recentValues.end(), 0.0) / recentValues.size();
                
                // Show as JSON-like format for easy parsing
                ss << "  Recent Stats (1h): {\"min\": " << recentStats.min
                   << ", \"max\": " << recentStats.max
                   << ", \"mean\": " << recentStats.mean
                   << ", \"count\": " << recentStats.count << "}\n";
            }
            ss << "\n";
        }
        
        return ss.str();
    }

private:
    // Enumerations for anomaly detection
    enum class AnomalyType {
        StandardDeviationOutlier,
        ThresholdViolation,
        SuddenChange,
        GradualTrend
    };

    enum class AnomalyDetectionMethod {
        StandardDeviation,
        Threshold,
        MovingAverage,
        ExponentialSmoothing
    };
};

} // namespace nlm
