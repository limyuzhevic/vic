# performance_monitor.hpp - Performance monitoring and debugging utilities

#pragma once

#include "../core/Types/Types.hpp"
#include <chrono>
#include <string>
#include <unordered_map>
#include <vector>
#include <memory>
#include <sstream>

namespace nlm {

/**
 * @class PerformanceMetrics
 * @brief Comprehensive performance monitoring for NLM simulations
 * 
 * This class tracks various performance metrics including execution time,
 * memory usage, spike statistics, and computational complexity. It's designed
 * to help optimize neural simulations and identify performance bottlenecks.
 * 
 * The metrics collection is designed to be lightweight and minimally invasive,
 * allowing long-running simulations to monitor performance without significant
 * overhead.
 */
class PerformanceMetrics {
public:
    /**
     * @brief Performance category for grouping related metrics
     */
    enum class Category {
        Simulation,      ///< Core simulation timing and statistics
        Memory,          ///< Memory allocation and usage
        Plasticity,       ///< Plasticity computation performance
        Connectivity,     ///< Network operations performance
        Learning,        ///< Learning and adaptation performance
        IOD,             ///< Input/output operations performance
        All              ///< Aggregate of all categories
    };

    /**
     * @brief Detailed timing information
     */
    struct TimingInfo {
        double totalTime;        ///< Total time in seconds
        double averageTime;      ///< Average time per operation
        double minTime;          ///< Minimum time
        double maxTime;          ///< Maximum time
        uint64_t operationCount; ///< Number of operations measured
        double timePercentage;   ///< Percentage of total time in this category
    };

    /**
     * @brief Memory usage statistics
     */
    struct MemoryInfo {
        size_t totalAllocated;    ///< Total memory allocated in bytes
        size_t currentUsage;      ///< Current memory usage in bytes
        size_t peakUsage;         ///< Peak memory usage in bytes
        size_t allocationCount;   ///< Number of allocation operations
        size_t deallocationCount; ///< Number of deallocation operations
    };

    /**
     * @brief Computational complexity metrics
     */
    struct ComplexityInfo {
        uint64_t totalOperations; ///< Total number of computational operations
        uint64_t spikesGenerated; ///< Total number of spikes generated
        uint64_t synapsesModified; ///< Total number of synapse modifications
        float avgOperationsPerStep; ///< Average operations per simulation step
        float efficiencyScore;      ///< Performance efficiency metric (0-1)
    };

    /**
     * @brief Spike statistics for performance analysis
     */
    struct SpikeInfo {
        uint64_t totalSpikes;          ///< Total spikes generated
        uint64_t spikesPerStepAverage;  ///< Average spikes per simulation step
        uint64_t maxSpikesInStep;       ///< Maximum spikes in any single step
        uint64_t stepsWithSpikes;       ///< Number of steps with spiking activity
        float activityRatio;            ///< Ratio of steps with activity (0-1)
    };

    /**
     * @brief Network connectivity statistics
     */
    struct ConnectivityInfo {
        uint64_t totalConnections;     ///< Total number of synaptic connections
        float connectionDensity;        ///< Connection density (0-1)
        float excitationInhibitionRatio; ///< E/I balance ratio
        uint64_t activeConnections;     ///< Number of currently active connections
    };

    /**
     * @brief Learning dynamics metrics
     */
    struct LearningInfo {
        float plasticityRate;          ///< Average plasticity rate (0-1)
        float learningProgress;        ///< Overall learning progress (0-1)
        uint64_t weightChanges;        ///< Total number of weight modifications
        float averageWeightChange;     ///< Average magnitude of weight changes
        float stabilityMetric;         ///< Network stability (0-1)
    };

    /**
     * @brief I/O operation metrics
     */
    struct IOMetrics {
        uint64_t sensoryInputsProcessed;  ///< Number of sensory inputs processed
        uint64_t motorOutputsGenerated;   ///< Number of motor outputs generated
        double avgInputProcessingTime;    ///< Average input processing time
        double avgOutputProcessingTime;   ///< Average output processing time
    };

    PerformanceMetrics();
    ~PerformanceMetrics();

    // === Core Operations ===

    /**
     * @brief Start timing for a specific operation category
     * 
     * @param category Performance category to start timing
     */
    void startTiming(Category category);

    /**
     * @brief Stop timing for a specific operation category
     * 
     * @param category Performance category to stop timing
     * @return double Time elapsed in seconds for this operation
     */
    double stopTiming(Category category);

    /**
     * @brief Record memory usage statistics
     * 
     * @param category Performance category
     * @param bytes Memory usage in bytes
     */
    void recordMemoryUsage(Category category, size_t bytes);

    /**
     * @brief Record computational operations
     * 
     * @param category Performance category
     * @param operations Number of operations performed
     */
    void recordOperations(Category category, uint64_t operations);

    /**
     * @brief Record spike statistics
     * 
     * @param info Spike statistics to record
     */
    void recordSpikeStatistics(const SpikeInfo& info);

    /**
     * @brief Record connectivity statistics
     * 
     * @param info Connectivity statistics to record
     */
    void recordConnectivityStatistics(const ConnectivityInfo& info);

    /**
     * @brief Record learning dynamics
     * 
     * @param info Learning statistics to record
     */
    void recordLearningStatistics(const LearningInfo& info);

    /**
     * @brief Record I/O operations
     * 
     * @param info I/O statistics to record
     */
    void recordIOMetrics(const IOMetrics& info);

    // === Data Access ===

    /**
     * @brief Get timing information for a specific category
     * 
     * @param category Performance category
     * @return TimingInfo Timing information for the category
     */
    TimingInfo getTimingInfo(Category category) const;

    /**
     * @brief Get memory information for a specific category
     * 
     * @param category Performance category
     * @return MemoryInfo Memory information for the category
     */
    MemoryInfo getMemoryInfo(Category category) const;

    /**
     * @brief Get spike statistics
     * 
     * @return SpikeInfo Spike statistics
     */
    SpikeInfo getSpikeInfo() const;

    /**
     * @brief Get connectivity statistics
     * 
     * @return ConnectivityInfo Connectivity statistics
     */
    ConnectivityInfo getConnectivityInfo() const;

    /**
     * @brief Get learning statistics
     * 
     * @return LearningInfo Learning statistics
     */
    LearningInfo getLearningInfo() const;

    /**
     * @brief Get I/O metrics
     * 
     * @return IOMetrics I/O metrics
     */
    IOMetrics getIOMetrics() const;

    // === Analysis and Reporting ===

    /**
     * @brief Generate a comprehensive performance report
     * 
     * @return std::string Formatted performance report
     */
    std::string generateReport() const;

    /**
     * @brief Generate a JSON-formatted performance report
     * 
     * @return std::string JSON-formatted performance report
     */
    std::string generateJSONReport() const;

    /**
     * @brief Reset all performance metrics to initial state
     */
    void reset();

    /**
     * @brief Check if performance monitoring is enabled
     * 
     * @return bool true if monitoring is enabled
     */
    bool isEnabled() const;

    /**
     * @brief Enable or disable performance monitoring
     * 
     * @param enabled true to enable monitoring, false to disable
     */
    void setEnabled(bool enabled);

    // === Debugging Support ===

    /**
     * @brief Get the current simulation step
     * 
     * @return uint64_t Current simulation step number
     */
    SimulationStep getCurrentStep() const;

    /**
     * @brief Set the current simulation step
     * 
     * @param step Current simulation step number
     */
    void setCurrentStep(SimulationStep step);

    /**
     * @brief Get performance metrics for a specific time window
     * 
     * @param steps Window size in simulation steps
     * @return PerformanceMetrics Metrics for the specified window
     */
    PerformanceMetrics getWindowMetrics(SimulationStep steps) const;

    /**
     * @brief Compare performance metrics with baseline
     * 
     * @param baseline Reference baseline metrics
     * @return std::string Comparison report
     */
    std::string compareWithBaseline(const PerformanceMetrics& baseline) const;

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @class DebugLogger
 * @brief Comprehensive debugging utilities for NLM simulations
 * 
 * This class provides detailed logging and debugging capabilities for NLM
 * simulations, including step-by-step tracing, state inspection, and
 * error detection. It's designed to help developers understand simulation
 * behavior and diagnose issues.
 * 
 * The debug logger can be configured with different log levels and filtering
 * to focus on specific aspects of the simulation.
 */
class DebugLogger {
public:
    /**
     * @brief Debug log levels
     */
    enum class LogLevel {
        Silent = 0,           ///< No logging
        Error = 1,            ///< Only errors
        Warning = 2,          ///< Errors and warnings
        Info = 3,             ///< General information
        Debug = 4,            ///< Detailed debugging information
        Trace = 5,            ///< Extremely detailed step-by-step tracing
        Performance = 6      ///< Performance metrics logging
    };

    /**
     * @brief Debug event types
     */
    enum class EventType {
        SimulationStart,      ///< Simulation starting
        SimulationEnd,        ///< Simulation ending
        StepComplete,         ///< Simulation step completed
        SpikeGenerated,        ///< Spike generated by neuron
        SynapseModified,       ///< Synapse weight modified
        MemoryAllocated,       ///< Memory allocation occurred
        MemoryFreed,          ///< Memory deallocated
        ErrorOccurred,         ///< Error or exception occurred
        PerformanceMetric,     ///< Performance metric recorded
        Custom               ///< Custom debug event
    };

    /**
     * @brief Debug event structure
     */
    struct DebugEvent {
        EventType type;                ///< Type of event
        LogLevel level;                ///< Severity level
        SimulationStep step;           ///< Current simulation step
        Timestamp timestamp;           ///< Event timestamp
        std::string message;           ///< Event message
        std::string details;           ///< Detailed event information
        std::unordered_map<std::string, std::string> metadata; ///< Additional metadata
        
        DebugEvent() : type(EventType::Custom), level(LogLevel::Info),
                     step(0), timestamp(0.0), message(), details() {}
    };

    using DebugEventCallback = std::function<void(const DebugEvent& event)>;

    DebugLogger();
    ~DebugLogger();

    // === Core Operations ===

    /**
     * @brief Set the minimum log level for debugging
     * 
     * @param level Minimum log level to output
     */
    void setLogLevel(LogLevel level);

    /**
     * @brief Get the current log level
     * 
     * @return LogLevel Current log level
     */
    LogLevel getLogLevel() const;

    /**
     * @brief Add a debug event callback function
     * 
     * @param callback Function to call for each debug event
     * @param priority Priority of this callback (higher numbers = earlier execution)
     */
    void addEventCallback(DebugEventCallback callback, int priority = 0);

    /**
     * @brief Remove all debug event callbacks
     */
    void clearEventCallbacks();

    /**
     * @brief Log a debug event
     * 
     * @param type Event type
     * @param level Event severity level
     * @param message Event message
     * @param details Detailed event information (optional)
     */
    void logEvent(EventType type, LogLevel level, const std::string& message,
                  const std::string& details = "");

    /**
     * @brief Log a simulation step event
     * 
     * @param step Simulation step number
     * @param timestamp Timestamp of the step
     * @param message Step description
     */
    void logStep(SimulationStep step, Timestamp timestamp, const std::string& message);

    /**
     * @brief Log a spike generation event
     * 
     * @param neuronId ID of neuron that generated the spike
     * @param step Simulation step when spike occurred
     * @param timestamp Timestamp of spike
     */
    void logSpikeGeneration(NeuronId neuronId, SimulationStep step, Timestamp timestamp);

    /**
     * @brief Log a synapse modification event
     * 
     * @param synapseId ID of modified synapse
     * @param oldWeight Previous weight
     * @param newWeight New weight
     * @param step Simulation step of modification
     */
    void logSynapseModification(SynapseId synapseId, SynapticWeight oldWeight,
                               SynapticWeight newWeight, SimulationStep step);

    /**
     * @brief Log a memory allocation event
     * 
     * @param bytes Number of bytes allocated
     * @param location Memory allocation location (file and line)
     */
    void logMemoryAllocation(size_t bytes, const std::string& location);

    /**
     * @brief Log a memory deallocation event
     * 
     * @param bytes Number of bytes deallocated
     * @param location Memory deallocation location (file and line)
     */
    void logMemoryDeallocation(size_t bytes, const std::string& location);

    /**
     * @brief Log an error event
     * 
     * @param errorMessage Error message
     * @param location Error location (file and line)
     * @param step Simulation step when error occurred (optional)
     */
    void logError(const std::string& errorMessage, const std::string& location,
                  SimulationStep step = 0);

    /**
     * @brief Log a performance metric event
     * 
     * @param category Performance category
     * @param metricName Name of the metric
     * @param value Metric value
     */
    void logPerformanceMetric(const std::string& category, const std::string& metricName,
                             double value);

    // === State Inspection ===

    /**
     * @brief Get the current debug log as a string
     * 
     * @return std::string Complete debug log
     */
    std::string getDebugLog() const;

    /**
     * @brief Get debug events for a specific time range
     * 
     * @param startStep Start simulation step
     * @param endStep End simulation step
     * @return std::vector<DebugEvent> Debug events in the specified range
     */
    std::vector<DebugEvent> getEventsForRange(SimulationStep startStep, SimulationStep endStep) const;

    /**
     * @brief Filter debug events by type
     * 
     * @param type Event type to filter
     * @return std::vector<DebugEvent> Filtered debug events
     */
    std::vector<DebugEvent> getEventsByType(EventType type) const;

    /**
     * @brief Get statistics about debug events
     * 
     * @return std::unordered_map<std::string, uint64_t> Event type counts
     */
    std::unordered_map<std::string, uint64_t> getEventStatistics() const;

    /**
     * @brief Clear the debug log
     */
    void clearDebugLog();

    /**
     * @brief Enable or disable real-time logging
     * 
     * @param enabled true to enable real-time logging, false to disable
     */
    void setRealTimeLogging(bool enabled);

    /**
     * @brief Check if real-time logging is enabled
     * 
     * @return bool true if real-time logging is enabled
     */
    bool isRealTimeLoggingEnabled() const;

    /**
     * @brief Export debug log to a file
     * 
     * @param filepath Output file path
     * @return bool true if export was successful
     */
    bool exportDebugLog(const std::string& filepath);

    /**
     * @brief Import debug log from a file
     * 
     * @param filepath Input file path
     * @return bool true if import was successful
     */
    bool importDebugLog(const std::string& filepath);

    // === Performance Integration ===

    /**
     * @brief Attach performance metrics to debug logging
     * 
     * @param metrics Performance metrics instance
     */
    void attachPerformanceMetrics(const PerformanceMetrics& metrics);

    /**
     * @brief Get the attached performance metrics
     * 
     * @return PerformanceMetrics Attached performance metrics
     */
    PerformanceMetrics getAttachedPerformanceMetrics() const;

    // === Configuration ===

    /**
     * @brief Set debug logging to file
     * 
     * @param filepath File path for debug logging
     * @param append Append to existing file if true, overwrite if false
     */
    void setLogFile(const std::string& filepath, bool append = true);

    /**
     * @brief Close the debug log file
     */
    void closeLogFile();

    /**
     * @brief Check if debug logging is enabled
     * 
     * @return bool true if debug logging is enabled
     */
    bool isEnabled() const;

    /**
     * @brief Enable or disable debug logging
     * 
     * @param enabled true to enable, false to disable
     */
    void setEnabled(bool enabled);

private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

/**
 * @brief Global debug logger instance
 * 
 * This provides access to a global debug logger instance that can be used
 * throughout the NLM framework for consistent debugging.
 * 
 * @return DebugLogger& Reference to the global debug logger
 */
DebugLogger& getGlobalDebugLogger();

/**
 * @brief Global performance metrics instance
 * 
 * This provides access to a global performance metrics instance that can be used
 * throughout the NLM framework for consistent performance monitoring.
 * 
 * @return PerformanceMetrics& Reference to the global performance metrics
 */
PerformanceMetrics& getGlobalPerformanceMetrics();

/**
 * @brief Initialize global debugging and performance monitoring
 * 
 * This function initializes global instances of the debug logger and performance
 * metrics that can be used throughout the NLM framework. It's called automatically
 * when the pynlm module is imported.
 */
void initializeGlobalMonitoring();

/**
 * @brief Shutdown global debugging and performance monitoring
 * 
 * This function cleans up global instances of the debug logger and performance
 * metrics. It's called automatically when the pynlm module is unloaded.
 */
void shutdownGlobalMonitoring();

} // namespace nlm
