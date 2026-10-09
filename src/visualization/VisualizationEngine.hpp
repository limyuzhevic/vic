// Core visualization engine header - main interface for NLM visualization and debugging
#pragma once

#include "VisualizationInterface.hpp"
#include <memory>
#include <string>
#include <vector>
#include <map>
#include <thread>
#include <mutex>
#include <atomic>
#include <chrono>
#include <unordered_map>

namespace nlm {

class NetworkVisualizer;
class ActivityAnalyzer;
class BrainStateDebugger;
class PerformanceProfiler;
class MemoryAnalyzer;
class DevelopmentInterface;
class ExportManager;
class PluginManager;

struct VisualizationData {
    SimulationStep step;
    Timestamp timestamp;
    std::vector<float> neuronActivity;  // Firing rates for all neurons
    std::vector<float> membranePotentials;  // Membrane potential for all neurons
    std::vector<int> spikeCounts;  // Number of spikes per neuron this step
    std::vector<std::vector<float>> synapticWeights;  // Weight matrix
    std::vector<float> neuromodulatorLevels;  // Neuromodulator concentrations
    std::vector<int> memoryUsage;  // Memory system usage
    std::vector<int> developmentStage;  // Development stage per neuron
    std::vector<int> regionIds;  // Region ID for each neuron
};

struct ActivityStats {
    SimulationStep step;
    Timestamp timestamp;
    float meanFiringRate;
    float stdFiringRate;
    float maxFiringRate;
    float minFiringRate;
    float synchronyLevel;
    float coherenceLevel;
    std::vector<float> firingRateDistribution;
    std::vector<float> crossCorrelationMatrix;
    std::map<std::string, float> regionalStats;
};

struct BrainSnapshot {
    SimulationStep step;
    Timestamp timestamp;
    std::vector<float> neuronMembranePotentials;
    std::vector<float> neuronFiringRates;
    std::vector<float> synapticWeights;
    std::vector<float> neuromodulatorLevels;
    std::vector<int> activeNeurons;
    std::vector<int> regionActivations;
    std::vector<int> memoryStates;
    std::vector<int> developmentStages;
    float globalExcitatoryInhibitionRatio;
    size_t totalSpikeCount;
    size_t activeNeuronCount;
    double simulationTime;
    std::string metadata;
};

struct PerformanceMetrics {
    SimulationStep step;
    Timestamp timestamp;
    double simulationTime;
    double updateTime;
    double renderTime;
    double analysisTime;
    size_t memoryUsage;
    size_t peakMemoryUsage;
    std::vector<double> componentTimes;
    std::vector<size_t> threadUsage;
    std::map<std::string, double> resourceUtilization;
};

struct MemoryUsage {
    SimulationStep step;
    Timestamp timestamp;
    size_t workingMemoryUsage;
    size_t episodicMemoryUsage;
    size_t associativeMemoryUsage;
    size_t totalMemoryUsage;
    bool hasLeak;
    std::vector<std::string> leakSources;
    double memoryGrowthRate;
};

class VisualizationEngine {
public:
    static VisualizationEngine& getInstance() {
        static VisualizationEngine instance;
        return instance;
    }

    bool initialize(std::shared_ptr<Brain> brain);
    void shutdown();
    
    // Core visualization methods
    void update(const VisualizationData& data);
    void render();
    void startVisualization();
    void stopVisualization();
    
    // Component access
    NetworkVisualizer& getNetworkVisualizer();
    ActivityAnalyzer& getActivityAnalyzer();
    BrainStateDebugger& getBrainStateDebugger();
    PerformanceProfiler& getPerformanceProfiler();
    MemoryAnalyzer& getMemoryAnalyzer();
    DevelopmentInterface& getDevelopmentInterface();
    ExportManager& getExportManager();
    PluginManager& getPluginManager();
    
    // Configuration
    void setUpdateRate(double hz);
    double getUpdateRate() const;
    bool isActive() const;
    
    // Data management
    void setBrain(std::shared_ptr<Brain> brain);
    std::shared_ptr<Brain> getBrain() const;
    
    // Thread management
    void startBackgroundThreads();
    void stopBackgroundThreads();
    bool isBackgroundThreadsActive() const;

private:
    VisualizationEngine();
    ~VisualizationEngine();
    VisualizationEngine(const VisualizationEngine&) = delete;
    VisualizationEngine& operator=(const VisualizationEngine&) = delete;
    
    // Private members
    std::shared_ptr<Brain> brain_;
    std::unique_ptr<NetworkVisualizer> networkVisualizer_;
    std::unique_ptr<ActivityAnalyzer> activityAnalyzer_;
    std::unique_ptr<BrainStateDebugger> brainStateDebugger_;
    std::unique_ptr<PerformanceProfiler> performanceProfiler_;
    std::unique_ptr<MemoryAnalyzer> memoryAnalyzer_;
    std::unique_ptr<DevelopmentInterface> developmentInterface_;
    std::unique_ptr<ExportManager> exportManager_;
    std::unique_ptr<PluginManager> pluginManager_;
    
    double updateRate_;
    std::atomic<bool> active_;
    std::thread updateThread_;
    std::thread renderThread_;
    mutable std::mutex mutex_;
    VisualizationData currentData_;
};
