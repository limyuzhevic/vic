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
//
// Key features:
// - Event-driven spike propagation with synaptic delays
// - Integrated memory systems (working, episodic, associative)
// - Prediction system for forward models
// - Cognitive mechanisms (planning, attention, concept formation)
// - Neuromodulation (dopamine, curiosity, novelty detection)
// - Developmental stages affecting plasticity rates
// - Structural plasticity (synaptogenesis and pruning)
// - Checkpoint-based persistence and replay
// - Real-time interaction with environment
//
// Architecture:
//   WORLD → SENSORY INPUT → NEURAL PROCESSING
//               → INTERNAL STATE → MEMORY/PREDICTION → MOTIVATION/NEUROMODULATION
//               → ACTION SELECTION → MOTOR OUTPUT → WORLD CONSEQUENCE
//
// The brain operates as a closed-loop control system with continuous learning
// through experience and interaction with the environment.

class Brain {
public:
    // Create brain with configuration
    // Initializes all neural subsystems and prepares for simulation
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving (for performance)
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    // Sets up neural populations, connectivity, and integrated systems
    bool initialize();
    
    // Main simulation step
    // Executes one timestep of the complete brain loop
    void step(SimulationStep currentStep);
    
    // Simulation time step with explicit timestamp
    // Useful for real-time applications and precise timing
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    // This models the brain's sensory encoding mechanisms
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    // Used for targeted stimulation or test experiments
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    // Useful for stimulating entire neural populations
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    // Returns the spike system for event-driven processing
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    // Returns STDP, Hebbian, and structural plasticity systems
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics and metrics
    // Returns excitation/inhibition balance
    float getExcitationInhibitionRatio() const;
    
    // Total spike count since initialization
    size_t getTotalSpikeCount() const;
    
    // Number of pending spike events (for latency analysis)
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    // Implements action selection from neural activity patterns
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    // Integrates reward prediction error, curiosity, and novelty signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    // Applies STDP, Hebbian, and structural plasticity rules
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    // Implements age-dependent plasticity modulation
    void develop();
    
    // Reset brain state
    // Clears all memories and resets to initial state
    void reset();
    
    // Save brain state to file (checkpointing)
    // Enables persistence and resumption of long-term learning
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // Restores saved neural state including weights and memories
    bool load(const std::string& filepath);
    
    // Region management
    // Creates a new neural region with specified characteristics
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    // Creates long-range connections between regions
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
    // Maintains neural representations through persistent activity
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    // Stores events as neural patterns with temporal context
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    // Links related experiences through synaptic connections
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    // Implements forward models and predictive coding
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    // Evaluates action sequences and selects optimal actions
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    // Extracts abstract patterns from neural activity
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    // Selects relevant information for processing
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    // Mediates reward prediction error and learning rate modulation
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    // Drives exploration of novel states
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    // Detects unexpected events and changes
    Novelty* getNovelty();
    
    // Prediction error signal
    // Core teaching signal for learning systems
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging and diagnostics
    // Outputs brain state and statistics for monitoring
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Helper functions for better code organization
    // Improves modularity and testability
    void updateAllNeurons(const std::vector<std::unique_ptr<NeuralRegion>>& regions, 
                         Timestamp currentTime, TimestepDuration timestep);
    void detectAndProcessSpikes(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                                SimulationStep currentStep, Timestamp currentTime,
                                SpikeSystem* spikeSystem);
    
    // Memory system helpers
    void updateWorkingMemory(TimestepDuration timestep);
    void applyNeuromodulationEffects();
    void applyPlasticityRules(float plasticityMod);
    void updateEpisodicMemory(SimulationStep currentStep, Timestamp currentTime);
    void updatePredictionSystem();
    void updateAttentionSystem(SimulationStep currentStep);
    void updateConceptFormation();
    void applyStructuralPlasticity(SimulationStep currentStep);
    void replayMemories(SimulationStep currentStep);
    void applyDevelopmentEffects(SimulationStep currentStep);
    void consolidateMemories(SimulationStep currentStep);
    void updateCheckpoints(SimulationStep currentStep, Timestamp currentTime);
};

} // namespace nlm
