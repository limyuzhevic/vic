#pragma once

// BrainCore.hpp - Core brain structure, initialization, and region management
// Contains basic brain structure and region management functionality

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations for integrated systems
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
class PredictionError;
class Novelty;

// Phase 2: Real neural computation components
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;

// Checkpoints
class CheckpointManager;

// Developmental stage
enum class DevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult
};

// Brain Core Implementation
class BrainCore {
public:
    struct Impl;
    Impl* pImpl;
    
    // Create brain with configuration
    explicit BrainCore(std::shared_ptr<Config> config);
    
    ~BrainCore();
    
    // Disable copying, enable moving
    BrainCore(const BrainCore&) = delete;
    BrainCore& operator=(const BrainCore&) = delete;
    BrainCore(BrainCore&&) noexcept;
    BrainCore& operator=(BrainCore&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
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
    RegionId addRegion(const std::string& name);
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
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory() { 
        return pImpl->workingMemory.get(); 
    }
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory() { 
        return pImpl->episodicMemory.get(); 
    }
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory() { 
        return pImpl->associativeMemory.get(); 
    }
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem() { 
        return pImpl->predictionSystem.get(); 
    }
    
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
    
    // Internal method for region creation (called by initialize)
    void createRegionsAndNeurons(size_t neuronCount, size_t regionCount, float connectionProbability);
    
    // Internal method for integrated system initialization
    void initializeIntegratedSystems();
    
    // Internal method for spike handlers registration
    void registerSpikeHandlers();
    
    // Internal method for checkpoint manager configuration
    void configureCheckpointManager(const std::string& checkpointDir);
    
    // Internal method for memory system initialization
    void initializeMemorySystems();
    
    // Internal method for cognition system initialization
    void initializeCognitionSystems();
    
    // Internal method for neuromodulation system initialization
    void initializeNeuromodulationSystems();
    
    // Internal method for plasticity configuration
    void configurePlasticitySystems();
    
    // Internal method for simulation parameters
    void getSimulationParameters();
    
    // Internal method for inter-region connectivity
    void initializeInterRegionConnectivity(size_t regionCount);
    
    // ========== INTERNAL DATA STRUCTURES ==========
    
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
    
    // Internal implementation details (Pimpl idiom)
    struct Impl {
        std::shared_ptr<Config> config;
        std::unique_ptr<RandomGenerator> rng;
        std::vector<std::unique_ptr<NeuralRegion>> regions;
        std::vector<InterRegionConnection> interRegionConnections;
        
        // ========== INTEGRATED MEMORY SYSTEMS ==========
        std::unique_ptr<NeuralWorkingMemory> workingMemory;
        std::unique_ptr<NeuralEpisodicMemory> episodicMemory;
        std::unique_ptr<NeuralAssociativeMemory> associativeMemory;
        
        // ========== INTEGRATED PREDICTION SYSTEM ==========
        std::unique_ptr<PredictionSystem> predictionSystem;
        
        // ========== INTEGRATED COGNITION SYSTEMS ==========
        std::unique_ptr<NeuralPlanner> planner;
        std::unique_ptr<ConceptFormation> conceptFormation;
        std::unique_ptr<AttentionalSelection> attention;
        
        // ========== DEVELOPMENT SYSTEM ==========
        std::unique_ptr<DevelopmentSystem> developmentSystem;
        
        // ========== NEUROMODULATION SYSTEMS ==========
        std::unique_ptr<Dopamine> dopamine;
        std::unique_ptr<Curiosity> curiosity;
        std::unique_ptr<PredictionError> predictionError;
        std::unique_ptr<Novelty> novelty;
        
        // Phase 2: Real neural computation components
        std::unique_ptr<SpikeSystem> spikeSystem;
        std::unique_ptr<STDP> stdp;
        std::unique_ptr<Hebbian> hebbian;
        std::unique_ptr<StructuralPlasticity> structuralPlasticity;
        
        // Simulation parameters
        TimestepDuration timestep;
        SimulationStep currentStep;
        Timestamp currentTime;
        
        // Statistics
        size_t totalSpikesThisStep;
        size_t totalSpikesTotal;
        
        // Sensory neurons for input injection
        std::vector<Neuron*> sensoryNeurons;
        std::vector<Neuron*> motorNeurons;
        
        // Integration state
        bool isResting;  // For sleep/rest cycle
        size_t stepsSinceLastEpisode;
        size_t replayInterval;
        size_t consolidationInterval;
        
        // Checkpoint system
        std::unique_ptr<CheckpointManager> checkpointManager;
        
        // Storage for previous sensory activity
        std::vector<float> previousSensoryActivity;
        
        Impl(std::shared_ptr<Config> cfg);
        
        DevelopmentalStage developmentalStage;
        RegionId nextRegionId;
    };
    
private:
    // Helper methods for step integration
    void processPendingDelayedSpikes(SimulationStep currentStep, Timestamp currentTime);
    void updateNeuronsLIF(Timestamp currentTime);
    void detectSpikesAndScheduleEvents(SimulationStep currentStep, Timestamp currentTime);
    void processImmediateSpikes(SimulationStep currentStep);
    void updateWorkingMemory(TimestepDuration dt);
    void applyNeuromodulationEffects();
    void applyPlasticityRules(float plasticityMod);
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime);
    void updatePredictionSystem(SimulationStep currentStep, Timestamp currentTime);
    void updateAttentionSystem();
    void updateConceptFormation();
    void applyStructuralPlasticity(SimulationStep currentStep);
    void replayImportantMemories(SimulationStep currentStep);
    void applyDevelopmentEffects(SimulationStep currentStep);
    void consolidateMemory(SimulationStep currentStep);
    void updateCheckpointManager(SimulationStep currentStep, Timestamp currentTime);
    
    // Utility methods
    void collectStatistics() const;
    void logStepDetails() const;
};

} // namespace nlm

