#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../core/AdvancedMemoryPool.hpp"
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
class SemanticMemory;
class ProceduralMemory;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;
class DevelopmentSystem;
class Dopamine;
class Acetylcholine;
class Norepinephrine;
class Serotonin;
class Curiosity;
class Novelty;
class Reward;
class PredictionError;
class CheckpointSystem;
class PerformanceMonitor;

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
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // Set neuromodulation levels
    void setDopamineLevel(float level);
    void setAcetylcholineLevel(float level);
    void setNorepinephrineLevel(float level);
    void setSerotoninLevel(float level);
    
    // Get neuromodulation levels
    float getDopamineLevel() const;
    float getAcetylcholineLevel() const;
    float getNorepinephrineLevel() const;
    float getSerotoninLevel() const;
    
    // Get neuromodulator objects
    Dopamine* getDopamine();
    Acetylcholine* getAcetylcholine();
    Norepinephrine* getNorepinephrine();
    Serotonin* getSerotonin();
    
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
    
    // Semantic memory - acquired knowledge
    SemanticMemory* getSemanticMemory();
    
    // Procedural memory - learned skills
    ProceduralMemory* getProceduralMemory();
    
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
    
    // Get developmental parameters
    float getMaturationRate() const;
    float getSynaptogenesisRate() const;
    float getPruningThreshold() const;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine();
    
    // Acetylcholine - attention and memory consolidation
    Acetylcholine* getAcetylcholine();
    
    // Norepinephrine - arousal and vigilance
    Norepinephrine* getNorepinephrine();
    
    // Serotonin - mood, impulsivity, and social behavior
    Serotonin* getSerotonin();
    
    // Novelty - novelty detection
    Novelty* getNovelty();
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity();
    
    // Reward - reward signal for reinforcement learning
    Reward* getReward();
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Apply neuromodulation signal
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // Set neuromodulation levels
    void setDopamineLevel(float level);
    void setAcetylcholineLevel(float level);
    void setNorepinephrineLevel(float level);
    void setSerotoninLevel(float level);
    
    // Get neuromodulation levels
    float getDopamineLevel() const;
    float getAcetylcholineLevel() const;
    float getNorepinephrineLevel() const;
    float getSerotoninLevel() const;
    
    // ========== CHECKPOINT MANAGEMENT ==========
    
    // Get checkpoint system
    CheckpointSystem* getCheckpointSystem();
    
    // Create checkpoint
    bool createCheckpoint(const std::string& name, const std::string& description = "");
    
    // Restore from checkpoint
    bool restoreCheckpoint(const std::string& name);
    
    // List checkpoints
    std::vector<std::string> listCheckpoints() const;
    
    // Get checkpoint metadata
    std::string getCheckpointMetadata(const std::string& name) const;
    
    // ========== PERFORMANCE MONITORING ==========
    
    // Get performance monitor
    PerformanceMonitor* getPerformanceMonitor();
    
    // Get brain state statistics
    struct BrainStateStats {
        size_t totalNeurons;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float excitationInhibitionRatio;
        size_t totalSpikes;
        size_t memoryPressure;
        float neuromodulationLoad;
        double simulationTime;
    };
    
    BrainStateStats getBrainStateStats() const;
    
    // Get neural activity patterns
    std::vector<float> getNeuralActivityPattern(size_t regionId = 0) const;
    
    // Get connectivity statistics
    struct ConnectivityStats {
        size_t totalSynapses;
        float averageStrength;
        float maxStrength;
        float minStrength;
        size_t denseConnections;
    };
    
    ConnectivityStats getConnectivityStats() const;
    
    // Get visualization data
    std::string getVisualizationData() const;
    
    // ========== DEVELOPMENT APIs ==========
    
    // Get developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Methods for tracking developmental progress
    float getDevelopmentalProgress() const;
    
    // Get developmental parameters
    float getDevelopmentalParameter(const std::string& parameter) const;
    void setDevelopmentalParameter(const std::string& parameter, float value);
    
    // Methods for accessing developmental stage
    const char* getCurrentStageName() const;
    float getTimeInCurrentStage() const;
    
    // Methods for accessing developmental parameters
    float getCriticalPeriodProgress() const;
    float getMaturationProgress() const;
    float getPlasticityModifier() const;
    
    // Methods for tracking developmental progress
    void advanceStage();
    void completeDevelopment();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
