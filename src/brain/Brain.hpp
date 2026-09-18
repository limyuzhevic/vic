#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../prediction/NeuralPrediction.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"

namespace nlm {

// Event type for triggering memory replay
enum class EventType {
    None,
    Reward,
    Punishment,
    Novelty,
    GoalAchieved,
    ThreatDetected,
    SocialInteraction,
    Exploration,
    Pain,
    Pleasure
};

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

    // ========== SLEEP/REST CYCLE ==========

    // Enter sleep mode with gradual transition
    void enterSleepMode();

    // Exit sleep mode with gradual transition
    void exitSleepMode();

    // Consolidate memories during sleep (systematic strengthening/weakening)
    void consolidateMemoriesDuringSleep();

    // Replay memories during sleep (replay selected memories)
    void replayMemoriesDuringSleep();

    // Update sleep phases (NREM, REM, transitions)
    void updateSleepPhases();

    // Check if brain is in sleep mode
    bool isInSleepMode() const;
    
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
    
// ========== COGNITION SYSTEM INTEGRATION ==========

    // Integrate cognition systems with memory and other brain systems
    void integrateCognitionWithMemory(const NeuralWorkingMemory& wm, const NeuralEpisodicMemory& em);
    void updateCognitionFromSensory(const SensoryInput& input);
    void applyCognitionToActionSelection();
    void updateConceptFormationFromEpisodic(const NeuralEpisodicMemory& em);
    void updatePlanningFromRewards(float reward);
    void applyAttentionToMemorySelection();
    
    // ========== NEUROMODULATION SYSTEM INTEGRATION ==========

    // Integrate neuromodulation with memory systems
    void integrateNeuromodulationWithMemory(NeuralWorkingMemory& wm, NeuralEpisodicMemory& em);
    void updateNeuromodulationFromSensory(const SensoryInput& input);
    void applyNeuromodulationToPlasticity();
    void updateNeuromodulationFromPrediction(float error);
    void updateNeuromodulationFromRewards(float reward);
    
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
    
    // Logging
    void logStatus() const;

    // ========== SLEEP/REST CYCLE ==========

    // Enter sleep mode with gradual transition
    void enterSleepMode();

    // Exit sleep mode with gradual transition
    void exitSleepMode();

    // Consolidate memories during sleep (systematic strengthening/weakening)
    void consolidateMemoriesDuringSleep();

    // Replay memories during sleep (replay selected memories)
    void replayMemoriesDuringSleep();

    // Update sleep phases (NREM, REM, transitions)
    void updateSleepPhases();

    // Check if brain is in sleep mode
    bool isInSleepMode() const;

    // Get current sleep phase
    std::string getSleepPhase() const;
    
    // Get sleep duration remaining
    SimulationStep getSleepDurationRemaining() const;
    
    // Get sleep consolidation effectiveness
    float getSleepConsolidationRate() const;

private:
    // Sleep/rest cycle state
    bool isResting;  // For sleep/rest cycle
    bool isSleeping; // True when in sleep mode
    size_t stepsSinceLastEpisode;
    size_t replayInterval;
    size_t consolidationInterval;
    
    // Sleep phase tracking
    enum SleepPhase {
        Awake,
        NREM1,
        NREM2,
        NREM3,
        REM,
        TransitionAwakeToSleep,
        TransitionSleepToAwake
    } currentSleepPhase;
    
    // Sleep timing
    SimulationStep sleepStartStep;
    SimulationStep sleepDuration;
    SimulationStep sleepProbability;
    
    // Sleep consolidation parameters
    float memoryConsolidationRate;
    float replayStrength;
    float pruningThreshold;

public:
    // Integrate sensory input to working memory
    void integrateSensoryToMemory(const SensoryInput& input);
    
    // Update memory from attention
    void updateMemoryFromAttention();
    
    // Trigger memory replay based on events
    void triggerMemoryReplay(EventType trigger);
    
    // Consolidate memories
    void consolidateMemories();
    
    // Update all memories from brain state
    void updateAllMemoriesFromBrainState();
    
    // Integrate memories with prediction
    void integrateMemoriesWithPrediction(const PredictionSystem& prediction);
    
    // ========== PREDICTION SYSTEM INTEGRATION ==========
    
    // Integrate prediction with sensory processing
    void integratePredictionWithSensory(const SensoryInput& input, const PredictionSystem& prediction);
    
    // Update prediction from working memory content
    void updatePredictionFromWorkingMemory(const NeuralWorkingMemory& wm);
    
    // Update prediction from episodic memory recall
    void updatePredictionFromEpisodicMemory(const NeuralEpisodicMemory& em);
    
    // Use prediction to influence action selection
    void applyPredictionToActionSelection();
    
    // Update prediction error signals
    void updatePredictionError(float error);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
