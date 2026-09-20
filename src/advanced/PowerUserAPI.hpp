#pragma once

#include <string>
#include <vector>
#include <memory>
#include <functional>
#include <map>
#include <unordered_map>
#include <variant>
#include <chrono>
#include <mutex>
#include <thread>
#include <algorithm>
#include <cmath>
#include <iomanip>
#include <sstream>
#include "../brain/Brain.hpp"
#include "../brain/Neuron.hpp"
#include "../brain/Synapse.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/EventTracking/EventTracking.hpp"

namespace nlm {

// Power-user API access types
struct NeuronAccessInfo {
    NeuronId id;
    NeuronType type;
    float membranePotential;
    float threshold;
    float firingRate;
    bool isActive;
    float lastSpikeTime;
    float refractoryTimeRemaining;
    float adaptationVariable;
    size_t synapseCount;
    std::map<std::string, float> customProperties;
};

struct SynapseAccessInfo {
    SynapseId id;
    NeuronId sourceId;
    NeuronId destinationId;
    float weight;
    float delay;
    SynapseType type;
    bool isExcitatory;
    float efficacy;
    float eligibilityTrace;
    float preSpikeHistory;
    float postSpikeHistory;
    size_t preSpikeCount;
    size_t postSpikeCount;
    std::map<std::string, float> customProperties;
};

struct PopulationAccessInfo {
    PopulationId id;
    std::string name;
    NeuronType neuronType;
    size_t neuronCount;
    float averageFiringRate;
    float averageWeight;
    float minWeight;
    float maxWeight;
};

struct RegionAccessInfo {
    RegionId id;
    std::string name;
    size_t neuronCount;
    size_t synapseCount;
    float averageSynapticWeight;
    float averageNeuronFiringRate;
    std::vector<PopulationAccessInfo> populations;
};

struct PlasticityRuleInfo {
    std::string name;
    std::string type;  // "STDP", "Hebbian", "Reward-modulated"
    float learningRate;
    float weightChangeLimit;
    bool enabled;
    std::map<std::string, float> parameters;
    std::chrono::steady_clock::time_point lastUpdated;
};

struct SpikePatternInfo {
    NeuronId neuronId;
    size_t spikeCount;
    float firstSpikeTime;
    float lastSpikeTime;
    float meanInterSpikeInterval;
    float coefficientOfVariation;
    std::vector<float> interSpikeIntervals;
};

struct ConnectivityInfo {
    size_t totalSynapses;
    size_t excitatorySynapses;
    size_t inhibitorySynapses;
    float averageWeight;
    float weightStandardDeviation;
    float connectionProbability;
    std::vector<std::pair<NeuronId, NeuronId>> hubConnections;
};

struct MemoryStateInfo {
    std::string systemType;
    size_t activeTraces;
    size_t totalTraces;
    float utilizationRatio;
    float totalActivity;
    std::map<std::string, float> metrics;
};

struct NeuromodulationStateInfo {
    std::string neuromodulator;
    float level;
    float effectOnExcitability;
    float effectOnPlasticity;
    float lastUpdateTime;
};

struct DevelopmentStateInfo {
    DevelopmentalStage stage;
    float stageProgress;  // 0.0 to 1.0
    float plasticityFactor;
    size_t activeNeurons;
    size_t totalNeurons;
    std::vector<std::string> activeProcesses;
};

struct PerformanceStats {
    double simulationTime;  // ms
    double realTimeElapsed;  // seconds
    size_t stepCount;
    double averageStepTime;  // ms
    size_t totalSpikes;  // All time
    size_t spikesThisStep;
    float averageFiringRate;
    size_t activeNeuronCount;
    float memoryUsageMB;
    float cpuUsagePercent;
    std::chrono::steady_clock::time_point lastUpdate;
};

struct EventSubscription {
    std::string id;
    EventFilterCategory category;
    EventType eventType;
    EventHandler handler;
    bool enabled;
    std::map<std::string, std::string> metadata;
};

// PowerUserAPI: Direct access to internal data structures and low-level control
class PowerUserAPI {
public:
    PowerUserAPI(std::shared_ptr<Brain> brain, std::shared_ptr<Config> config);
    ~PowerUserAPI();
    
    // Direct neuron access
    NeuronAccessInfo getNeuronInfo(NeuronId neuronId) const;
    bool setNeuronProperty(NeuronId neuronId, const std::string& property, float value);
    float getNeuronProperty(NeuronId neuronId, const std::string& property) const;
    std::vector<NeuronId> getAllNeurons(NeuronType type = NeuronType::All) const;
    std::vector<NeuronId> findNeuronsByProperty(const std::string& property, 
                                               float value, float tolerance = 0.01f) const;
    
    // Direct synapse access
    SynapseAccessInfo getSynapseInfo(SynapseId synapseId) const;
    bool setSynapseProperty(SynapseId synapseId, const std::string& property, float value);
    float getSynapseProperty(SynapseId synapseId, const std::string& property) const;
    std::vector<SynapseId> getIncomingSynapses(NeuronId neuronId) const;
    std::vector<SynapseId> getOutgoingSynapses(NeuronId neuronId) const;
    std::vector<SynapseId> findSynapsesByWeight(float minWeight, float maxWeight = std::numeric_limits<float>::max()) const;
    
    // Population and region access
    PopulationAccessInfo getPopulationInfo(PopulationId populationId) const;
    RegionAccessInfo getRegionInfo(RegionId regionId) const;
    std::vector<PopulationAccessInfo> getPopulations(RegionId regionId = 0) const;
    std::vector<RegionAccessInfo> getAllRegions() const;
    
    // Plasticity rule control
    bool setPlasticityRule(PlasticityType type, float weight, float learningRate = 0.01f);
    bool enablePlasticityRule(PlasticityType type, bool enable);
    PlasticityRuleInfo getPlasticityRuleInfo(PlasticityType type) const;
    bool customPlasticityUpdate(const std::string& ruleName, const std::map<std::string, float>& parameters);
    
    // Spike and pattern analysis
    SpikePatternInfo analyzeSpikePattern(NeuronId neuronId, size_t windowSize = 100) const;
    std::vector<SpikePatternInfo> analyzeAllSpikePatterns() const;
    std::vector<NeuronId> findSynchronousNeurons(float synchronyThreshold = 0.8f) const;
    std::vector<std::pair<NeuronId, NeuronId>> findRecurrentConnections() const;
    
    // Connectivity analysis
    ConnectivityInfo analyzeConnectivity() const;
    std::vector<std::pair<NeuronId, NeuronId>> getStrongConnections(float threshold = 0.1f) const;
    std::vector<std::pair<NeuronId, NeuronId>> getWeakConnections(float threshold = 0.01f) const;
    float calculateNetworkEfficiency() const;
    float calculateSmallWorldness() const;
    
    // Memory system access
    MemoryStateInfo getMemoryState(const std::string& systemType = "working") const;
    std::vector<NeuronId> getActiveMemoryTraces(const std::string& systemType = "working") const;
    bool storeToMemory(NeuronId neuronId, float value, const std::string& memoryType = "working");
    bool recallFromMemory(NeuronId neuronId, float& value, const std::string& memoryType = "working");
    
    // Neuromodulation access
    NeuromodulationStateInfo getNeuromodulationState(const std::string& neuromodulator) const;
    std::vector<NeuromodulationStateInfo> getAllNeuromodulationStates() const;
    bool applyNeuromodulation(const std::string& neuromodulator, float level);
    
    // Development system access
    DevelopmentStateInfo getDevelopmentState() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    bool isDevelopmentComplete() const;
    std::vector<std::string> getActiveDevelopmentProcesses() const;
    
    // External event system
    std::string subscribeToEvents(EventFilterCategory category, EventType eventType, EventHandler handler);
    bool unsubscribeFromEvents(const std::string& subscriptionId);
    bool enableEventSubscription(const std::string& subscriptionId, bool enable);
    void processExternalEvent(const TimelineEvent& event);
    
    // Performance monitoring
    PerformanceStats getPerformanceStats() const;
    double getSimulationTime() const;
    size_t getStepCount() const;
    float getAverageStepTime() const;
    float getCPUUsage() const;
    float getMemoryUsageMB() const;
    
    // Low-level controls
    bool injectCurrent(NeuronId neuronId, float current);
    bool injectCurrentToPopulation(PopulationId populationId, float current);
    bool injectCurrentToType(NeuronType type, float current);
    bool setNeuronThreshold(NeuronId neuronId, float threshold);
    bool setNeuronRestingPotential(NeuronId neuronId, float potential);
    bool setNeuronRefractoryPeriod(NeuronId neuronId, uint32_t steps);
    
    // Simulation controls
    void pauseSimulation(bool pause);
    bool isSimulationPaused() const;
    void setTimestep(float timestep);
    float getTimestep() const;
    void resetSimulation();
    bool saveState(const std::string& filepath) const;
    bool loadState(const std::string& filepath);
    
    // Batch operations
    std::vector<NeuronId> batchSetProperty(const std::vector<NeuronId>& neuronIds,
                                           const std::string& property, float value);
    std::vector<SynapseId> batchSetWeight(const std::vector<SynapseId>& synapseIds, float weight);
    std::vector<NeuronId> batchInjectCurrent(const std::vector<NeuronId>& neuronIds, float current);
    
    // Analysis and utilities
    std::string generateNetworkReport() const;
    std::string generatePlasticityReport() const;
    std::string generateDevelopmentReport() const;
    std::string generateMemoryReport() const;
    std::string generateEventReport() const;
    
    // Integration
    void attachToBrain(std::shared_ptr<Brain> brain);
    void attachToConfig(std::shared_ptr<Config> config);
    void attachToEventManager(std::shared_ptr<EventTrackingManager> eventManager);
    bool isAttached() const;
    
    // Configuration
    void enableDebugMode(bool enable);
    void setMaxNeuronsToAnalyze(size_t maxNeurons);
    void setAnalysisWindow(size_t steps);
    void setPerformanceMonitoringInterval(double intervalSeconds);
    
    // Cleanup
    void cleanup() noexcept;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
