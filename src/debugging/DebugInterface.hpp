#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <unordered_map>
#include <set>
#include <thread>
#include <atomic>
#include <functional>

// Forward declarations
namespace nlm {
    class Brain;
    class Neuron;
    class Synapse;
    class Config;
    class Logger;
    class RandomGenerator;
    class SimulationClock;
    class AgentBrain;
    class SimpleWorld;
}

namespace nlm {

// Memory allocation tracking
struct MemoryAllocation {
    void* address;
    size_t size;
    std::chrono::steady_clock::time_point allocationTime;
    std::string allocationSite;
    std::thread::id threadId;
    bool isLeaked;
};

// Performance metrics
struct PerformanceMetrics {
    double cpuUsage; // percentage
    double memoryUsage; // percentage
    uint64_t totalMemory; // bytes
    uint64_t usedMemory; // bytes
    uint64_t peakMemory; // bytes
    uint64_t networkBytesSent;
    uint64_t networkBytesReceived;
    double diskUsage; // percentage
    std::chrono::steady_clock::time_point timestamp;
};

// Neural activity metrics
struct NeuralMetrics {
    size_t totalNeuronCount;
    size_t activeNeuronCount;
    size_t spikingNeuronCount;
    float averageFiringRate;
    float excitationInhibitionRatio;
    float meanMembranePotential;
    float meanSynapticWeight;
    float connectivityDensity;
    std::chrono::steady_clock::time_point timestamp;
};

// Simulation performance metrics
struct SimulationMetrics {
    uint64_t currentStep;
    double simulationTime; // seconds
    double wallClockTime; // seconds
    double stepsPerSecond;
    double efficiency; // simulation time / real time
    uint64_t spikeCount;
    uint64_t eventCount;
    uint64_t memoryPoolAllocationCount;
    uint64_t memoryPoolDeallocationCount;
    std::chrono::steady_clock::time_point timestamp;
};

// Resource usage tracking
struct ResourceUsage {
    uint64_t memoryPoolUsed;
    uint64_t memoryPoolAllocated;
    uint64_t eventQueueSize;
    uint64_t pendingSpikes;
    uint64_t checkpointSize;
    double cpuTemperature;
    double memoryTemperature;
    std::chrono::steady_clock::time_point timestamp;
};

// Custom monitoring hooks
struct MonitoringHook {
    std::string hookName;
    std::function<void(const std::unordered_map<std::string, double>&)> callback;
    std::chrono::steady_clock::time_point lastExecution;
    double executionInterval; // seconds
    bool isActive;
    std::string hookType; // "pre_step", "post_step", "periodic", "event"
};

// Error and exception information
struct ErrorInfo {
    std::chrono::steady_clock::time_point timestamp;
    std::string errorType; // "exception", "assertion", "validation", "runtime"
    std::string errorCode;
    std::string errorMessage;
    std::string stackTrace;
    std::unordered_map<std::string, std::string> context;
    bool isRecovered;
    bool isCritical;
};

// Step-by-step debugging state
struct DebugStepInfo {
    uint64_t step;
    std::chrono::steady_clock::time_point timestamp;
    std::unordered_map<std::string, std::string> neuronStates;
    std::unordered_map<std::string, std::string> synapseStates;
    std::unordered_map<std::string, std::string> systemStates;
    std::vector<std::string> warnings;
    std::vector<std::string> errors;
};

// Configuration validation result
struct ValidationResult {
    bool isValid;
    std::vector<std::string> errors;
    std::vector<std::string> warnings;
    std::unordered_map<std::string, std::string> correctedValues;
    std::chrono::steady_clock::time_point validationTime;
};

// Debugging mode
enum class DebugMode {
    None,
    Basic,
    Detailed,
    Expert
};

// Monitoring mode
enum class MonitoringMode {
    Disabled,
    Passive,
    Active,
    Alerting
};

// Debug command
struct DebugCommand {
    std::string commandName;
    std::string description;
    std::function<bool(const std::vector<std::string>&)> executor;
    std::vector<std::string> requiredParameters;
    std::string category; // "brain", "world", "performance", "memory", "config"
};

class DebugInterface {
public:
    DebugInterface();
    ~DebugInterface();
    
    // Initialize debug interface
    bool initialize();
    
    // Update debug information
    void update();
    
    // Close debug interface
    void close();
    
    // Check if debug is active
    bool isActive() const;
    
    // Set debug mode
    void setDebugMode(DebugMode mode);
    DebugMode getDebugMode() const;
    
    // Set monitoring mode
    void setMonitoringMode(MonitoringMode mode);
    MonitoringMode getMonitoringMode() const;
    
    // Real-time debugging operations
    void enableRealTimeDebugging(bool enable);
    bool isRealTimeDebuggingEnabled() const;
    
    // Memory leak detection
    void enableMemoryLeakDetection(bool enable);
    bool isMemoryLeakDetectionEnabled() const;
    std::vector<MemoryAllocation> detectMemoryLeaks() const;
    void trackMemoryAllocation(void* address, size_t size, const std::string& site);
    void trackMemoryDeallocation(void* address);
    
    // Performance profiling
    void enablePerformanceProfiling(bool enable);
    bool isPerformanceProfilingEnabled() const;
    PerformanceMetrics getPerformanceMetrics() const;
    double getCPUUsage() const;
    double getMemoryUsage() const;
    double getDiskUsage() const;
    
    // Neural activity monitoring
    NeuralMetrics getNeuralMetrics(const Brain& brain) const;
    float getAverageFiringRate(const Brain& brain) const;
    size_t getActiveNeuronCount(const Brain& brain) const;
    std::vector<uint64_t> getMostActiveNeurons(const Brain& brain, size_t count) const;
    
    // State inspection
    DebugStepInfo getCurrentDebugStep() const;
    DebugStepInfo getDebugStep(uint64_t step) const;
    std::string inspectNeuron(const Neuron& neuron) const;
    std::string inspectSynapse(const Synapse& synapse) const;
    std::string inspectNetwork(const Brain& brain) const;
    
    // Simulation debugging
    void enableStepByStepDebugging(bool enable);
    bool isStepByStepDebuggingEnabled() const;
    void setDebugStep(uint64_t step);
    uint64_t getCurrentDebugStep() const;
    void stepForward();
    void stepBackward();
    bool hasMoreStepsForward() const;
    bool hasMoreStepsBackward() const;
    
    // Error detection and recovery
    void enableErrorDetection(bool enable);
    bool isErrorDetectionEnabled() const;
    std::vector<ErrorInfo> getRecentErrors() const;
    std::vector<ErrorInfo> getCriticalErrors() const;
    bool hasRecentErrors() const;
    void logError(const std::string& errorCode, const std::string& errorMessage, 
                  bool isCritical = false, const std::unordered_map<std::string, std::string>& context = {});
    
    // Configuration validation
    ValidationResult validateConfiguration(const Config& config) const;
    std::vector<std::string> getConfigurationErrors(const Config& config) const;
    std::vector<std::string> getConfigurationWarnings(const Config& config) const;
    
    // System and resource monitoring
    ResourceUsage getResourceUsage() const;
    SimulationMetrics getSimulationMetrics() const;
    void trackSystemEvents(const std::string& eventType, const std::unordered_map<std::string, std::string>& eventData);
    std::vector<std::unordered_map<std::string, std::string>> getRecentEvents(const std::string& eventType) const;
    
    // Custom monitoring hooks
    void addMonitoringHook(const MonitoringHook& hook);
    void removeMonitoringHook(const std::string& hookName);
    void executeMonitoringHooks();
    std::vector<MonitoringHook> getMonitoringHooks() const;
    
    // Interactive debugging interface
    void enableCommandLineInterface(bool enable);
    bool isCommandLineInterfaceEnabled() const;
    bool executeCommand(const std::string& commandLine);
    std::vector<DebugCommand> getAvailableCommands() const;
    std::string getCommandHelp(const std::string& commandName) const;
    
    // Debug output and reporting
    void enableDebugOutput(bool enable);
    bool isDebugOutputEnabled() const;
    void setOutputLevel(int level); // 0=quiet, 1=info, 2=verbose, 3=debug
    int getOutputLevel() const;
    std::string generateDebugReport() const;
    void exportDebugData(const std::string& filename) const;
    
    // Callback and event handling
    void setDebugEventCallback(std::function<void(const std::string&, const std::string&)> callback);
    void setPerformanceCallback(std::function<void(const PerformanceMetrics&)> callback);
    void setMemoryCallback(std::function<void(const std::vector<MemoryAllocation>&)> callback);
    void setNeuralCallback(std::function<void(const NeuralMetrics&)> callback);
    
    // Recording and replay
    void startRecording(const std::string& filename);
    void stopRecording();
    bool isRecording() const;
    void replayDebugData(const std::string& filename, uint64_t startStep = 0, uint64_t endStep = 0);
    
    // Configuration and settings
    void loadDebugConfiguration(const std::string& configFile);
    void saveDebugConfiguration(const std::string& configFile) const;
    
protected:
    // Internal helper methods
    virtual void initializeBackends();
    virtual void cleanupBackends();
    virtual bool areBackendsInitialized() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm