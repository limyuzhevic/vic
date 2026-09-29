// Performance Monitoring and Profiling System
// Provides performance metrics, timing measurements, and resource usage tracking

#pragma once

#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <mutex>
#include <sstream>
#include <iomanip>

namespace nlm {

namespace Performance {

// Performance measurement types
enum class MeasurementType {
    Time,
    Memory,
    CPU,
    Network,
    Custom
};

// Measurement result structure
struct MeasurementResult {
    std::string name;
    double value;
    std::string unit;
    std::chrono::system_clock::time_point timestamp;
    MeasurementType type;
    std::unordered_map<std::string, std::string> metadata;
    
    MeasurementResult(const std::string& name, double value, const std::string& unit, 
                     MeasurementType type = MeasurementType::Time)
        : name(name), value(value), unit(unit), timestamp(std::chrono::system_clock::now()),
          type(type) {}
};

// Timer for measuring operation duration
class Timer {
private:
    std::chrono::system_clock::time_point startTime;
    std::string name;
    bool enabled;
    
public:
    Timer(const std::string& name, bool enabled = true) 
        : name(name), enabled(enabled) {
        if (enabled) {
            startTime = std::chrono::system_clock::now();
        }
    }
    
    ~Timer() {
        if (enabled) {
            auto endTime = std::chrono::system_clock::now();
            auto duration = std::chrono::duration<double, std::milli>(endTime - startTime);
            std::cout << "[TIMER] " << name << ": " << duration.count() << " ms" << std::endl;
        }
    }
    
    double elapsedMilliseconds() const {
        if (!enabled) return 0.0;
        auto endTime = std::chrono::system_clock::now();
        auto duration = std::chrono::duration<double, std::milli>(endTime - startTime);
        return duration.count();
    }
    
    double elapsedSeconds() const {
        return elapsedMilliseconds() / 1000.0;
    }
};

// Performance profiler for tracking system-wide metrics
class PerformanceProfiler {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    // Start/stop profiling
    void start();
    void stop();
    bool isRunning() const;
    
    // Add a measurement
    void addMeasurement(const MeasurementResult& measurement);
    
    // Get measurements by name
    std::vector<MeasurementResult> getMeasurements(const std::string& name) const;
    
    // Get all measurements
    std::vector<MeasurementResult> getAllMeasurements() const;
    
    // Clear measurements
    void clear();
    
    // Generate performance report
    std::string generateReport() const;
    
    // Get total time measured
    double getTotalTime() const;
    
    // Get average time for a specific measurement
    double getAverageTime(const std::string& name) const;
    
    // Get measurement count
    size_t getMeasurementCount() const;
};

// Resource usage monitor
class ResourceMonitor {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    ResourceMonitor();
    ~ResourceMonitor();
    
    // Get current memory usage
    size_t getMemoryUsage() const;
    
    // Get CPU usage percentage
    double getCPUUsage() const;
    
    // Get system uptime
    std::chrono::seconds getSystemUptime() const;
    
    // Get thread count
    int getThreadCount() const;
    
    // Monitor specific process
    void monitorProcess(pid_t pid);
    
    // Get process resource usage
    struct ProcessResourceUsage {
        pid_t pid;
        size_t memoryUsage;
        double cpuUsage;
        int threadCount;
        std::chrono::seconds startTime;
    };
    
    ProcessResourceUsage getProcessUsage(pid_t pid) const;
    
    // Get all monitored processes
    std::vector<ProcessResourceUsage> getAllProcessUsage() const;
};

// Neural simulation performance analyzer
class NeuralSimulationProfiler {
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
public:
    NeuralSimulationProfiler();
    ~NeuralSimulationProfiler();
    
    // Profile neuron step
    void profileNeuronStep(int neuronCount, double executionTime);
    
    // Profile spike propagation
    void profileSpikePropagation(int spikeCount, double propagationTime);
    
    // Profile plasticity updates
    void profilePlasticityUpdates(int updateCount, double updateTime);
    
    // Profile memory operations
    void profileMemoryOperation(const std::string& operation, double executionTime, size_t dataSize);
    
    // Profile prediction system
    void profilePrediction(int predictionCount, double computationTime);
    
    // Generate neural simulation report
    std::string generateNeuralReport() const;
    
    // Get statistics
    struct Statistics {
        int totalNeuronSteps = 0;
        double totalNeuronStepTime = 0.0;
        int totalSpikeCount = 0;
        double totalSpikePropagationTime = 0.0;
        int totalPlasticityUpdates = 0;
        double totalPlasticityUpdateTime = 0.0;
        int totalMemoryOperations = 0;
        double totalMemoryOperationTime = 0.0;
        int totalPredictions = 0;
        double totalPredictionTime = 0.0;
    };
    
    Statistics getStatistics() const;
    
    // Clear statistics
    void clearStatistics();
};

// Global performance monitoring instances
inline PerformanceProfiler& getGlobalProfiler() {
    static PerformanceProfiler profiler;
    return profiler;
}

inline ResourceMonitor& getResourceMonitor() {
    static ResourceMonitor monitor;
    return monitor;
}

inline NeuralSimulationProfiler& getNeuralProfiler() {
    static NeuralSimulationProfiler neuralProfiler;
    return neuralProfiler;
}

// Convenience functions for easy profiling

// Time a function call
template<typename Func, typename... Args>
auto timeFunction(const std::string& name, Func&& func, Args&&... args) -> decltype(func(std::forward<Args>(args)...)) {
    Performance::Timer timer(name, true);
    auto result = func(std::forward<Args>(args)...);
    return result;
}

// Profile neural step
inline void profileNeuralStep(int neuronCount, int spikeCount, double executionTime) {
    getNeuralProfiler().profileNeuronStep(neuronCount, executionTime);
    getNeuralProfiler().profileSpikePropagation(spikeCount, executionTime * 0.3);
}

// Profile plasticity
inline void profilePlasticity(int updateCount, double updateTime) {
    getNeuralProfiler().profilePlasticityUpdates(updateCount, updateTime);
}

// Profile memory
inline void profileMemory(const std::string& operation, double executionTime, size_t dataSize) {
    getNeuralProfiler().profileMemoryOperation(operation, executionTime, dataSize);
}

// Generate comprehensive performance report
inline std::string generatePerformanceReport() {
    std::ostringstream oss;
    oss << "=== NLM Performance Report ===" << std::endl;
    oss << "Generated at: " << std::chrono::system_clock::to_time_t(std::chrono::system_clock::now()) << std::endl;
    oss << std::endl;
    
    // Global profiler report
    oss << getGlobalProfiler().generateReport() << std::endl;
    
    // Neural simulation report
    oss << getNeuralProfiler().generateNeuralReport() << std::endl;
    
    // Resource usage
    oss << "=== Resource Usage ===" << std::endl;
    oss << "Memory Usage: " << getResourceMonitor().getMemoryUsage() / 1024 / 1024 << " MB" << std::endl;
    oss << "CPU Usage: " << getResourceMonitor().getCPUUsage() << "%" << std::endl;
    oss << "System Uptime: " << getResourceMonitor().getSystemUptime().count() << " seconds" << std::endl;
    oss << "Thread Count: " << getResourceMonitor().getThreadCount() << std::endl;
    
    return oss.str();
}

// Enable/disable performance monitoring
inline void enablePerformanceMonitoring(bool enabled) {
    getGlobalProfiler().start();
}

inline bool isPerformanceMonitoringEnabled() {
    return getGlobalProfiler().isRunning();
}

// Clear all performance data
inline void clearPerformanceData() {
    getGlobalProfiler().clear();
    getNeuralProfiler().clearStatistics();
}

} // namespace Performance

} // namespace nlm