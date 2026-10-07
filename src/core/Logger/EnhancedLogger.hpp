#pragma once

#include <string>
#include <memory>
#include <sstream>
#include <functional>
#include <vector>
#include <chrono>
#include <unordered_map>
#include <mutex>
#include <thread>
#include <fstream>
#include <iomanip>
#include <algorithm>
#include <queue>
#include <map>
#include <atomic>

namespace nlm {

// Enhanced log severity levels
enum class LogLevel {
    Debug = 0,
    Info = 1,
    Warning = 2,
    Error = 3,
    Critical = 4,
    Performance = 5,      // Performance profiling logs
    Memory = 6,           // Memory debugging logs
    Neural = 7,           // Neural activity debugging logs
    Simulation = 8,       // Simulation state debugging logs
    Visualization = 9,    // Visualization debugging logs
    System = 10          // System-level debugging logs
};

// Performance measurement structure
struct PerformanceMetric {
    std::string name;
    double value;
    std::chrono::system_clock::time_point timestamp;
    std::string unit;
    std::string context;
};

// Memory allocation record
struct MemoryAllocation {
    void* address;
    size_t size;
    std::chrono::system_clock::time_point timestamp;
    std::string file;
    int line;
    std::string function;
    bool allocated;
};

// Neural activity record
struct NeuralActivity {
    uint64_t neuronId;
    float firingRate;
    float membranePotential;
    std::chrono::system_clock::time_point timestamp;
    uint64_t spikeCount;
    float averageWeight;
};

// Simulation state record
struct SimulationState {
    uint64_t step;
    double time;
    size_t neuronCount;
    size_t activeNeuronCount;
    size_t totalSpikeCount;
    float averageFiringRate;
    std::chrono::system_clock::time_point timestamp;
    std::string stateDescription;
};

// Enhanced Log entry structure
struct EnhancedLogEntry {
    LogLevel level;
    std::string message;
    std::string file;
    int line;
    std::string function;
    std::chrono::system_clock::time_point timestamp;
    
    // Debugging context
    std::string context;
    std::unordered_map<std::string, std::string> metadata;
    
    // Performance metrics
    std::vector<PerformanceMetric> performanceMetrics;
    
    // Memory information
    std::vector<MemoryAllocation> memoryAllocations;
    
    // Neural activity
    std::vector<NeuralActivity> neuralActivities;
    
    // Simulation state
    std::vector<SimulationState> simulationStates;
};

// Advanced ILogger interface
class IDebugLogger {
public:
    virtual ~IDebugLogger() = default;
    virtual void log(const EnhancedLogEntry& entry) = 0;
    virtual void setLevel(LogLevel level) = 0;
    virtual LogLevel getLevel() const = 0;
    
    // Performance monitoring
    virtual void recordPerformance(const PerformanceMetric& metric) = 0;
    
    // Memory debugging
    virtual void recordAllocation(const MemoryAllocation& alloc) = 0;
    virtual void recordDeallocation(void* address) = 0;
    
    // Neural activity tracking
    virtual void recordNeuralActivity(const NeuralActivity& activity) = 0;
    
    // Simulation state tracking
    virtual void recordSimulationState(const SimulationState& state) = 0;
    
    // Real-time debugging
    virtual void startRealTimeMonitoring() = 0;
    virtual void stopRealTimeMonitoring() = 0;
    virtual bool isRealTimeMonitoring() const = 0;
    
    // Performance profiling
    virtual void startProfiling(const std::string& operation) = 0;
    virtual void stopProfiling() = 0;
    virtual std::chrono::duration<double> getLastProfileTime() const = 0;
};

// Console debug logger implementation
class ConsoleDebugLogger : public IDebugLogger {
public:
    ConsoleDebugLogger();
    explicit ConsoleDebugLogger(LogLevel level);
    ~ConsoleDebugLogger() override;
    
    void log(const EnhancedLogEntry& entry) override;
    void setLevel(LogLevel level) override;
    LogLevel getLevel() const override;
    
    // Performance monitoring
    void recordPerformance(const PerformanceMetric& metric) override;
    void recordMemoryUsage(size_t memoryBytes, const std::string& context = "");
    
    // Memory debugging
    void recordAllocation(const MemoryAllocation& alloc) override;
    void recordDeallocation(void* address) override;
    
    // Neural activity tracking
    void recordNeuralActivity(const NeuralActivity& activity) override;
    
    // Simulation state tracking
    void recordSimulationState(const SimulationState& state) override;
    
    // Real-time debugging
    void startRealTimeMonitoring() override;
    void stopRealTimeMonitoring() override;
    bool isRealTimeMonitoring() const override;
    
    // Performance profiling
    void startProfiling(const std::string& operation) override;
    void stopProfiling() override;
    std::chrono::duration<double> getLastProfileTime() const override;
    
    // Enable/disable colors and formatting
    void setUseColors(bool useColors);
    bool getUseColors() const;
    void setEnablePerformanceMetrics(bool enable);
    bool getEnablePerformanceMetrics() const;
    void setEnableMemoryTracking(bool enable);
    bool getEnableMemoryTracking() const;
    void setEnableNeuralTracking(bool enable);
    bool getEnableNeuralTracking() const;
    
    // Output file management
    void setOutputFile(const std::string& filepath);
    void closeOutputFile();
    
    // Statistics and reports
    struct DebugStats {
        size_t totalLogEntries;
        size_t performanceMetricsRecorded;
        size_t memoryAllocations;
        size_t neuralActivitiesRecorded;
        size_t simulationStatesRecorded;
        double cpuUsage;
        size_t memoryUsage;
        double averageFrameTime;
        double fps;
    };
    
    DebugStats getStats() const;
    
    // Generate debugging reports
    std::string generatePerformanceReport() const;
    std::string generateMemoryReport() const;
    std::string generateNeuralActivityReport() const;
    std::string generateSimulationReport() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    std::string levelToString(LogLevel level) const;
    std::string levelToColor(LogLevel level) const;
    void formatEntry(std::ostringstream& oss, const EnhancedLogEntry& entry) const;
    void writeToFile(const std::string& line);
};

// Performance profiler
class PerformanceProfiler {
public:
    PerformanceProfiler();
    ~PerformanceProfiler();
    
    void startProfiling(const std::string& operation);
    void stopProfiling();
    double getCurrentProfileTime() const;
    void recordMetric(const std::string& name, double value, 
                     const std::string& unit = "", 
                     const std::string& context = "");
    std::vector<PerformanceMetric> getRecentMetrics(int count = 100) const;
    void clearMetrics();
    
    // Thread-safe profiling
    void startThreadProfiling(const std::string& threadName);
    void stopThreadProfiling();
    std::string getThreadProfileSummary() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Memory debugger
class MemoryDebugger {
public:
    MemoryDebugger();
    ~MemoryDebugger();
    
    void* trackAllocation(size_t size, const std::string& file, int line, 
                         const std::string& function);
    void trackDeallocation(void* address);
    void recordMemoryUsage(size_t bytes, const std::string& context = "");
    
    struct MemoryInfo {
        size_t totalAllocated;
        size_t currentUsage;
        size_t peakUsage;
        size_t allocationCount;
        std::vector<MemoryAllocation> recentAllocations;
    };
    
    MemoryInfo getMemoryInfo() const;
    std::string getMemoryLeakReport() const;
    void clearTracking();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Neural activity debugger
class NeuralActivityDebugger {
public:
    NeuralActivityDebugger();
    ~NeuralActivityDebugger();
    
    void recordActivity(const NeuralActivity& activity);
    void recordSpike(uint64_t neuronId, double timestamp, float weight);
    void recordFiringRate(uint64_t neuronId, float rate, double timestamp);
    void recordConnectionChange(uint64_t source, uint64_t target, float oldWeight, float newWeight);
    
    struct NeuralStats {
        size_t totalNeurons;
        size_t activeNeurons;
        double averageFiringRate;
        double averageMembranePotential;
        size_t totalSpikes;
        float maxFiringRate;
        float minFiringRate;
        std::map<uint64_t, float> neuronFiringRates;
    };
    
    NeuralStats getNeuralStats() const;
    std::string getNeuralActivityReport() const;
    std::vector<NeuralActivity> getRecentActivities(int count = 100) const;
    void clearTracking();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Simulation state debugger
class SimulationStateDebugger {
public:
    SimulationStateDebugger();
    ~SimulationStateDebugger();
    
    void recordState(const SimulationState& state);
    void recordStepTransition(uint64_t fromStep, uint64_t toStep, 
                              double timeAdvance, const std::string& reason);
    void recordError(const std::string& error, const std::string& context);
    
    struct SimulationStats {
        uint64_t currentStep;
        double currentTime;
        size_t totalSteps;
        size_t errorCount;
        double averageStepTime;
        double simulationSpeed;
        std::vector<SimulationState> recentStates;
        std::vector<std::string> errors;
    };
    
    SimulationStats getSimulationStats() const;
    std::string getSimulationReport() const;
    std::vector<SimulationState> getRecentStates(int count = 100) const;
    void clearTracking();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Enhanced Logger class with debugging capabilities
class EnhancedLogger {
public:
    EnhancedLogger();
    ~EnhancedLogger();
    
    // Disable copying
    EnhancedLogger(const EnhancedLogger&) = delete;
    EnhancedLogger& operator=(const EnhancedLogger&) = delete;
    
    // Add debug logger sink
    void addDebugLogger(std::shared_ptr<IDebugLogger> logger);
    
    // Remove all debug loggers
    void clearDebugLoggers();
    
    // Set minimum level for all debug loggers
    void setLevel(LogLevel level);
    
    // Global logging functions
    void log(LogLevel level, const std::string& message, 
             const std::string& file = "", int line = 0, 
             const std::string& function = "",
             const std::string& context = "");
    
    // Convenience methods
    void debug(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& context = "");
    void info(const std::string& message, const std::string& file = "", int line = 0, 
              const std::string& function = "", const std::string& context = "");
    void warning(const std::string& message, const std::string& file = "", int line = 0, 
                 const std::string& function = "", const std::string& context = "");
    void error(const std::string& message, const std::string& file = "", int line = 0, 
               const std::string& function = "", const std::string& context = "");
    void critical(const std::string& message, const std::string& file = "", int line = 0, 
                  const std::string& function = "", const std::string& context = "");
    
    // Enhanced logging with context
    void performance(const std::string& message, const PerformanceMetric& metric,
                    const std::string& file = "", int line = 0, 
                    const std::string& function = "");
    void memory(const std::string& message, const MemoryAllocation& alloc,
               const std::string& file = "", int line = 0, 
               const std::string& function = "");
    void neural(const std::string& message, const NeuralActivity& activity,
               const std::string& file = "", int line = 0, 
               const std::string& function = "");
    void simulation(const std::string& message, const SimulationState& state,
                   const std::string& file = "", int line = 0, 
                   const std::string& function = "");
    
    // Convenience macros
    void logPerformance(const std::string& name, double value, 
                       const std::string& unit = "ms",
                       const std::string& context = "",
                       const std::string& file = "", int line = 0, 
                       const std::string& function = "");
    
    void logMemory(void* address, size_t size, 
                   const std::string& file = "", int line = 0, 
                   const std::string& function = "");
    
    void logNeuralActivity(uint64_t neuronId, float firingRate, float membranePotential,
                          double timestamp = 0.0,
                          const std::string& file = "", int line = 0, 
                          const std::string& function = "");
    
    void logSimulationState(uint64_t step, double time, size_t neuronCount,
                           size_t activeNeuronCount, float averageFiringRate,
                           const std::string& file = "", int line = 0, 
                           const std::string& function = "");
    
    // Check if level is enabled
    bool isEnabled(LogLevel level) const;
    
    // Get global logger instance
    static EnhancedLogger& getGlobal();
    
    // Set global logger
    static void setGlobal(std::shared_ptr<EnhancedLogger> logger);
    
    // Performance profiler access
    static PerformanceProfiler& getProfiler();
    
    // Memory debugger access
    static MemoryDebugger& getMemoryDebugger();
    
    // Neural activity debugger access
    static NeuralActivityDebugger& getNeuralDebugger();
    
    // Simulation state debugger access
    static SimulationStateDebugger& getSimulationDebugger();
    
    // Global debugging control
    static void enableRealTimeDebugging(bool enable);
    static bool isRealTimeDebuggingEnabled();
    
    // Generate comprehensive debugging report
    static std::string generateDebugReport();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

// Convenience macros for enhanced logging
#define NLM_LOG_DEBUG(msg) nlm::EnhancedLogger::getGlobal().debug(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_INFO(msg) nlm::EnhancedLogger::getGlobal().info(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_WARNING(msg) nlm::EnhancedLogger::getGlobal().warning(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_ERROR(msg) nlm::EnhancedLogger::getGlobal().error(msg, __FILE__, __LINE__, __func__)
#define NLM_LOG_CRITICAL(msg) nlm::EnhancedLogger::getGlobal().critical(msg, __FILE__, __LINE__, __func__)

// Performance profiling macros
#define NLM_PERF_START(op) nlm::PerformanceProfiler::getProfiler().startProfiling(op)
#define NLM_PERF_STOP() nlm::PerformanceProfiler::getProfiler().stopProfiling()
#define NLM_PERF_RECORD(name, value, unit) nlm::EnhancedLogger::getGlobal().logPerformance(name, value, unit, __FILE__, __LINE__, __func__)

// Memory debugging macros
#define NLM_MEM_ALLOC(size) nlm::MemoryDebugger::getMemoryDebugger().trackAllocation(size, __FILE__, __LINE__, __func__)
#define NLM_MEM_FREE(ptr) nlm::MemoryDebugger::getMemoryDebugger().trackDeallocation(ptr)

// Neural activity debugging macros
#define NLM_NEURAL_ACTIVITY(neuronId, firingRate, potential) \
    nlm::EnhancedLogger::getGlobal().logNeuralActivity(neuronId, firingRate, potential, 0.0, __FILE__, __LINE__, __func__)

// Simulation state debugging macros
#define NLM_SIMULATION_STATE(step, time, neuronCount, activeCount, avgFiringRate) \
    nlm::EnhancedLogger::getGlobal().logSimulationState(step, time, neuronCount, activeCount, avgFiringRate, __FILE__, __LINE__, __func__)

} // namespace nlm
