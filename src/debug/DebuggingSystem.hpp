#pragma once

#include <memory>
#include <vector>
#include <string>
#include <unordered_map>
#include <functional>
#include "DebugTrace.hpp"

namespace nlm {

// Debug event types for the debugging system
enum class DebugEventType {
    StepBegin,
    StepEnd,
    NeuronSpike,
    SynapseFire,
    MemoryAccess,
    PlasticityChange,
    Neuromodulation,
    PerformanceMetric,
    ResourceUsage,
    MemoryAllocation,
    MemoryDeallocation,
    Custom,
    ConnectionCreate,
    ConnectionRemove,
    WeightChange,
    RegionCreate,
    RegionDestroy,
    SensorInput,
    MotorOutput,
    RewardSignal,
    PredictionError,
    LearningSignal,
    DevelopmentChange,
    Atrophy,
    Synaptogenesis
};

// Debug system status
enum class DebugStatus {
    Inactive,
    Initializing,
    Active,
    Paused,
    ShuttingDown,
    Error
};

// DebugConfiguration
struct DebugConfiguration {
    // Trace settings
    bool enableTrace{true};
    bool enableEventFiltering{false};
    std::vector<DebugEventType> eventFilter;
    size_t maxTraceEvents{10000};
    
    // Performance settings
    bool enablePerformanceProfiling{true};
    double profilingInterval{0.1}; // seconds
    bool autoDetectBottlenecks{true};
    
    // Memory settings
    bool enableMemoryTracking{true};
    bool detectMemoryLeaks{true};
    size_t memoryCheckInterval{1000}; // steps
    
    // Logging settings
    bool enableLogging{true};
    std::string logFile{ "debug.log" };
    bool enableConsoleOutput{true};
    
    // Visualization settings
    bool enableVisualization{false};
    std::string visualizationPort{"8080"};
    bool enableLiveUpdates{false};
    
    // Integration settings
    bool integrateWithBrain{true};
    bool integrateWithAgent{true};
    bool integrateWithVisualization{true};
    
    // Remote debugging
    bool enableRemoteDebugging{false};
    std::string remoteHost{"localhost"};
    uint16_t remotePort{9999};
    
    // Export settings
    bool enableExport{true};
    std::string exportFormat{"json"};
    bool exportToFile{true};
    bool exportToDatabase{false};
};

// DebugHook: Callback for debugging events
class DebugHook {
public:
    virtual ~DebugHook() = default;
    
    // Called when a debug event occurs
    virtual void onDebugEvent(const DebugEvent& event) = 0;
    
    // Called for performance metrics
    virtual void onPerformanceMetric(const std::string& metricName, double value) = 0;
    
    // Called for memory events
    virtual void onMemoryEvent(const std::string& operation, size_t size) = 0;
    
    // Called when debugging is starting/stopping
    virtual void onDebugStatusChange(DebugStatus oldStatus, DebugStatus newStatus) = 0;
    
    // Priority for hook execution (higher priority hooks execute first)
    virtual int getPriority() const { return 0; }
};

// DebugData: General debug data container
struct DebugData {
    std::string name;
    std::string type;
    std::string value;
    std::chrono::system_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> metadata;
    
    DebugData(const std::string& name, const std::string& type, const std::string& value)
        : name(name), type(type), value(value), timestamp(std::chrono::system_clock::now()) {}
};

// DebuggingSystem: Central debugging manager
class DebuggingSystem {
public:
    DebuggingSystem();
    explicit DebuggingSystem(const DebugConfiguration& config);
    ~DebuggingSystem();
    
    // Configuration
    void configure(const DebugConfiguration& config);
    const DebugConfiguration& getConfiguration() const { return config_; }
    
    // Status management
    DebugStatus getStatus() const { return status_; }
    bool isActive() const { return status_ == DebugStatus::Active; }
    
    // Control
    void start();
    void stop();
    void pause();
    void resume();
    void reset();
    
    // Hook management
    void addHook(std::shared_ptr<DebugHook> hook);
    void removeHook(std::shared_ptr<DebugHook> hook);
    void clearHooks();
    
    // Trace management
    void recordEvent(DebugEventType type, const std::string& source, const std::string& message);
    void recordEvent(DebugEvent event);
    std::shared_ptr<DebugTrace> getTrace() const { return trace_; }
    
    // Performance profiling
    void recordPerformanceMetric(const std::string& name, double value, const std::string& unit = "");
    double getAveragePerformance(const std::string& name) const;
    double getMaxPerformance(const std::string& name) const;
    
    // Resource monitoring
    void recordResourceUsage(double cpu, double memory, size_t memoryAllocations, size_t threadCount);
    
    // Memory tracking
    void recordMemoryAllocation(size_t size, const std::string& context = "");
    void recordMemoryDeallocation(size_t size, const std::string& context = "");
    
    // State tracking
    void recordState(const std::string& stateName, const std::string& stateData);
    std::string getState(const std::string& stateName) const;
    
    // Integration methods
    void integrateWithBrain(std::shared_ptr<class Brain> brain);
    void integrateWithAgent(std::shared_ptr<class AgentBrain> agent);
    void integrateWithVisualization(std::shared_ptr<class VisualizationInterface> visualization);
    
    // Analysis methods
    void analyzeAnomalies();
    void detectAnomalies();
    
    // Export
    bool exportDebugData(const std::string& filepath) const;
    std::string exportDebugDataAsString() const;
    
    // Get debug data
    std::vector<DebugData> getDebugData(const std::string& category) const;
    
    // Remote debugging
    void startRemoteDebugger();
    void stopRemoteDebugger();
    
    // Multi-session support
    void startNewSession(const std::string& sessionName);
    void endCurrentSession();
    std::string getCurrentSessionName() const;
    
    // Configuration management
    void saveConfiguration(const std::string& filepath) const;
    bool loadConfiguration(const std::string& filepath);
    
    // Utility
    void logStatus() const;
    
    // Friendship for integration
    friend class PerformanceProfiler;
    friend class EventLogger;
    friend class MemoryProfiler;
    friend class DebugVisualizer;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    DebugConfiguration config_;
    DebugStatus status_;
    std::shared_ptr<DebugTrace> trace_;
    
    // Helper methods
    void notifyHooks(const DebugEvent& event);
    void notifyPerformanceHooks(const std::string& name, double value);
    void notifyMemoryHooks(const std::string& operation, size_t size);
    void notifyStatusChangeHooks(DebugStatus oldStatus, DebugStatus newStatus);
};

} // namespace nlm
