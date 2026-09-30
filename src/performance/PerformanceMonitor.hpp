// Performance Monitoring and Profiling Tools for NLM
// Provides comprehensive performance monitoring, profiling, and analysis capabilities

#pragma once

#include <chrono>
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <thread>
#include <atomic>
#include <functional>
#include <unordered_map>
#include <mutex>

namespace nlm {

// Performance metrics structure
struct PerformanceMetrics {
    // Time measurements
    double totalTime = 0.0;           // Total execution time in seconds
    double initializationTime = 0.0; // Brain initialization time
    double simulationTime = 0.0;     // Total simulation time
    double stepTime = 0.0;           // Average time per simulation step
    double bottleneckTime = 0.0;     // Time spent in bottleneck operations
    
    // Resource usage
    size_t peakMemoryUsage = 0;      // Peak memory usage in bytes
    size_t currentMemoryUsage = 0;   // Current memory usage in bytes
    int cpuUsage = 0;                // CPU usage percentage
    int threadCount = 0;             // Number of active threads
    
    // Activity metrics
    size_t totalSpikes = 0;          // Total spikes generated
    size_t spikeRate = 0;            // Spikes per second
    double firingRate = 0.0;         // Average firing rate per neuron
    size_t totalPlastictyEvents = 0; // Total plasticity events
    
    // Efficiency metrics
    double computeEfficiency = 0.0;  // Utilization of computational resources
    double memoryEfficiency = 0.0;   // Memory utilization efficiency
    double throughput = 0.0;         // Operations per second
    
    // Timestamps
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point endTime;
    
    PerformanceMetrics() : startTime(std::chrono::steady_clock::now()) {}
    
    void updateTimestamps() {
        endTime = std::chrono::steady_clock::now();
        totalTime = std::chrono::duration<double>(endTime - startTime).count();
    }
    
    double getStepTime(size_t steps) const {
        return steps > 0 ? totalTime / steps : 0.0;
    }
};

// Performance counter for measuring specific operations
class PerformanceCounter {
public:
    PerformanceCounter() : count(0), totalTime(0.0), minTime(std::numeric_limits<double>::max()), 
                         maxTime(0.0), lastTime(0.0), enabled(true) {}
    
    void start() {
        if (enabled) {
            startTime = std::chrono::high_resolution_clock::now();
        }
    }
    
    void stop() {
        if (enabled) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            
            std::lock_guard<std::mutex> lock(mutex);
            count++;
            totalTime += duration;
            minTime = std::min(minTime, duration);
            maxTime = std::max(maxTime, duration);
            lastTime = duration;
        }
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex);
        count = 0;
        totalTime = 0.0;
        minTime = std::numeric_limits<double>::max();
        maxTime = 0.0;
        lastTime = 0.0;
    }
    
    double getAverageTime() const {
        return count > 0 ? totalTime / count : 0.0;
    }
    
    double getMinTime() const { return minTime; }
    double getMaxTime() const { return maxTime; }
    double getLastTime() const { return lastTime; }
    size_t getCount() const { return count; }
    double getTotalTime() const { return totalTime; }
    
    void setEnabled(bool enabled) { this->enabled = enabled; }
    bool isEnabled() const { return enabled; }
    
private:
    std::chrono::high_resolution_clock::time_point startTime;
    mutable std::mutex mutex;
    size_t count;
    double totalTime;
    double minTime;
    double maxTime;
    double lastTime;
    bool enabled;
};

// Thread-safe performance monitor
class PerformanceMonitor {
public:
    PerformanceMonitor() : running(false), updateInterval(1.0), startTime(std::chrono::steady_clock::now()) {
        // Initialize standard performance counters
        counters["brain_initialization"] = std::make_unique<PerformanceCounter>();
        counters["simulation_step"] = std::make_unique<PerformanceCounter>();
        counters["spike_processing"] = std::make_unique<PerformanceCounter>();
        counters["plasticity_update"] = std::make_unique<PerformanceCounter>();
        counters["memory_operation"] = std::make_unique<PerformanceCounter>();
        counters["neuron_update"] = std::make_unique<PerformanceCounter>();
    }
    
    ~PerformanceMonitor() {
        stopMonitoring();
    }
    
    // Start monitoring
    void startMonitoring() {
        running = true;
        monitorThread = std::thread(&PerformanceMonitor::monitorLoop, this);
    }
    
    // Stop monitoring
    void stopMonitoring() {
        if (running) {
            running = false;
            if (monitorThread.joinable()) {
                monitorThread.join();
            }
        }
    }
    
    // Record a performance counter
    void recordCounter(const std::string& name) {
        auto it = counters.find(name);
        if (it != counters.end()) {
            it->second->start();
        }
    }
    
    // Finish a performance counter
    void finishCounter(const std::string& name) {
        auto it = counters.find(name);
        if (it != counters.end()) {
            it->second->stop();
        }
    }
    
    // Update custom metric
    void updateMetric(const std::string& name, double value) {
        std::lock_guard<std::mutex> lock(metricsMutex);
        customMetrics[name] = value;
        lastUpdateTime = std::chrono::steady_clock::now();
    }
    
    // Get performance metrics
    PerformanceMetrics getMetrics() const {
        PerformanceMetrics metrics;
        metrics.startTime = startTime;
        metrics.updateTimestamps();
        
        // Calculate from counters
        for (const auto& pair : counters) {
            const auto& counter = pair.second;
            if (counter->getCount() > 0) {
                double avgTime = counter->getAverageTime();
                if (pair.first == "brain_initialization") {
                    metrics.initializationTime = avgTime;
                } else if (pair.first == "simulation_step") {
                    metrics.stepTime = avgTime;
                } else if (pair.first == "spike_processing") {
                    // Spike processing time affects performance
                } else if (pair.first == "plasticity_update") {
                    metrics.bottleneckTime += avgTime * counter->getCount();
                }
            }
        }
        
        // Add custom metrics
        std::lock_guard<std::mutex> lock(metricsMutex);
        for (const auto& pair : customMetrics) {
            if (pair.first == "spike_count") {
                metrics.totalSpikes = static_cast<size_t>(pair.second);
            } else if (pair.first == "memory_usage") {
                metrics.currentMemoryUsage = static_cast<size_t>(pair.second);
            } else if (pair.first == "cpu_usage") {
                metrics.cpuUsage = static_cast<int>(pair.second);
            }
        }
        
        return metrics;
    }
    
    // Get specific counter value
    double getCounterValue(const std::string& name) const {
        auto it = counters.find(name);
        if (it != counters.end()) {
            return it->second->getAverageTime();
        }
        return 0.0;
    }
    
    // Start a timed section
    class Timer {
    public:
        Timer(PerformanceMonitor& monitor, const std::string& name)
            : monitor(monitor), name(name) {
            monitor.recordCounter(name);
            startTime = std::chrono::high_resolution_clock::now();
        }
        
        ~Timer() {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration<double>(endTime - startTime).count();
            monitor.finishCounter(name);
            monitor.updateMetric(name + "_time", duration);
        }
        
    private:
        PerformanceMonitor& monitor;
        std::string name;
        std::chrono::high_resolution_clock::time_point startTime;
    };
    
    // Convenience timer creation
    Timer createTimer(const std::string& name) {
        return Timer(*this, name);
    }
    
    // Get monitoring status
    bool isRunning() const { return running; }
    double getUpdateInterval() const { return updateInterval; }
    
    // Set update interval
    void setUpdateInterval(double interval) {
        updateInterval = interval;
    }
    
    // Export metrics to JSON
    std::string exportToJSON() const {
        PerformanceMetrics metrics = getMetrics();
        
        json j;
        j["total_time"] = metrics.totalTime;
        j["initialization_time"] = metrics.initializationTime;
        j["simulation_time"] = metrics.simulationTime;
        j["step_time"] = metrics.stepTime;
        j["bottleneck_time"] = metrics.bottleneckTime;
        j["total_spikes"] = metrics.totalSpikes;
        j["spike_rate"] = metrics.spikeRate;
        j["firing_rate"] = metrics.firingRate;
        j["total_plasticity_events"] = metrics.totalPlastictyEvents;
        j["compute_efficiency"] = metrics.computeEfficiency;
        j["memory_efficiency"] = metrics.memoryEfficiency;
        j["throughput"] = metrics.throughput;
        j["cpu_usage"] = metrics.cpuUsage;
        j["thread_count"] = metrics.threadCount;
        
        // Add counter data
        json countersJson = json::object();
        for (const auto& pair : counters) {
            json counterJson;
            counterJson["count"] = pair.second->getCount();
            counterJson["average_time"] = pair.second->getAverageTime();
            counterJson["min_time"] = pair.second->getMinTime();
            counterJson["max_time"] = pair.second->getMaxTime();
            counterJson["last_time"] = pair.second->getLastTime();
            countersJson[pair.first] = counterJson;
        }
        j["counters"] = countersJson;
        
        // Add custom metrics
        json customJson = json::object();
        std::lock_guard<std::mutex> lock(const_cast<PerformanceMonitor*>(this)->metricsMutex);
        for (const auto& pair : customMetrics) {
            customJson[pair.first] = pair.second;
        }
        j["custom_metrics"] = customJson;
        
        return j.dump(2);
    }
    
    // Export metrics to CSV
    std::string exportToCSV() const {
        PerformanceMetrics metrics = getMetrics();
        
        std::stringstream csv;
        
        // Header
        csv << "metric,value" << std::endl;
        csv << "total_time," << metrics.totalTime << std::endl;
        csv << "initialization_time," << metrics.initializationTime << std::endl;
        csv << "simulation_time," << metrics.simulationTime << std::endl;
        csv << "step_time," << metrics.stepTime << std::endl;
        csv << "bottleneck_time," << metrics.bottleneckTime << std::endl;
        csv << "total_spikes," << metrics.totalSpikes << std::endl;
        csv << "spike_rate," << metrics.spikeRate << std::endl;
        csv << "firing_rate," << metrics.firingRate << std::endl;
        csv << "total_plasticity_events," << metrics.totalPlastictyEvents << std::endl;
        csv << "compute_efficiency," << metrics.computeEfficiency << std::endl;
        csv << "memory_efficiency," << metrics.memoryEfficiency << std::endl;
        csv << "throughput," << metrics.throughput << std::endl;
        csv << "cpu_usage," << metrics.cpuUsage << std::endl;
        csv << "thread_count," << metrics.threadCount << std::endl;
        
        // Counter data
        csv << std::endl << "counter_name,count,average_time,min_time,max_time,last_time" << std::endl;
        for (const auto& pair : counters) {
            const auto& counter = pair.second;
            csv << pair.first << "," 
                << counter->getCount() << "," 
                << counter->getAverageTime() << "," 
                << counter->getMinTime() << ","
                << counter->getMaxTime() << ","
                << counter->getLastTime() << std::endl;
        }
        
        return csv.str();
    }
    
    // Print performance report
    void printReport() const {
        PerformanceMetrics metrics = getMetrics();
        
        std::cout << "=== NLM Performance Report ===" << std::endl;
        std::cout << std::fixed << std::setprecision(3);
        std::cout << "Total execution time: " << metrics.totalTime << " seconds" << std::endl;
        std::cout << "Initialization time: " << metrics.initializationTime << " seconds" << std::endl;
        std::cout << "Average step time: " << metrics.stepTime << " seconds" << std::endl;
        std::cout << "Total spikes generated: " << metrics.totalSpikes << std::endl;
        std::cout << "Spike rate: " << metrics.spikeRate << " spikes/second" << std::endl;
        std::cout << "Firing rate: " << metrics.firingRate << " Hz" << std::endl;
        std::cout << "Total plasticity events: " << metrics.totalPlastictyEvents << std::endl;
        std::cout << "Throughput: " << metrics.throughput << " operations/second" << std::endl;
        std::cout << "CPU usage: " << metrics.cpuUsage << "%" << std::endl;
        std::cout << "Thread count: " << metrics.threadCount << std::endl;
        
        // Print counter details
        std::cout << std::endl << "Detailed Counter Statistics:" << std::endl;
        for (const auto& pair : counters) {
            const auto& counter = pair.second;
            if (counter->getCount() > 0) {
                std::cout << "  " << std::left << std::setw(25) << pair.first 
                          << ": count=" << counter->getCount()
                          << ", avg_time=" << counter->getAverageTime()
                          << ", min_time=" << counter->getMinTime()
                          << ", max_time=" << counter->getMaxTime()
                          << std::endl;
            }
        }
    }
    
    // Get specific counter
    PerformanceCounter* getCounter(const std::string& name) {
        auto it = counters.find(name);
        if (it != counters.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    // Add custom counter
    void addCounter(const std::string& name) {
        counters[name] = std::make_unique<PerformanceCounter>();
    }
    
    // Start monitoring thread loop
    void monitorLoop() {
        auto lastUpdate = std::chrono::steady_clock::now();
        
        while (running) {
            auto now = std::chrono::steady_clock::now();
            auto elapsed = std::chrono::duration<double>(now - lastUpdate).count();
            
            if (elapsed >= updateInterval) {
                // Update system metrics
                updateSystemMetrics();
                lastUpdate = now;
            }
            
            // Sleep briefly to avoid busy waiting
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
    
    // Update system metrics
    void updateSystemMetrics() {
        // Update memory usage (simplified)
        updateMetric("memory_usage", getMemoryUsage());
        
        // Update CPU usage (simplified)
        updateMetric("cpu_usage", getCPUUsage());
        
        // Update thread count
        updateMetric("thread_count", getThreadCount());
        
        // Update spike statistics
        updateMetric("spike_count", getSpikeCount());
    }
    
    // Get memory usage (simplified implementation)
    double getMemoryUsage() const {
        // This is a simplified implementation
        // In a real system, you would use platform-specific APIs
        return 0.0;
    }
    
    // Get CPU usage (simplified implementation)
    double getCPUUsage() const {
        // This is a simplified implementation
        // In a real system, you would use platform-specific APIs
        return 0.0;
    }
    
    // Get thread count (simplified implementation)
    double getThreadCount() const {
        // This is a simplified implementation
        // In a real system, you would count active threads
        return std::thread::hardware_concurrency();
    }
    
    // Get spike count (simplified implementation)
    double getSpikeCount() const {
        // This would be provided by the brain system
        return 0.0;
    }

private:
    std::unordered_map<std::string, std::unique_ptr<PerformanceCounter>> counters;
    std::unordered_map<std::string, double> customMetrics;
    std::mutex metricsMutex;
    bool running;
    double updateInterval;
    std::chrono::steady_clock::time_point startTime;
    std::chrono::steady_clock::time_point lastUpdateTime;
    std::thread monitorThread;
};

// Global performance monitor instance
PerformanceMonitor& getGlobalPerformanceMonitor() {
    static PerformanceMonitor monitor;
    return monitor;
}

// Convenience functions for timing operations
class ScopedTimer {
public:
    ScopedTimer(const std::string& name, PerformanceMonitor& monitor = getGlobalPerformanceMonitor())
        : name(name), monitor(monitor) {
        monitor.recordCounter(name);
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    ~ScopedTimer() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        monitor.finishCounter(name);
        monitor.updateMetric(name + "_duration", duration);
    }
    
    double getElapsedTime() const {
        auto endTime = std::chrono::high_resolution_clock::now();
        return std::chrono::duration<double>(endTime - startTime).count();
    }
    
private:
    std::string name;
    PerformanceMonitor& monitor;
    std::chrono::high_resolution_clock::time_point startTime;
};

// Benchmark function for comparing performance
class Benchmark {
public:
    Benchmark(const std::string& name, PerformanceMonitor& monitor = getGlobalPerformanceMonitor())
        : name(name), monitor(monitor) {
        monitor.addCounter(name);
        startTime = std::chrono::high_resolution_clock::now();
    }
    
    ~Benchmark() {
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration<double>(endTime - startTime).count();
        monitor.finishCounter(name);
        
        std::cout << "Benchmark " << name << ": " << duration << " seconds" << std::endl;
    }
    
private:
    std::string name;
    PerformanceMonitor& monitor;
    std::chrono::high_resolution_clock::time_point startTime;
};

// Profiling utilities
class Profiler {
public:
    Profiler(PerformanceMonitor& monitor = getGlobalPerformanceMonitor())
        : monitor(monitor) {}
    
    template<typename Func>
    auto profile(const std::string& name, Func&& func) {
        monitor.addCounter(name + "_total");
        
        // Start timers
        monitor.recordCounter(name + "_setup");
        auto setupStart = std::chrono::high_resolution_clock::now();
        
        // Execute function
        auto result = func();
        
        // Stop setup timer
        auto setupEnd = std::chrono::high_resolution_clock::now();
        monitor.finishCounter(name + "_setup");
        
        // Start execution timer
        monitor.recordCounter(name + "_execution");
        
        auto execStart = std::chrono::high_resolution_clock::now();
        auto execResult = func(); // Execute again
        auto execEnd = std::chrono::high_resolution_clock::now();
        monitor.finishCounter(name + "_execution");
        
        // Calculate statistics
        double setupTime = std::chrono::duration<double>(setupEnd - setupStart).count();
        double execTime = std::chrono::duration<double>(execEnd - execStart).count();
        
        std::cout << name << " Profile:" << std::endl;
        std::cout << "  Setup time: " << setupTime << " seconds" << std::endl;
        std::cout << "  Execution time: " << execTime << " seconds" << std::endl;
        std::cout << "  Average per execution: " << (execTime / 2.0) << " seconds" << std::endl;
        
        return result;
    }
    
private:
    PerformanceMonitor& monitor;
};

} // namespace nlm

// Global performance monitor convenience functions
inline void startPerformanceMonitoring() {
    getGlobalPerformanceMonitor().startMonitoring();
}

inline void stopPerformanceMonitoring() {
    getGlobalPerformanceMonitor().stopMonitoring();
}

inline PerformanceMonitor& getPerformanceMonitor() {
    return getGlobalPerformanceMonitor();
}

inline nlm::ScopedTimer createTimer(const std::string& name) {
    return nlm::ScopedTimer(name);
}

inline nlm::Benchmark createBenchmark(const std::string& name) {
    return nlm::Benchmark(name);
}

inline nlm::Profiler createProfiler() {
    return nlm::Profiler();
}