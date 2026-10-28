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

/**
 * The Brain class serves as the central control unit for the neural system.
 * 
 * This class orchestrates all neural computation, managing:
 * - Real-time spiking neural dynamics using Leaky Integrate-and-Fire (LIF) neurons
 * - Event-driven spike propagation with synaptic delays
 * - Multiple plasticity rules (STDP, Hebbian, structural)
 * - Integrated memory systems (working, episodic, associative)
 * - Neuromodulatory systems (dopamine, curiosity, novelty, prediction error)
 * - Cognitive functions (planning, concept formation, attention)
 * - Developmental processes affecting plasticity rates
 * - Sensory input and motor output integration
 * - Checkpoint persistence and replay capabilities
 * 
 * The brain operates as a closed-loop system that receives sensory input,
 * processes it through neural dynamics, and produces motor actions that
 * affect the environment.
 */

class Brain {
public:
    /**
     * Constructor for Brain
     * 
     * @param config Shared pointer to configuration object containing
     *               simulation parameters such as neuron count, region count,
     *               plasticity parameters, etc.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * Destructor
     */
    ~Brain();
    
    /**
     * Disable copying
     */
    Brain(const Brain&) = delete;
    
    /**
     * Disable copy assignment
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * Move constructor
     * 
     * @param other Rvalue reference to another Brain object
     */
    Brain(Brain&&) noexcept;
    
    /**
     * Move assignment operator
     * 
     * @param other Rvalue reference to another Brain object
     * @return Reference to this Brain object
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * Initialize the brain with configuration
     * 
     * This method sets up all brain systems including neural regions,
     * connections, and initializes internal state.
     * 
     * @return True if initialization succeeds, false otherwise
     */
    bool initialize();
    
    /**
     * Execute one simulation step using only step number
     * 
     * @param currentStep Current simulation step number
     */
    void step(SimulationStep currentStep);
    
    /**
     * Execute one simulation step with explicit time
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * Receive sensory input from environment
     * 
     * Injects current into sensory neurons based on the input pattern.
     * This simulates the brain's response to environmental stimuli.
     * 
     * @param input Reference to SensoryInput object containing the stimulus
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * Inject current directly into a specific neuron
     * 
     * Useful for targeted stimulation or debugging.
     * 
     * @param neuron ID of the neuron to inject current into
     * @param current Amount of current to inject (in nanoamperes)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * Inject current into all neurons of a specific type
     * 
     * @param type Neuron type to affect (Sensory, Motor, Interneuron, etc.)
     * @param current Amount of current to inject per neuron
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * Get pointer to spike system
     * 
     * @return Pointer to SpikeSystem object or nullptr if not available
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * Get constant pointer to spike system
     * 
     * @return Constant pointer to SpikeSystem object or nullptr if not available
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * Get pointer to STDP plasticity system
     * 
     * @return Pointer to STDP object or nullptr if not available
     */
    STDP* getSTDP();
    
    /**
     * Get pointer to Hebbian plasticity system
     * 
     * @return Pointer to Hebbian object or nullptr if not available
     */
    Hebbian* getHebbian();
    
    /**
     * Get pointer to structural plasticity system
     * 
     * @return Pointer to StructuralPlasticity object or nullptr if not available
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * Get excitation-inhibition ratio
     * 
     * @return Ratio of excitatory to inhibitory activity (0.0 = all inhibitory,
     *         1.0 = balanced, >1.0 = excitatory dominance)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * Get total spike count across all neurons
     * 
     * @return Number of spikes generated since last reset
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * Get number of pending spike events
     * 
     * @return Number of spikes scheduled but not yet delivered
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * Produce motor/action output based on motor neuron activity
     * 
     * This method analyzes motor neuron firing patterns and produces
     * appropriate actions for the environment.
     * 
     * @return Unique pointer to Action object or nullptr if no action needed
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * Apply neuromodulatory signals
     * 
     * @param signal Neuromodulator object containing signals like dopamine,
     *              curiosity, novelty, etc.
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * Update plasticity rules
     * 
     * This method is called automatically during simulation steps to
     * update synaptic weights based on plasticity rules.
     */
    void updatePlasticity();
    
    /**
     * Apply developmental changes
     * 
     * This method is called automatically during simulation steps to
     * modify plasticity rates and other parameters based on developmental stage.
     */
    void develop();
    
    /**
     * Reset brain state to initial conditions
     * 
     * This resets all neurons, synapses, and memory systems to their
     * initial state.
     */
    void reset();
    
    /**
     * Save brain state to file (checkpointing)
     * 
     * Saves the complete brain state including neuron states, synaptic weights,
     * memory contents, and developmental stage.
     * 
     * @param filepath Path to the checkpoint file
     * @return True if save succeeds, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * Load brain state from file
     * 
     * Restores brain state from a checkpoint file.
     * 
     * @param filepath Path to the checkpoint file
     * @return True if load succeeds, false otherwise
     */
    bool load(const std::string& filepath);
    
    /**
     * Add a new neural region
     * 
     * @param name Optional name for the region (for identification)
     * @return ID of the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * Get neural region by ID
     * 
     * @param id Region ID to retrieve
     * @return Pointer to NeuralRegion object or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * Get neural region by ID (const version)
     * 
     * @param id Region ID to retrieve
     * @return Constant pointer to NeuralRegion object or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * Get number of regions in the brain
     * 
     * @return Number of neural regions
     */
    size_t getRegionCount() const;
    
    /**
     * Get all region IDs
     * 
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * Get all neural regions
     * 
     * @return Constant reference to vector of NeuralRegion pointers
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * Add inter-region connection (long-range connectivity)
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight (default: 0.0)
     * @param delay Connection delay in simulation steps (default: 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * Remove inter-region connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * Get total neuron count across all regions
     * 
     * @return Total number of neurons
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * Get total synapse count across all regions
     * 
     * @return Total number of synapses
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * Get number of neurons currently active (above threshold)
     * 
     * @return Number of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * Get number of neurons that fired in the last step
     * 
     * @return Number of firing neurons
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * Get average firing rate across all neurons
     * 
     * @return Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * Get working memory system
     * 
     * @return Pointer to NeuralWorkingMemory object or nullptr if not available
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * Get episodic memory system
     * 
     * @return Pointer to NeuralEpisodicMemory object or nullptr if not available
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * Get associative memory system
     * 
     * @return Pointer to NeuralAssociativeMemory object or nullptr if not available
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * Get prediction system
     * 
     * @return Pointer to PredictionSystem object or nullptr if not available
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * Get neural planner for action planning
     * 
     * @return Pointer to NeuralPlanner object or nullptr if not available
     */
    NeuralPlanner* getPlanner();
    
    /**
     * Get concept formation system
     * 
     * @return Pointer to ConceptFormation object or nullptr if not available
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * Get attentional selection system
     * 
     * @return Pointer to AttentionalSelection object or nullptr if not available
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * Get development system
     * 
     * @return Pointer to DevelopmentSystem object or nullptr if not available
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * Get current developmental stage
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * Set developmental stage
     * 
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * Get dopamine neuromodulator
     * 
     * @return Pointer to Dopamine object or nullptr if not available
     */
    Dopamine* getDopamine();
    
    /**
     * Get curiosity neuromodulator
     * 
     * @return Pointer to Curiosity object or nullptr if not available
     */
    Curiosity* getCuriosity();
    
    /**
     * Get novelty neuromodulator
     * 
     * @return Pointer to Novelty object or nullptr if not available
     */
    Novelty* getNovelty();
    
    /**
     * Get prediction error signal
     * 
     * @return Pointer to PredictionError object or nullptr if not available
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * Get current configuration
     * 
     * @return Shared pointer to constant Config object
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * Get random number generator
     * 
     * @return Pointer to RandomGenerator object or nullptr if not available
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * Log brain status and statistics
     * 
     * Outputs comprehensive information about brain state including
     * neuron counts, spike statistics, and system status.
     */
    void logStatus() const;
    
private:
    /**
     * Pimpl pattern implementation details
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
