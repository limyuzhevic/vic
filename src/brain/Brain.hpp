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
// PHASE 6: Integrated Artificial Brain (1134 lines)
// Contains 14+ integrated subsystems:
// - Memory: WorkingMemory, EpisodicMemory, AssociativeMemory
// - Prediction: PredictionSystem
// - Cognition: NeuralPlanner, ConceptFormation, AttentionalSelection
// - Neuromodulation: Dopamine, Curiosity, PredictionError, Novelty
// - Plasticity: STDP, Hebbian, SpikeSystem, StructuralPlasticity
// - Development: DevelopmentSystem
// - Performance: O(1) neuron lookup, optimized memory management
// 
// Integration Architecture:
// All subsystems are unified through event-driven spiking dynamics
// where neurons communicate via spikes, and neuromodulators provide
// dynamic control over plasticity, attention, and learning rates.
// Configuration-driven parameters enable flexible brain configuration.
// The Brain class serves as the central coordinator, implementing:
// - Spiking neural dynamics with real-time LIF neuron models
// - Event-driven computation using spike queues for efficiency
// - Memory systems for experience storage and recall
// - Prediction systems for forward modeling and error correction
// - Cognitive systems for planning, concept formation, and attention
// - Neuromodulation for motivation and adaptive behavior
// - Plasticity rules for learning and adaptation
// - Development stages for life-cycle progression
// This integrated architecture enables emergent cognition from neural interactions.
public:
    // Create brain with configuration
    // 
    // Parameters:
    //   config - Shared configuration object containing all brain parameters.
    //           This parameter is essential and must not be null for proper operation.
    //           The configuration object drives the entire brain architecture and behavior,
    //           including neural connectivity, plasticity parameters, neuromodulation levels,
    //           memory system capacities, and developmental parameters.
    //           Typical configuration parameters include:
    //           - neuron_count: Total number of neurons in the network (default: 1000)
    //           - region_count: Number of brain regions (default: 1)
    //           - connection_probability: Probability of creating random connections (default: 0.1)
    //           - random_seed: Seed for random number generation (default: 42)
    //           - simulation_timestep: Time step duration for simulation (default: 0.001s)
    //           - replay_interval: Steps between memory replay operations (default: 100)
    //           - consolidation_interval: Steps between memory consolidation (default: 1000)
    //           - stdp_*: STDP plasticity parameters (weights, time constants)
    //           - synaptogenesis_rate: Rate of new synapse formation
    //           - pruning_rate: Rate of synapse elimination
    //           - memory_capacity: Maximum working memory capacity
    //           - max_episodes: Maximum episodic memory storage
    //
    // Returns:
    //   Constructs a Brain object with the specified configuration.
    //   The brain uses the pImpl pattern to maintain implementation independence,
    //   reducing compilation dependencies and allowing for architectural evolution.
    //   The constructor does not allocate significant resources immediately;
    //   most initialization occurs in the initialize() method.
    //
    // Memory Management:
    //   Uses move semantics for efficiency. Copying is disabled (deleted) to prevent
    //   deep copies of the complex neural network structure (35+ million possible synapses).
    //   Move constructor efficiently transfers ownership of the pImpl pointer.
    //   The constructor does not throw exceptions (exception-safe).
    //
    // Integration Rationale:
    //   The Brain class serves as the central coordinator for an artificial neural system,
    //   integrating 14+ subsystems: 3 memory systems (working, episodic, associative),
    //   1 prediction system, 3 cognition systems (planner, concept formation, attention),
    //   1 development system, 4 neuromodulation systems (dopamine, curiosity, novelty,
    //   prediction error), and 4 plasticity systems (STDP, Hebbian, spike system,
    //   structural plasticity). All subsystems are unified through event-driven spiking
    //   dynamics where neurons communicate via spikes, and neuromodulators provide
    //   dynamic control over plasticity, attention, and learning rates.
    //
    // Regional Organization:
    //   Brain regions are created during initialization with neurons organized
    //   into functional populations: sensory (input processing), internal (integration),
    //   and motor (output generation). Inter-region connections provide long-range
    //   communication and integrate information across regions for complex cognitive tasks.
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
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
