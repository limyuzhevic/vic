// Brain.hpp - Central Neural Computation Engine
//
// This file defines the Brain class, which serves as the central coordinator
// of the NLM (Neural Learning Machine) artificial brain system. The Brain class
// implements real spiking neural computation with event-driven dynamics and
// integrates all major neural systems including memory, prediction, cognition,
// neuromodulation, and development.
//
// The Brain class is the core of the NLM Phase 6 integration, coordinating
// interaction between all subsystems to create a coherent artificial brain.
//
// Key Features:
// - Real spiking neural computation using Leaky Integrate-and-Fire (LIF) neurons
// - Event-driven spike propagation with synaptic delays
// - Integration of 15+ different neural systems
// - Checkpoint save/load functionality for state persistence
// - Comprehensive memory management and error handling
// - Detailed logging and monitoring capabilities
//
// The architecture follows a multi-layered approach:
// 1. Core computation layer (neurons, synapses, spike system)
// 2. Memory layer (working, episodic, associative memory)
// 3. Prediction layer (forward and inverse models)
// 4. Cognition layer (planning, concept formation, attention)
// 5. Development layer (plasticity modulation over time)
// 6. Neuromodulation layer (dopamine, curiosity, novelty)
//
// All systems are interconnected through a unified brain loop that:
// 1. Processes delayed spike events
// 2. Updates neuron membrane potentials
// 3. Detects and schedules spikes
// 4. Updates working memory
// 5. Applies neuromodulation effects
// 6. Applies plasticity rules
// 7. Updates episodic memory
// 8. Updates prediction system
// 9. Updates attention system
// 10. Updates concept formation
// 11. Applies structural plasticity
// 12. Replays important memories
// 13. Applies development effects
// 14. Collects statistics
// 15. Manages checkpoints

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

// Forward declarations for all integrated systems
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

/**
 * @brief Inter-regional connection for long-range neural communication
 * 
 * Represents a connection between two different neural regions that allows
 * for long-range communication and coordination between brain areas.
 * 
 * This is essential for implementing complex brain architectures where
 * different regions specialize in different functions and need to communicate.
 * 
 * @note Connections are bidirectional and can have different delays and weights
 * for each direction.
 */
struct InterRegionConnection {
    /** @brief Source region identifier */
    RegionId sourceRegion;
    /** @brief Target region identifier */
    RegionId targetRegion;
    /** @brief Connection strength (weight) */
    float weight;
    /** @brief Signal transmission delay in time steps */
    Delay delay;
    /** @brief Plasticity rules that apply to this connection */
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Default constructor
     * 
     * Creates an empty connection with default parameters.
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param src Source region identifier
     * @param tgt Target region identifier
     * @param w Connection weight (default: 0.0f)
     * @param d Transmission delay (default: 1 step)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief Main Brain class - Central Neural Computation Engine
 * 
 * The Brain class is the core component of the NLM artificial brain system.
 * It coordinates all neural subsystems and implements the complete integrated
 * brain loop that processes sensory input, generates motor output, and maintains
 * internal state through memory and learning processes.
 * 
 * The brain follows a closed-loop architecture where:
 * - External sensory input is processed through neural networks
 * - Internal state is maintained through working and episodic memory
 * - Actions are selected based on neural computations
 * - Learning occurs through plasticity rules (STDP, Hebbian)
 * - Development affects learning rates and capabilities over time
 * - Neuromodulators regulate learning, attention, and motivation
 * 
 * This class implements Phase 6: Final Integration, demonstrating that all
 * individual brain components work together as a coherent system.
 * 
 * @note This class uses the PIMPL pattern for implementation hiding to reduce
 * compilation dependencies and improve compilation times.
 */
class Brain {
public:
    /**
     * @brief Constructor with configuration
     * 
     * Creates a new brain instance with the specified configuration.
     * The configuration contains parameters for all aspects of brain operation
     * including neural network size, plasticity rules, and simulation parameters.
     * 
     * @param config Shared pointer to the configuration object
     * @note Configuration is copied during construction for safety.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     * 
     * Cleans up all brain resources and performs final shutdown procedures.
     */
    ~Brain();
    
    /**
     * @brief Disable copying (PIMPL pattern)
     * 
     * Copying brains is not supported as it would require complex
     * coordination of all internal state.
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disable assignment (PIMPL pattern)
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor (enables ownership transfer)
     * 
     * Transfers ownership of the brain implementation from one object
     * to another, allowing efficient brain transfer.
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator (enables ownership transfer)
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * Sets up the brain structure by creating neural regions, neurons,
     * populations, and synapses according to the configuration parameters.
     * 
     * This method must be called before any other brain operations.
     * 
     * @return bool true if initialization was successful, false otherwise
     * 
     * @note Initialization includes setting up all integrated systems:
     * - Memory systems (working, episodic, associative)
     * - Prediction system
     * - Cognition systems (planner, concept formation, attention)
     * - Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
     * - Plasticity systems (STDP, Hebbian, structural plasticity)
     */
    bool initialize();
    
    /**
     * @brief Main simulation step (time-based)
     * 
     * Advances the brain simulation by one time step using the default timestep.
     * This executes the complete integrated brain loop including all neural
     * computations and system updates.
     * 
     * @param currentStep Current simulation step number
     * 
     * @note This method calls the internal step method with the calculated
     * current time based on the configured timestep.
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step (time and step-based)
     * 
     * Advances the brain simulation by one time step with explicit time tracking.
     * This is the core method that implements the 15-step integrated brain loop.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     * 
     * @note This method implements the complete integrated brain loop:
     * 1. Process pending delayed spike events
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
     * 4. Update working memory
     * 5. Apply neuromodulation effects
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity
     * 12. Replay important memories
     * 13. Apply development effects
     * 14. Collect statistics
     * 15. Checkpoint management
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Processes sensory input from the environment by injecting appropriate
     * current into sensory neurons. This represents the brain's perception
     * of the external world.
     * 
     * @param input Sensory input data containing environmental information
     * 
     * @note The input encoding is simple but effective, converting environmental
     * features into neural current injections that modulate neuron activity.
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Manually injects current into a specific neuron, useful for testing,
     * debugging, or forcing specific neural activity patterns.
     * 
     * @param neuron Neuron identifier
     * @param current Current amount to inject (in nanoamps)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Applies current injection to all neurons of a given type (Sensory,
     * Internal, or Motor), allowing for coordinated stimulation of neuron
     * populations.
     * 
     * @param type Neuron type to stimulate
     * @param current Current amount to inject (in nanoamps)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Access spike system
     * 
     * Provides access to the spike system that manages spike generation,
     * propagation, and timing.
     * 
     * @return SpikeSystem* Pointer to the spike system
     * @return const SpikeSystem* Pointer to the spike system (const version)
     */
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access spike timing dependence plasticity (STDP) system
     * 
     * @return STDP* Pointer to the STDP plasticity system
     */
    STDP* getSTDP();
    
    /**
     * @brief Access Hebbian plasticity system
     * 
     * @return Hebbian* Pointer to the Hebbian plasticity system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Access structural plasticity system
     * 
     * @return StructuralPlasticity* Pointer to the structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation-inhibition ratio
     * 
     * Calculates the balance between excitatory and inhibitory neural activity,
     * which is important for neural stability and information processing.
     * 
     * @return float Current E/I ratio (typically around 4:1 to 1:1 for stability)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * 
     * @return size_t Total number of spikes generated since initialization
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * @return size_t Number of spike events waiting to be delivered
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * Converts neural activity into motor commands that can be used to
     * interact with the environment. This represents the brain's output
     * functionality.
     * 
     * @return std::unique_ptr<class Action> Motor command action
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     * 
     * Applies neuromodulatory signals (dopamine, curiosity, novelty) that
     * affect neural excitability, plasticity, and learning.
     * 
     * @param signal Neuromodulator signal to apply
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * Applies all plasticity rules including STDP, Hebbian, and other
     * learning mechanisms. This method is called automatically during
     * each simulation step.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * Applies developmental processes that affect brain maturation,
     * including changes in plasticity rates, structural changes, and
     * functional maturation. This method is called automatically during
     * each simulation step.
     */
    void develop();
    
    /**
     * @brief Reset brain state
     * 
     * Resets the brain to its initial state, clearing memory, statistics,
     * and neural activity. This is useful for running multiple experiments
     * or cleaning up after simulations.
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Saves the current brain state to a file for checkpointing purposes.
     * This allows the brain to be paused and resumed at a later time.
     * 
     * @param filepath Path to the checkpoint file
     * @return bool true if the state was successfully saved, false otherwise
     * 
     * @note The saved state includes:
     * - Neuron membrane potentials and states
     * - Synapse weights and eligibility traces
     * - Memory system contents
     * - All internal system states
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * Restores a previously saved brain state from a checkpoint file.
     * This allows the brain to be resumed from a previous point in time.
     * 
     * @param filepath Path to the checkpoint file
     * @return bool true if the state was successfully loaded, false otherwise
     * 
     * @note Loading restores the complete brain state including all
     * neural parameters, memory contents, and system states.
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add neural region
     * 
     * Creates a new neural region with the specified name.
     * Regions are collections of neural populations that work together.
     * 
     * @param name Name of the region (default: "")
     * @return RegionId Identifier of the created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get neural region by ID
     * 
     * @param id Region identifier
     * @return NeuralRegion* Pointer to the region
     * @return const NeuralRegion* Pointer to the region (const version)
     */
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions
     * 
     * @return size_t Number of neural regions in the brain
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * 
     * @return std::vector<RegionId> List of all region identifiers
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Reference to regions vector
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection
     * 
     * Creates a connection between two different neural regions to allow
     * for long-range communication and coordination.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Connection strength (default: 0.0f)
     * @param delay Signal transmission delay (default: 1 step)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * 
     * Removes the connection between two neural regions.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count
     * 
     * @return size_t Total number of neurons in all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count
     * 
     * @return size_t Total number of synapses in all regions
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count
     * 
     * @return size_t Number of neurons currently in active states
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * 
     * @return size_t Number of neurons currently firing or in refractory state
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * 
     * @return float Average firing rate across all neurons (in Hz)
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Access working memory system
     * 
     * @return NeuralWorkingMemory* Pointer to the working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Access episodic memory system
     * 
     * @return NeuralEpisodicMemory* Pointer to the episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Access associative memory system
     * 
     * @return NeuralAssociativeMemory* Pointer to the associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Access prediction system
     * 
     * @return PredictionSystem* Pointer to the prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Access neural planner
     * 
     * @return NeuralPlanner* Pointer to the neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Access concept formation system
     * 
     * @return ConceptFormation* Pointer to the concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Access attentional selection system
     * 
     * @return AttentionalSelection* Pointer to the attentional selection system
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Access development system
     * 
     * @return DevelopmentSystem* Pointer to the development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * @param stage Developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @brief Access dopamine neuromodulation system
     * 
     * @return Dopamine* Pointer to the dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Access curiosity neuromodulation system
     * 
     * @return Curiosity* Pointer to the curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Access novelty neuromodulation system
     * 
     * @return Novelty* Pointer to the novelty system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Access prediction error neuromodulation system
     * 
     * @return PredictionError* Pointer to the prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get current configuration
     * 
     * @return std::shared_ptr<const Config> Shared pointer to the configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator
     * 
     * @return RandomGenerator* Pointer to the random number generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log brain status
     * 
     * Outputs detailed information about the current brain state including
     * statistics, memory contents, and system status.
     * 
     * @note This is useful for debugging and monitoring brain operation.
     */
    void logStatus() const;
    
private:
    /** @brief Forward declaration of implementation (PIMPL pattern) */
    struct Impl;
    /** @brief Pointer to implementation */
    Impl* pImpl;
};

} // namespace nlm