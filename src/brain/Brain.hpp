#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "NeuralRegion.hpp"
#include "SplitSystems.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>
#include <unordered_map>
#include <functional>

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
    const STDP* getSTDP() const;
    Hebbian* getHebbian();
    const Hebbian* getHebbian() const;
    StructuralPlasticity* getStructuralPlasticity();
    const StructuralPlasticity* getStructuralPlasticity() const;
    
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

    // Learned preference trait (Phase 5): a single scalar, -1..1,
    // updated only by sandbox reward through AgentBrain, saved in the
    // checkpoint (SimulationState section) and used as a small bias in
    // later action choice. This is the measured "personality" step: a
    // preference learned from experience that survives a reload and
    // changes later behaviour. It is not a language-model persona and
    // it does not, on its own, make the brain a human brain.
    float getLearnedPreference() const;
    void setLearnedPreference(float preference);
    
    // Logging
    void logStatus() const;
    
    // ===============================================
    // ADVANCED API FOR POWER USERS
    // ===============================================
    
    // ========== MEMORY SYSTEMS (CONST versions) ==========
    
    // Working memory - transient active information
    const NeuralWorkingMemory* getWorkingMemory() const;
    
    // Episodic memory - experience storage
    const NeuralEpisodicMemory* getEpisodicMemory() const;
    
    // Associative memory - pattern associations
    const NeuralAssociativeMemory* getAssociativeMemory() const;
    
    // ========== COGNITION SYSTEMS (CONST versions) ==========
    
    // Neural planner for action planning
    const NeuralPlanner* getPlanner() const;
    
    // Concept formation for pattern discovery
    const ConceptFormation* getConceptFormation() const;
    
    // Attentional selection for focus
    const AttentionalSelection* getAttention() const;
    
    // ========== NEUROMODULATION SYSTEMS (CONST versions) ==========
    
    // Dopamine - reward and reinforcement
    const Dopamine* getDopamine() const;
    
    // Curiosity - exploration motivation
    const Curiosity* getCuriosity() const;
    
    // Novelty - novelty detection
    const Novelty* getNovelty() const;
    
    // Prediction error signal
    const PredictionError* getPredictionErrorSignal() const;
    
    // ========== ADVANCED SYSTEM INSPECTION ==========
    
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Get comprehensive brain statistics
    struct BrainStats {
        size_t totalNeurons;
        size_t totalSynapses;
        size_t activeNeurons;
        size_t firingNeurons;
        size_t totalSpikeCount;
        float averageFiringRate;
        float excitationInhibitionRatio;
        float learnedPreference;
        DevelopmentalStage developmentalStage;
        size_t regionCount;
        std::string getSummary() const;
    };
    
    BrainStats getBrainStats() const;
    
    // Get system status and health
    bool isHealthy() const;
    std::string getSystemStatus() const;
    
    // Get simulation core statistics
    SimulationStep getCurrentStep() const;
    Timestamp getCurrentTime() const;
    TimestepDuration getTimestep() const;
    bool isResting() const;
    size_t getStepsSinceLastEpisode() const;
    
    // ========== ADVANCED CONTROL AND CONFIGURATION ==========
    
    // Set simulation parameters
    void setTimestep(TimestepDuration timestep);
    void setReplayInterval(size_t interval);
    void setConsolidationInterval(size_t interval);
    
    // Get current simulation configuration
    TimestepDuration getConfiguredTimestep() const;
    size_t getConfiguredReplayInterval() const;
    size_t getConfiguredConsolidationInterval() const;
    
    // ========== ADVANCED NEURAL OPERATIONS ==========
    
    // Get detailed neuron information
    struct NeuronInfo {
        NeuronId id;
        NeuronType type;
        MembranePotential membranePotential;
        FiringState firingState;
        float activation;
        std::vector<SynapseId> incomingSynapses;
        std::vector<SynapseId> outgoingSynapses;
    };
    
    std::vector<NeuronInfo> getAllNeuronInfo() const;
    NeuronInfo getNeuronInfo(NeuronId id) const;
    std::vector<NeuronInfo> getNeuronsByType(NeuronType type) const;
    
    // Get detailed synapse information
    struct SynapseInfo {
        SynapseId id;
        NeuronId sourceNeuron;
        NeuronId targetNeuron;
        float weight;
        SynapseType synapseType;
        PlasticityFlags plasticityFlags;
        float effectiveConductance;
    };
    
    std::vector<SynapseInfo> getAllSynapseInfo() const;
    SynapseInfo getSynapseInfo(SynapseId id) const;
    
    // ========== ADVANCED PLASTICITY CONTROL ==========
    
    // Configure plasticity parameters with validation
    bool configureSTDP(float ltpWeight, float ltdWeight, float tau);
    bool configureStructuralPlasticity(float synaptogenesisRate, float pruningRate);
    
    // Get current plasticity parameters
    struct PlasticityParameters {
        float stdpLtpWeight;
        float stdpLtdWeight;
        float stdpTau;
        float synaptogenesisRate;
        float pruningRate;
    };
    
    PlasticityParameters getPlasticityParameters() const;
    
    // ========== ADVANCED MEMORY OPERATIONS ==========
    
    // Force memory consolidation
    void consolidateMemories();
    
    // Get memory system statistics
    struct MemoryStats {
        size_t workingMemoryCapacity;
        size_t episodicMemoryCapacity;
        size_t associativeMemoryCapacity;
        size_t activeWorkingMemoryItems;
        size_t totalEpisodicMemoryEpisodes;
        float memoryDecayRate;
    };
    
    MemoryStats getMemoryStats() const;
    
    // ========== ADVANCED NEUROMODULATION CONTROL ==========
    
    // Set neuromodulation levels
    void setDopamineLevel(float level);
    void setCuriosityLevel(float level);
    void setNoveltyLevel(float level);
    
    // Get current neuromodulation levels
    float getDopamineLevel() const;
    float getCuriosityLevel() const;
    float getNoveltyLevel() const;
    float getPredictionErrorLevel() const;
    
    // ========== ADVANCED EVENT CALLBACKS ==========
    
    // Register event callbacks for advanced monitoring
    using SpikeCallback = std::function<void(const NeuronId&, const SynapseId&, float)>
    using MemoryCallback = std::function<void(const MemoryEvent&)>
    using NeuromodulationCallback = std::function<void(const Neuromodulator&)>
    
    void registerSpikeCallback(const SpikeCallback& callback);
    void registerMemoryCallback(const MemoryCallback& callback);
    void registerNeuromodulationCallback(const NeuromodulationCallback& callback);
    
    // ========== ADVANCED CHECKPOINT AND REPLAY ==========
    
    // Advanced checkpoint operations
    bool saveCheckpoint(const std::string& filepath, bool includeFullState = true) const;
    bool loadCheckpoint(const std::string& filepath);
    
    // Get checkpoint status
    bool isCheckpointingEnabled() const;
    std::string getCheckpointStatus() const;
    
    // ========== DEVELOPMENT SYSTEM ADVANCED OPERATIONS ==========
    
    // Advance development to next stage
    bool advanceDevelopmentStage();
    
    // Get development system status
    struct DevelopmentStatus {
        DevelopmentalStage currentStage;
        size_t criticalPeriodRemaining;
        float plasticityFactor;
        bool isMaturationComplete;
        std::string getStatusDescription() const;
    };
    
    DevelopmentStatus getDevelopmentStatus() const;
    
    // ========== PREDICTION SYSTEM ADVANCED OPERATIONS ==========
    
    // Get prediction system statistics
    struct PredictionStats {
        float predictionAccuracy;
        float predictionError;
        size_t predictionMemorySize;
        float predictionConfidence;
    };
    
    PredictionStats getPredictionStats() const;
    
    // ========== VISUALIZATION AND DEBUGGING ==========
    
    // Generate visualization data
    std::vector<float> getVisualizationData() const;
    
    // Debug helper methods
    std::string debugInfo() const;
    void enableDebugLogging(bool enable);
    bool isDebugLoggingEnabled() const;
    
private:
    struct Impl;
    Impl* pImpl;
};
