#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
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
    
    // ========== ADVANCED STATISTICAL ANALYSIS ==========
    
    // Get comprehensive neural activity statistics
    std::string getNeuralStatistics() const;
    
    // Get statistics about synaptic connections
    std::string getSynapseStatistics() const;
    
    // Get correlation between neural activities
    float getActivityCorrelation(size_t neuronId1, size_t neuronId2) const;
    
    // Get the brain network topology information
    std::string getNetworkTopology() const;
    
    // ========== PERFORMANCE MONITORING ==========
    
    // Get performance metrics for the brain simulation
    struct PerformanceMetrics {
        double simulationSpeed;        // steps per second
        double memoryUsageMB;          // memory usage in MB
        double cpuUsagePercent;        // CPU usage percentage
        size_t activeNeurons;         // number of active neurons
        size_t totalSpikes;            // total spike count
        double avgFiringRate;          // average firing rate
        size_t connectionsPerStep;     // connections processed per step
    };
    PerformanceMetrics getPerformanceMetrics() const;
    
    // Get computational load information
    struct ComputationalLoad {
        double neuralProcessingTime;   // time spent in neural processing
        double plasticityTime;        // time spent in plasticity
        double memoryAccessTime;       // time spent in memory access
        double totalStepTime;          // total time per step
    };
    ComputationalLoad getComputationalLoad() const;
    
    // Get memory usage statistics
    size_t getMemoryUsage() const;
    
    // ========== ADVANCED CONFIGURATION ==========
    
    // Configure brain for specific network size
    void configureForNetworkSize(size_t neuronCount, size_t regionCount);
    
    // Configure brain for learning scenarios
    void configureForLearning(float learningRate, bool plasticityEnabled);
    
    // Configure brain for developmental dynamics
    void configureForDevelopment(float developmentSpeed, bool ageBasedPlasticity);
    
    // Get detailed configuration report with recommendations
    std::string getConfigReport() const;
    
    // ========== ADVANCED BRAIN MANIPULATION ==========
    
    // Activate a specific brain region with given intensity
    void activateRegion(RegionId regionId, float intensity = 1.0f);
    
    // Deactivate a specific brain region
    void deactivateRegion(RegionId regionId);
    
    // Scale all synaptic connections by factor
    void scaleAllConnections(float scaleFactor);
    
    // Apply modulation pattern to the network
    void applyNetworkModulation(const std::vector<float>& modulationPattern);
    
    // ========== DEBUGGING AND ANALYSIS TOOLS ==========
    
    // Dump current network state for debugging
    std::string dumpNetworkState() const;
    
    // Analyze network health and identify issues
    std::string analyzeNetworkHealth() const;
    
    // Generate comprehensive debug report
    std::string generateDebugReport() const;
    
    // Find neurons with potential issues
    std::vector<NeuronId> findProblematicNeurons() const;
    
    // Identify synaptic connections that need strengthening
    std::vector<std::pair<SynapseId, float>> identifyWeakConnections() const;
    
    // ========== ADVANCED PLANNING AND COGNITION ==========
    
    // Optimize action selection based on current state
    void optimizeActionSelection();
    
    // Generate alternative action plans
    std::vector<std::vector<ActionType>> getAlternativePlans() const;
    
    // Evaluate quality of an action plan
    float evaluatePlanQuality(const std::vector<ActionType>& plan) const;
    
    // Generate insights from learning history
    std::string generateLearningInsights() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
