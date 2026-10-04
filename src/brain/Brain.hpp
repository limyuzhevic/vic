#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

// Inter-regional connection (long-range connectivity)
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
    // ========== PERFORMANCE OPTIMIZATION ==========
    
    // Batch simulation for improved performance
    void stepBatch(SimulationStep startStep, SimulationStep endStep);
    
    // Parallel processing capabilities
    void stepParallel(SimulationStep startStep, SimulationStep endStep, size_t numThreads = 0);
    
    // Memory-efficient batch processing
    void stepMemoryEfficient(SimulationStep startStep, SimulationStep endStep);
    
    // Performance monitoring and profiling
    struct PerformanceStats {
        SimulationStep stepsProcessed;
        float stepsPerSecond;
        float memoryUsageMB;
        float cpuUsagePercent;
        size_t activeNeurons;
        size_t firingNeurons;
        float totalSpikes;
        double startTime;
        double endTime;
        std::vector<double> stepTimes;
    };
    
    // Get performance statistics
    PerformanceStats getPerformanceStats() const;
    
    // Reset performance statistics
    void resetPerformanceStats();
    
    // Check if performance monitoring is enabled
    bool isPerformanceMonitoring() const;
    
    // Enable/disable performance monitoring
    void setPerformanceMonitoring(bool enabled);
    
    // Get current performance metrics
    double getCurrentFPS() const;
    double getAverageFPS() const;
    
    // ========== ADVANCED BATCH OPERATIONS ==========
    
    // Run multiple simulations with different configurations
    static std::vector<PerformanceStats> runBatchSimulation(
        const std::vector<std::shared_ptr<Config>>& configs,
        size_t stepsPerConfig,
        bool parallel = false);
    
    // Time series analysis of brain activity
    struct ActivityTimeSeries {
        std::vector<SimulationStep> steps;
        std::vector<size_t> firingCounts;
        std::vector<float> averageFiringRates;
        std::vector<float> noveltyLevels;
        std::vector<float> curiosityLevels;
        std::vector<float> energyLevels;
        std::vector<DevelopmentalStage> developmentalStages;
    };
    
    // Collect time series data
    ActivityTimeSeries collectTimeSeriesData(SimulationStep startStep, SimulationStep endStep);
    
    // Export performance data to file
    bool exportPerformanceData(const std::string& filepath) const;
    
    // Import performance data from file
    bool importPerformanceData(const std::string& filepath);
    
    // Compute performance metrics
    struct PerformanceMetrics {
        double throughput;           // steps per second
        double efficiency;            // actual vs theoretical maximum
        double memoryFootprint;      // MB
        double cpuUtilization;       // percentage
        size_t activeNeuronCount;     // average active neurons
        size_t firingNeuronCount;     // average firing neurons
        float plasticityRate;        // average plasticity events per step
        float learningRate;          // average learning rate
    };
    
    // Compute comprehensive performance metrics
    PerformanceMetrics computePerformanceMetrics() const;
    
    // Profile performance hotspots
    std::vector<std::pair<std::string, double>> profileHotspots() const;
    
    // Enable/disable profiling
    void setProfilingEnabled(bool enabled);
    bool isProfilingEnabled() const;
    
    // Get profiling results
    std::string getProfilingResults() const;
    
    // Clear profiling data
    void clearProfilingData();
    
    // ========== EXPERIMENTAL / DEBUGGING ==========
    
    // Run scientific experiment
    bool runScientificExperiment(const std::string& experimentName,
                                 const std::map<std::string, std::string>& parameters);
    
    // Get experimental results
    std::string getExperimentalResults(const std::string& experimentName) const;
    
    // List available experiments
    std::vector<std::string> listAvailableExperiments() const;
    
    // Enable/disable debugging mode
    void setDebugMode(bool enabled);
    bool isDebugMode() const;
    
    // Set debug verbosity level
    void setDebugLevel(int level);
    int getDebugLevel() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
