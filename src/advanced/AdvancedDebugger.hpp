#pragma once

#include <string>
#include <vector>
#include <memory>
#include <chrono>
#include <map>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include "../brain/Brain.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/EventTracking/EventTracking.hpp"

namespace nlm {

// Debugging severities
enum class DebugSeverity {
    Info,
    Warning,
    Error,
    Critical,
    Debug
};

// State inspection targets
enum class StateTarget {
    Neurons,
    Synapses,
    Populations,
    Regions,
    Memory,
    Neuromodulation,
    Development,
    Plasticity,
    Simulation,
    All
};

// Neural event categories for filtering
enum class EventFilterCategory {
    All,
    Spiking,
    Plasticity,
    Memory,
    Neuromodulation,
    Development,
    Input,
    Output,
    Performance,
    Error
};

// Plasticity rule analysis targets
enum class PlasticityTarget {
    STDP,
    Hebbian,
    RewardModulated,
    Structural,
    All
};

// Development stage controls
enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Unknown
};

// Debug information structures
struct NeuronDebugInfo {
    NeuronId id;
    std::string type;
    Coordinate position;
    float membranePotential;
    float threshold;
    float firingRate;
    bool isActive;
    std::vector<float> spikeHistory;
    std::map<std::string, float> properties;
};

struct SynapseDebugInfo {
    SynapseId id;
    NeuronId sourceId;
    NeuronId destinationId;
    float weight;
    float delay;
    bool isExcitatory;
    float preSpikeHistory;
    float postSpikeHistory;
    float eligibilityTrace;
    std::map<std::string, float> properties;
};

struct PopulationDebugInfo {
    PopulationId id;
    std::string name;
    NeuronType neuronType;
    size_t neuronCount;
    float averageFiringRate;
    float averageWeight;
    std::vector<NeuronId> neuronIds;
};

struct RegionDebugInfo {
    RegionId id;
    std::string name;
    size_t neuronCount;
    size_t synapseCount;
    float averageSynapticWeight;
    std::vector<PopulationId> populationIds;
    std::map<std::string, float> properties;
};

struct MemoryDebugInfo {
    std::string systemType;  // "working", "episodic", "associative"
    size_t traceCount;
    size_t episodeCount;
    float totalActivity;
    std::map<std::string, float> properties;
};

struct PlasticityDebugInfo {
    std::string ruleType;  // "STDP", "Hebbian", "Reward-modulated"
    float learningRate;
    float weightChangeHistory;
    size_t updateCount;
    float efficacy;
};

struct TimelineDebugInfo {
    SimulationStep step;
    Timestamp time;
    std::vector<TimelineEvent> events;
    size_t eventCount;
    float processingTime;
};

// Advanced debugger for neural simulation debugging and analysis
class AdvancedDebugger {
public:
    AdvancedDebugger(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    ~AdvancedDebugger();
    
    // Core debugging functionality
    void startDebugging();
    void stopDebugging();
    bool isDebugging() const;
    void setDebugLevel(DebugSeverity level);
    DebugSeverity getDebugLevel() const;
    
    // State inspection
    NeuronDebugInfo getNeuronDebugInfo(NeuronId neuronId) const;
    SynapseDebugInfo getSynapseDebugInfo(SynapseId synapseId) const;
    PopulationDebugInfo getPopulationDebugInfo(PopulationId populationId) const;
    RegionDebugInfo getRegionDebugInfo(RegionId regionId) const;
    MemoryDebugInfo getMemoryDebugInfo(const std::string& systemType) const;
    PlasticityDebugInfo getPlasticityDebugInfo(PlasticityTarget target) const;
    
    // Neural event tracing
    void startEventTracing(EventFilterCategory category, EventType eventType);
    void stopEventTracing();
    std::vector<TimelineEvent> getRecentEvents(EventFilterCategory category,
                                              EventType eventType = EventType::Unknown,
                                              SimulationStep maxEvents = 1000) const;
    TimelineDebugInfo getTimelineDebugInfo(SimulationStep startStep = 0,
                                         SimulationStep endStep = static_cast<SimulationStep>(-1)) const;
    
    // Plasticity rule analysis
    void analyzeSTDP(const std::string& synapseFilter = "");
    void analyzeHebbian(const std::string& synapseFilter = "");
    void analyzeRewardModulated(const std::string& synapseFilter = "");
    void analyzeStructuralPlasticity();
    float calculateWeightChangeRatio(const std::string& synapseFilter = "") const;
    float calculatePlasticityEfficacy(PlasticityTarget target) const;
    
    // Development stage controls
    void setDevelopmentalStage(DevelopmentalStage stage);
    DevelopmentalStage getDevelopmentalStage() const;
    void accelerateDevelopment();
    void slowDevelopment();
    std::vector<DevelopmentEvent> getDevelopmentEvents() const;
    
    // Simulation controls
    void setBreakOnSpike(bool enable, NeuronId neuronId = 0);
    void setBreakOnPlasticity(bool enable, PlasticityTarget target = PlasticityTarget::All);
    void setBreakOnMemory(bool enable, const std::string& memoryType = "");
    bool shouldBreak() const;
    void stepDebug();
    void continueDebug();
    void pauseDebug();
    bool isPaused() const;
    
    // Analysis and reporting
    std::string generateNeuronReport(NeuronId neuronId) const;
    std::string generateSynapseReport(SynapseId synapseId) const;
    std::string generateNetworkReport() const;
    std::string generatePlasticityReport() const;
    std::string generateDevelopmentReport() const;
    std::string generateMemoryReport() const;
    std::string generateEventReport(EventFilterCategory category) const;
    std::string generateDebugSummary() const;
    
    // Integration
    void attachToBrain(std::shared_ptr<Brain> brain);
    void detachFromBrain();
    bool isAttached() const;
    
    // Event filtering
    void addEventFilter(std::shared_ptr<EventFilter> filter);
    void removeEventFilter(std::shared_ptr<EventFilter> filter);
    void clearEventFilters();
    
    // Configuration
    void configureDebugOutput(const std::string& outputFile);
    void enableConsoleOutput(bool enable);
    void setMaxDebugEvents(size_t maxEvents);
    void setEventHistoryDuration(double durationSeconds);
    
    // Visualization support
    void enableVisualization(bool enable);
    void setVisualizationScale(float scale);
    void setVisualizationStyle(const std::string& style);
    
    // Cleanup
    void cleanup() noexcept;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
