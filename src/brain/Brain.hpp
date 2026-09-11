// Brain.hpp
// Phase 6: Final Integration - Artificial Neural Brain
// 
// This header file defines the main Brain class, which serves as the central
// coordinator of the NLM artificial neural system. The Brain class implements
// the complete integrated brain loop that coordinates all subsystems:
// - Real spiking neural computation (Phase 2)
// - Memory systems (working, episodic, associative)
// - Prediction and cognition systems
// - Neuromodulation (dopamine, curiosity, novelty)
// - Developmental processes
// - Structural plasticity
// - Experience replay and consolidation
//
// The Brain class represents the culmination of all previous phases and
// provides a unified interface for simulating an artificial brain that
// learns, adapts, and exhibits intelligent behavior through neural dynamics.

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
// This class represents Phase 6: FINAL INTEGRATION of the NLM artificial brain.
// It coordinates all subsystems to function as a unified cognitive architecture
// capable of learning, memory, prediction, attention, and adaptive behavior.

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
    // Sets up all neural regions, populations, and integrated subsystems
    // This must be called before any other brain operations
    bool initialize();
    
    // Main simulation step
    // Implements the complete integrated brain loop as documented in Phase 6
    void step(SimulationStep currentStep);
    
    // Simulation time step with explicit timestamp
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    // This bridges the external world with the neural substrate
    // Input data represents raw sensory measurements (0.0-1.0 range)
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    // Useful for testing and targeted stimulation
    // neuron: Target neuron ID (0-10000 range)
    // current: Current to inject (in nanofarads, -100 to 100 typical range)
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    // Used for global modulation and testing
    // type: Neuron type to target
    // current: Current to inject to each neuron
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    // Returns pointer to the spike system managing neural spike events
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    // Returns pointers to synaptic plasticity mechanisms
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics and diagnostics
    // Returns excitation/inhibition ratio (weights > 0 / weights < 0)
    float getExcitationInhibitionRatio() const;
    // Total spike count since brain initialization
    size_t getTotalSpikeCount() const;
    // Number of pending spike events (immediate and delayed)
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    // Translates neural activity into behavioral commands
    // Returns unique_ptr to Action object containing selected action
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    // Implements dopamine, serotonin, and other neuromodulator effects
    // signal: Neuromodulator containing type, level, and timing
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    // Maintains synaptic changes based on activity and neuromodulation
    // This is called automatically during each simulation step
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    // Implements developmental stages and aging effects
    // This is called automatically during each simulation step
    void develop();
    
    // Reset brain state
    // Restores brain to initial state, clearing memory and resetting timers
    void reset();
    
    // Save brain state to file (checkpointing)
    // Enables persistence and resume functionality
    // filepath: Path where brain state will be saved
    // Returns true on success, false on failure
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // Restores previous brain state for testing and continuity
    // filepath: Path to checkpoint file
    // Returns true on success, false on failure
    bool load(const std::string& filepath);
    
    // Region management
    // Creates a new neural region with the given name (empty = auto-generated)
    RegionId addRegion(const std::string& name = "");
    // Get pointer to region by ID
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    // Get number of regions
    size_t getRegionCount() const;
    // Get list of all region IDs
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    // Returns vector of unique_ptr to all neural regions
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
    // Implements persistent activity and competitive selection for short-term memory
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    // Stores past experiences with timestamps, sensory states, and rewards
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    // Creates and retrieves relationships between experiences and neural patterns
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    // Enables predictive coding and error-based learning
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    // Evaluates possible actions and sequences based on current state
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    // Extracts abstract concepts from sensory patterns and experiences
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    // Selects relevant information and suppresses irrelevant inputs
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    // Get current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    // Set developmental stage (typically internal, updated automatically)
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
