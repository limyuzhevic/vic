#pragma once

#include "core/Config/Config.hpp"
#include "brain/Brain.hpp"
#include <iostream>
#include <string>
#include <vector>
#include <map>
#include <unordered_set>
#include <algorithm>
#include <cmath>
#include <chrono>
#include <iomanip>

namespace nlm {

class DebugTools {
public:
    DebugTools() {}
    ~DebugTools() {}
    
    // Disable copying
    DebugTools(const DebugTools&) = delete;
    DebugTools& operator=(const DebugTools&) = delete;
    
    // Brain state analysis
    void analyzeBrainState(std::shared_ptr<Brain> brain) const;
    void analyzeNeurons(std::shared_ptr<Brain> brain) const;
    void analyzeSynapses(std::shared_ptr<Brain> brain) const;
    void analyzeConnectivity(std::shared_ptr<Brain> brain) const;
    
    // Plasticity analysis
    void analyzePlasticity(std::shared_ptr<Brain> brain) const;
    void analyzeSTDP(std::shared_ptr<Brain> brain) const;
    void analyzeHebbian(std::shared_ptr<Brain> brain) const;
    
    // Memory analysis
    void analyzeMemory(std::shared_ptr<Brain> brain) const;
    void analyzeWorkingMemory(std::shared_ptr<Brain> brain) const;
    void analyzeEpisodicMemory(std::shared_ptr<Brain> brain) const;
    
    // Neuromodulation analysis
    void analyzeNeuromodulation(std::shared_ptr<Brain> brain) const;
    void analyzeDopamine(std::shared_ptr<Brain> brain) const;
    void analyzeCuriosity(std::shared_ptr<Brain> brain) const;
    
    // Performance profiling
    void profilePerformance(std::shared_ptr<Brain> brain, size_t steps) const;
    void profileMemoryUsage(std::shared_ptr<Brain> brain) const;
    void profileNetworkDynamics(std::shared_ptr<Brain> brain, size_t steps) const;
    
    // Development analysis
    void analyzeDevelopment(std::shared_ptr<Brain> brain) const;
    void analyzeMaturation(std::shared_ptr<Brain> brain) const;
    void analyzePruning(std::shared_ptr<Brain> brain) const;
    
    // Visualization and output
    void printBrainTopology(std::shared_ptr<Brain> brain) const;
    void printNetworkStatistics(std::shared_ptr<Brain> brain) const;
    void printPlasticityStatistics(std::shared_ptr<Brain> brain) const;
    void printMemoryStatistics(std::shared_ptr<Brain> brain) const;
    void printNeuromodulationStatistics(std::shared_ptr<Brain> brain) const;
    
    // Advanced experimental commands
    void runDebugExperiment(std::shared_ptr<Brain> brain, const std::string& experimentName, 
                           size_t steps, float parameter) const;
    void runConnectivityExperiment(std::shared_ptr<Brain> brain, size_t numConnections) const;
    void runPlasticityExperiment(std::shared_ptr<Brain> brain, float learningRate, 
                                size_t trials) const;
    void runMemoryExperiment(std::shared_ptr<Brain> brain, size_t episodeCount) const;
    
    // Configuration tools
    void printConfiguration(std::shared_ptr<Config> config) const;
    void validateConfiguration(std::shared_ptr<Config> config) const;
    void compareConfigurations(std::shared_ptr<Config> config1, std::shared_ptr<Config> config2) const;
    void exportConfiguration(std::shared_ptr<Config> config, const std::string& filename) const;
    
    // Session management
    void saveBrainSession(std::shared_ptr<Brain> brain, const std::string& filename) const;
    void loadBrainSession(std::shared_ptr<Brain> brain, const std::string& filename) const;
    void snapshotBrainState(std::shared_ptr<Brain> brain, const std::string& tag) const;
    void restoreBrainState(std::shared_ptr<Brain> brain, const std::string& tag) const;
    
    // Advanced debugging
    void enableDebugLogging(std::shared_ptr<Brain> brain, bool enable) const;
    void setDebugLevel(std::shared_ptr<Brain> brain, int level) const;
    void enablePerformanceCounters(std::shared_ptr<Brain> brain, bool enable) const;
    void captureNetworkActivity(std::shared_ptr<Brain> brain, size_t duration) const;
    
    // Utility functions
    static std::string formatTime(double seconds) const;
    static std::string formatBytes(size_t bytes) const;
    static std::string formatPercentage(float value) const;
    static std::string formatRate(float value) const;
    
private:
    struct DebugData {
        std::chrono::steady_clock::time_point startTime;
        std::chrono::steady_clock::time_point endTime;
        uint64_t stepCount;
        uint64_t totalSpikes;
        uint64_t peakSpikesPerStep;
        float averageFiringRate;
        size_t memoryUsage;
        size_t peakMemoryUsage;
        std::vector<float> firingHistory;
        std::map<NeuronId, float> neuronActivity;
        std::map<SynapseId, float> synapseWeights;
        
        DebugData() : stepCount(0), totalSpikes(0), peakSpikesPerStep(0), 
                     averageFiringRate(0.0f), memoryUsage(0), peakMemoryUsage(0) {}
    };
    
    mutable DebugData currentDebugData;
    mutable std::unordered_map<std::string, std::vector<std::string>> snapshotStore;
};

} // namespace nlm
