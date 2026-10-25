#pragma once

#include <chrono>
#include <iostream>
#include <iomanip>
#include <sstream>
#include <thread>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <algorithm>
#include <cmath>
#include <numeric>
#include <string>

namespace nlm {

/**
 * Performance Monitoring System
 * 
 * Tracks and analyzes neural network performance metrics
 * Provides detailed profiling and optimization insights
 */

class PerformanceMonitor {
public:
    /**
     * Performance metric types
     */
    enum class MetricType {
        Time,
        Memory,
        CPU,
        IOWait,
        Network,
        Accuracy,
        Throughput,
        SpikeRate,
        Energy,
        Utilization
    };

    /**
     * Performance sample point
     */
    struct SamplePoint {
        std::chrono::time_point<std::chrono::steady_clock> timestamp;
        double value;
        MetricType metric;
        std::string label;
        std::unordered_map<std::string, std::string> metadata;
        
        SamplePoint(double val, MetricType metric, const std::string& lbl = "")
            : timestamp(std::chrono::steady_clock::now()), value(val), 
              metric(metric), label(lbl) {}
        
        double getElapsedSeconds() const;
        std::string getFormattedValue() const;
    };

    /**
     * Performance statistics
     */
    struct PerformanceStats {
        size_t sampleCount;
        double mean;
        double median;
        double stddev;
        double min;
        double max;
        double p95; // 95th percentile
        double p99; // 99th percentile
        double throughput; // samples per second
        std::chrono::duration<double> duration;
        
        PerformanceStats() : sampleCount(0), mean(0.0), median(0.0), 
                           stddev(0.0), min(0.0), max(0.0), p95(0.0), 
                           p99(0.0), throughput(0.0) {}
        
        std::string toString() const;
        std::string toJSON() const;
    };

    /**
     * System resource usage
     */
    struct ResourceUsage {
        double cpuUsage;      // Percentage (0-100)
        double memoryUsage;   // Percentage (0-100)
        double memoryUsedMB;  // MB used
        double memoryTotalMB; // MB total
        size_t activeThreads;
        std::chrono::time_point<std::chrono::steady_clock> updateTime;
        
        ResourceUsage() : cpuUsage(0.0), memoryUsage(0.0), memoryUsedMB(0.0),
                         memoryTotalMB(0.0), activeThreads(0) {}
    };

    /**
     * Performance counter for real-time tracking
     */
    class Counter {
    public:
        Counter(const std::string& name, MetricType metric, 
                double scale = 1.0, const std::string& unit = "")
            : name_(name), metric_(metric), scale_(scale), unit_(unit),
              value_(0.0), reset_(false), lastReset_(std::chrono::steady_clock::now()) {}
        
        void increment(double amount = 1.0);
        void decrement(double amount = 1.0);
        void set(double value);
        double get() const { return value_ * scale_; }
        std::string getName() const { return name_; }
        MetricType getMetric() const { return metric_; }
        std::string getUnit() const { return unit_; }
        std::chrono::time_point<std::chrono::steady_clock> getLastReset() const { return lastReset_; }
        void reset() { value_ = 0.0; reset_ = true; lastReset_ = std::chrono::steady_clock::now(); }
        
    private:
        std::string name_;
        MetricType metric_;
        double scale_;
        std::string unit_;
        double value_;
        bool reset_;
        std::chrono::time_point<std::chrono::steady_clock> lastReset_;
    };

    /**
     * Performance profiler for time measurements
     */
    class Profiler {
    public:
        struct ProfileData {
            std::string functionName;
            std::chrono::duration<double> elapsedTime;
            size_t callCount;
            double averageTime;
            double totalTime;
            std::unordered_map<std::string, double> nestedProfiles;
            
            ProfileData(const std::string& name) 
                : functionName(name), callCount(0), averageTime(0.0), totalTime(0.0) {}
        };
        
        Profiler(const std::string& name);
        ~Profiler();
        
        void start();
        void stop();
        double getElapsedTime() const;
        std::string getName() const { return name_; }
        
        static void setGlobalProfiler(Profiler* profiler) { globalProfiler_ = profiler; }
        static Profiler* getGlobalProfiler() { return globalProfiler_; }
        
        static void beginProfile(const std::string& name);
        static void endProfile();
        static std::vector<ProfileData> getProfileResults();
        static void clearProfileResults();
        static std::string getProfileReport();
        
    private:
        std::string name_;
        std::chrono::time_point<std::chrono::steady_clock> startTime_;
        bool running_;
        static Profiler* globalProfiler_;
        std::vector<ProfileData> profileResults_;
        std::stack<Profiler*> profilerStack_;
    };

    PerformanceMonitor(const std::string& name);
    ~PerformanceMonitor();
    
    // Disable copying, enable moving
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    PerformanceMonitor(PerformanceMonitor&&) noexcept;
    PerformanceMonitor& operator=(PerformanceMonitor&&) noexcept;

    /**
     * Sample a performance metric
     */
    void sample(MetricType metric, double value, const std::string& label = "");
    void sample(Counter& counter);
    
    /**
     * Get performance statistics for a metric
     */
    PerformanceStats getStats(MetricType metric) const;
    PerformanceStats getStats(const std::string& label) const;
    PerformanceStats getOverallStats() const;
    
    /**
     * Get counter value
     */
    double getCounterValue(const std::string& name) const;
    
    /**
     * Create and register a counter
     */
    Counter* createCounter(const std::string& name, MetricType metric,
                          double scale = 1.0, const std::string& unit = "");
    
    /**
     * Get resource usage information
     */
    ResourceUsage getResourceUsage() const;
    
    /**
     * Start/stop monitoring
     */
    void start();
    void stop();
    bool isRunning() const { return running_; }
    
    /**
     * Generate performance report
     */
    std::string getReport() const;
    std::string getJSONReport() const;
    std::string getDetailedReport() const;
    
    /**
     * Export data for analysis
     */
    bool exportCSV(const std::string& filename) const;
    bool exportJSON(const std::string& filename) const;
    
    /**
     * Clear all collected data
     */
    void clear();
    
    /**
     * Get monitor name
     */
    std::string getName() const { return name_; }

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::string name_;
    bool running_;
    
    void updateResourceUsage();
    double calculateStats(const std::vector<SamplePoint>& samples) const;
    std::vector<SamplePoint> getSamplesForMetric(MetricType metric) const;
};

// Inline implementations

inline double PerformanceMonitor::SamplePoint::getElapsedSeconds() const {
    auto now = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(now - timestamp).count();
}

inline std::string PerformanceMonitor::SamplePoint::getFormattedValue() const {
    std::stringstream ss;
    ss << std::fixed << std::setprecision(2) << value;
    return ss.str();
}

inline std::string PerformanceMonitor::PerformanceStats::toString() const {
    std::stringstream ss;
    ss << "Samples: " << sampleCount << std::endl;
    ss << "Mean: " << mean << std::endl;
    ss << "Median: " << median << std::endl;
    ss << "Std Dev: " << stddev << std::endl;
    ss << "Min: " << min << std::endl;
    ss << "Max: " << max << std::endl;
    ss << "95th percentile: " << p95 << std::endl;
    ss << "99th percentile: " << p99 << std::endl;
    ss << "Throughput: " << throughput << " samples/sec" << std::endl;
    return ss.str();
}

inline std::string PerformanceMonitor::PerformanceStats::toJSON() const {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"sampleCount\": " << sampleCount << "," << std::endl;
    ss << "  \"mean\": " << mean << "," << std::endl;
    ss << "  \"median\": " << median << ",\" << std::endl;
    ss << "  \"stddev\": " << stddev << "," << std::endl;
    ss << "  \"min\": " << min << "," << std::endl;
    ss << "  \"max\": " << max << "," << std::endl;
    ss << "  \"p95\": " << p95 << "," << std::endl;
    ss << "  \"p99\": " << p99 << "," << std::endl;
    ss << "  \"throughput\": " << throughput" << std::endl;
    ss << "}" << std::endl;
    return ss.str();
}

inline void PerformanceMonitor::Counter::increment(double amount) {
    value_ += amount;
}

inline void PerformanceMonitor::Counter::decrement(double amount) {
    value_ -= amount;
    if (value_ < 0.0) value_ = 0.0;
}

inline void PerformanceMonitor::Counter::set(double value) {
    value_ = value;
}

inline PerformanceMonitor::Profiler* PerformanceMonitor::Profiler::globalProfiler_ = nullptr;

inline PerformanceMonitor::Profiler::Profiler(const std::string& name) 
    : name_(name), running_(false) {}

inline PerformanceMonitor::Profiler::~Profiler() = default;

inline void PerformanceMonitor::Profiler::start() {
    if (running_) return;
    startTime_ = std::chrono::steady_clock::now();
    running_ = true;
}

inline void PerformanceMonitor::Profiler::stop() {
    if (!running_) return;
    auto endTime = std::chrono::steady_clock::now();
    double elapsed = std::chrono::duration<double>(endTime - startTime_).count();
    
    // Find or create profile data
    for (auto& profile : profileResults_) {
        if (profile.functionName == name_) {
            profile.callCount++;
            profile.totalTime += elapsed;
            profile.averageTime = profile.totalTime / profile.callCount;
            break;
        }
    }
    
    running_ = false;
}

inline double PerformanceMonitor::Profiler::getElapsedTime() const {
    if (!running_) return 0.0;
    auto endTime = std::chrono::steady_clock::now();
    return std::chrono::duration<double>(endTime - startTime_).count();
}

inline void PerformanceMonitor::Profiler::beginProfile(const std::string& name) {
    if (globalProfiler_) {
        globalProfiler_->profilerStack_.push(globalProfiler_);
        globalProfiler_ = new Profiler(name);
        globalProfiler_->start();
    }
}

inline void PerformanceMonitor::Profiler::endProfile() {
    if (globalProfiler_) {
        globalProfiler_->stop();
        Profiler* parentProfiler = globalProfiler_->profilerStack_.top();
        globalProfiler_->profilerStack_.pop();
        delete globalProfiler_;
        globalProfiler_ = parentProfiler;
    }
}

inline std::vector<PerformanceMonitor::Profiler::ProfileData> PerformanceMonitor::Profiler::getProfileResults() {
    if (globalProfiler_) {
        return globalProfiler_->profileResults_;
    }
    return std::vector<ProfileData>();
}

inline void PerformanceMonitor::Profiler::clearProfileResults() {
    if (globalProfiler_) {
        globalProfiler_->profileResults_.clear();
    }
}

inline std::string PerformanceMonitor::Profiler::getProfileReport() {
    if (globalProfiler_) {
        std::stringstream ss;
        ss << "=== Profile Report ===" << std::endl;
        for (const auto& profile : globalProfiler_->profileResults_) {
            ss << profile.functionName << ":" << std::endl;
            ss << "  Calls: " << profile.callCount << std::endl;
            ss << "  Total time: " << profile.totalTime << "s" << std::endl;
            ss << "  Average time: " << profile.averageTime << "s" << std::endl;
            ss << "  Percentage: " << (profile.totalTime / getTotalTime() * 100.0) << "%" << std::endl;
        }
        return ss.str();
    }
    return "No profiler active";
}

inline PerformanceMonitor::PerformanceMonitor(const std::string& name) 
    : name_(name), running_(false) {
    pImpl = std::make_unique<Impl>();
}

inline PerformanceMonitor::~PerformanceMonitor() = default;

inline PerformanceMonitor::PerformanceMonitor(PerformanceMonitor&& other) noexcept
    : pImpl(std::move(other.pImpl)), name_(std::move(other.name_)), running_(other.running_) {}

inline PerformanceMonitor& PerformanceMonitor::operator=(PerformanceMonitor&& other) noexcept {
    if (this != &other) {
        pImpl = std::move(other.pImpl);
        name_ = std::move(other.name_);
        running_ = other.running_;
    }
    return *this;
}

inline void PerformanceMonitor::sample(MetricType metric, double value, const std::string& label) {
    if (running_) {
        pImpl->samples.push_back(SamplePoint(value, metric, label));
    }
}

inline void PerformanceMonitor::sample(Counter& counter) {
    sample(counter.getMetric(), counter.get(), counter.getName());
}

inline double PerformanceMonitor::getCounterValue(const std::string& name) const {
    auto it = pImpl->counters.find(name);
    if (it != pImpl->counters.end()) {
        return it->second->get();
    }
    return 0.0;
}

inline PerformanceMonitor::Counter* PerformanceMonitor::createCounter(const std::string& name, 
                                                                     MetricType metric, double scale, const std::string& unit) {
    if (pImpl->counters.find(name) != pImpl->counters.end()) {
        return nullptr; // Counter already exists
    }
    
    auto counter = std::make_unique<Counter>(name, metric, scale, unit);
    Counter* rawPtr = counter.get();
    pImpl->counters[name] = std::move(counter);
    return rawPtr;
}

inline ResourceUsage PerformanceMonitor::getResourceUsage() const {
    // Basic implementation - in practice this would read system-specific data
    ResourceUsage usage;
    usage.updateTime = std::chrono::steady_clock::now();
    usage.cpuUsage = 0.0; // Would read from /proc/stat or platform-specific API
    usage.memoryUsage = 0.0; // Would read from /proc/meminfo or platform-specific API
    usage.memoryUsedMB = 0.0;
    usage.memoryTotalMB = 0.0;
    usage.activeThreads = 0; // Would get from thread count API
    return usage;
}

inline void PerformanceMonitor::start() {
    if (!running_) {
        running_ = true;
        pImpl->lastResourceUpdate = std::chrono::steady_clock::now();
    }
}

inline void PerformanceMonitor::stop() {
    running_ = false;
}

inline std::string PerformanceMonitor::getReport() const {
    std::stringstream ss;
    ss << "=== Performance Report for " << name_ << " ===" << std::endl;
    ss << "Monitoring: " << (running_ ? "Running" : "Stopped") << std::endl;
    ss << "Total samples: " << pImpl->samples.size() << std::endl;
    ss << "Counters: " << pImpl->counters.size() << std::endl;
    ss << "Resource samples: " << pImpl->resourceUsageSamples.size() << std::endl;
    
    ss << std::endl << "=== Metric Statistics ===" << std::endl;
    std::map<MetricType, PerformanceStats> metricStats;
    std::map<std::string, PerformanceStats> labelStats;
    
    for (const auto& sample : pImpl->samples) {
        if (metricStats.find(sample.metric) == metricStats.end()) {
            metricStats[sample.metric] = PerformanceStats();
        }
        updateStats(metricStats[sample.metric], sample.value);
        
        if (!sample.label.empty()) {
            if (labelStats.find(sample.label) == labelStats.end()) {
                labelStats[sample.label] = PerformanceStats();
            }
            updateStats(labelStats[sample.label], sample.value);
        }
    }
    
    for (const auto& pair : metricStats) {
        ss << metricTypeToString(pair.first) << ":" << std::endl;
        ss << pair.second.toString() << std::endl;
    }
    
    return ss.str();
}

inline std::string PerformanceMonitor::getJSONReport() const {
    std::stringstream ss;
    ss << "{" << std::endl;
    ss << "  \"monitorName\": \"" << name_ << "\"," << std::endl;
    ss << "  \"running\": " << (running_ ? "true" : "false") << "," << std::endl;
    ss << "  \"sampleCount\": " << pImpl->samples.size() << "," << std::endl;
    ss << "  \"counters\": [" << std::endl;
    
    bool firstCounter = true;
    for (const auto& pair : pImpl->counters) {
        if (!firstCounter) ss << "," << std::endl;
        ss << "    {\"name\": \"" << pair.first << "\", \"value\": " << pair.second->get();
        firstCounter = false;
    }
    
    ss << "]," << std::endl;
    ss << "  \"metrics\": [" << std::endl;
    
    // Add metric statistics
    bool firstMetric = true;
    for (const auto& pair : pImpl->samplesByMetric) {
        if (!firstMetric) ss << "," << std::endl;
        ss << "    {\"metric\": \"" << metricTypeToString(pair.first) << "\", \"stats\": " << pair.second.toJSON();
        firstMetric = false;
    }
    
    ss << std::endl << "  ]" << std::endl;
    ss << "}" << std::endl;
    
    return ss.str();
}

inline std::string PerformanceMonitor::getDetailedReport() const {
    std::stringstream ss;
    ss << getReport() << std::endl;
    
    ss << std::endl << "=== Resource Usage ===" << std::endl;
    for (const auto& usage : pImpl->resourceUsageSamples) {
        ss << "CPU: " << usage.cpuUsage << "%, Memory: " << usage.memoryUsage << "%" << std::endl;
    }
    
    ss << std::endl << "=== Counter Values ===" << std::endl;
    for (const auto& pair : pImpl->counters) {
        ss << pair.first << ": " << pair.second->get() << " " << pair.second->getUnit() << std::endl;
    }
    
    return ss.str();
}

inline bool PerformanceMonitor::exportCSV(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << "Timestamp,Metric,Value,Label" << std::endl;
    for (const auto& sample : pImpl->samples) {
        file << sample.timestamp.time_since_epoch().count() << ","
             << metricTypeToString(sample.metric) << ","
             << sample.value << ","
             << sample.label << std::endl;
    }
    
    return true;
}

inline bool PerformanceMonitor::exportJSON(const std::string& filename) const {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << getJSONReport();
    return true;
}

inline void PerformanceMonitor::clear() {
    pImpl->samples.clear();
    pImpl->counters.clear();
    pImpl->resourceUsageSamples.clear();
}

// Helper functions (implementation in .cpp file)
inline double PerformanceMonitor::calculateStats(const std::vector<SamplePoint>& samples) const {
    if (samples.empty()) return 0.0;
    
    double sum = 0.0;
    double min = samples[0].value;
    double max = samples[0].value;
    
    for (const auto& sample : samples) {
        sum += sample.value;
        if (sample.value < min) min = sample.value;
        if (sample.value > max) max = sample.value;
    }
    
    double mean = sum / samples.size();
    double variance = 0.0;
    for (const auto& sample : samples) {
        double diff = sample.value - mean;
        variance += diff * diff;
    }
    variance /= samples.size();
    
    double stddev = std::sqrt(variance);
    
    // Calculate median, percentiles (simplified)
    std::vector<double> values;
    for (const auto& sample : samples) {
        values.push_back(sample.value);
    }
    std::sort(values.begin(), values.end());
    
    double median = values.size() % 2 == 0
        ? (values[values.size()/2 - 1] + values[values.size()/2]) / 2.0
        : values[values.size()/2];
    
    double p95 = values.size() > 0 ? values[static_cast<size_t>(values.size() * 0.95)] : 0.0;
    double p99 = values.size() > 0 ? values[static_cast<size_t>(values.size() * 0.99)] : 0.0;
    
    // Update stats structure
    PerformanceStats stats;
    stats.sampleCount = samples.size();
    stats.mean = mean;
    stats.median = median;
    stats.stddev = stddev;
    stats.min = min;
    stats.max = max;
    stats.p95 = p95;
    stats.p99 = p99;
    stats.throughput = samples.size() / getDuration().count();
    stats.duration = getDuration();
    
    return stats;
}

inline std::vector<PerformanceMonitor::SamplePoint> PerformanceMonitor::getSamplesForMetric(MetricType metric) const {
    std::vector<SamplePoint> result;
    for (const auto& sample : pImpl->samples) {
        if (sample.metric == metric) {
            result.push_back(sample);
        }
    }
    return result;
}

inline void PerformanceMonitor::updateResourceUsage() {
    auto now = std::chrono::steady_clock::now();
    if (std::chrono::duration<double>(now - pImpl->lastResourceUpdate).count() >= 1.0) {
        ResourceUsage usage = getResourceUsage();
        pImpl->resourceUsageSamples.push_back(usage);
        pImpl->lastResourceUpdate = now;
    }
}

inline void PerformanceMonitor::updateStats(PerformanceStats& stats, double value) {
    stats.sampleCount++;
    stats.min = (stats.sampleCount == 1) ? value : std::min(stats.min, value);
    stats.max = (stats.sampleCount == 1) ? value : std::max(stats.max, value);
    
    double oldMean = stats.mean;
    stats.mean = oldMean + (value - oldMean) / stats.sampleCount;
    stats.stddev = (stats.sampleCount == 1) ? 0.0 : 
        std::sqrt((stats.sampleCount - 2) * stats.stddev * stats.stddev / stats.sampleCount + 
                  (value - oldMean) * (value - stats.mean));
}

inline std::string PerformanceMonitor::metricTypeToString(MetricType metric) {
    static const std::unordered_map<MetricType, std::string> metricStrings = {
        {MetricType::Time, "Time"},
        {MetricType::Memory, "Memory"},
        {MetricType::CPU, "CPU"},
        {MetricType::IOWait, "IOWait"},
        {MetricType::Network, "Network"},
        {MetricType::Accuracy, "Accuracy"},
        {MetricType::Throughput, "Throughput"},
        {MetricType::SpikeRate, "SpikeRate"},
        {MetricType::Energy, "Energy"},
        {MetricType::Utilization, "Utilization"}
    };
    
    auto it = metricStrings.find(metric);
    if (it != metricStrings.end()) {
        return it->second;
    }
    return "Unknown";
}

} // namespace nlm
