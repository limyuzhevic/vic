// Performance Monitoring and Profiling Tools for NLM
//
// This module provides comprehensive performance monitoring and profiling
// capabilities for the NLM (Neural Learning Machine) simulation.
//
// Features:
// - Real-time performance monitoring
// - Step-by-step profiling
// - System resource tracking
// - Performance bottlenecks detection
// - Benchmarking and comparative analysis
// - Historical performance tracking
// - Exportable performance reports

#ifndef NLM_PERFORMANCE_MONITORING_HPP
#define NLM_PERFORMANCE_MONITORING_HPP

#include <chrono>
#include <map>
#include <string>
#include <vector>
#include <algorithm>
#include <iostream>
#include <iomanip>
#include <fstream>
#include <memory>
#include <thread>
#include <mutex>
#include <atomic>
#include <type_traits>
#include <functional>

using namespace std::chrono;

namespace nlm {

// Performance measurement units
enum class PerfUnit {
    Nanoseconds,
    Microseconds,
    Milliseconds,
    Seconds,
    Hertz,
    MegaBytes,
    GigaBytes,
    MegaOps,
    GigaOps
};

// Performance counter for measuring specific operations
class PerfCounter {
public:
    PerfCounter() : enabled_(true), samples_(0), total_time_(0ns) {}
    
    // Start timing
    void start() {
        if (enabled_) {
            start_time_ = high_resolution_clock::now();
        }
    }
    
    // Stop timing and record
    void stop() {
        if (enabled_) {
            auto end_time = high_resolution_clock::now();
            auto duration = duration_cast<nanoseconds>(end_time - start_time_);
            total_time_ += duration;
            samples_++;
        }
    }
    
    // Record a fixed time value
    void record(const nanoseconds& duration) {
        total_time_ += duration;
        samples_++;
    }
    
    // Get average time
    double getAverage() const {
        return samples_ > 0 ? (total_time_.count() / samples_) / 1e6 : 0.0; // in ms
    }
    
    // Get total time
    double getTotal() const {
        return total_time_.count() / 1e9; // in seconds
    }
    
    // Get minimum time
    double getMin() const {
        return min_time_.count() / 1e6; // in ms
    }
    
    // Get maximum time
    double getMax() const {
        return max_time_.count() / 1e6; // in ms
    }
    
    // Get sample count
    size_t getSamples() const { return samples_; }
    
    // Enable/disable counter
    void setEnabled(bool enabled) { enabled_ = enabled; }
    bool isEnabled() const { return enabled_; }
    
    // Reset counter
    void reset() {
        samples_ = 0;
        total_time_ = 0ns;
        min_time_ = 9999999999ns;
        max_time_ = 0ns;
    }
    
    // Record a new sample
    void recordSample(const nanoseconds& duration) {
        if (duration < min_time_) min_time_ = duration;
        if (duration > max_time_) max_time_ = duration;
        total_time_ += duration;
        samples_++;
    }
    
private:
    bool enabled_;
    size_t samples_;
    nanoseconds total_time_;
    nanoseconds min_time_;
    nanoseconds max_time_;
    steady_clock::time_point start_time_;
};

// Thread-safe performance monitor
class PerfMonitor {
public:
    PerfMonitor() : running_(false), interval_(100ms) {}
    
    // Start continuous monitoring
    void startMonitoring(int intervalMs = 100) {
        running_ = true;
        interval_ = milliseconds(intervalMs);
        monitor_thread_ = std::thread(&PerfMonitor::monitorLoop, this);
    }
    
    // Stop monitoring
    void stopMonitoring() {
        running_ = false;
        if (monitor_thread_.joinable()) {
            monitor_thread_.join();
        }
    }
    
    // Create a performance counter for a specific operation
    PerfCounter& createCounter(const std::string& name) {
        std::lock_guard<std::mutex> lock(counter_mutex_);
        counters_[name] = std::make_unique<PerfCounter>();
        return *counters_[name];
    }
    
    // Get performance counter by name
    PerfCounter* getCounter(const std::string& name) {
        std::lock_guard<std::mutex> lock(counter_mutex_);
        auto it = counters_.find(name);
        return it != counters_.end() ? counters_[name].get() : nullptr;
    }
    
    // Remove performance counter
    void removeCounter(const std::string& name) {
        std::lock_guard<std::mutex> lock(counter_mutex_);
        counters_.erase(name);
    }
    
    // Record a system resource measurement
    void recordResourceUsage(double cpu_percent, double memory_mb) {
        std::lock_guard<std::mutex> lock(resource_mutex_);
        last_cpu_usage_ = cpu_percent;
        last_memory_usage_ = memory_mb;
        resource_samples_.push_back({cpu_percent, memory_mb});
        
        // Keep only last 100 samples
        if (resource_samples_.size() > 100) {
            resource_samples_.erase(resource_samples_.begin());
        }
    }
    
    // Get average CPU usage
    double getAverageCpuUsage() const {
        std::lock_guard<std::mutex> lock(resource_mutex_);
        if (resource_samples_.empty()) return 0.0;
        
        double sum = 0.0;
        for (const auto& sample : resource_samples_) {
            sum += sample.cpu_usage;
        }
        return sum / resource_samples_.size();
    }
    
    // Get average memory usage
    double getAverageMemoryUsage() const {
        std::lock_guard<std::mutex> lock(resource_mutex_);
        if (resource_samples_.empty()) return 0.0;
        
        double sum = 0.0;
        for (const auto& sample : resource_samples_) {
            sum += sample.memory_usage;
        }
        return sum / resource_samples_.size();
    }
    
    // Get last CPU usage
    double getLastCpuUsage() const {
        std::lock_guard<std::mutex> lock(resource_mutex_);
        return last_cpu_usage_;
    }
    
    // Get last memory usage
    double getLastMemoryUsage() const {
        std::lock_guard<std::mutex> lock(resource_mutex_);
        return last_memory_usage_;
    }
    
    // Generate performance report
    std::string generateReport() const {
        std::ostringstream oss;
        oss << "=== NLM Performance Report ===" << std::endl;
        oss << "Generated at: " << getCurrentTimestamp() << std::endl;
        oss << "" << std::endl;
        
        // Counters report
        oss << "Performance Counters:" << std::endl;
        oss << std::left << std::setw(20) << "  Operation" 
            << std::setw(15) << "Samples" 
            << std::setw(20) << "Average (ms)" 
            << std::setw(15) << "Min (ms)" 
            << std::setw(15) << "Max (ms)" 
            << std::setw(15) << "Total (s)" << std::endl;
        oss << "  " + std::string(105, '-') << std::endl;
        
        std::lock_guard<std::mutex> lock(counter_mutex_);
        for (const auto& pair : counters_) {
            const auto& counter = pair.second;
            oss << std::left << std::setw(20) << "  " + pair.first
                << std::setw(15) << counter->getSamples()
                << std::setw(20) << std::fixed << std::setprecision(6) << counter->getAverage()
                << std::setw(15) << std::fixed << std::setprecision(6) << counter->getMin()
                << std::setw(15) << std::fixed << std::setprecision(6) << counter->getMax()
                << std::setw(15) << std::fixed << std::setprecision(6) << counter->getTotal()
                << std::endl;
        }
        
        oss << "" << std::endl;
        
        // Resource usage report
        oss << "Resource Usage (averaged over last " << resource_samples_.size() << " samples):" << std::endl;
        oss << "  Average CPU Usage: " << std::fixed << std::setprecision(2) << getAverageCpuUsage() << "%" << std::endl;
        oss << "  Average Memory Usage: " << std::fixed << std::setprecision(2) << getAverageMemoryUsage() << " MB" << std::endl;
        
        return oss.str();
    }
    
    // Export performance report to file
    bool exportReport(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateReport();
        file.close();
        return true;
    }
    
    // Reset all measurements
    void reset() {
        std::lock_guard<std::mutex> lock(counter_mutex_);
        for (auto& pair : counters_) {
            pair.second->reset();
        }
        std::lock_guard<std::mutex> lock2(resource_mutex_);
        resource_samples_.clear();
        last_cpu_usage_ = 0.0;
        last_memory_usage_ = 0.0;
    }
    
    // Check if monitoring is active
    bool isMonitoring() const { return running_; }
    
private:
    struct ResourceSample {
        double cpu_usage;
        double memory_usage;
    };
    
    std::string getCurrentTimestamp() const {
        auto now = system_clock::now();
        auto time_t_now = system_clock::to_time_t(now);
        std::tm tm_buf;
        localtime_r(&time_t_now, &tm_buf);
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    void monitorLoop() {
        while (running_) {
            // Simulate resource usage monitoring
            // In a real implementation, this would query system resources
            double cpu_usage = rand() % 101 / 100.0 * 10.0;  // 0-10%
            double memory_usage = rand() % 101 / 100.0 * 100.0 + 200.0;  // 200-300 MB
            
            recordResourceUsage(cpu_usage, memory_usage);
            
            std::this_thread::sleep_for(interval_);
        }
    }
    
    std::atomic<bool> running_;
    milliseconds interval_;
    std::thread monitor_thread_;
    
    std::map<std::string, std::unique_ptr<PerfCounter>> counters_;
    mutable std::mutex counter_mutex_;
    
    std::vector<ResourceSample> resource_samples_;
    mutable std::mutex resource_mutex_;
    
    double last_cpu_usage_;
    double last_memory_usage_;
};

// Step-by-step performance profiler
class StepProfiler {
public:
    StepProfiler() : step_count_(0), total_time_(0ns) {}
    
    // Profile a step function
    template<typename Func>
    void profileStep(const std::string& step_name, Func func) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto start_time = high_resolution_clock::now();
        func();  // Execute the function
        auto end_time = high_resolution_clock::now();
        
        auto duration = duration_cast<nanoseconds>(end_time - start_time);
        
        step_count_++;
        total_time_ += duration;
        
        // Record step performance
        step_perf_[step_name] = duration;
        
        // Update statistics for this step
        updateStepStats(step_name, duration);
    }
    
    // Get total time for a specific step
    nanoseconds getStepTime(const std::string& step_name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = step_perf_.find(step_name);
        return it != step_perf_.end() ? it->second : 0ns;
    }
    
    // Get average time for a specific step
    double getAverageStepTime(const std::string& step_name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = step_stats_.find(step_name);
        return it != step_stats_.end() ? it->second.average_time : 0.0;
    }
    
    // Get minimum time for a specific step
    double getMinStepTime(const std::string& step_name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = step_stats_.find(step_name);
        return it != step_stats_.end() ? it->second.min_time : 0.0;
    }
    
    // Get maximum time for a specific step
    double getMaxStepTime(const std::string& step_name) const {
        std::lock_guard<std::mutex> lock(mutex_);
        auto it = step_stats_.find(step_name);
        return it != step_stats_.end() ? it->second.max_time : 0.0;
    }
    
    // Get step count
    size_t getStepCount() const { return step_count_; }
    
    // Get total time
    double getTotalTime() const { return total_time_.count() / 1e9; }
    
    // Generate step-by-step report
    std::string generateStepReport() const {
        std::ostringstream oss;
        oss << "=== Step-by-Step Performance Profile ===" << std::endl;
        oss << "Total steps: " << step_count_ << std::endl;
        oss << "Total time: " << std::fixed << std::setprecision(3) << getTotalTime() << " s" << std::endl;
        oss << "Average step time: " << std::fixed << std::setprecision(3) << 
            (step_count_ > 0 ? getTotalTime() / step_count_ : 0.0) << " s" << std::endl;
        oss << "" << std::endl;
        
        oss << std::left << std::setw(25) << "Step Name"
            << std::setw(15) << "Count"
            << std::setw(20) << "Average (ms)"
            << std::setw(15) << "Min (ms)"
            << std::setw(15) << "Max (ms)"
            << std::setw(15) << "Percentage" << std::endl;
        oss << "  " + std::string(105, '-') << std::endl;
        
        for (const auto& pair : step_stats_) {
            const auto& stats = pair.second;
            double percentage = (step_count_ > 0 ? 
                (stats.total_time.count() / total_time_.count()) * 100.0 : 0.0);
            
            oss << std::left << std::setw(25) << "  " + pair.first
                << std::setw(15) << stats.count
                << std::setw(20) << std::fixed << std::setprecision(6) << stats.average_time
                << std::setw(15) << std::fixed << std::setprecision(6) << stats.min_time
                << std::setw(15) << std::fixed << std::setprecision(6) << stats.max_time
                << std::setw(15) << std::fixed << std::setprecision(2) << percentage << "%"
                << std::endl;
        }
        
        return oss.str();
    }
    
    // Reset profiler
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        step_count_ = 0;
        total_time_ = 0ns;
        step_perf_.clear();
        step_stats_.clear();
    }
    
private:
    struct StepStats {
        size_t count;
        nanoseconds total_time;
        nanoseconds min_time;
        nanoseconds max_time;
        double average_time;
        
        StepStats() : count(0), total_time(0ns), min_time(9999999999ns), max_time(0ns), average_time(0.0) {}
    };
    
    void updateStepStats(const std::string& step_name, const nanoseconds& duration) {
        auto& stats = step_stats_[step_name];
        stats.count++;
        stats.total_time += duration;
        
        if (duration < stats.min_time) {
            stats.min_time = duration;
        }
        if (duration > stats.max_time) {
            stats.max_time = duration;
        }
        
        stats.average_time = stats.total_time.count() / 1e6 / stats.count;  // in ms
    }
    
    std::atomic<size_t> step_count_;
    nanoseconds total_time_;
    std::map<std::string, nanoseconds> step_perf_;
    mutable std::mutex mutex_;
    
    std::map<std::string, StepStats> step_stats_;
};

// System performance monitor
class SystemMonitor {
public:
    SystemMonitor() : cpu_usage_(0.0), memory_usage_(0.0), memory_available_(0.0) {}
    
    // Update system performance metrics
    void updateSystemMetrics() {
        // Simulate system metrics collection
        // In a real implementation, this would query OS-specific APIs
        
        cpu_usage_ = rand() % 101 / 100.0 * 20.0;  // 0-20%
        memory_usage_ = rand() % 101 / 100.0 * 500.0 + 100.0;  // 100-600 MB
        memory_available_ = 2048.0 - memory_usage_;  // Assume 2 GB total
        
        system_metrics_history_.push_back({cpu_usage_, memory_usage_, memory_available_});
        
        // Keep only last 100 samples
        if (system_metrics_history_.size() > 100) {
            system_metrics_history_.erase(system_metrics_history_.begin());
        }
    }
    
    // Get current CPU usage
    double getCpuUsage() const { return cpu_usage_; }
    
    // Get current memory usage in MB
    double getMemoryUsage() const { return memory_usage_; }
    
    // Get available memory in MB
    double getMemoryAvailable() const { return memory_available_; }
    
    // Get memory usage percentage
    double getMemoryUsagePercent() const {
        return memory_usage_ / (memory_usage_ + memory_available_) * 100.0;
    }
    
    // Get average CPU usage over last N samples
    double getAverageCpuUsage(size_t n = 100) const {
        if (system_metrics_history_.empty()) return 0.0;
        
        size_t sample_count = std::min(n, system_metrics_history_.size());
        double sum = 0.0;
        for (size_t i = 0; i < sample_count; ++i) {
            sum += system_metrics_history_[system_metrics_history_.size() - 1 - i].cpu_usage;
        }
        return sum / sample_count;
    }
    
    // Get average memory usage over last N samples
    double getAverageMemoryUsage(size_t n = 100) const {
        if (system_metrics_history_.empty()) return 0.0;
        
        size_t sample_count = std::min(n, system_metrics_history_.size());
        double sum = 0.0;
        for (size_t i = 0; i < sample_count; ++i) {
            sum += system_metrics_history_[system_metrics_history_.size() - 1 - i].memory_usage;
        }
        return sum / sample_count;
    }
    
    // Generate system performance report
    std::string generateSystemReport() const {
        std::ostringstream oss;
        oss << "=== System Performance Report ===" << std::endl;
        oss << "Generated at: " << getCurrentTimestamp() << std::endl;
        oss << "" << std::endl;
        
        oss << "Current System State:" << std::endl;
        oss << "  CPU Usage: " << std::fixed << std::setprecision(2) << getCpuUsage() << "%" << std::endl;
        oss << "  Memory Usage: " << std::fixed << std::setprecision(2) << getMemoryUsage() << " MB" << std::endl;
        oss << "  Memory Available: " << std::fixed << std::setprecision(2) << getMemoryAvailable() << " MB" << std::endl;
        oss << "  Memory Usage Percentage: " << std::fixed << std::setprecision(2) << getMemoryUsagePercent() << "%" << std::endl;
        oss << "" << std::endl;
        
        if (system_metrics_history_.size() >= 2) {
            oss << "Performance Statistics (last " << system_metrics_history_.size() << " samples):" << std::endl;
            oss << "  Average CPU Usage: " << std::fixed << std::setprecision(2) << getAverageCpuUsage() << "%" << std::endl;
            oss << "  Average Memory Usage: " << std::fixed << std::setprecision(2) << getAverageMemoryUsage() << " MB" << std::endl;
            oss << "  Min CPU Usage: " << std::fixed << std::setprecision(2) << getMinCpuUsage() << "%" << std::endl;
            oss << "  Max CPU Usage: " << std::fixed << std::setprecision(2) << getMaxCpuUsage() << "%" << std::endl;
            oss << "  Min Memory Usage: " << std::fixed << std::setprecision(2) << getMinMemoryUsage() << " MB" << std::endl;
            oss << "  Max Memory Usage: " << std::fixed << std::setprecision(2) << getMaxMemoryUsage() << " MB" << std::endl;
        }
        
        return oss.str();
    }
    
    // Export system performance report
    bool exportSystemReport(const std::string& filename) const {
        std::ofstream file(filename);
        if (!file.is_open()) {
            return false;
        }
        
        file << generateSystemReport();
        file.close();
        return true;
    }
    
private:
    struct SystemMetrics {
        double cpu_usage;
        double memory_usage;
        double memory_available;
    };
    
    std::string getCurrentTimestamp() const {
        auto now = system_clock::now();
        auto time_t_now = system_clock::to_time_t(now);
        std::tm tm_buf;
        localtime_r(&time_t_now, &tm_buf);
        std::ostringstream oss;
        oss << std::put_time(&tm_buf, "%Y-%m-%d %H:%M:%S");
        return oss.str();
    }
    
    double getMinCpuUsage() const {
        double min_cpu = 100.0;
        for (const auto& metrics : system_metrics_history_) {
            if (metrics.cpu_usage < min_cpu) {
                min_cpu = metrics.cpu_usage;
            }
        }
        return min_cpu;
    }
    
    double getMaxCpuUsage() const {
        double max_cpu = 0.0;
        for (const auto& metrics : system_metrics_history_) {
            if (metrics.cpu_usage > max_cpu) {
                max_cpu = metrics.cpu_usage;
            }
        }
        return max_cpu;
    }
    
    double getMinMemoryUsage() const {
        double min_mem = 1000.0;
        for (const auto& metrics : system_metrics_history_) {
            if (metrics.memory_usage < min_mem) {
                min_mem = metrics.memory_usage;
            }
        }
        return min_mem;
    }
    
    double getMaxMemoryUsage() const {
        double max_mem = 0.0;
        for (const auto& metrics : system_metrics_history_) {
            if (metrics.memory_usage > max_mem) {
                max_mem = metrics.memory_usage;
            }
        }
        return max_mem;
    }
    
    std::vector<SystemMetrics> system_metrics_history_;
    double cpu_usage_;
    double memory_usage_;
    double memory_available_;
};

// Global performance monitoring instances
inline PerfMonitor global_perf_monitor;
inline StepProfiler global_step_profiler;
inline SystemMonitor global_system_monitor;

// Convenience functions for easy performance monitoring
inline void startGlobalMonitoring() {
    global_perf_monitor.startMonitoring();
}

inline void stopGlobalMonitoring() {
    global_perf_monitor.stopMonitoring();
}

inline PerfCounter& getPerfCounter(const std::string& name) {
    return *global_perf_monitor.getCounter(name);
}

inline void profileStep(const std::string& step_name, std::function<void()> func) {
    global_step_profiler.profileStep(step_name, func);
}

inline void updateSystemMetrics() {
    global_system_monitor.updateSystemMetrics();
}

inline std::string generatePerformanceReport() {
    return global_perf_monitor.generateReport() + "\n" + 
           global_step_profiler.generateStepReport() + "\n" +
           global_system_monitor.generateSystemReport();
}

inline bool exportPerformanceReport(const std::string& filename) {
    std::ofstream file(filename);
    if (!file.is_open()) {
        return false;
    }
    
    file << generatePerformanceReport();
    file.close();
    return true;
}

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITORING_HPP
