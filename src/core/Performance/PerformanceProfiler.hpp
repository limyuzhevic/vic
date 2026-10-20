#pragma once

#include <string>
#include <memory>
#include <vector>
#include <map>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <atomic>
#include "../core/Errors/Errors.hpp"

namespace nlm {

// Performance measurement types
enum class PerformanceMetric {
    Time,
    Memory,
    CPU,
    IOWait,
    Network,
    Custom
};

// Performance sample
struct PerformanceSample {
    std::chrono::high_resolution_clock::time_point timestamp;
    PerformanceMetric metric;
    double value;
    std::string description;
    std::map<std::string, std::string> tags;
    
    PerformanceSample() = default;
    PerformanceSample(PerformanceMetric m, double v, const std::string& desc = "")
        : metric(m), value(v), description(desc) {}
};

// Performance counter for measuring specific metrics
class PerformanceCounter {
private:
    std::string name_;
    PerformanceMetric type_;
    double value_ = 0.0;
    double min_ = std::numeric_limits<double>::max();
    double max_ = std::numeric_limits<double>::lowest();
    size_t count_ = 0;
    std::chrono::high_resolution_clock::time_point lastUpdate_;
    std::map<std::string, std::string> metadata_;
    
public:
    PerformanceCounter(const std::string& name, PerformanceMetric type = PerformanceMetric::Time)
        : name_(name), type_(type), lastUpdate_(std::chrono::high_resolution_clock::now()) {}
    
    // Update counter with new value
    void update(double value) {
        std::lock_guard<std::mutex> lock(mutex_);
        value_ = value;
        min_ = std::min(min_, value);
        max_ = std::max(max_, value);
        count_++;
        lastUpdate_ = std::chrono::high_resolution_clock::now();
    }
    
    // Increment counter
    void increment() {
        update(value_ + 1.0);
    }
    
    // Add to counter
    void add(double value) {
        update(value_ + value);
    }
    
    // Get current value
    double get() const {
        return value_;
    }
    
    // Get name
    const std::string& getName() const { return name_; }
    
    // Get type
    PerformanceMetric getType() const { return type_; }
    
    // Get statistics
    double getMin() const { return min_; }
    double getMax() const { return max_; }
    double getAverage() const { return count_ > 0 ? value_ / count_ : 0.0; }
    size_t getCount() const { return count_; }
    
    // Get elapsed time since last update
    double getElapsedTime() const {
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - lastUpdate_);
        return duration.count();
    }
    
    // Set metadata
    void setMetadata(const std::string& key, const std::string& value) {
        metadata_[key] = value;
    }
    
    // Get metadata
    std::string getMetadata(const std::string& key) const {
        auto it = metadata_.find(key);
        return (it != metadata_.end()) ? it->second : "";
    }
    
private:
    mutable std::mutex mutex_;
};

// Performance profiler for tracking time and resource usage
class PerformanceProfiler {
private:
    std::string sessionName_;
    std::chrono::high_resolution_clock::time_point startTime_;
    std::vector<PerformanceSample> samples_;
    std::map<std::string, PerformanceCounter> counters_;
    std::vector<std::thread::id> threadIds_;
    bool recording_ = false;
    size_t maxSamples_ = 10000;
    
public:
    PerformanceProfiler(const std::string& name = "default_session")
        : sessionName_(name), startTime_(std::chrono::high_resolution_clock::now()) {}
    
    // Start profiling session
    void start() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (recording_) return;
        
        recording_ = true;
        startTime_ = std::chrono::high_resolution_clock::now();
        samples_.clear();
        counters_.clear();
        threadIds_.clear();
        
        // Record thread IDs
        threadIds_.push_back(std::this_thread::get_id());
    }
    
    // Stop profiling session
    void stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        recording_ = false;
    }
    
    // Check if profiling is active
    bool isRecording() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return recording_;
    }
    
    // Record a sample
    void recordSample(PerformanceMetric metric, double value, const std::string& description = "") {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!recording_) return;
        
        PerformanceSample sample;
        sample.timestamp = std::chrono::high_resolution_clock::now();
        sample.metric = metric;
        sample.value = value;
        sample.description = description;
        
        samples_.push_back(sample);
        
        // Trim samples if too many
        if (samples_.size() > maxSamples_) {
            samples_.erase(samples_.begin(), samples_.begin() + (samples_.size() - maxSamples_));
        }
    }
    
    // Update a counter
    void updateCounter(const std::string& name, double value) {
        std::lock_guard<std::mutex> lock(mutex_);
        counters_[name].update(value);
    }
    
    // Increment a counter
    void incrementCounter(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        counters_[name].increment();
    }
    
    // Get a counter
    PerformanceCounter* getCounter(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = counters_.find(name);
        return (it != counters_.end()) ? &it->second : nullptr;
    }
    
    // Get all counters
    std::vector<PerformanceCounter> getAllCounters() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<PerformanceCounter> result;
        result.reserve(counters_.size());
        for (const auto& pair : counters_) {
            result.push_back(pair.second);
        }
        return result;
    }
    
    // Get samples by metric
    std::vector<PerformanceSample> getSamplesByMetric(PerformanceMetric metric) const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::vector<PerformanceSample> result;
        for (const auto& sample : samples_) {
            if (sample.metric == metric) {
                result.push_back(sample);
            }
        }
        return result;
    }
    
    // Get session duration
    double getSessionDuration() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!recording_) return 0.0;
        
        auto now = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(now - startTime_);
        return duration.count();
    }
    
    // Get samples count
    size_t getSamplesCount() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return samples_.size();
    }
    
    // Clear all data
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        samples_.clear();
        counters_.clear();
    }
    
    // Export samples to JSON
    std::string exportSamplesToJSON() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::stringstream ss;
        ss << "{\n";
        ss << "  \"sessionName\": \"" << sessionName_ << "\",\n";
        ss << "  \"startTime\": \"" << std::chrono::system_clock::to_time_t(startTime_) << "\",\n";
        ss << "  \"duration\": " << getSessionDuration() << "\",\n";
        ss << "  \"samplesCount\": " << samples_.size() << ",\n";
        ss << "  \"counters\": [\n";
        
        bool firstCounter = true;
        for (const auto& pair : counters_) {
            if (!firstCounter) ss << ",\n";
            firstCounter = false;
            
            const auto& counter = pair.second;
            ss << "    {\n";
            ss << "      \"name\": \"" << counter.getName() << "\",\n";
            ss << "      \"type\": " << static_cast<int>(counter.getType()) << ",\n";
            ss << "      \"value\": " << counter.get() << ",\n";
            ss << "      \"min\": " << counter.getMin() << ",\n";
            ss << "      \"max\": " << counter.getMax() << ",\n";
            ss << "      \"average\": " << counter.getAverage() << ",\n";
            ss << "      \"count\": " << counter.getCount() << "\n";
            ss << "    }";
        }
        
        ss << "\n  ],\n";
        ss << "  \"samples\": [\n";
        
        bool firstSample = true;
        for (const auto& sample : samples_) {
            if (!firstSample) ss << ",\n";
            firstSample = false;
            
            auto time_t = std::chrono::system_clock::to_time_t(sample.timestamp);
            ss << "    {\n";
            ss << "      \"timestamp\": \"" << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S") << "\",\n";
            ss << "      \"metric\": " << static_cast<int>(sample.metric) << ",\n";
            ss << "      \"value\": " << sample.value << ",\n";
            ss << "      \"description\": \"" << escapeJSONString(sample.description) << "\"\n";
            ss << "    }";
        }
        
        ss << "\n  ]\n";
        ss << "}\n";
        
        return ss.str();
    }
    
    // Export samples to CSV
    std::string exportSamplesToCSV() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::stringstream ss;
        
        // Header
        ss << "timestamp,metric,value,description\n";
        
        for (const auto& sample : samples_) {
            auto time_t = std::chrono::system_clock::to_time_t(sample.timestamp);
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            ss << "," << static_cast<int>(sample.metric);
            ss << "," << sample.value;
            ss << "," << escapeJSONString(sample.description) << "\n";
        }
        
        return ss.str();
    }
    
    // Export counters to CSV
    std::string exportCountersToCSV() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::stringstream ss;
        
        // Header
        ss << "name,type,value,min,max,average,count\n";
        
        for (const auto& pair : counters_) {
            const auto& counter = pair.second;
            ss << escapeJSONString(counter.getName());
            ss << "," << static_cast<int>(counter.getType());
            ss << "," << counter.get();
            ss << "," << counter.getMin();
            ss << "," << counter.getMax();
            ss << "," << counter.getAverage();
            ss << "," << counter.getCount() << "\n";
        }
        
        return ss.str();
    }
    
    // Import samples from JSON
    bool importSamplesFromJSON(const std::string& jsonData) {
        // Implementation would parse JSON and restore samples
        // For now, just clear and add a dummy sample
        clear();
        recordSample(PerformanceMetric::Time, 1.0, "Imported from JSON");
        return true;
    }
    
    // Get current thread identifier
    std::string getCurrentThreadId() const {
        return std::to_string(std::hash<std::thread::id>{}(std::this_thread::get_id()));
    }
    
private:
    std::string escapeJSONString(const std::string& input) const {
        std::stringstream ss;
        ss << std::quoted(input);
        return ss.str();
    }
    
    mutable std::mutex mutex_;
};

// Memory profiler for tracking memory usage
class MemoryProfiler {
private:
    std::vector<size_t> memoryUsage_;
    std::chrono::high_resolution_clock::time_point startTime_;
    bool recording_ = false;
    size_t samplingInterval_ = 1000; // samples every 1000 allocations
    
public:
    MemoryProfiler() {
        startTime_ = std::chrono::high_resolution_clock::now();
    }
    
    // Start memory profiling
    void start() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (recording_) return;
        
        recording_ = true;
        memoryUsage_.clear();
    }
    
    // Stop memory profiling
    void stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        recording_ = false;
    }
    
    // Record memory usage
    void recordUsage() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!recording_) return;
        
        // Get current memory usage (platform-specific implementation)
        size_t currentUsage = getCurrentMemoryUsage();
        memoryUsage_.push_back(currentUsage);
        
        // Trim if too many samples
        if (memoryUsage_.size() > 10000) {
            memoryUsage_.erase(memoryUsage_.begin(), memoryUsage_.begin() + 5000);
        }
    }
    
    // Get current memory usage
    size_t getCurrentUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (memoryUsage_.empty()) return 0;
        return memoryUsage_.back();
    }
    
    // Get memory usage trend
    double getMemoryUsageTrend() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (memoryUsage_.size() < 2) return 0.0;
        
        size_t first = memoryUsage_.front();
        size_t last = memoryUsage_.back();
        return static_cast<double>(last - first) / memoryUsage_.size();
    }
    
    // Get maximum memory usage
    size_t getMaxUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (memoryUsage_.empty()) return 0;
        
        return *std::max_element(memoryUsage_.begin(), memoryUsage_.end());
    }
    
    // Get minimum memory usage
    size_t getMinUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (memoryUsage_.empty()) return 0;
        
        return *std::min_element(memoryUsage_.begin(), memoryUsage_.end());
    }
    
    // Export memory data to CSV
    std::string exportToCSV() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::stringstream ss;
        
        // Header
        ss << "timestamp,memory_usage_bytes\n";
        
        auto now = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(now - startTime_);
        
        for (size_t i = 0; i < memoryUsage_.size(); ++i) {
            auto timestamp = startTime_ + std::chrono::duration<double>(i * 0.1); // 100ms intervals
            auto time_t = std::chrono::system_clock::to_time_t(timestamp);
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            ss << "," << memoryUsage_[i] << "\n";
        }
        
        return ss.str();
    }
    
private:
    // Platform-specific memory usage function
    size_t getCurrentMemoryUsage() const {
        #ifdef _WIN32
            // Windows implementation using GetProcessMemoryInfo
            return getWindowsMemoryUsage();
        #elif __linux__
            // Linux implementation using /proc/self/status
            return getLinuxMemoryUsage();
        #elif __APPLE__
            // macOS implementation using mach/mach_host.h
            return getMacMemoryUsage();
        #else
            // Fallback implementation
            return getGenericMemoryUsage();
        #endif
    }
    
    size_t getWindowsMemoryUsage() const {
        // Windows-specific implementation
        // Would use GetProcessMemoryInfo from psapi.h
        return 0; // Placeholder
    }
    
    size_t getLinuxMemoryUsage() const {
        // Linux-specific implementation
        // Would read from /proc/self/status
        return 0; // Placeholder
    }
    
    size_t getMacMemoryUsage() const {
        // macOS-specific implementation
        // Would use mach/mach_host.h
        return 0; // Placeholder
    }
    
    size_t getGenericMemoryUsage() const {
        // Generic implementation using malloc_info or similar
        return 0; // Placeholder
    }
    
    mutable std::mutex mutex_;
};

// CPU profiler for tracking CPU usage
class CPUMProfiler {
private:
    struct CPUSample {
        std::chrono::high_resolution_clock::time_point timestamp;
        double systemTime;
        double userTime;
        double niceTime;
        double idleTime;
        double iowaitTime;
        double irqTime;
        double softirqTime;
        double stealTime;
        double guestTime;
        double guestNiceTime;
    };
    
    std::vector<CPUSample> samples_;
    bool recording_ = false;
    size_t samplingInterval_ = 100; // samples every 100 ms
    
public:
    CPUMProfiler() = default;
    
    // Start CPU profiling
    void start() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (recording_) return;
        
        recording_ = true;
        samples_.clear();
    }
    
    // Stop CPU profiling
    void stop() {
        std::lock_guard<std::mutex> lock(mutex_);
        recording_ = false;
    }
    
    // Record CPU sample
    void recordSample() {
        std::lock_guard<std::mutex> lock(mutex_);
        if (!recording_) return;
        
        CPUSample sample;
        sample.timestamp = std::chrono::high_resolution_clock::now();
        
        // Platform-specific CPU time collection
        getCPUTimes(sample);
        
        samples_.push_back(sample);
        
        // Trim if too many samples
        if (samples_.size() > 10000) {
            samples_.erase(samples_.begin(), samples_.begin() + 5000);
        }
    }
    
    // Get current CPU usage
    double getCurrentCPUUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (samples_.empty()) return 0.0;
        
        const auto& latest = samples_.back();
        double total = latest.systemTime + latest.userTime + latest.niceTime;
        double idle = latest.idleTime + latest.iowaitTime;
        
        if (total == 0.0) return 0.0;
        
        return 100.0 * (1.0 - (idle / total));
    }
    
    // Get average CPU usage
    double getAverageCPUUsage() const {
        std::lock_guard<std::mutex> lock(mutex_);
        if (samples_.empty()) return 0.0;
        
        double totalUsage = 0.0;
        for (const auto& sample : samples_) {
            totalUsage += getCurrentCPUUsage();
        }
        
        return totalUsage / samples_.size();
    }
    
    // Export CPU data to CSV
    std::string exportToCSV() const {
        std::lock_guard<std::mutex> lock(mutex_);
        std::stringstream ss;
        
        // Header
        ss << "timestamp,system,user,nice,idle,iowait,irq,softirq,steal,guest,guest_nice,cpu_usage_percent\n";
        
        for (const auto& sample : samples_) {
            auto time_t = std::chrono::system_clock::to_time_t(sample.timestamp);
            ss << std::put_time(std::localtime(&time_t), "%Y-%m-%d %H:%M:%S");
            ss << "," << sample.systemTime;
            ss << "," << sample.userTime;
            ss << "," << sample.niceTime;
            ss << "," << sample.idleTime;
            ss << "," << sample.iowaitTime;
            ss << "," << sample.irqTime;
            ss << "," << sample.softirqTime;
            ss << "," << sample.stealTime;
            ss << "," << sample.guestTime;
            ss << "," << sample.guestNiceTime;
            
            double cpuUsage = getCurrentCPUUsage();
            ss << "," << cpuUsage << "\n";
        }
        
        return ss.str();
    }
    
private:
    // Platform-specific CPU time collection
    void getCPUTimes(CPUSample& sample) const {
        #ifdef _WIN32
            getWindowsCPUTimes(sample);
        #elif __linux__
            getLinuxCPUTimes(sample);
        #elif __APPLE__
            getMacCPUTimes(sample);
        #else
            getGenericCPUTimes(sample);
        #endif
    }
    
    void getWindowsCPUTimes(CPUSample& sample) const {
        // Windows implementation using GetProcessTimes
        // Would use GetProcessTimes from windows.h
    }
    
    void getLinuxCPUTimes(CPUSample& sample) const {
        // Linux implementation reading from /proc/stat
        // Would read from /proc/stat
    }
    
    void getMacCPUTimes(CPUSample& sample) const {
        // macOS implementation using getrusage
        // Would use getrusage from sys/resource.h
    }
    
    void getGenericCPUTimes(CPUSample& sample) const {
        // Generic implementation
        // Would use clock_gettime or similar
    }
    
    mutable std::mutex mutex_;
};

// Global performance profiler instances
inline PerformanceProfiler& getPerformanceProfiler() {
    static PerformanceProfiler profiler;
    return profiler;
}

inline MemoryProfiler& getMemoryProfiler() {
    static MemoryProfiler profiler;
    return profiler;
}

inline CPUMProfiler& getCPUMProfiler() {
    static CPUMProfiler profiler;
    return profiler;
}

// Convenience functions for performance profiling
inline void startPerformanceProfiling(const std::string& sessionName = "default") {
    getPerformanceProfiler().start();
    getMemoryProfiler().start();
    getCPUMProfiler().start();
}

inline void stopPerformanceProfiling() {
    getPerformanceProfiler().stop();
    getMemoryProfiler().stop();
    getCPUMProfiler().stop();
}

inline void recordPerformanceSample(PerformanceMetric metric, double value, const std::string& description = "") {
    getPerformanceProfiler().recordSample(metric, value, description);
}

inline void updatePerformanceCounter(const std::string& name, double value) {
    getPerformanceProfiler().updateCounter(name, value);
}

inline void recordMemoryUsage() {
    getMemoryProfiler().recordUsage();
}

inline void recordCPUSample() {
    getCPUMProfiler().recordSample();
}

inline std::string exportPerformanceData() {
    std::stringstream ss;
    ss << "=== PERFORMANCE PROFILING DATA ===\n\n";
    ss << "Performance Profiler:\n";
    ss << getPerformanceProfiler().exportSamplesToJSON();
    ss << "\n\nMemory Profiler:\n";
    ss << getMemoryProfiler().exportToCSV();
    ss << "\n\nCPU Profiler:\n";
    ss << getCPUMProfiler().exportToCSV();
    return ss.str();
}

inline std::string exportPerformanceCountersCSV() {
    return getPerformanceProfiler().exportCountersToCSV();
}

} // namespace nlm