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

// ============================================================================
// Brain: The central coordinator of the neural system
// ============================================================================
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems
// 
// This class serves as the main interface to the NLM artificial brain system,
// providing methods for brain initialization, simulation stepping, and interaction
// with the various brain subsystems (memory, prediction, cognition, neuromodulation).
// 
// The brain architecture follows a biologically-inspired model with:
// - Real Leaky Integrate-and-Fire (LIF) neuron dynamics
// - Event-driven spike propagation with synaptic delays
// - Multiple plasticity mechanisms (STDP, Hebbian, structural)
// - Integrated memory systems (working, episodic, associative)
// - Neuromodulation (dopamine, curiosity, prediction error)
// - Developmental stages affecting plasticity rates
// - Cognitive functions (attention, planning, concept formation)
// - Prediction systems for forward model
// - Environment interaction via sensory and motor systems
//
// The brain operates as a closed-loop system where:
// 1. Sensory input from environment is processed
// 2. Neural computation occurs (LIF dynamics, plasticity)
// 3. Motor actions are produced
// 4. World consequences provide reward/prediction error
// 5. Synaptic weights are updated based on experience
//
// @note Phase 6: FINAL INTEGRATION - All systems are now integrated into a coherent brain loop
//

class Brain {
public:
    // ============================================================================
    // Lifecycle Management
    // ============================================================================

    /**
     * @brief Create brain with configuration
     * 
     * @param config Shared pointer to configuration object containing brain parameters
     *               (neuron count, connection probability, plasticity settings, etc.)
     * @throws std::bad_alloc if memory allocation fails
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy brain and clean up resources
     */
    ~Brain();
    
    // Disable copying, enable moving (Rule of Five compliance)
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;

    // ============================================================================
    // Brain Initialization and Configuration
    // ============================================================================

    /**
     * @brief Initialize brain with configuration parameters
     * 
     * Creates and configures all brain subsystems including neurons, synapses,
     * plasticity rules, memory systems, and neuromodulators based on the
     * configuration.
     * 
     * @return true if initialization succeeded, false if configuration was invalid
     *         or system resources were insufficient
     * 
     * @note Must be called before any other brain operations
     * @note Resets brain state before initialization if called multiple times
     */
    bool initialize();

    // ============================================================================
    // Simulation Core
    // ============================================================================

    /**
     * @brief Perform a simulation step with current timestamp
     * 
     * @param currentStep Current simulation step number (used for logging and time calculations)
     * @param currentTime Current simulation time in seconds (used for STDP timing)
     * 
     * @note Integrates all brain subsystems:
     *       - Spike system processes neuron dynamics
     *       - Plasticity system updates synaptic weights
     *       - Development system updates plasticity rates based on age
     *       - Memory systems encode and retrieve experiences
     *       - Neuromodulators affect learning and behavior
     */
    void step(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Convenience method for simulation step with default timestep
     * 
     * @param currentStep Current simulation step number
     * 
     * @note Uses 1ms timestep and computes timestamp internally
     */
    void step(SimulationStep currentStep);

    // ============================================================================
    // Sensory Processing and Input
    // ============================================================================

    /**
     * @brief Receive sensory input from environment and inject into brain
     * 
     * Processes sensory percept data (vision, touch, internal signals, proprioception)
     * and injects appropriate current into sensory neurons to drive neural activity.
     * 
     * @param input Sensory percept containing environmental and internal information
     * 
     * @note Typically called by agent brain interface after world update
     * @note Input is converted to synaptic currents based on sensory neuron properties
     */
    void receiveSensoryInput(const class SensoryInput& input);

    /**
     * @brief Inject current directly into a specific neuron
     * 
     * @param neuron Neuron ID to receive current injection
     * @param current Current amplitude to inject (positive for excitatory, negative for inhibitory)
     * 
     * @note Useful for creating targeted stimulation patterns
     * @note Overrides normal synaptic input for this neuron in the current timestep
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);

    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type Neuron type (Excitatory, Inhibitory, Sensory, etc.)
     * @param current Current amplitude to inject
     * 
     * @note Used for creating broad input patterns (e.g., all sensory neurons)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);

    // ============================================================================
    // System Access and Subsystems
    // ============================================================================

    /**
     * @brief Get spike system for spike processing and propagation
     * 
     * @return Pointer to spike system (never null after initialization)
     */
    SpikeSystem* getSpikeSystem();

    /**
     * @brief Get const spike system (thread-safe access)
     * 
     * @return Const pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;

    /**
     * @brief Get spike detection and propagation system
     * 
     * @return Pointer to STDP (Spike-Timing-Dependent Plasticity) system
     */
    STDP* getSTDP();

    /**
     * @brief Get Hebbian learning system for simultaneous activation learning
     * 
     * @return Pointer to Hebbian plasticity system
     */
    Hebbian* getHebbian();

    /**
     * @brief Get structural plasticity system for synapse formation and elimination
     * 
     * @return Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();

    // ============================================================================
    // Statistics and Monitoring
    // ============================================================================

    /**
     * @brief Get excitation-to-inhibition balance ratio
     * 
     * @return Ratio of total excitatory to inhibitory synaptic influence
     *         (>1.0 means more excitation, <1.0 means more inhibition)
     */
    float getExcitationInhibitionRatio() const;

    /**
     * @brief Get total spike count since last reset
     * 
     * @return Total number of spikes generated by all neurons
     */
    size_t getTotalSpikeCount() const;

    /**
     * @brief Get count of spikes pending propagation
     * 
     * @return Number of spikes scheduled to propagate to postsynaptic neurons
     */
    size_t getPendingSpikeEventCount() const;

    // ============================================================================
    // Action Production and Motor Control
    // ============================================================================

    /**
     * @brief Produce motor action based on current motor neuron activity
     * 
     * @return Unique pointer to Action object with motor command and parameters
     *         (nullptr if no motor neurons are active)
     */
    std::unique_ptr<class Action> produceAction();

    // ============================================================================
    // Neuromodulation
    // ============================================================================

    /**
     * @brief Apply neuromodulatory signal to affect learning and behavior
     * 
     * @param signal Neuromodulator signal (dopamine, curiosity, prediction error, etc.)
     */
    void applyNeuromodulation(const class Neuromodulator& signal);

    // ============================================================================
    // Plasticity Control
    // ============================================================================

    /**
     * @brief Update plasticity rules for all synapses
     * 
     * @note Called automatically during each simulation step
     * @note Updates STDP, Hebbian, and structural plasticity based on current brain state
     */
    void updatePlasticity();

    // ============================================================================
    // Development
    // ============================================================================

    /**
     * @brief Apply developmental changes based on current brain age
     * 
     * @note Called automatically during each simulation step
     * @note Affects plasticity rates, synaptic stability, and structural changes
     */
    void develop();

    // ============================================================================
    // State Management
    // ============================================================================

    /**
     * @brief Reset brain state to initial conditions
     * 
     * @note Clears all spike history, synaptic weights, and neuron states
     * @note Does not change configuration parameters
     */
    void reset();

    /**
     * @brief Save current brain state to file (checkpointing)
     * 
     * @param filepath Path to save checkpoint file
     * @return true if save succeeded, false if file could not be written
     */
    bool save(const std::string& filepath) const;

    /**
     * @brief Load brain state from file (checkpoint restoration)
     * 
     * @param filepath Path to checkpoint file
     * @return true if load succeeded, false if file could not be read or was corrupted
     */
    bool load(const std::string& filepath);

    // ============================================================================
    // Region Management
    // ============================================================================

    /**
     * @brief Add a new neural region to the brain
     * 
     * @param name Optional name for the region (for debugging/logging)
     * @return New region ID that can be used to access the region
     */
    RegionId addRegion(const std::string& name = "");

    /**
     * @brief Get neural region by ID
     * 
     * @param id Region ID to retrieve
     * @return Pointer to region (nullptr if ID is invalid)
     */
    NeuralRegion* getRegion(RegionId id);

    /**
     * @brief Get const neural region by ID (thread-safe)
     * 
     * @param id Region ID to retrieve
     * @return Const pointer to region (nullptr if ID is invalid)
     */
    const NeuralRegion* getRegion(RegionId id) const;

    /**
     * @brief Get count of regions in the brain
     * 
     * @return Number of neural regions
     */
    size_t getRegionCount() const;

    /**
     * @brief Get all region IDs
     * 
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;

    /**
     * @brief Get all regions (const access)
     * 
     * @return Vector of all neural regions (owner of pointers)
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;

    // ============================================================================
    // Inter-Regional Connectivity
    // ============================================================================

    /**
     * @brief Add long-range connection between brain regions
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Initial connection weight (default: 0.0)
     * @param delay Connection delay in simulation steps (default: 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);

    /**
     * @brief Remove inter-regional connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);

    // ============================================================================
    // Global Statistics
    // ============================================================================

    /**
     * @brief Get total neuron count across all regions
     * 
     * @return Total number of neurons in the brain
     */
    size_t getTotalNeuronCount() const;

    /**
     * @brief Get total synapse count across all regions
     * 
     * @return Total number of synapses in the brain
     */
    size_t getTotalSynapseCount() const;

    /**
     * @brief Get count of neurons currently active
     * 
     * @return Number of neurons with membrane potential above threshold
     */
    size_t getActiveNeuronCount() const;

    /**
     * @brief Get count of neurons currently firing spikes
     * 
     * @return Number of neurons in active firing state
     */
    size_t getFiringNeuronCount() const;

    /**
     * @brief Get average firing rate across all neurons
     * 
     * @return Average firing rate in Hz (spikes per second)
     */
    float getAverageFiringRate() const;

    // ============================================================================
    // MEMORY SYSTEMS
    // ============================================================================

    /**
     * @brief Get working memory system (transient active information)
     * 
     * @return Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();

    /**
     * @brief Get episodic memory system (experience storage)
     * 
     * @return Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();

    /**
     * @brief Get associative memory system (pattern associations)
     * 
     * @return Pointer to associative memory system
     */
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
