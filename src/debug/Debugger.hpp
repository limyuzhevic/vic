#pragma once

#include <string>
#include <vector>
#include <unordered_map>
#include <chrono>
#include <memory>
#include <sstream>
#include "../core/Logger/Logger.hpp"
#include "../core/Config/Config.hpp"
#include "../brain/Brain.hpp"
#include "../performance/Performance.hpp"

namespace nlm {

/**
 * Advanced debugging and profiling tools for NLM
 * 
 * This module provides comprehensive debugging capabilities for:
 * - Neural state inspection (neurons, synapses, regions)
 * - Memory usage tracking and analysis
 * - Performance profiling and timing
 * - Activity pattern analysis
 * - Configuration validation
 * - Error state detection
 * 
 * Designed to help users debug, optimize, and extend the NLM system.
 */

/**
 * Debug level for controlling verbosity
 */
enum class DebugLevel {
    Off = 0,
    Error = 1,
    Warning = 2,
    Info = 3,
    Debug = 4,
    Verbose = 5,
    Trace = 6
};

/**
 * Debug context for organizing debugging sessions
 */
struct DebugContext {
    std::string sessionId;
    std::chrono::system_clock::time_point startTime;
    DebugLevel level;
    std::string description;
    
    DebugContext(const std::string& id, DebugLevel lvl, const std::string& desc)
        : sessionId(id), startTime(std::chrono::system_clock::now())
        , level(lvl), description(desc) {}
};

/**
 * Neuron state snapshot for debugging
 */
struct NeuronDebugInfo {
    NeuronId id;
    NeuronType type;
    FiringState state;
    float membranePotential;
    float restingPotential;
    float threshold;
    float resetPotential;
    float leakConductance;
    float firingRate;
    uint32_t refractoryRemaining;
    uint32_t refractoryPeriod;
    float adaptationVariable;
    float lastSpikeTime;
    uint64_t regionId;
    uint64_t populationId;
    uint64_t lastPreSpike;
    uint64_t lastPostSpike;
    float eligibilityTrace;
    float efficacy;
    std::vector<float> incomingWeights;
    std::vector<NeuronId> incomingNeurons;
    
    // Helper methods
    std::string toString() const {
        std::stringstream ss;
        ss << "NeuronId(" << id.value << ") type:" << static_cast<int>(type)
           << " state:" << static_cast<int>(state)
           << " V:" << membranePotential
           << " rate:" << firingRate
           << " ref:" << refractoryRemaining << "/" << refractoryPeriod
           << " region:" << regionId;
        return ss.str();
    }
};

/**
 * Synapse state snapshot for debugging
 */
struct SynapseDebugInfo {
    SynapseId id;
    NeuronId sourceNeuron;
    NeuronId destinationNeuron;
    float weight;
    uint32_t delay;
    SynapseType type;
    PlasticityFlags plasticityFlags;
    float shortTermDepression;
    float shortTermFacilitation;
    float lastPreSpikeTime;
    float lastPostSpikeTime;
    float eligibilityTrace;
    float efficacy;
    uint64_t regionId;
    uint64_t populationId;
    
    // Helper methods
    std::string toString() const {
        std::stringstream ss;
        ss << "SynapseId(" << id.value << ") "
           << "src:" << sourceNeuron.value << " dst:" << destinationNeuron.value
           << " weight:" << weight << " delay:" << delay
           << " type:" << static_cast<int>(type)
           << " plastic:" << (plasticityFlags == PlasticityFlags::All ? "All" : "None");
        return ss.str();
    }
};

/**
 * Memory usage tracking
 */
struct MemoryProfile {
    size_t totalAllocated;
    size_t currentUsage;
    size_t peakUsage;
    size_t allocationCount;
    std::unordered_map<std::string, size_t> allocationsByType;
    
    MemoryProfile() : totalAllocated(0), currentUsage(0), peakUsage(0), allocationCount(0) {}
    
    void recordAllocation(size_t size, const std::string& type = "unknown") {
        totalAllocated += size;
        currentUsage += size;
        peakUsage = std::max(peakUsage, currentUsage);
        allocationCount++;
        allocationsByType[type]++;
    }
    
    void recordDeallocation(size_t size) {
        currentUsage -= size;
    }
    
    double getMemoryUtilization() const {
        return peakUsage > 0 ? static_cast<double>(currentUsage) / peakUsage : 0.0;
    }
};

/**
 * Performance timer
 */
class Timer {
public:
    Timer(const std::string& name, DebugLevel level = DebugLevel::Info)
        : name_(name), level_(level), started_(false) {}
    
    void start() {
        if (level_ > DebugLevel::Off) {
            startTime_ = std::chrono::high_resolution_clock::now();
            started_ = true;
        }
    }
    
    void stop() {
        if (started_) {
            auto endTime = std::chrono::high_resolution_clock::now();
            auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
            
            if (level_ <= DebugLevel::Info) {
                NLM_LOG_INFO("Timer " + name_ + ": " + std::to_string(duration.count()) + " μs");
            }
            started_ = false;
        }
    }
    
    double getDurationMs() const {
        if (!started_) return 0.0;
        auto endTime = std::chrono::high_resolution_clock::now();
        auto duration = std::chrono::duration_cast<std::chrono::microseconds>(endTime - startTime_);
        return duration.count() / 1000.0;
    }
    
private:
    std::string name_;
    DebugLevel level_;
    bool started_;
    std::chrono::high_resolution_clock::time_point startTime_;
};

/**
 * Advanced debugging and profiling system
 */
class Debugger {
public:
    Debugger(std::shared_ptr<Config> config);
    ~Debugger();
    
    // Session management
    std::string startDebugSession(DebugLevel level, const std::string& description);
    void endDebugSession(const std::string& sessionId);
    
    // Configuration and state inspection
    std::vector<NeuronDebugInfo> getAllNeuronDebugInfo() const;
    std::vector<SynapseDebugInfo> getAllSynapseDebugInfo() const;
    std::vector<NeuronDebugInfo> getNeuronDebugInfo(const std::vector<NeuronId>& ids) const;
    std::vector<SynapseDebugInfo> getSynapseDebugInfo(const std::vector<SynapseId>& ids) const;
    
    // Activity analysis
    std::vector<NeuronDebugInfo> getActiveNeurons() const;
    std::vector<SynapseDebugInfo> getHighActivitySynapses(float threshold = 0.1f) const;
    std::unordered_map<RegionId, size_t> getRegionActivity() const;
    
    // Memory and performance profiling
    MemoryProfile getMemoryProfile() const;
    void recordMemoryAllocation(size_t size, const std::string& type = "unknown");
    void recordMemoryDeallocation(size_t size);
    
    // Timeline and event tracking
    void logEvent(const std::string& eventName, const std::string& details = "");
    std::vector<std::string> getEventTimeline() const;
    
    // Configuration validation
    std::vector<std::string> validateConfiguration() const;
    std::vector<std::string> checkForIssues() const;
    
    // Performance analysis
    std::string getPerformanceReport() const;
    double getAverageFiringRate() const;
    double getSynapseDensity() const;
    
    // State export/import
    bool exportDebugState(const std::string& filepath) const;
    bool importDebugState(const std::string& filepath);
    
    // Debug output
    void printNeuronStats() const;
    void printSynapseStats() const;
    void printMemoryStats() const;
    void printPerformanceStats() const;
    void printAllStats();
    
    // Configuration integration
    void applyDebugConfiguration(const std::string& configKeyPrefix);
    void updateDebugLevel(DebugLevel level);
    DebugLevel getDebugLevel() const { return currentLevel_; }
    
    // Convenience methods for common debugging tasks
    void debugNeuronConnections(NeuronId neuronId) const;
    void debugSynapseDynamics(SynapseId synapseId) const;
    void debugNetworkTopology() const;
    void debugMemoryUsage() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl_;
    DebugLevel currentLevel_;
    std::shared_ptr<Config> config_;
    std::string currentSessionId_;
    
    // Helper methods
    void logDebugMessage(DebugLevel level, const std::string& message) const;
    bool shouldLog(DebugLevel level) const;
};

// Global debugging instance (can be accessed from anywhere)
extern std::unique_ptr<Debugger> globalDebugger;

// Convenience functions for easy debugging
inline void debugNeuronState(NeuronId neuronId) {
    if (globalDebugger) {
        globalDebugger->debugNeuronConnections(neuronId);
    }
}

inline void debugSynapseState(SynapseId synapseId) {
    if (globalDebugger) {
        globalDebugger->debugSynapseDynamics(synapseId);
    }
}

inline void startPerformanceProfiling(const std::string& sessionName) {
    if (globalDebugger) {
        globalDebugger->startDebugSession(DebugLevel::Debug, "Performance profiling: " + sessionName);
    }
}

inline void stopPerformanceProfiling() {
    if (globalDebugger && globalDebugger->getDebugLevel() >= DebugLevel::Debug) {
        globalDebugger->endDebugSession(globalDebugger->getDebugLevel());
    }
}

inline std::string getDebugReport() {
    if (globalDebugger) {
        return globalDebugger->getPerformanceReport();
    }
    return "Debugger not initialized";
}

} // namespace nlm
