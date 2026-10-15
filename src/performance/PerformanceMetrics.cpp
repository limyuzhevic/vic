#include "PerformanceMetrics.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

double PerformanceAnalyzer::calculateAverageFiringRate(const std::vector<PerformanceMetrics>& metrics) {
    if (metrics.empty()) return 0.0;
    
    double total_firing_rate = 0.0;
    uint32_t valid_count = 0;
    
    for (const auto& metric : metrics) {
        if (metric.average_firing_rate > 0.0) {
            total_firing_rate += metric.average_firing_rate;
            valid_count++;
        }
    }
    
    return valid_count > 0 ? total_firing_rate / valid_count : 0.0;
}

double PerformanceAnalyzer::calculateCpuUsage(const std::vector<PerformanceMetrics>& metrics) {
    if (metrics.empty()) return 0.0;
    
    double total_cpu = 0.0;
    uint32_t valid_count = 0;
    
    for (const auto& metric : metrics) {
        if (metric.cpu_usage_percent >= 0.0) {
            total_cpu += metric.cpu_usage_percent;
            valid_count++;
        }
    }
    
    return valid_count > 0 ? total_cpu / valid_count : 0.0;
}

double PerformanceAnalyzer::calculateMemoryUsage(const std::vector<PerformanceMetrics>& metrics) {
    if (metrics.empty()) return 0.0;
    
    double total_memory = 0.0;
    uint32_t valid_count = 0;
    
    for (const auto& metric : metrics) {
        if (metric.memory_usage_mb > 0.0) {
            total_memory += metric.memory_usage_mb;
            valid_count++;
        }
    }
    
    return valid_count > 0 ? total_memory / valid_count : 0.0;
}

std::vector<double> PerformanceAnalyzer::calculateMovingAverage(const std::vector<double>& data, uint32_t window_size) {
    std::vector<double> result;
    if (data.empty() || window_size == 0) return result;
    
    double window_sum = 0.0;
    for (size_t i = 0; i < data.size(); ++i) {
        window_sum += data[i];
        if (i >= window_size) {
            window_sum -= data[i - window_size];
        }
        
        if (i >= window_size - 1) {
            result.push_back(window_sum / window_size);
        }
    }
    
    return result;
}

uint64_t PerformanceAnalyzer::findMaxValue(const std::vector<PerformanceMetrics>& metrics,
                                           double (PerformanceMetrics::* getter)() const) {
    if (metrics.empty()) return 0;
    
    uint64_t max_value = 0;
    for (const auto& metric : metrics) {
        uint64_t current_value = static_cast<uint64_t>(metric.*getter());
        max_value = std::max(max_value, current_value);
    }
    
    return max_value;
}

uint64_t PerformanceAnalyzer::findMinValue(const std::vector<PerformanceMetrics>& metrics,
                                           double (PerformanceMetrics::* getter)() const) {
    if (metrics.empty()) return 0;
    
    uint64_t min_value = UINT64_MAX;
    for (const auto& metric : metrics) {
        uint64_t current_value = static_cast<uint64_t>(metric.*getter());
        min_value = std::min(min_value, current_value);
    }
    
    return min_value == UINT64_MAX ? 0 : min_value;
}

void PerformanceAnalyzer::detectAnomalies(const std::vector<PerformanceMetrics>& metrics,
                                          std::vector<PerformanceAlert>& alerts) {
    if (metrics.size() < 3) return; // Need enough data for meaningful analysis
    
    // Calculate baseline statistics
    double avg_firing_rate = calculateAverageFiringRate(metrics);
    double std_firing_rate = 0.0;
    
    // Calculate standard deviation of firing rate
    if (avg_firing_rate > 0.0) {
        double sum_squared_diff = 0.0;
        uint32_t valid_count = 0;
        
        for (const auto& metric : metrics) {
            if (metric.average_firing_rate > 0.0) {
                double diff = metric.average_firing_rate - avg_firing_rate;
                sum_squared_diff += diff * diff;
                valid_count++;
            }
        }
        
        if (valid_count > 0) {
            std_firing_rate = std::sqrt(sum_squared_diff / valid_count);
        }
    }
    
    // Detect anomalies in current metrics (last N samples)
    size_t recent_samples = std::min<size_t>(10, metrics.size());
    if (recent_samples > 0) {
        auto recent_start = metrics.end() - recent_samples;
        for (auto it = recent_start; it != metrics.end(); ++it) {
            const auto& metric = *it;
            
            // High firing rate anomaly
            if (metric.average_firing_rate > avg_firing_rate + 3.0 * std_firing_rate && metric.average_firing_rate > 0.0) {
                PerformanceAlert alert;
                alert.type = PerformanceAlert::AlertType::SPIKE_RATE_HIGH;
                alert.condition = "Firing rate anomaly detected";
                alert.threshold_value = avg_firing_rate + 3.0 * std_firing_rate;
                alert.current_value = metric.average_firing_rate;
                alert.severity = "WARNING";
                alert.message = "Firing rate significantly higher than baseline";
                alert.timestamp_ns = metric.timestamp_ns;
                alerts.push_back(alert);
            }
        }
    }
}

} // namespace nlm