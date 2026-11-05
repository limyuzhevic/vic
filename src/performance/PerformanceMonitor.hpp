#pragma once

/**
 * Performance monitoring for NLM (Neural Learning Machine)
 * 
 * Provides performance monitoring and profiling tools for neural simulations.
 * Enables real-time monitoring of simulation metrics and performance analysis.
 */

#include <chrono>
#include <thread>
#include <atomic>
#include <vector>
#include <string>
#include <mutex>
#include <iostream>
#include <iomanip>

namespace nlm {

/**
 * Performance metrics collected during neural simulation
 */
struct PerformanceMetrics {
    uint64_t totalSteps;
    float totalTime;
    float avgStepsPerSecond;
    float avgNeuronsActive;
    float avgNeuronsFiring;
    float avgFiringRate;
    float memoryUsage;
    float cpuUsage;
    
    PerformanceMetrics() 
        : totalSteps(0), totalTime(0.0f), avgStepsPerSecond(0.0f)
        , avgNeuronsActive(0.0f), avgNeuronsFiring(0.0f)
        , avgFiringRate(0.0f), memoryUsage(0.0f), cpuUsage(0.0f) {}
};

/**
 * Performance monitor for neural simulations
 * 
 * Provides real-time monitoring and reporting of simulation performance.
 * Can be used to profile simulation efficiency and identify bottlenecks.
 */
class PerformanceMonitor {
public:
    PerformanceMonitor() : running_(false), collectionInterval_(0.1f) {}
    ~PerformanceMonitor() { stop(); }
    
    /**
     * Start monitoring performance
     */
    void start() {
        running_ = true;
        startTime_ = std::chrono::high_resolution_clock::now();
        collectionThread_ = std::thread(&PerformanceMonitor::collectMetrics, this);
    }
    
    /**
     * Stop monitoring performance
     */
    void stop() {
        if (running_) {
            running_ = false;
            if (collectionThread_.joinable()) {
                collectionThread_.join();
            }
        }
    }
    
    /**
     * Get current performance metrics
     */
    PerformanceMetrics getMetrics() const {
        std::lock_guard<std::mutex> lock(mutex_);
        return currentMetrics_;
    }
    
    /**
     * Register a performance data source
     */
    template<typename MetricProvider>
    void registerMetricProvider(const std::string& name, MetricProvider provider) {
        std::lock_guard<std::mutex> lock(mutex_);
        metricProviders_.push_back({name, provider});
    }
    
    /**
     * Update neural activity metrics for performance monitoring
     */
    void updateNeuralActivity(size_t totalNeurons, size_t activeNeurons, 
                            size_t firingNeurons, float firingRate) {
        std::lock_guard<std::mutex> lock(mutex_);
        currentMetrics_.totalNeurons = totalNeurons;
        currentMetrics_.activeNeurons = activeNeurons;
        currentMetrics_.firingNeurons = firingNeurons;
        currentMetrics_.avgFiringRate = firingRate;
    }
    
    /**
     * Report performance metrics
     */
    void reportMetrics() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::cout << "\n=== NLM PERFORMANCE REPORT ===" << std::endl;
        std::cout << std::fixed << std::setprecision(2);
        
        std::cout << "Total Steps: " << currentMetrics_.totalSteps << std::endl;
        std::cout << "Simulation Time: " << currentMetrics_.totalTime << "s" << std::endl;
        std::cout << "Average Steps/Second: " << currentMetrics_.avgStepsPerSecond << std::endl;
        std::cout << "Average Neurons Active: " << currentMetrics_.avgNeuronsActive << std::endl;
        std::cout << "Average Neurons Firing: " << currentMetrics_.avgNeuronsFiring << std::endl;
        std::cout << "Average Firing Rate: " << currentMetrics_.avgFiringRate << " Hz" << std::endl;
        std::cout << "Memory Usage: " << currentMetrics_.memoryUsage << " MB" << std::endl;
        std::cout << "CPU Usage: " << currentMetrics_.cpuUsage << "%" << std::endl;
        
        std::cout << "\nRegistered Metrics:" << std::endl;
        for (const auto& provider : metricProviders_) {
            try {
                float value = provider.provider();
                std::cout << "  " << provider.name << ": " << value << std::endl;
            } catch (const std::exception& e) {
                std::cout << "  " << provider.name << ": ERROR - " << e.what() << std::endl;
            }
        }
        
        std::cout << "=== END PERFORMANCE REPORT ===\n" << std::endl;
    }
    
private:
    struct MetricProvider {
        std::string name;
        std::function<float()> provider;
    };
    
    struct CollectedMetrics {
        uint64_t totalSteps = 0;
        float totalTime = 0.0f;
        float avgStepsPerSecond = 0.0f;
        size_t totalNeurons = 0;
        size_t activeNeurons = 0;
        size_t firingNeurons = 0;
        float avgFiringRate = 0.0f;
        float memoryUsage = 0.0f;
        float cpuUsage = 0.0f;
        size_t collectionCount = 0;
    };
    
    void collectMetrics() {
        while (running_) {
            std::this_thread::sleep_for(std::chrono::duration<float>(collectionInterval_));
            
            auto currentTime = std::chrono::high_resolution_clock::now();
            auto elapsed = std::chrono::duration<float>(currentTime - startTime_).count();
            
            std::lock_guard<std::mutex> lock(mutex_);
            
            // Calculate metrics
            currentMetrics_.totalTime = elapsed;
            currentMetrics_.avgStepsPerSecond = static_cast<float>(currentMetrics_.totalSteps) / std::max(1.0f, elapsed);
            currentMetrics_.avgNeuronsActive = static_cast<float>(currentMetrics_.activeNeurons) / std::max(1.0f, currentMetrics_.totalNeurons);
            currentMetrics_.avgNeuronsFiring = static_cast<float>(currentMetrics_.firingNeurons) / std::max(1.0f, currentMetrics_.totalNeurons);
            
            currentMetrics_.collectionCount++;
        }
    }
    
    mutable std::mutex mutex_;
    bool running_;
    std::thread collectionThread_;
    PerformanceMetrics currentMetrics_;
    std::chrono::high_resolution_clock::time_point startTime_;
    float collectionInterval_;
    std::vector<MetricProvider> metricProviders_;
};

/**
 * Performance profiler for neural simulations
 * 
 * Provides detailed profiling of neural simulation performance,
 * including timing of individual operations and memory usage.
 */
class PerformanceProfiler {
public:
    PerformanceProfiler() = default;
    ~PerformanceProfiler() = default;
    
    /**
     * Start profiling a specific operation
     */
    void startProfile(const std::string& operation) {
        std::lock_guard<std::mutex> lock(mutex_);
        auto now = std::chrono::high_resolution_clock::now();
        profileStack_.push_back({operation, now});
    }
    
    /**
     * Stop profiling the current operation and record timing
     */
    void stopProfile() {
        if (profileStack_.empty()) return;
        
        auto now = std::chrono::high_resolution_clock::now();
        auto last = profileStack_.back();
        
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(now - last.startTime);
        
        std::lock_guard<std::mutex> lock(mutex_);
        
        // Record timing
        timingData_[last.operation].push_back(duration.count());
        
        // Remove from stack
        profileStack_.pop_back();
    }
    
    /**
     * Get profiling statistics for a specific operation
     */
    struct ProfileStats {
        std::string operation;
        uint64_t count;
        uint64_t totalTime;
        uint64_t minTime;
        uint64_t maxTime;
        float avgTime;
        float stdDevTime;
    };
    
    ProfileStats getStats(const std::string& operation) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = timingData_.find(operation);
        if (it == timingData_.end()) {
            return {operation, 0, 0, 0, 0, 0.0f, 0.0f};
        }
        
        const auto& times = it->second;
        if (times.empty()) {
            return {operation, 0, 0, 0, 0, 0.0f, 0.0f};
        }
        
        uint64_t total = 0;
        uint64_t minTime = std::numeric_limits<uint64_t>::max();
        uint64_t maxTime = 0;
        
        for (uint64_t time : times) {
            total += time;
            minTime = std::min(minTime, time);
            maxTime = std::max(maxTime, time);
        }
        
        float avg = static_cast<float>(total) / times.size();
        
        // Calculate standard deviation
        float variance = 0.0f;
        for (uint64_t time : times) {
            float diff = static_cast<float>(time) - avg;
            variance += diff * diff;
        }
        variance /= times.size();
        float stdDev = std::sqrt(variance);
        
        return {
            operation,
            static_cast<uint64_t>(times.size()),
            total,
            minTime,
            maxTime,
            avg,
            stdDev
        };
    }
    
    /**
     * Get all profiling statistics
     */
    std::vector<ProfileStats> getAllStats() {
        std::lock_guard<std::mutex> lock(mutex_);
        
        std::vector<ProfileStats> stats;
        for (const auto& pair : timingData_) {
            stats.push_back(getStats(pair.first));
        }
        
        return stats;
    }
    
    /**
     * Print profiling statistics
     */
    void printStats() {
        std::cout << "\n=== NLM PROFILING STATISTICS ===" << std::endl;
        
        auto stats = getAllStats();
        if (stats.empty()) {
            std::cout << "No profiling data available." << std::endl;
            return;
        }
        
        // Sort by average time (descending)
        std::sort(stats.begin(), stats.end(), 
                 [](const ProfileStats& a, const ProfileStats& b) {
                     return a.avgTime > b.avgTime;
                 });
        
        std::cout << std::fixed << std::setprecision(3);
        for (const auto& stat : stats) {
            std::cout << stat.operation << ": "
                      << stat.count << " calls, "
                      << stat.totalTime << " total µs, "
                      << stat.avgTime << " avg µs, "
                      << stat.stdDevTime << " std dev µs" << std::endl;
        }
        
        std::cout << "=== END PROFILING STATISTICS ===\n" << std::endl;
    }
    
    /**
     * Clear all profiling data
     */
    void clear() {
        std::lock_guard<std::mutex> lock(mutex_);
        timingData_.clear();
        profileStack_.clear();
    }
    
private:
    mutable std::mutex mutex_;
    std::vector<std::pair<std::string, std::chrono::high_resolution_clock::time_point>> profileStack_;
    std::unordered_map<std::string, std::vector<uint64_t>> timingData_;
};

/**
 * Command-line interface for performance monitoring
 * 
 * Provides command-line tools for monitoring and profiling neural simulations.
 */
class PerformanceCLI {
public:
    PerformanceCLI() : monitor_(nullptr), profiler_(nullptr) {}
    ~PerformanceCLI() { cleanup(); }
    
    /**
     * Initialize performance monitoring CLI
     */
    bool initialize(int argc, char* argv[]) {
        // Parse command line arguments
        for (int i = 1; i < argc; ++i) {
            std::string arg = argv[i];
            
            if (arg == "--performance-monitor" || arg == "-perf") {
                enablePerformanceMonitoring_ = true;
            } else if (arg == "--profile" || arg == "-prof") {
                enableProfiling_ = true;
            } else if (arg == "--report-interval" || arg == "-interval") {
                if (i + 1 < argc) {
                    collectionInterval_ = std::stof(argv[++i]);
                }
            } else if (arg == "--help" || arg == "-h") {
                printHelp();
                return false;
            }
        }
        
        // Create performance monitoring objects if enabled
        if (enablePerformanceMonitoring_) {
            monitor_ = new PerformanceMonitor();
            monitor_->setCollectionInterval(collectionInterval_);
            monitor_->start();
        }
        
        if (enableProfiling_) {
            profiler_ = new PerformanceProfiler();
        }
        
        return true;
    }
    
    /**
     * Cleanup performance monitoring resources
     */
    void cleanup() {
        if (profiler_) {
            profiler_->printStats();
            delete profiler_;
            profiler_ = nullptr;
        }
        
        if (monitor_) {
            monitor_->reportMetrics();
            delete monitor_;
            monitor_ = nullptr;
        }
    }
    
    /**
     * Get performance monitor instance
     */
    PerformanceMonitor* getMonitor() { return monitor_; }
    
    /**
     * Get performance profiler instance
     */
    PerformanceProfiler* getProfiler() { return profiler_; }
    
    /**
     * Print help information
     */
    void printHelp() {
        std::cout << "=== NLM PERFORMANCE MONITORING HELP ===" << std::endl;
        std::cout << std::endl;
        std::cout << "Performance monitoring options:" << std::endl;
        std::cout << "  --performance-monitor, -perf    Enable real-time performance monitoring" << std::endl;
        std::cout << "  --profile, -prof               Enable detailed profiling" << std::endl;
        std::cout << "  --report-interval, -interval   Set reporting interval (seconds)" << std::endl;
        std::cout << "  --help, -h                     Show this help message" << std::endl;
        std::cout << std::endl;
        std::cout << "Performance monitoring provides:" << std::endl;
        std::cout << "  - Real-time metrics (steps/second, neuron activity)" << std::endl;
        std::cout << "  - Memory and CPU usage tracking" << std::endl;
        std::cout << "  - Custom metric providers" << std::endl;
        std::cout << "  - Detailed profiling of simulation operations" << std::endl;
        std::cout << "=== END HELP ===" << std::endl;
    }
    
private:
    PerformanceMonitor* monitor_;
    PerformanceProfiler* profiler_;
    bool enablePerformanceMonitoring_ = false;
    bool enableProfiling_ = false;
    float collectionInterval_ = 0.1f;
};

} // namespace nlm
