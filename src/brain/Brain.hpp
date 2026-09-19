#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/NeuralAssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <string>

namespace nlm {

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
    
    // ========== ADVANCED CHECKPOINT MANAGEMENT ==========
    
    // Save checkpoint with custom name and compression level
    bool saveCheckpoint(const std::string& name, CompressionLevel compression = CompressionLevel::Balanced);
    
    // Load checkpoint by name or step
    bool loadCheckpoint(const std::string& name = "");
    
    // Load checkpoint by simulation step
    bool loadCheckpointAtStep(uint64_t step);
    
    // Delete checkpoint by name
    bool deleteCheckpoint(const std::string& name);
    
    // List all available checkpoints
    std::vector<std::string> listCheckpoints() const;
    
    // Get checkpoint manager status
    bool isCheckpointManagerReady() const;
    
    // Set checkpoint interval
    void setCheckpointInterval(uint64_t interval);
    
    // ================ BRAIN INTROSPECTION ================ 
    
    // Get detailed connectivity matrix
    std::vector<std::vector<float>> getConnectivityMatrix() const;
    
    // Analyze brain region connectivity
    struct RegionConnectivity {
        RegionId regionId;
        size_t outgoingConnections;
        size_t incomingConnections;
        float totalWeight;
        float averageWeight;
    };
    std::vector<RegionConnectivity> analyzeRegionConnectivity() const;
    
    // Get brain statistics
    struct BrainStatistics {
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        float averageFiringRate;
        float excitationInhibitionRatio;
        size_t totalSpikeCount;
        size_t pendingSpikeEvents;
        double simulationTime;
        SimulationStep currentStep;
    };
    BrainStatistics getBrainStatistics() const;
    
    // Get synaptic statistics
    struct SynapticStatistics {
        float meanWeight;
        float stdWeight;
        float minWeight;
        float maxWeight;
        float meanDelay;
        size_t totalPlasticityEnabled;
        size_t totalExcitatory;
        size_t totalInhibitory;
    };
    SynapticStatistics getSynapticStatistics() const;
    
    // ================ DEVELOPMENT CONTROL ============
    
    // Manually set developmental stage
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Get developmental progress percentage
    float getDevelopmentalProgress() const;
    
    // Get remaining time in current developmental stage
    double getRemainingTimeInStage() const;
    
    // Apply custom development effects
    void applyDevelopmentEffect(const std::string& effectType, float intensity);
    
    // ================ NEUROMODULATION CONTROL ============
    
    // Set neuromodulator levels directly
    void setDopamineLevel(float level);
    void setCuriosityLevel(float level);
    void setNoveltyLevel(float level);
    void setPredictionErrorLevel(float level);
    
    // Get current neuromodulator levels
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionErrorLevel() const;
    
    // Apply custom neuromodulation
    void applyCustomNeuromodulation(float dopamine, float curiosity, float novelty, float predictionError);
    
    // ================ PLASTICITY RULE CONFIGURATION =========
    
    // Configure STDP parameters
    void configureSTDP(float ltpWeight, float ltdWeight, float tau);
    
    // Configure Hebbian parameters
    void configureHebbian(float learningRate, float decayRate);
    
    // Configure structural plasticity parameters
    void configureStructuralPlasticity(float synaptogenesisRate, float pruningRate);
    
    // Get current STDP parameters
    struct STDPParameters {
        float ltpWeight;
        float ltdWeight;
        float tau;
    };
    STDPParameters getSTDPParameters() const;
    
    // ================ MEMORY SYSTEM ANALYSIS =========
    
    // Get working memory statistics
    struct WorkingMemoryStats {
        size_t neuronCount;
        size_t activeTraces;
        float decayRate;
        size_t capacity;
        float averageActivation;
    };
    WorkingMemoryStats analyzeWorkingMemory() const;
    
    // Get episodic memory statistics
    struct EpisodicMemoryStats {
        size_t episodeCount;
        size_t maxEpisodes;
        bool replayEnabled;
        float averageReward;
        size_t maxActiveNeuronsPerEpisode;
    };
    EpisodicMemoryStats analyzeEpisodicMemory() const;
    
    // Get associative memory statistics
    struct AssociativeMemoryStats {
        size_t associationCount;
        size_t maxAssociations;
        float averageStrength;
    };
    AssociativeMemoryStats analyzeAssociativeMemory() const;
    
    // ================ PERFORMANCE PROFILING =========
    
    // Get spike processing performance statistics
    struct SpikeProcessingStats {
        size_t totalSpikesThisStep;
        size_t totalSpikesTotal;
        size_t pendingSpikes;
        size_t pendingDelayedSpikes;
        float spikesPerSecond;
        double lastProcessingTime;
    };
    SpikeProcessingStats getSpikeProcessingStats() const;
    
    // Get plasticity application statistics
    struct PlasticityStats {
        size_t stdpUpdates;
        size_t hebbianUpdates;
        size_t structuralPlasticityUpdates;
        float weightChangeMean;
        float weightChangeStd;
    };
    PlasticityStats getPlasticityStats() const;
    
    // Get memory access statistics
    struct MemoryAccessStats {
        size_t workingMemoryAccesses;
        size_t episodicMemoryAccesses;
        size_t associativeMemoryAccesses;
        size_t memoryReplayEvents;
        double averageMemoryAccessTime;
    };
    MemoryAccessStats getMemoryAccessStats() const;
    
    // ============= EXPERIMENT CONTROL UTILITIES =============
    
    // Pause simulation
    void pause() { pImpl->isResting = true; }
    
    // Resume simulation
    void resume() { pImpl->isResting = false; }
    
    // Check if simulation is paused
    bool isPaused() const { return pImpl->isResting; }
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

} // namespace nlm
