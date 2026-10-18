// Advanced debugging and profiling tools for NLM
// Provides comprehensive debugging capabilities for neural simulation analysis

#ifndef NLM_DEBUG_TOOLS_HPP
#define NLM_DEBUG_TOOLS_HPP

#include <string>
#include <vector>
#include <map>
#include <unordered_map>
#include <memory>
#include <chrono>
#include <fstream>
#include <sstream>
#include <iomanip>
#include <algorithm>
#include <functional>

namespace nlm {

// Debug level enumeration
enum class DebugLevel {
    Quiet = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4,
    Trace = 5
};

// Event types for debugging
enum class DebugEventType {
    SimulationStart,
    SimulationEnd,
    StepCompleted,
    SpikeEmitted,
    PlasticityUpdate,
    MemoryUpdate,
    NeuromodulationUpdate,
    ErrorOccurred,
    WarningIssued,
    ConfigurationLoaded,
    BrainInitialized,
    ExperimentStarted,
    ExperimentCompleted
};

// Debug event for detailed tracking
struct DebugEvent {
    DebugEventType type;
    double timestamp;
    std::string description;
    std::string details;
    std::unordered_map<std::string, std::string> metadata;
    
    DebugEvent(DebugEventType t, const std::string& desc, const std::string& det = "")
        : type(t), timestamp(0.0), description(desc), details(det) {}
};

// Performance profiler for detailed timing analysis
class DebugProfiler {
public:
    struct ProfileData {
        std::string operationName;
        double duration;
        uint64_t calls;
        double averageDuration;
        double minDuration;
        double maxDuration;
        std::chrono::high_resolution_clock::time_point startTime;
    };
    
    DebugProfiler() = default;
    ~DebugProfiler() = default;
    
    void startProfile(const std::string& operation);
    void endProfile(const std::string& operation);
    void reset();
    double getTotalTime(const std::string& operation) const;
    uint64_t getCallCount(const std::string& operation) const;
    const std::vector<ProfileData>& getProfileData() const { return profileData; }
    void printReport() const;
    void saveReport(const std::string& filename) const;
    
    // Nested profiling support
    class ProfilerScope {
    public:
        ProfilerScope(DebugProfiler& profiler, const std::string& operation)
            : profiler(profiler), operation(operation) {
            profiler.startProfile(operation);
        }
        
        ~ProfilerScope() {
            profiler.endProfile(operation);
        }
    
    private:
        DebugProfiler& profiler;
        std::string operation;
    };
    
    template<typename T>
    ProfilerScope measure(const std::string& operation) {
        return ProfilerScope(*this, operation);
    }
    
private:
    std::vector<ProfileData> profileData;
    std::unordered_map<std::string, ProfileData> profileMap;
};

// Memory profiler for tracking memory usage
class MemoryProfiler {
public:
    struct MemorySnapshot {
        double timestamp;
        size_t totalMemory;
        size_t allocatedMemory;
        size_t peakMemory;
        size_t allocationCount;
        size_t deallocationCount;
        std::unordered_map<std::string, size_t> allocationTypes;
    };
    
    MemoryProfiler() : trackingEnabled(false), snapshotInterval(0.1), 
                       totalAllocated(0), totalDeallocated(0) {}
    
    void startTracking();
    void stopTracking();
    void takeSnapshot();
    void recordAllocation(size_t size, const std::string& type = "unknown");
    void recordDeallocation(size_t size, const std::string& type = "unknown");
    const std::vector<MemorySnapshot>& getSnapshots() const { return snapshots; }
    size_t getCurrentMemoryUsage() const { return currentMemoryUsage; }
    size_t getPeakMemoryUsage() const { return peakMemoryUsage; }
    double getTotalAllocated() const { return totalAllocated; }
    double getTotalDeallocated() const { return totalDeallocated; }
    void printReport() const;
    void saveReport(const std::string& filename) const;
    
    // Memory leak detection
    struct LeakInfo {
        std::string type;
        size_t size;
        std::string allocationStack;
    };
    
    std::vector<LeakInfo> detectMemoryLeaks() const;
    
private:
    bool trackingEnabled;
    double snapshotInterval;
    std::vector<MemorySnapshot> snapshots;
    std::chrono::high_resolution_clock::time_point startTime;
    size_t currentMemoryUsage;
    size_t peakMemoryUsage;
    double totalAllocated;
    double totalDeallocated;
    std::unordered_map<std::string, size_t> allocationMap;
    std::unordered_map<std::string, std::vector<std::string>> allocationStacks;
};

// Event logger for tracking simulation events
class DebugEventLogger {
public:
    DebugEventLogger() : logLevel(DebugLevel::Info), maxEvents(10000), 
                        currentEventCount(0), logFile(""), consoleOutput(true) {}
    
    void setLogLevel(DebugLevel level) { logLevel = level; }
    void setMaxEvents(size_t max) { maxEvents = max; }
    void setLogFile(const std::string& filename) { logFile = filename; }
    void enableConsoleOutput(bool enable) { consoleOutput = enable; }
    
    void logEvent(DebugEventType type, const std::string& description, 
                  const std::string& details = "", 
                  const std::unordered_map<std::string, std::string>& metadata = {});
    
    const std::vector<DebugEvent>& getEvents() const { return events; }
    std::vector<DebugEvent> getEventsByType(DebugEventType type) const;
    std::vector<DebugEvent> getEventsSince(double timestamp) const;
    void clearEvents();
    
    void printEvents() const;
    void saveEvents(const std::string& filename) const;
    void exportEventsJSON(const std::string& filename) const;
    void exportEventsCSV(const std::string& filename) const;
    
    // Event filtering
    void setEventFilter(std::function<bool(const DebugEvent&)> filter);
    std::vector<DebugEvent> getFilteredEvents() const;
    
private:
    DebugLevel logLevel;
    size_t maxEvents;
    size_t currentEventCount;
    std::string logFile;
    bool consoleOutput;
    std::vector<DebugEvent> events;
    std::function<bool(const DebugEvent&)> eventFilter;
};

// Neural network debugger for analyzing brain state
class NeuralNetworkDebugger {
public:
    NeuralNetworkDebugger() : brain(nullptr), enableNeuronLogging(false), 
                           enableSynapseLogging(false), enableRegionLogging(false) {}
    
    void attachBrain(std::shared_ptr<class Brain> brain) { this->brain = brain; }
    bool isBrainAttached() const { return brain != nullptr; }
    
    void enableNeuronLogging(bool enable) { enableNeuronLogging = enable; }
    void enableSynapseLogging(bool enable) { enableSynapseLogging = enable; }
    void enableRegionLogging(bool enable) { enableRegionLogging = enable; }
    
    // Neuron debugging
    void logNeuronState(NeuronId neuronId) const;
    void logAllNeuronStates() const;
    std::vector<NeuronId> getActiveNeurons() const;
    std::vector<NeuronId> getFiringNeurons() const;
    std::vector<NeuronId> getRefractoryNeurons() const;
    
    // Synapse debugging
    void logSynapseState(SynapseId synapseId) const;
    void logAllSynapseStates() const;
    std::vector<SynapseId> getActiveSynapses() const;
    std::vector<SynapseId> getPotentiallyPlastic() const;
    
    // Region debugging
    void logRegionState(RegionId regionId) const;
    void logAllRegionStates() const;
    std::vector<RegionId> getActiveRegions() const;
    
    // Network analysis
    void analyzeNetworkTopology() const;
    void analyzeNetworkDynamics() const;
    void analyzePlasticityState() const;
    
    // State comparison
    void compareStates(const std::string& label1, const std::string& label2);
    
    // Memory analysis
    void analyzeMemoryUsage() const;
    void analyzePlasticityMemory() const;
    
    // Performance analysis
    void analyzePerformanceMetrics() const;
    void analyzePlasticityEfficiency() const;
    
    // Export functionality
    void exportNetworkState(const std::string& filename) const;
    void exportNeuronStates(const std::string& filename) const;
    void exportSynapseStates(const std::string& filename) const;
    void exportRegionStates(const std::string& filename) const;
    
private:
    std::shared_ptr<class Brain> brain;
    bool enableNeuronLogging;
    bool enableSynapseLogging;
    bool enableRegionLogging;
    
    // Helper methods for logging
    void logNeuronDebugInfo(const Neuron* neuron) const;
    void logSynapseDebugInfo(const Synapse* synapse) const;
    void logRegionDebugInfo(const NeuralRegion* region) const;
};

// Comprehensive debugging suite combining all debugging tools
class DebugSuite {
public:
    DebugSuite() : profiler(), memoryProfiler(), eventLogger(), networkDebugger() {}
    
    // Configuration
    void setLogLevel(DebugLevel level);
    void setDebugLevel(DebugLevel level) { setLogLevel(level); }
    void enableProfiling(bool enable) { profilerEnabled = enable; }
    void enableMemoryTracking(bool enable) { memoryProfiler.startTracking(); }
    void enableEventLogging(bool enable) { eventLogger.enableConsoleOutput(enable); }
    void enableNetworkDebugging(bool enable) { networkDebuggerAttached = enable; }
    
    // Control
    void startDebugSession();
    void endDebugSession();
    void reset();
    
    // Logging
    void logEvent(DebugEventType type, const std::string& description,
                  const std::string& details = "",
                  const std::unordered_map<std::string, std::string>& metadata = {});
    
    // Profiling
    void startProfiler(const std::string& operation);
    void endProfiler(const std::string& operation);
    
    // Memory tracking
    void recordAllocation(size_t size, const std::string& type = "unknown");
    void recordDeallocation(size_t size, const std::string& type = "unknown");
    
    // Network debugging
    void logNeuronState(NeuronId neuronId);
    void logSynapseState(SynapseId synapseId);
    void logRegionState(RegionId regionId);
    
    // Analysis and reporting
    void generateDebugReport();
    void saveDebugData(const std::string& directory);
    void exportAllData(const std::string& filename);
    
    // Configuration
    void loadDebugConfig(const std::string& filename);
    void saveDebugConfig(const std::string& filename) const;
    
    // Utility functions
    static std::string generateTimestamp();
    static std::string formatDuration(double seconds);
    static std::string formatMemorySize(size_t bytes);
    
private:
    bool profilerEnabled;
    bool networkDebuggerAttached;
    
    DebugProfiler profiler;
    MemoryProfiler memoryProfiler;
    DebugEventLogger eventLogger;
    NeuralNetworkDebugger networkDebugger;
};

// Global debug suite instance
inline DebugSuite debugSuite;

// Convenience functions for common debugging tasks
inline void debugStart(const std::string& operation) {
    debugSuite.startProfiler(operation);
}

inline void debugEnd(const std::string& operation) {
    debugSuite.endProfiler(operation);
}

inline void debugLog(DebugEventType type, const std::string& description, 
                     const std::string& details = "") {
    debugSuite.logEvent(type, description, details);
}

inline void debugNeuronState(NeuronId neuronId) {
    debugSuite.logNeuronState(neuronId);
}

inline void debugSynapseState(SynapseId synapseId) {
    debugSuite.logSynapseState(synapseId);
}

inline void generateDebugReport() {
    debugSuite.generateDebugReport();
}

inline void saveDebugData(const std::string& directory) {
    debugSuite.saveDebugData(directory);
}

} // namespace nlm

#endif // NLM_DEBUG_TOOLS_HPP