// Performance monitoring utilities for NLM
// Provides profiling, statistics collection, and performance metrics

#ifndef NLM_PERFORMANCE_MONITORING_H
#define NLM_PERFORMANCE_MONITORING_H

#include <chrono>
#include <vector>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <numeric>
#include <sstream>
#include <iomanip>

namespace nlm {

// Performance metrics for different system components
struct PerformanceMetrics {
    // Timing metrics
    std::chrono::duration<double> cpu_time{0.0};
    std::chrono::duration<double> wall_time{0.0};
    
    // Operation counts
    size_t operations{0};
    size_t spikes{0};
    size_t synaptic_events{0};
    
    // Memory usage
    size_t memory_bytes{0};
    
    // Statistical data
    double min_value{0.0};
    double max_value{0.0};
    double avg_value{0.0};
    double median_value{0.0};
    std::vector<double> sample_values;
    
    // Component-specific metrics
    std::unordered_map<std::string, PerformanceMetrics> component_metrics;
    
    PerformanceMetrics() : sample_values() {}
    
    // Add a sample value for statistical calculation
    void addSample(double value) {
        sample_values.push_back(value);
        if (sample_values.size() == 1) {
            min_value = max_value = avg_value = value;
        } else {
            min_value = std::min(min_value, value);
            max_value = std::max(max_value, value);
            avg_value = (avg_value * (sample_values.size() - 1) + value) / sample_values.size();
        }
    }
    
    // Calculate statistics
    void calculateStatistics() {
        if (sample_values.empty()) return;
        
        // Sort for median calculation
        std::vector<double> sorted = sample_values;
        std::sort(sorted.begin(), sorted.end());
        
        if (sorted.size() % 2 == 0) {
            median_value = (sorted[sorted.size()/2 - 1] + sorted[sorted.size()/2]) / 2.0;
        } else {
            median_value = sorted[sorted.size()/2];
        }
    }
    
    // Reset metrics
    void reset() {
        cpu_time = wall_time = std::chrono::duration<double>(0.0);
        operations = spikes = synaptic_events = memory_bytes = 0;
        min_value = max_value = avg_value = median_value = 0.0;
        sample_values.clear();
        component_metrics.clear();
    }
};

// Performance monitor for tracking system-wide metrics
class PerformanceMonitor {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    PerformanceMetrics global_metrics;
    std::unordered_map<std::string, PerformanceMetrics> component_metrics;
    bool enabled{false};
    
public:
    PerformanceMonitor() : start_time(std::chrono::high_resolution_clock::now()) {}
    
    // Start monitoring
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
        enabled = true;
    }
    
    // Stop monitoring and calculate final metrics
    void stop() {
        auto end_time = std::chrono::high_resolution_clock::now();
        global_metrics.wall_time = std::chrono::duration<double>(end_time - start_time);
        enabled = false;
    }
    
    // Get current wall clock time
    double getElapsedTime() const {
        if (!enabled) return global_metrics.wall_time.count();
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(now - start_time).count();
    }
    
    // Increment operation count for component
    void incrementOperation(const std::string& component, double value = 1.0) {
        if (!enabled) return;
        global_metrics.operations++;
        global_metrics.addSample(value);
        
        if (component_metrics.find(component) == component_metrics.end()) {
            component_metrics[component] = PerformanceMetrics();
        }
        component_metrics[component].addSample(value);
    }
    
    // Record spike event
    void recordSpike() {
        incrementOperation("spikes", 1.0);
    }
    
    // Record synaptic event
    void recordSynapticEvent() {
        incrementOperation("synaptic_events", 1.0);
    }
    
    // Get metrics for component
    PerformanceMetrics getMetrics(const std::string& component) const {
        PerformanceMetrics metrics;
        if (component_metrics.find(component) != component_metrics.end()) {
            metrics = component_metrics.at(component);
        }
        metrics.calculateStatistics();
        return metrics;
    }
    
    // Get global metrics
    PerformanceMetrics getGlobalMetrics() const {
        PerformanceMetrics metrics = global_metrics;
        metrics.calculateStatistics();
        return metrics;
    }
    
    // Reset all metrics
    void reset() {
        global_metrics.reset();
        component_metrics.clear();
    }
    
    // Generate performance report
    std::string generateReport() const {
        std::stringstream ss;
        ss << "=== NLM Performance Report ===\n";
        ss << "Wall Time: " << global_metrics.wall_time.count() << " seconds\n";
        ss << "Total Operations: " << global_metrics.operations << "\n";
        ss << "Total Spikes: " << global_metrics.spikes << "\n";
        ss << "Total Synaptic Events: " << global_metrics.synaptic_events << "\n";
        ss << "\nComponent Statistics:\n";
        
        for (const auto& pair : component_metrics) {
            const auto& metrics = pair.second;
            ss << "  " << pair.first << ": "
               << "Min: " << metrics.min_value << " "
               << "Max: " << metrics.max_value << " "
               << "Avg: " << metrics.avg_value << " "
               << "Median: " << metrics.median_value << " "
               << "Samples: " << metrics.sample_values.size() << "\n";
        }
        
        return ss.str();
    }
};

// Timer utility for measuring specific operation durations
class Timer {
private:
    std::chrono::time_point<std::chrono::high_resolution_clock> start_time;
    std::string name;
    bool running{false};
    
public:
    Timer(const std::string& timer_name = "Timer") : name(timer_name) {}
    
    // Start the timer
    void start() {
        start_time = std::chrono::high_resolution_clock::now();
        running = true;
    }
    
    // Stop the timer and return elapsed time in seconds
    double stop() {
        if (!running) return 0.0;
        
        auto end_time = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(end_time - start_time);
        running = false;
        return duration.count();
    }
    
    // Get elapsed time if timer is running
    double getElapsed() const {
        if (!running) return 0.0;
        auto now = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(now - start_time).count();
    }
    
    // Check if timer is running
    bool isRunning() const { return running; }
}; 

// Simplified performance monitor interface
inline PerformanceMonitor& getPerformanceMonitor() {
    static PerformanceMonitor monitor;
    return monitor;
}

// Convenience macros for timing operations
#define TIME_OPERATION(name) Timer name##_timer(name); name##_timer.start();
#define END_OPERATION(name) double name##_elapsed = name##_timer.stop();
#define RECORD_PERFORMANCE(component, value) getPerformanceMonitor().incrementOperation(component, value);
#define RECORD_SPIKE() getPerformanceMonitor().recordSpike();
#define RECORD_SYNAPTIC_EVENT() getPerformanceMonitor().recordSynapticEvent();

} // namespace nlm

#endif // NLM_PERFORMANCE_MONITORING_H
