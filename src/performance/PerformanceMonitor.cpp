// PerformanceMonitor.cpp implementation
#include "PerformanceMonitor.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <numeric>
#include <cmath>

namespace nlm {

// PerformanceMonitor::Impl implementation
struct PerformanceMonitor::Impl {
    std::vector<SamplePoint> samples;
    std::unordered_map<std::string, std::unique_ptr<Counter>> counters;
    std::vector<ResourceUsage> resourceUsageSamples;
    std::map<MetricType, std::vector<double>> samplesByMetric;
    std::chrono::time_point<std::chrono::steady_clock> startTime;
    std::chrono::time_point<std::chrono::steady_clock> lastResourceUpdate;
    std::string name;
    bool running;
    
    Impl(const std::string& monitorName) : name(monitorName), running(false) {}
};

PerformanceMonitor::PerformanceMonitor(PerformanceMonitor&& other) noexcept
    : pImpl(std::move(other.pImpl)), name_(std::move(other.name_)), running_(other.running_) {}

PerformanceMonitor& PerformanceMonitor::operator=(PerformanceMonitor&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        name_ = std::move(other.name_);
        running_ = other.running_;
    }
    return *this;
}

// Static Profiler member
PerformanceMonitor::Profiler* PerformanceMonitor::Profiler::globalProfiler_ = nullptr;

// Helper functions
PerformanceMonitor::PerformanceStats PerformanceMonitor::getStats(MetricType metric) const {
    auto samples = getSamplesForMetric(metric);
    if (samples.empty()) {
        return PerformanceStats();
    }
    
    PerformanceStats stats;
    stats.sampleCount = samples.size();
    
    double sum = 0.0;
    double min = samples[0].value;
    double max = samples[0].value;
    
    std::vector<double> values;
    for (const auto& sample : samples) {
        double val = sample.value;
        values.push_back(val);
        sum += val;
        if (val < min) min = val;
        if (val > max) max = val;
    }
    
    stats.mean = sum / samples.size();
    stats.min = min;
    stats.max = max;
    
    // Calculate standard deviation
    double variance = 0.0;
    for (double val : values) {
        double diff = val - stats.mean;
        variance += diff * diff;
    }
    stats.stddev = std::sqrt(variance / values.size());
    
    // Calculate median and percentiles
    std::sort(values.begin(), values.end());
    stats.median = values.size() % 2 == 0
        ? (values[values.size()/2 - 1] + values[values.size()/2]) / 2.0
        : values[values.size()/2];
    
    stats.p95 = values.size() > 0 ? values[static_cast<size_t>(values.size() * 0.95)] : 0.0;
    stats.p99 = values.size() > 0 ? values[static_cast<size_t>(values.size() * 0.99)] : 0.0;
    
    // Calculate throughput
    if (running_ && pImpl->startTime != std::chrono::steady_clock::time_point()) {
        auto now = std::chrono::steady_clock::now();
        double duration = std::chrono::duration<double>(now - pImpl->startTime).count();
        if (duration > 0.0) {
            stats.throughput = samples.size() / duration;
            stats.duration = std::chrono::duration<double>(duration);
        }
    }
    
    return stats;
}

PerformanceMonitor::PerformanceStats PerformanceMonitor::getStats(const std::string& label) const {
    PerformanceStats stats;
    
    for (const auto& sample : pImpl->samples) {
        if (sample.label == label) {
            updateStats(stats, sample.value);
        }
    }
    
    return stats;
}

PerformanceMonitor::PerformanceStats PerformanceMonitor::getOverallStats() const {
    PerformanceStats overallStats;
    double sum = 0.0;
    double min = std::numeric_limits<double>::max();
    double max = std::numeric_limits<double>::lowest();
    
    for (const auto& sample : pImpl->samples) {
        updateStats(overallStats, sample.value);
        sum += sample.value;
        min = std::min(min, sample.value);
        max = std::max(max, sample.value);
    }
    
    if (!pImpl->samples.empty()) {
        overallStats.mean = sum / pImpl->samples.size();
        overallStats.min = min;
        overallStats.max = max;
    }
    
    return overallStats;
}

bool PerformanceMonitor::exportCSV(const std::string& filename) const {
    return pImpl->exportCSV(filename);
}

bool PerformanceMonitor::exportJSON(const std::string& filename) const {
    return pImpl->exportJSON(filename);
}

} // namespace nlm
