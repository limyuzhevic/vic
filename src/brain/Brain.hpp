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

// Inter-regional connection (long-range connectivity)
// Represents a connection between different brain regions
struct InterRegionConnection {
    RegionId sourceRegion;          // Origin region of this connection
    RegionId targetRegion;          // Destination region
    float weight;                   // Connection strength (synaptic weight)
    Delay delay;                    // Transmission delay in simulation steps
    PlasticityFlags plasticityFlags; // Which plasticity mechanisms apply
    
    // Default constructor
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    // Parameterized constructor
    // @param src Source region ID
    // @param tgt Target region ID
    // @param w Initial connection weight
    // @param d Transmission delay (default: 1 step)
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems
// 
// The Brain class serves as the main interface for the NLM neural system.
// It manages neural populations, synaptic connections, plasticity mechanisms,
// and higher-level cognitive functions in a unified simulation framework.
// 
// Architecture follows the closed-loop brain model:
//   Input → Neural Processing → Memory/Cognition → Action → Output
// 
// Key features:
// - Real spiking neuron dynamics with LIF models
// - Event-driven spike propagation with delays
// - Multiple plasticity rules (STDP, Hebbian, structural)
// - Integrated memory systems (working, episodic, associative)
// - Neuromodulation (dopamine, curiosity, novelty, prediction error)
// - Prediction and cognitive systems (planning, attention, concept formation)
// - Developmental stages affecting plasticity
// - Checkpoint persistence for state saving/loading
class Brain {
public:
    // Constructor - creates brain with configuration
    // @param config Shared configuration object for brain parameters
    explicit Brain(std::shared_ptr<Config> config);
    
    // Destructor
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // ========== INITIALIZATION ==========
    
    // Initialize brain with configuration
    // Creates neural populations, establishes connectivity, initializes
    // all subsystems (memory, plasticity, neuromodulation, cognition)
    // @return bool true if initialization successful, false on error
    bool initialize();
    
    // ========== SIMULATION CORE ==========
    
    // Main simulation step (time-based)
    // Updates all neural dynamics and processes events
    // @param currentStep Current simulation step number
    void step(SimulationStep currentStep);
    
    // Main simulation step (time-based with absolute time)
    // Complete version including absolute timestamp tracking
    // @param currentStep Current simulation step number
    // @param currentTime Current simulation time in milliseconds
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // ========== INPUT PROCESSING ==========
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    // This is the primary input interface for environmental interaction
    // @param input Sensory input data from environment
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    // Used for targeted stimulation or testing specific neural pathways
    // @param neuron Neuron ID to inject current into
    // @param current Amount of current to inject (mV)
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    // Used to activate entire neural populations (e.g., sensory drive)
    // @param type NeuronType to activate (Sensory, Internal, Motor)
    // @param current Amount of current per neuron (mV)
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // ========== SYSTEM ACCESS ==========
    
    // Access spike system
    // @return SpikeSystem* Pointer to spike system for spike manipulation
    SpikeSystem* getSpikeSystem();
    
    // Access spike system (const version)
    // @return const SpikeSystem* Pointer to spike system (const access)
    const SpikeSystem* getSpikeSystem() const;
    
    // Access STDP plasticity system
    // @return STDP* Pointer to STDP mechanism
    STDP* getSTDP();
    
    // Access Hebbian plasticity system
    // @return Hebbian* Pointer to Hebbian learning mechanism
    Hebbian* getHebbian();
    
    // Access structural plasticity system
    // @return StructuralPlasticity* Pointer to structural plasticity mechanism
    StructuralPlasticity* getStructuralPlasticity();
    
    // ========== STATISTICAL ACCESS ==========
    
    // Get excitation-to-inhibition ratio
    // Measures balance between excitatory and inhibitory connections
    // @return float E/I ratio (higher = more excitation)
    float getExcitationInhibitionRatio() const;
    
    // Get total spike count
    // Cumulative spikes across entire brain simulation
    // @return size_t Total spike events
    size_t getTotalSpikeCount() const;
    
    // Get pending spike event count
    // Number of spikes scheduled but not yet processed
    // @return size_t Count of pending spike events
    size_t getPendingSpikeEventCount() const;
    
    // ========== OUTPUT GENERATION ==========
    
    // Produce motor/action output based on motor neuron activity
    // Selects appropriate action based on current neural state
    // @return std::unique_ptr<Action> Generated action (or Wait if none)
    std::unique_ptr<class Action> produceAction();
    
    // ========== NEUROMODULATION ==========
    
    // Apply neuromodulatory signals
    // Integrates signals from neuromodulation systems (dopamine, curiosity, etc.)
    // @param signal Neuromodulator signal to apply
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // ========== PLASTICITY ==========
    
    // Update plasticity rules (called automatically in step)
    // Applies STDP, Hebbian, and other plasticity mechanisms
    // Uses recent spike history and neuromodulation signals
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    // Modifies brain properties based on developmental stage
    void develop();
    
    // ========== STATE MANAGEMENT ==========
    
    // Reset brain state to initial conditions
    // Clears spike history, resets neuron states, clears memory traces
    void reset();
    
    // Save brain state to file (checkpointing)
    // Serializes neural state for persistence/resumption
    // @param filepath Path where checkpoint should be saved
    // @return bool true if save successful, false on error
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // Restores neural state from previous checkpoint
    // @param filepath Path to checkpoint file
    // @return bool true if load successful, false on error
    bool load(const std::string& filepath);
    
    // ========== REGION MANAGEMENT ==========
    
    // Add new neural region
    // Creates new population of neurons with specified name
    // @param name Optional name for the new region
    // @return RegionId Unique identifier for new region
    RegionId addRegion(const std::string& name = "");
    
    // Get neural region by ID
    // @param id Region identifier
    // @return NeuralRegion* Pointer to region (null if not found)
    NeuralRegion* getRegion(RegionId id);
    
    // Get neural region by ID (const version)
    // @param id Region identifier
    // @return const NeuralRegion* Pointer to region (null if not found)
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Get number of regions
    // @return size_t Total region count
    size_t getRegionCount() const;
    
    // Get all region IDs
    // @return std::vector<RegionId> List of all region identifiers
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    // @return const std::vector<std::unique_ptr<NeuralRegion>>& All regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    // Create connection between different brain regions
    // @param source Source region ID
    // @param target Target region ID
    // @param weight Initial connection weight
    // @param delay Transmission delay in simulation steps
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    // Remove inter-region connection
    // @param source Source region ID
    // @param target Target region ID
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // ========== GLOBAL STATISTICS ==========
    
    // Get total neuron count
    // @return size_t Total number of neurons across all regions
    size_t getTotalNeuronCount() const;
    
    // Get total synapse count
    // @return size_t Total number of connections across all regions
    size_t getTotalSynapseCount() const;
    
    // Get active neuron count
    // @return size_t Number of neurons currently above threshold
    size_t getActiveNeuronCount() const;
    
    // Get firing neuron count
    // @return size_t Number of neurons that fired in current step
    size_t getFiringNeuronCount() const;
    
    // Get average firing rate
    // @return float Average firing rate across all neurons (Hz)
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    // @return NeuralWorkingMemory* Pointer to working memory system
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    // @return NeuralEpisodicMemory* Pointer to episodic memory system
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    // @return NeuralAssociativeMemory* Pointer to associative memory system
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    // @return PredictionSystem* Pointer to prediction system
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    // @return NeuralPlanner* Pointer to neural planning system
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    // @return ConceptFormation* Pointer to concept formation system
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    // @return AttentionalSelection* Pointer to attention system
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    // Development system
    // @return DevelopmentSystem* Pointer to development system
    DevelopmentSystem* getDevelopmentSystem();
    
    // Get current developmental stage
    // @return DevelopmentalStage Current developmental phase
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Set developmental stage
    // @param stage Developmental stage to enter
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    // @return Dopamine* Pointer to dopamine system
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    // @return Curiosity* Pointer to curiosity system
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    // @return Novelty* Pointer to novelty detection system
    Novelty* getNovelty();
    
    // Prediction error signal
    // @return PredictionError* Pointer to prediction error system
    PredictionError* getPredictionErrorSignal();
    
    // ========== UTILITY ACCESS ==========
    
    // Get current configuration
    // @return std::shared_ptr<const Config> Pointer to configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    // @return RandomGenerator* Pointer to random number generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    // Outputs comprehensive brain status including regions, neurons,
    // synapses, memory states, and neuromodulation levels
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
