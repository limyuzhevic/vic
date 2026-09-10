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
class WorkingMemory;
class SemanticMemory;
class ProceduralMemory;
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
class CheckpointManager;

// Inter-regional connection (long-range connectivity)
// Sleep stage enumeration
enum class SleepStage {
    Awake,
    NREM1,      // Light sleep (initial stage)
    NREM2,      // Deeper sleep
    NREM3,      // Slow wave sleep (deepest NREM)
    REM,        // Rapid eye movement sleep
    TransitionToSleep,
    TransitionToWake
};

// Sleep state structure for memory consolidation
struct SleepState {
    SleepStage currentStage;
    Timestamp stageStartTime;     // When current stage began
    Timestamp sleepStartTime;     // When sleep began
    float sleepPressure;          // Homeostatic sleep drive
    float hippocampalEngagement;  // Pattern completion level
    float synapticWeightChange;   // Cumulative weight modification
    float memoryPriority;         // Memory consolidation priority
    bool reactivationActive;      // Neural pattern reactivation active
    
    SleepState()
        : currentStage(SleepStage::Awake)
        , stageStartTime(0.0)
        , sleepStartTime(0.0)
        , sleepPressure(0.0f)
        , hippocampalEngagement(0.0f)
        , synapticWeightChange(0.0f)
        , memoryPriority(0.0f)
        , reactivationActive(false) {}
};

// Replay schedule for memory consolidation
struct replaySchedule {
    size_t replayInterval;        // Steps between replay events
    size_t consolidationInterval; // Steps between consolidation events
    bool replayDuringSleep;       // Whether replay occurs during sleep
    size_t maxReplaysPerStage;    // Maximum number of replays per sleep stage
    float consolidationStrength; // How strongly to consolidate memories
    
    replaySchedule()
        : replayInterval(100)
        , consolidationInterval(1000)
        , replayDuringSleep(true)
        , maxReplaysPerStage(5)
        , consolidationStrength(0.8f) {}
};

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
    
    // Sleep and consolidation methods
    void startSleep();
    void endSleep();
    bool isInSleep() const;
    SleepStage getCurrentSleepStage() const;
    void updateSleepSchedule(Timestamp currentTime);
    void processSleepMemories();
    
    // Memory consolidation during sleep
    void consolidateMemoriesDuringSleep();
    void replayMemoriesDuringSleep();
    void strengthenImportantMemories();
    void pruneWeakConnections();
    
    // Development during sleep
    void updateDevelopmentalStageDuringSleep();
    
    // Neuromodulation effects on sleep
    void applyAChEffects();
    void applyNEEffects();
    void apply5HTEffects();
    
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
    WorkingMemory* getWorkingMemory() { return pImpl->workingMemory.get(); }
    NeuralWorkingMemory* getNeuralWorkingMemory() const { return pImpl->workingMemory.get(); }
    
    // Episodic memory - experience storage
    EpisodicMemory* getEpisodicMemory() { return pImpl->episodicMemory.get(); }
    NeuralEpisodicMemory* getNeuralEpisodicMemory() const { return pImpl->episodicMemory.get(); }
    
    // Semantic memory - knowledge storage
    SemanticMemory* getSemanticMemory() { return pImpl->semanticMemory.get(); }
    
    // Procedural memory - skills and habits
    ProceduralMemory* getProceduralMemory() { return pImpl->proceduralMemory.get(); }
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory() const { return pImpl->associativeMemory.get(); }
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem();
    
// ========== COGNITION SYSTEMS ==========

    // Neural planner for action planning
    NeuralPlanner* getNeuralPlanner();

    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation();

    // Attentional selection for focus
    AttentionalSelection* getAttention();

    // Get prediction system for error-based learning
    PredictionSystem* getPredictionSystem();

    // Get neuromodulation systems for error signals and adaptation
    Acetylcholine* getAcetylcholine();
    Norepinephrine* getNorepinephrine();
    Serotonin* getSerotonin();

    // Get working memory for temporary storage
    NeuralWorkingMemory* getWorkingMemory() const;
    
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
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
