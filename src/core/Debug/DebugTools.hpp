#ifndef NLM_DEBUG_TOOLS_H
#define NLM_DEBUG_TOOLS_H

#include <string>
#include <vector>
#include <map>
#include <chrono>
#include <unordered_map>
#include <memory>

namespace nlm {

// Debug log levels
enum DebugLogLevel {
    DebugLevelOff = 0,
    DebugLevelError = 1,
    DebugLevelWarning = 2,
    DebugLevelInfo = 3,
    DebugLevelDebug = 4,
    DebugLevelTrace = 5
};

// Performance measurement structures
struct PerformanceMetrics {
    size_t stepCount = 0;
    double totalWallTime = 0.0;
    size_t totalSpikeCount = 0;
    double avgFiringRate = 0.0;
    std::unordered_map<std::string, double> stepTimes;
    std::vector<double> spikeRates;
    std::map<std::string, size_t> neuronCounts;
    std::map<std::string, size_t> synapseCounts;
    double memoryUsage = 0.0;
    size_t checkpointCount = 0;
    size_t replayCount = 0;
    size_t developmentStages = 0;
};

// Event tracking structures
struct DebugEvent {
    std::chrono::high_resolution_clock::time_point timestamp;
    std::string eventType;
    std::string description;
    std::map<std::string, std::string> metadata;
    std::string threadId;
};

// Memory analysis structure
struct MemoryAnalysis {
    size_t totalAllocated = 0;
    size_t totalFreed = 0;
    size_t currentAllocated = 0;
    std::map<std::string, size_t> typeAllocations;
    std::vector<size_t> memorySnapshots;
    double peakMemoryUsage = 0.0;
    size_t snapshotInterval = 0;
};

// Brain state snapshot for debugging
struct BrainStateSnapshot {
    size_t step = 0;
    double time = 0.0;
    std::vector<size_t> neuronCountsByType;
    std::vector<float> averageWeights;
    std::vector<double> firingRates;
    double totalSpikes = 0.0;
    double eIrRatio = 0.0;
    std::map<std::string, double> systemMetrics;
};

class DebugTools {
public:
    static DebugTools& getInstance() {
        static DebugTools instance;
        return instance;
    }
    
    // Configuration
    void setLogLevel(DebugLogLevel level) { currentLogLevel = level; }
    DebugLogLevel getLogLevel() const { return currentLogLevel; }
    
    // Performance tracking
    void startStepTimer() { stepStartTime = std::chrono::high_resolution_clock::now(); }
    void endStepTimer(const std::string& stepName = "default");
    PerformanceMetrics getPerformanceMetrics() const { return metrics; }
    void resetPerformanceMetrics() { metrics = PerformanceMetrics(); }
    
    // Event tracking
    void logEvent(const std::string& eventType, const std::string& description, 
                  const std::map<std::string, std::string>& metadata = {});
    std::vector<DebugEvent> getEvents(const std::string& eventType = "", 
                                     size_t limit = 100) const;
    
    // Memory analysis
    void takeMemorySnapshot();
    MemoryAnalysis getMemoryAnalysis() const { return memoryAnalysis; }
    
    // Brain state debugging
    void recordBrainSnapshot(size_t step, const std::string& description = "");
    std::vector<BrainStateSnapshot> getBrainSnapshots(size_t limit = 100) const;
    
    // System monitoring
    void updateSystemMetrics(const std::string& component, 
                           const std::map<std::string, double>& metrics);
    
    // Configuration validation
    static bool validateConfig(const std::string& configFilepath);
    static void printConfigSummary(const std::string& configFilepath);
    
    // Performance analysis
    void generatePerformanceReport(const std::string& outputFile = "") const;
    double getAverageStepTime() const;
    double getSpikeRate(size_t steps = 100) const;
    
    // Debugging helpers
    template<typename T>
    static std::string toString(const T& value);
    
    // Advanced user tools
    static void createDebugProfile(const std::string& outputFile = "");
    static void analyzePlasticityPatterns(const std::string& outputFile = "");
    static void generateVisualizationData(const std::string& outputFile = "");
    
private:
    DebugTools() = default;
    ~DebugTools() = default;
    DebugTools(const DebugTools&) = delete;
    DebugTools& operator=(const DebugTools&) = delete;
    
    DebugLogLevel currentLogLevel = DebugLevelInfo;
    PerformanceMetrics metrics;
    MemoryAnalysis memoryAnalysis;
    std::vector<DebugEvent> events;
    std::vector<BrainStateSnapshot> brainSnapshots;
    std::map<std::string, std::map<std::string, double>> systemMetrics;
    std::chrono::high_resolution_clock::time_point stepStartTime;
};

// Global debug instance
inline DebugTools& debug = DebugTools::getInstance();

// Convenience macros for debugging
#define DEBUG_LOG(level, msg) if (debug.getLogLevel() >= level) debug.logEvent(#level, msg)
#define DEBUG_PERF_START() debug.startStepTimer()
#define DEBUG_PERF_END(stepName) debug.endStepTimer(stepName)

} // namespace nlm

#endif // NLM_DEBUG_TOOLS_H
