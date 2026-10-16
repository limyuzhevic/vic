#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "BrainErrorHandling.hpp"
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

/**
 * @file Brain.hpp
 * @brief The central coordinator of the neural system
 * 
 * This file defines the Brain class, which serves as the main controller for
 * the NLM (Neural Learning Machine) framework. It implements real spiking
 * neural computation with event-driven dynamics and integrates multiple
 * specialized subsystems including memory, prediction, cognition, and
 * neuromodulation systems.
 * 
 * @details
 * The Brain class follows a modular architecture where different neural
 * computation aspects are handled by dedicated subsystems. This design
 * allows for flexible scaling and maintenance while providing a unified
 * interface for neural simulation and interaction with external environments.
 * 
 * Key Features:
 * - Event-driven spiking neural computation
 * - Real-time simulation with millisecond precision
 * - Multi-regional brain architecture (cerebral cortex, subcortical, brainstem)
 * - Integrated memory systems (working, episodic, associative)
 * - Prediction and cognition systems
 * - Neuromodulatory control (dopamine, acetylcholine, etc.)
 * - Plasticity learning mechanisms (STDP, Hebbian, structural)
 * - Developmental adaptation over time
 * - Checkpoint/restore for simulation persistence
 * 
 * @architecture Brain Architecture:
 * ┌─────────────────────────────────────┐
 * │          Brain (Main Coordinator)   │
 * │  ┌──────────────────────────────┐  │
 * │  │      Neural Regions           │  │
 * │  │  ┌─────────────────────┐     │  │
 * │  │  │ Neural Populations │     │  │
 * │  │  │     ┌─────┐        │     │  │
 * │  │  │     │Neuron│        │     │  │
 * │  │  │     │     │        │     │  │
 * │  │  │     └─────┘        │     │  │
 * │  │  │         ┌─────┐    │     │  │
 * │  │  │         │Synapse│    │     │  │
 * │  │  │         └─────┘    │     │  │
 * │  │  └─────────────────────┘     │  │
 * │  └──────────────────────────────┘  │
 * │        │  │  │  │                  │
 * │  ┌─────────────────────┐     ┌─────┐│
 * │  │    Spike System     │     │Error│ │
 * │  │  ┌──────────────┐  │     │Handler│ │
 * │  │  │   Spike     │  │     │       │ │
 * │  │  │ Injection    │  │     │       │ │
 * │  │  │ Generation   │  │     │       │ │
 * │  │  └──────────────┘  │     │       │ │
 * │  └─────────────────────┘     │   ┌───┴───┐
 * │                              │   │Memory│   │
 * │  ┌─────────────────────┐     │ ┌─────┴─────┐│
 * │  │   Plasticity       │     │ │Working    │ │
 * │  │  ┌──────────────┐  │     │ │ Memory    │ │
 * │  │  │   STDP       │  │     │ │           │ │
 * │  │  │              │  │     │ └─────┬─────┘│
 * │  │  │   Hebbian    │  │     │       │       │
 * │  │  │              │  │     │ ┌─────┴─────┐│
 * │  │  └──────────────┘  │     │ │Episodic   │ │
 * │  └─────────────────────┘     │ │ Memory    │ │
 * │                              │ └─────┬─────┘│
 * │  ┌─────────────────────┐     │       │       │
 * │  │  Neuromodulation   │     │ ┌─────┴─────┐│
 * │  │  ┌──────────────┐  │     │ │Associative│ │
 * │  │  │Dopamine      │  │     │ │ Memory    │ │
 * │  │  │Acetylcholine │  │     │ │           │ │
 * │  │  │...            │  │     │ └─────┬─────┘│
 * │  └─────────────────────┘     │       │       │
 * │                              │ ┌─────┴─────┐│
 * │  ┌─────────────────────┐     │ │Prediction │ │
 * │  │    Cognition       │     │ │ System    │ │
 * │  │  ┌──────────────┐  │     │ └─────┬─────┘│
 * │  │  │ Neural       │  │     │       │       │
 * │  │  │ Planner      │  │     │ ┌─────┴─────┐│
 * │  │  │Concept       │  │     │ │Concept   │ │
 * │  │  │ Formation    │  │     │ │Formation │ │
 * │  │  └──────────────┘  │     │ └─────┬─────┘│
 * │  └─────────────────────┘     │       │       │
 * └─────────────────────────────────────┘       │
 *                                               │
 * ┌─────────────────────────────────────┐     ┌─────┴─────┐
 * │          Development              │     │Development │
 * │         ┌──────────────┐         │     │ System    │
 * │         │   Stages    │         │     │           │
 * │         │(Juvenile,   │         │     │           │
 * │         │  Adolescence│         │     │           │
 * │         │, Mature)     │         │     │           │
 * │         └──────────────┘         │     │           │
 * └─────────────────────────────────────┘     │           │
 *                                              │           │
 * ┌─────────────────────────────────────┐     ┌─────┴─────┐
 * │           Environment             │     │Experiment │
 * │         ┌──────────────┐         │     │   Runner  │
 * │         │Sensory      │         │     │           │
 * │         │ Input      │         │     │           │
 * │         └──────────────┘         │     │           │
 * └─────────────────────────────────────┘     │           │
 *                                               │           │
 * @version 6.0.0
 * @date 2026
 * @author NLM Development Team
 * @copyright Copyright (c) 2025 NLM Framework
 * 
 * @note This class uses the Pimpl idiom (Pointer to Implementation) to hide
 * implementation details and improve compilation times.
 * 
 * @warning The Brain class is NOT thread-safe for concurrent access from multiple
 * threads. Use appropriate synchronization when accessing from multiple
 * threads or processes.
 * 
 * @see NeuralRegion, Neuron, Synapse
 * @see PredictionSystem, NeuralPlanner, ConceptFormation
 * @see Dopamine, Acetylcholine, Neuromodulator
 * @see Config
 * @see nlm namespace documentation
 */

/**
     * @brief Constructs a new Brain instance with the specified configuration
     * 
     * @param config Shared pointer to the Brain configuration object. This configuration
     *              controls all aspects of the brain's operation including neural
     *              parameters, simulation settings, memory constraints, and subsystem
     *              configurations. The configuration must remain valid for the lifetime
     *              of the Brain object.
     * 
     * @throws NLMError if configuration is invalid or null
     * 
     * @pre Configuration object must be properly initialized with valid parameters
     * @post Brain object is created but not yet initialized (call initialize() to start)
     * 
     * @note Memory management uses shared_ptr for the Config object to allow for
     *       flexible ownership semantics and prevent premature deletion.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroys the Brain instance and cleans up all resources
     * 
     * This destructor properly cleans up all dynamically allocated resources,
     * including all neural regions, synapses, subsystems, and associated memory.
     * The Pimpl idiom ensures implementation details are properly hidden and
     * safely destroyed.
     * 
     * @note This is a virtual destructor to ensure proper cleanup when deleting
     *       through Brain* pointers (though Brain currently doesn't inherit from
     *       any polymorphic base class except the implicit object).
     */
    ~Brain();
    
    /**
     * @brief Copy constructor is deleted to prevent shallow copying
     * 
     * The Brain class does not support copying because it contains complex
     * internal state management, pointer-to-implementation (Pimpl) pattern,
     * and circular dependencies between components. The move constructor
     * should be used instead for transferring ownership.
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted to prevent shallow copying
     * 
     * Similar to the copy constructor, assignment is prohibited to maintain
     * the integrity of the internal implementation and prevent resource leaks.
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership from temporary object
     * 
     * This noexcept constructor efficiently transfers ownership of the internal
     * implementation pointer from the source object to the new Brain instance.
     * After the move operation, the source object is left in a valid but
     * unspecified state.
     * 
     * @param other The Brain object to move from (will be in valid but unspecified state)
     * 
     * @post The moved-from object has its pImpl set to nullptr
     * @exception May throw std::bad_alloc if memory allocation fails for the new implementation
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership from temporary object
     * 
     * This noexcept operator efficiently transfers ownership of the internal
     * implementation pointer from the source object to the current Brain instance.
     * Proper cleanup of the existing implementation is performed before the transfer.
     * 
     * @param other The Brain object to move from (will be in valid but unspecified state)
     * @return Reference to this Brain object after the move assignment
     * @post The moved-from object has its pImpl set to nullptr, this object's state is updated
     * @exception May throw std::bad_alloc if memory allocation fails
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initializes the brain with configuration parameters
     * 
     * This method sets up all neural subsystems, creates initial regions and neurons,
     * allocates memory structures, and establishes initial connections. It performs
     * comprehensive validation of all configuration parameters and sets up the
     * computational infrastructure required for simulation.
     * 
     * @return bool True if initialization succeeds, false otherwise
     * 
     * @retval true Initialization completed successfully
     * @retval false Failed to initialize (configuration invalid, resources unavailable, etc.)
     * 
     * @post Brain is ready to begin simulation steps
     * @pre Brain constructor has been called
     * @pre Configuration has been loaded and validated
     * 
     * @note This method can be called multiple times to reinitialize after reset
     * 
     * @see Config, Neuron, Synapse, NeuralRegion
     * 
     * @warning If initialization fails, the Brain object may be left in an inconsistent
     *          state. Call reset() to attempt recovery.
     */
    bool initialize();
    
    /**
     * @brief Advances the brain simulation by one time step
     * 
     * This is the core simulation function that drives neural computation for one
     * discrete time step. It performs the complete neural computation cycle including
     * spike generation, synaptic transmission, plasticity updates, neuromodulation,
     * and development processes. This method should be called repeatedly to run
     * the simulation.
     * 
     * @param currentStep The current simulation step number (discrete time index)
     * @param currentTime The current simulation time in milliseconds or other time units
     *                   depending on configuration
     * 
     * @pre Brain must be initialized with initialize()
     * @post All neural components have been updated for this time step
     * @note Only the two-parameter version is recommended for production use as it provides
     *       more precise temporal control. The single-parameter version uses a default
     *       time step of 1.0ms.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Advances the brain simulation by one time step (legacy version)
     * 
     * Simplified version of the step function that uses a default time step of 1.0ms.
     * This version is maintained for backward compatibility but the two-parameter
     * version should be used for new code as it provides better temporal control.
     * 
     * @param currentStep The current simulation step number
     * 
     * @see step(SimulationStep, Timestamp) for more precise control
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Receive sensory input from the environment and inject into neural tissue
     * 
     * This method processes external sensory inputs and converts them into neural
     * currents that activate sensory neurons. This mimics how biological sensory
     * systems translate environmental stimuli into neural signals that propagate
     * through the brain.
     * 
     * @param input The sensory input pattern containing modalities (vision, audition,
     *              proprioception, etc.) and their corresponding intensities and timing
     * 
     * @pre Brain must be initialized
     * @post Sensory neurons have received appropriate current injections based on input
     * @note This input is integrated into the brain's perception and prediction systems
     * 
     * @see SensoryInput, receiveSensoryInput()
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * This method provides fine-grained control over neural activity by injecting
     * arbitrary current into specific neurons. This is useful for implementing
     * external control signals, test stimuli, or targeted neural activation.
     * 
     * @param neuron The ID of the neuron to receive the current injection
     * @param current The membrane current to inject (in nanoamperes or appropriate units)
     * 
     * @pre Brain must be initialized
     * @pre Neuron with specified ID must exist
     * @post The specified neuron receives the injected current
     * 
     * @warning Direct current injection can override natural neural dynamics and may
     *          cause unexpected behavior if used incorrectly.
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specified type
     * 
     * This method provides population-level control by injecting current into all
     * neurons of a specific type (e.g., all excitatory, all inhibitory, sensory,
     * motor, etc.). This is useful for global modulation or experimental paradigms.
     * 
     * @param type The neuron type to target (e.g., NeuronType::Excitatory)
     * @param current The membrane current to inject into all neurons of the specified type
     * 
     * @pre Brain must be initialized
     * @post All neurons of the specified type receive the injected current
     * @note Efficiency note: This method iterates through all neurons of the specified type
     * 
     * @see NeuronType, NeuronId
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Access the spike system for direct manipulation and monitoring
     * 
     * @return SpikeSystem* Pointer to the internal spike system object
     * 
     * @post Caller gains direct access to spike system for advanced control
     * @note This provides low-level access to spike dynamics and should be used
     *       with caution as improper use can destabilize the simulation
     * 
     * @see SpikeSystem, getSpikeSystem() const
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Access the spike system for direct manipulation and monitoring (const version)
     * 
     * @return const SpikeSystem* Const pointer to the internal spike system object
     * 
     * @post Caller gains read-only access to spike system
     * @note This version is for read-only access when modifying spike dynamics is not required
     * 
     * @see SpikeSystem, getSpikeSystem()
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access the synaptic plasticity STDP system
     * 
     * Spike-Timing-Dependent Plasticity (STDP) is a Hebbian learning rule where
     * synaptic strength depends on the precise timing between pre- and post-synaptic
     * spikes. This method provides access to the STDP implementation.
     * 
     * @return STDP* Pointer to the internal STDP system
     * 
     * @post Caller gains direct access to STDP learning dynamics
     * @note STDP is a core mechanism for temporal learning in biological neural networks
     * 
     * @see STDP, getHebbian(), getStructuralPlasticity()
     */
    STDP* getSTDP();
    
    /**
     * @brief Access the synaptic plasticity Hebbian system
     * 
     * This method provides access to the Hebbian learning system, which implements
     * activity-dependent synaptic strengthening (not timing-dependent like STDP).
     * Hebbian learning is based on the principle "neurons that fire together, wire together".
     * 
     * @return Hebbian* Pointer to the internal Hebbian plasticity system
     * 
     * @post Caller gains direct access to Hebbian learning dynamics
     * @note Hebbian plasticity complements STDP by providing rate-based learning
     * 
     * @see Hebbian, getSTDP(), getStructuralPlasticity()
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Access the structural plasticity system for long-term connectivity changes
     * 
     * This method provides access to the structural plasticity system, which manages
     * changes to the physical connectivity of the brain - formation and elimination
     * of synapses, axon growth, and structural rewiring. This operates on a much
     * slower timescale than synaptic plasticity.
     * 
     * @return StructuralPlasticity* Pointer to the internal structural plasticity system
     * 
     * @post Caller gains direct access to structural connectivity changes
     * @note Structural plasticity underlies learning over longer timescales (hours to days)
     * 
     * @see StructuralPlasticity, getSTDP(), getHebbian()
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * This method generates motor actions from the brain's motor neuron population
     * based on the current neural activity patterns. The action represents a
     * behavioral output that can be sent to an environment or effector system.
     * 
     * @return std::unique_ptr<class Action> Pointer to the generated action object
     * 
     * @retval nullptr No action was generated (motor system inactive)
     * @retval valid pointer Action object with motor parameters and timing
     * 
     * @post Action is created based on current motor neuron state
     * @pre Brain must be initialized and have motor neurons
     * 
     * @note The returned Action object is managed by the caller - Brain does not
     *       retain ownership of the action
     * 
     * @see Action, MotorSystem
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals to the brain
     * 
     * This method applies neuromodulatory signals that affect neural plasticity,
     * firing rates, and synaptic transmission. Neuromodulators such as dopamine,
     * acetylcholine, and serotonin modulate brain state and learning processes.
     * 
     * @param signal Neuromodulator object containing the modulation signal
     * 
     * @pre Brain must be initialized
     * @post Neuromodulation signals are applied to appropriate neural populations
     * 
     * @note Neuromodulators can have both immediate and long-term effects on
     *       neural computation and plasticity
     * 
     * @see Neuromodulator, Dopamine, Acetylcholine, getDopamine()
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules based on current neural activity
     * 
     * This method implements the plasticity learning algorithms (STDP, Hebbian,
     * reward-modulated) based on the current spike patterns and neuromodulatory
     * signals. This is typically called automatically during the step function
     * but can be called explicitly for control.
     * 
     * @post Synaptic weights are updated according to plasticity rules
     * @pre Brain must be initialized
     * @pre Neurons and synapses must be present for plasticity to occur
     * 
     * @note This method runs on a different timescale than the main simulation
     *       and may be called multiple times per simulation step
     * 
     * @see getSTDP(), getHebbian(), applyNeuromodulation()
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes to the brain structure
     * 
     * This method implements developmental processes that modify brain structure
     * over time, including synaptogenesis, pruning, maturation, and regional
     * specialization. Developmental processes operate on longer timescales
     * and contribute to brain maturation and adaptation.
     * 
     * @post Brain structure may be modified based on developmental rules
     * @pre Brain must be initialized
     * 
     * @note Development occurs on a much slower timescale than synaptic plasticity
     *       and typically spans hours to days of simulation time
     * 
     * @see DevelopmentSystem, getDevelopmentalStage()
     */
    void develop();
    
    /**
     * @brief Reset the brain to its initial state
     * 
     * This method resets all neural components to their initial states as defined
     * in the configuration. This is useful for starting a new simulation run or
     * recovering from corrupted state.
     * 
     * @post All neural state is reset to initial values
     * @pre Brain must be initialized
     * @note This operation cannot be undone - it's a hard reset
     */
    void reset();
    
    /**
     * @brief Save brain state to a file for checkpointing and persistence
     * 
     * This method serializes the complete brain state (neurons, synapses, weights,
     * internal subsystem states) to a file. This can be used for saving progress,
     * restarting simulations, or creating checkpoint files for recovery.
     * 
     * @param filepath Path to the file where brain state will be saved
     * 
     * @return bool True if save succeeds, false otherwise
     * 
     * @retval true Save completed successfully
     * @retval false Failed to save (file permissions, disk space, serialization error)
     * 
     * @post Brain state is persisted to file
     * @pre Brain must be initialized
     * 
     * @throws NLMError if file operations fail
     * 
     * @note Saved state can be loaded later with load() method
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from a file
     * 
     * This method deserializes brain state from a file saved by the save() method.
     * This allows for restoring simulation progress, loading pretrained models,
     * or recovering from checkpoints.
     * 
     * @param filepath Path to the file containing saved brain state
     * 
     * @return bool True if load succeeds, false otherwise
     * 
     * @retval true Load completed successfully
     * @retval false Failed to load (file not found, corrupted data, version mismatch)
     * 
     * @post Brain state is restored from file
     * @pre Brain has been properly initialized
     * 
     * @throws NLMError if file operations fail or data is invalid
     * 
     * @note Loaded state replaces current brain state completely
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region to the brain
     * 
     * This method creates a new neural region (e.g., cortical area, subcortical nucleus)
     * with the specified name. Regions are logical groupings of neurons that serve
     * different computational functions in the brain.
     * 
     * @param name Optional name for the new region (empty string for auto-generated)
     * 
     * @return RegionId Unique identifier for the newly created region
     * 
     * @post New region is created and added to brain's region collection
     * @pre Brain must be initialized
     * 
     * @retval > 0 Valid region ID
     * @retval 0 Invalid region ID (should not happen with valid input)
     * 
     * @see NeuralRegion, RegionId
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a reference to a neural region by its ID
     * 
     * This method provides access to a specific neural region for inspection or
     * modification of its contents and properties.
     * 
     * @param id Region ID of the neural region to access
     * 
     * @return NeuralRegion* Pointer to the neural region (nullptr if not found)
     * 
     * @retval non-null Pointer to the neural region
     * @retval nullptr Region with specified ID does not exist
     * 
     * @post Caller gains access to the neural region
     * 
     * @see NeuralRegion, addRegion(), getRegionCount()
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a const reference to a neural region by its ID
     * 
     * This version provides read-only access to a neural region, useful for
     * inspection without modifying region contents.
     * 
     * @param id Region ID of the neural region to access
     * 
     * @return const NeuralRegion* Const pointer to the neural region (nullptr if not found)
     * 
     * @retval non-null Const pointer to the neural region
     * @retval nullptr Region with specified ID does not exist
     * 
     * @post Caller gains read-only access to the neural region
     * 
     * @see NeuralRegion, getRegion(RegionId id)
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get the number of neural regions currently in the brain
     * 
     * @return size_t Number of neural regions
     * 
     * @post Count reflects current brain architecture
     * 
     * @see NeuralRegion, addRegion(), getRegionIds()
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs currently in the brain
     * 
     * @return std::vector<RegionId> Vector containing all region IDs
     * 
     * @post Vector contains all region IDs for iteration
     * 
     * @note Region IDs are unique but may have gaps if regions were removed
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all neural regions in the brain
     * 
     * This method provides access to the complete collection of neural regions
     * in the brain, allowing for comprehensive inspection of the brain's
     * architecture and all contained neural populations.
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Reference to region's vector
     * 
     * @post Caller gains access to all regions for iteration or inspection
     * 
     * @note The vector contains ownership of region objects
     * 
     * @see NeuralRegion, getRegionCount()
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add a connection between two neural regions (long-range connectivity)
     * 
     * This method establishes a structural connection between two neural regions,
     * enabling inter-regional communication through long-range axonal projections.
     * These connections are used for integrating information across different
     * brain areas and implementing global brain dynamics.
     * 
     * @param source Region ID of the source region
     * @param target Region ID of the target region
     * @param weight Initial connection strength (default: 0.0f)
     * @param delay Transmission delay in simulation steps (default: 1)
     * 
     * @post Inter-regional connection is established
     * @pre Both regions must exist in the brain
     * 
     * @warning Connection weights and delays affect brain dynamics significantly.
     *          Use appropriate values based on biological plausibility or
     *          experimental requirements.
     * 
     * @see InterRegionConnection, removeInterRegionConnection()
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove a connection between two neural regions
     * 
     * This method removes an existing inter-regional connection, effectively
     * disconnecting those two brain regions from each other.
     * 
     * @param source Region ID of the source region
     * @param target Region ID of the target region
     * 
     * @post Inter-regional connection is removed (if it existed)
     * @pre Both regions must exist in the brain (if connection exists)
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get the total number of neurons in the brain
     * 
     * This includes all neurons across all regions and populations, both active
     * and inactive. This is a global count of the brain's computational elements.
     * 
     * @return size_t Total number of neurons
     * 
     * @post Count reflects current brain size
     * 
     * @see Neuron, getTotalSynapseCount(), getActiveNeuronCount()
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the total number of synapses in the brain
     * 
     * This includes all synaptic connections between neurons, representing the
     * brain's connectivity and communication structure. Each synapse enables
     * communication between a pre-synaptic and post-synaptic neuron.
     * 
     * @return size_t Total number of synapses
     * 
     * @post Count reflects current brain connectivity
     * 
     * @see Synapse, getTotalNeuronCount()
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get the number of currently active neurons
     * 
     * Active neurons are those that are not in resting state - either firing,
     * in refractory period, or otherwise engaged in computation. This count
     * helps monitor the brain's engagement level.
     * 
     * @return size_t Number of active neurons
     * 
     * @post Count reflects current neural activity level
     * 
     * @see getTotalNeuronCount(), getFiringNeuronCount()
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the number of currently firing neurons
     * 
     * Firing neurons are those that have generated action potentials and are
     * currently in the process of transmitting signals to downstream neurons.
     * This is a key measure of neural activity and information processing.
     * 
     * @return size_t Number of firing neurons
     * 
     * @post Count reflects current spike activity
     * 
     * @see getTotalNeuronCount(), getActiveNeuronCount()
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons
     * 
     * This provides a global measure of brain activity and computational
     * load. The average is computed across all neurons in the brain.
     * 
     * @return float Average firing rate (in Hz)
     * 
     * @post Average is computed from current neuron states
     * 
     * @retval 0.0f No neurons are firing
     * @retval > 0.0f Positive firing activity
     * 
     * @see getFiringNeuronCount(), getTotalNeuronCount()
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Access the working memory system for transient information storage
     * 
     * Working memory maintains active information needed for ongoing cognitive
     * tasks, decision making, and immediate computation. It operates on a
     * fast timescale and is essential for real-time processing.
     * 
     * @return NeuralWorkingMemory* Pointer to the working memory system
     * 
     * @post Caller gains direct access to working memory
     * @note Working memory contains representations that are currently being
     *       actively used by cognitive processes
     * 
     * @see NeuralWorkingMemory, getEpisodicMemory(), getAssociativeMemory()
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Access the episodic memory system for experience storage
     * 
     * Episodic memory stores detailed representations of past experiences in
     * temporal order, allowing for reconstruction of previous events and
     * learning from past mistakes or successes.
     * 
     * @return NeuralEpisodicMemory* Pointer to the episodic memory system
     * 
     * @post Caller gains direct access to episodic memory
     * @note Episodic memory is crucial for planning, decision making, and
     *       adaptive behavior based on past experience
     * 
     * @see NeuralEpisodicMemory, getWorkingMemory(), getAssociativeMemory()
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Access the associative memory system for pattern associations
     * 
     * Associative memory stores associations between related concepts, patterns,
     * or events. This enables pattern completion, recognition, and inference
     * based on partial or degraded inputs.
     * 
     * @return NeuralAssociativeMemory* Pointer to the associative memory system
     * 
     * @post Caller gains direct access to associative memory
     * @note Associative memory supports generalization and transfer learning
     * 
     * @see NeuralAssociativeMemory, getWorkingMemory(), getEpisodicMemory()
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Access the prediction system for sensory prediction and error computation
     * 
     * The prediction system implements predictive coding and error-based learning,
     * allowing the brain to generate predictions about sensory input and update
     * internal models based on prediction errors. This is fundamental to perception
     * and adaptive behavior.
     * 
     * @return PredictionSystem* Pointer to the prediction system
     * 
     * @post Caller gains direct access to prediction and error computation
     * @note Prediction errors drive learning and motivate exploratory behavior
     * 
     * @see PredictionSystem, NeuralPlanner, getPredictionErrorSignal()
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Access the neural planner for action planning
     * 
     * The neural planner generates sequences of actions to achieve goals based on
     * current brain state, memory, and environmental context. It implements
     * forward planning and decision making at the neural level.
     * 
     * @return NeuralPlanner* Pointer to the neural planner
     * 
     * @post Caller gains direct access to action planning
     * @note Planning operates on multiple timescales and integrates with memory
     *       and prediction systems
     * 
     * @see NeuralPlanner, getConceptFormation(), getAttention()
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Access the concept formation system for pattern discovery
     * 
     * Concept formation discovers abstract patterns and categories from sensory
     * input and experience, enabling generalization, categorization, and
     * abstract reasoning. This is essential for high-level cognition.
     * 
     * @return ConceptFormation* Pointer to the concept formation system
     * 
     * @post Caller gains direct access to concept formation
     * @note Concepts support transfer learning and abstract thought
     * 
     * @see ConceptFormation, getPlanner(), getAttention()
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Access the attentional selection system for focus
     * 
     * Attentional selection determines which neural representations receive
     * processing resources and priority. This system implements selective
     * attention mechanisms that filter and prioritize information processing.
     * 
     * @return AttentionalSelection* Pointer to the attentional selection system
     * 
     * @post Caller gains direct access to attentional control
     * @note Attention enhances relevant signals while suppressing irrelevant ones
     * 
     * @see AttentionalSelection, getPlanner(), getConceptFormation()
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Access the development system for brain maturation and adaptation
     * 
     * The development system manages brain maturation over time, including
     * synaptogenesis, pruning, myelination, and regional specialization. This
     * is particularly important for developmental models and lifespan studies.
     * 
     * @return DevelopmentSystem* Pointer to the development system
     * 
     * @post Caller gains direct access to developmental processes
     * @note Development spans multiple timescales and affects brain structure
     *       throughout the simulation
     * 
     * @see DevelopmentSystem, getDevelopmentalStage()
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get the current developmental stage of the brain
     * 
     * This method returns the current developmental stage which determines which
     * developmental processes are active and their intensity. Different stages
     * have different characteristic behaviors and learning capabilities.
     * 
     * @return DevelopmentalStage Current developmental stage
     * 
     * @post Stage reflects current developmental state
     * 
     * @retval DevelopmentalStage::Juvenile Early learning phase with high plasticity
     * @retval DevelopmentalStage::Adolescence Transitional phase with mixed capabilities
     * @retval DevelopmentalStage::Mature Full adult functionality with refined circuits
     * 
     * @see DevelopmentSystem, setDevelopmentalStage()
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set the current developmental stage of the brain
     * 
     * This method allows explicit control over developmental processes by
     * setting the current developmental stage. This is useful for testing
     * stage-specific behaviors or implementing custom developmental trajectories.
     * 
     * @param stage Target developmental stage to set
     * 
     * @post Developmental stage is updated and appropriate processes are activated
     * 
     * @warning Changing developmental stage may cause unexpected behavior if not
     *          handled carefully, as different stages have different characteristics.
     * 
     * @see DevelopmentSystem, getDevelopmentalStage()
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @brief Access the dopamine system for reward and reinforcement learning
     * 
     * The dopamine system implements reward prediction error signaling and
     * reinforcement learning. Dopamine neurons encode reward information and
     * modulate plasticity based on reward feedback, driving learning toward
     * maximizing future rewards.
     * 
     * @return Dopamine* Pointer to the dopamine neuromodulator
     * 
     * @post Caller gains direct access to reward processing
     * @note Dopamine is a key signal for motivation, learning, and exploration
     * 
     * @see Dopamine, getCuriosity(), getNovelty(), getPredictionErrorSignal()
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Access the curiosity system for exploration motivation
     * 
     * The curiosity system implements intrinsic motivation and exploratory
     * behavior based on prediction errors and novelty detection. This system
     * drives agents to seek out informative experiences and learn about
     * their environment.
     * 
     * @return Curiosity* Pointer to the curiosity neuromodulator
     * 
     * @post Caller gains direct access to exploration motivation
     * 
     * @see Curiosity, getDopamine(), getNovelty()
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Access the novelty detection system for surprise detection
     * 
     * The novelty system detects unexpected or surprising events in the
     * environment, signaling information gain and driving learning about
     * novel patterns and regularities.
     * 
     * @return Novelty* Pointer to the novelty detection system
     * 
     * @post Caller gains direct access to novelty detection
     * 
     * @see Novelty, getDopamine(), getCuriosity()
     */
    Novelty* getNovelty();
    
    /**
     * @brief Access the prediction error signal system
     * 
     * The prediction error system computes the difference between expected
     * and actual sensory input, providing a fundamental learning signal for
     * predictive coding and temporal difference learning.
     * 
     * @return PredictionError* Pointer to the prediction error signal
     * 
     * @post Caller gains direct access to prediction error computation
     * 
     * @see PredictionError, getPredictionSystem(), getDopamine()
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Access the current configuration object for parameter inspection
     * 
     * This method provides read-only access to the Brain's configuration object,
     * allowing inspection of all parameters, validation results, and settings.
     * This is useful for debugging, monitoring, and understanding the current
     * brain state and configuration.
     * 
     * @return std::shared_ptr<const Config> Const shared pointer to configuration
     * 
     * @post Configuration object can be inspected (read-only)
     * 
     * @note Configuration changes are made through the Config object before
     *       brain initialization. After initialization, many parameters become
     *       read-only.
     * 
     * @see Config, initialize()
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Access the random number generator for stochastic operations
     * 
     * This method provides access to the brain's random number generator,
     * which is used for stochastic initialization, noise injection, and
     * probabilistic computations. The random generator is typically seeded
     * from configuration parameters.
     * 
     * @return RandomGenerator* Pointer to the random number generator
     * 
     * @post Caller gains access to random number generation
     * @note Randomness is essential for learning, exploration, and initialization
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log current brain status for monitoring and debugging
     * 
     * This method generates a comprehensive status report including key
     * statistics, subsystem states, and diagnostic information. The status
     * is typically logged to the logging system for monitoring and analysis.
     * 
     * @post Brain status is logged for monitoring
     * 
     * @see Logger, getTotalNeuronCount(), getAverageFiringRate()
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
