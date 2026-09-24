#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <iomanip>
#include <sstream>
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Performance measurement types
enum class MeasurementType {
    ElapsedTime,
    MemoryUsage,
    OperationCount,
    Throughput,
    Custom
};

// Performance measurement result
struct MeasurementResult {
    std::string name;
    MeasurementType type;
    double value;
    std::string unit;
    std::chrono::system_clock::time_point timestamp;
    std::string context;
};

// Performance counter for measuring cumulative metrics
class PerformanceCounter {
public:
    PerformanceCounter(const std::string& name, MeasurementType type, const std::string& unit = "")
        : name_(name), type_(type), unit_(unit), count_(0), startTime_(std::chrono::system_clock::now()) {}
    
    void increment() {
        std::lock_guard<std::mutex> lock(mutex_);
        ++count_;
    }
    
    void increment(double value) {
        std::lock_guard<std::mutex> lock(mutex_);
        count_ += value;
    }
    
    template<typename Rep, typename Period>
    void addTime(const std::chrono::duration<Rep, Period>& duration) {
        std::lock_guard<std::mutex> lock(mutex_);
        elapsedTime_ += std::chrono::duration_cast<std::chrono::microseconds>(duration);
    }
    
    MeasurementResult getResult() const {
        MeasurementResult result;
        result.name = name_;
        result.type = type_;
        result.unit = unit_;
        result.timestamp = std::chrono::system_clock::now();
        
        switch (type_) {
            case MeasurementType::ElapsedTime:
                result.value = static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime_).count()) / 1000.0; // Convert to ms
                break;
            case MeasurementType::OperationCount:
                result.value = static_cast<double>(count_);
                break;
            case MeasurementType::Throughput:
                result.value = count_ / (static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(elapsedTime_).count()) + 0.001);
                break;
            case MeasurementType::MemoryUsage:
                result.value = count_;
                break;
            case MeasurementType::Custom:
                result.value = count_;
                break;
        }
        
        return result;
    }
    
    void reset() {
        std::lock_guard<std::mutex> lock(mutex_);
        count_ = 0;
        elapsedTime_ = std::chrono::microseconds(0);
    }
    
    std::string getFormattedValue() const {
        std::ostringstream oss;
        oss << std::fixed << std::setprecision(2);
        
        switch (type_) {
            case MeasurementType::ElapsedTime:
                oss << static_cast<double>(std::chrono::duration_cast<std::chrono::microseconds>(elapsedTime_).count()) / 1000.0 << " ms";
                break;
            case MeasurementType::OperationCount:
                oss << count_;
                break;
            case MeasurementType::Throughput:
                oss << count_ / (static_cast<double>(std::chrono::duration_cast<std::chrono::seconds>(elapsedTime_).count()) + 0.001) << " ops/sec";
                break;
            case MeasurementType::MemoryUsage:
                oss << count_ << " MB";
                break;
            case MeasurementType::Custom:
                oss << count_ << unit_;
                break;
        }
        
        return oss.str();
    }
    
private:
    std::string name_;
    MeasurementType type_;
    std::string unit_;
    mutable std::mutex mutex_;
    double count_ = 0.0;
    std::chrono::microseconds elapsedTime_ = std::chrono::microseconds(0);
};

// RAII timer for measuring elapsed time
class ScopedTimer {
public:
    ScopedTimer(PerformanceCounter& counter)
        : counter_(counter), startTime_(std::chrono::system_clock::now()) {}
    
    ~ScopedTimer() {
        auto endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        counter_.addTime(duration);
    }
    
    void restart() {
        startTime_ = std::chrono::system_clock::now();
    }
    
    auto getElapsedTime() const {
        auto endTime = std::chrono::system_clock::now();
        return std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
    }
    
private:
    PerformanceCounter& counter_;
    std::chrono::system_clock::time_point startTime_;
};

// Performance monitor for tracking system-wide metrics
class PerformanceMonitor {
public:
    static PerformanceMonitor& getInstance() {
        static PerformanceMonitor instance;
        return instance;
    }
    
    // Register a performance counter
    void registerCounter(const std::string& name, MeasurementType type, 
                        const std::string& unit = "", bool trackTime = false) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        if (trackTime) {
            counters_[name] = std::make_unique<PerformanceCounter>(name, type, unit);
        } else {
            counters_[name] = std::make_unique<PerformanceCounter>(name, type, unit);
        }
        
        NLM_LOG_INFO("PerformanceMonitor: registered counter '{}' (type: {}, unit: {})",
                     name, static_cast<int>(type), unit.empty() ? "none" : unit);
    }
    
    // Get performance counter by name
    PerformanceCounter* getCounter(const std::string& name) {
        std::lock_guard<std::mutex> lock(mutex_);
        
        auto it = counters_.find(name);
        if (it != counters_.end()) {
            return it->second.get();
        }
        return nullptr;
    }
    
    // Create a scoped timer for a specific operation
    ScopedTimer createTimer(const std::string& counterName) {
        PerformanceCounter* counter = getCounter(counterName);
        if (counter) {
            return ScopedTimer(*counter);
        }
        // Return dummy timer if counter not found
        return ScopedTimer(*getDummyCounter());
    }
    
    // Get all measurement results
    std::vector<MeasurementResult> getAllResults() const {
        std::vector<MeasurementResult> results;
        std::lock_guard<std::mutex> lock(mutex_);
        
        for (const auto& pair : counters_) {
            results.push_back(pair.second->getResult());
        }
        
        return results;
    }
    
    // Log all performance results
    void logAllResults() const {
        std::vector<MeasurementResult> results = getAllResults();

        if (results.empty()) {
            NLM_LOG_INFO("PerformanceMonitor: no performance counters registered");
            return;
        }

        NLM_LOG_INFO("=== Performance Monitor Results ===");
        for (const auto& result : results) {
            NLM_LOG_INFO("  {}: {:.6f} {} ({} at {})",
                         result.name, result.value, result.unit, result.context,
                         std::chrono::system_clock::to_time_t(result.timestamp));
        }
        NLM_LOG_INFO("=== End Performance Monitor Results ===");
    }
    
    // Reset all counters
    void resetAll() {
        std::lock_guard<std::mutex> lock(mutex_);
        for (const auto& pair : counters_) {
            pair.second->reset();
        }
        NLM_LOG_INFO("PerformanceMonitor: all counters reset");
    }
    
    // Start performance monitoring for simulation
    void startSimulationMonitoring() {
        NLM_LOG_INFO("PerformanceMonitor: starting simulation monitoring");
        
        // Register common counters
        registerCounter("STDP_update_time", MeasurementType::ElapsedTime, "ms", true);
        registerCounter("Hebbian_update_time", MeasurementType::ElapsedTime, "ms", true);
        registerCounter("spike_processing_time", MeasurementType::ElapsedTime, "ms", true);
        registerCounter("weight_updates", MeasurementType::OperationCount, "updates");
        registerCounter("spike_count", MeasurementType::OperationCount, "spikes");
        registerCounter("simulation_step_time", MeasurementType::ElapsedTime, "ms", true);
        registerCounter("memory_usage", MeasurementType::MemoryUsage, "MB");
    }
    
    // Stop performance monitoring for simulation
    void stopSimulationMonitoring() {
        logAllResults();
        NLM_LOG_INFO("PerformanceMonitor: stopped simulation monitoring");
    }
    
    // Get system resource usage (placeholder implementation)
    double getSystemMemoryUsage() const {
        // Placeholder implementation - in practice, this would read from system-specific sources
        return 0.0;
    }
    
private:
    PerformanceMonitor() = default;
    ~PerformanceMonitor() = default;
    PerformanceMonitor(const PerformanceMonitor&) = delete;
    PerformanceMonitor& operator=(const PerformanceMonitor&) = delete;
    
    PerformanceCounter* getDummyCounter() {
        static PerformanceCounter dummy("dummy", MeasurementType::OperationCount);
        return &dummy;
    }
    
    mutable std::mutex mutex_;
    std::unordered_map<std::string, std::unique_ptr<PerformanceCounter>> counters_;
};

// Convenience macros for performance monitoring
#define NLM_PERF_SCOPE(counterName) nlm::ScopedTimer _perfTimer_##__LINE__(nlm::PerformanceMonitor::getInstance().getCounter(counterName))
#define NLM_PERF_INCREMENT(counterName, amount) do { if (auto counter = nlm::PerformanceMonitor::getInstance().getCounter(counterName)) counter->increment(amount); } while(0)
#define NLM_PERF_START_TIMER(counterName) nlm::ScopedTimer _perfTimer_##__LINE__(nlm::PerformanceMonitor::getInstance().getCounter(counterName))

} // namespace nlm
