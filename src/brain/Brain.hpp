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

/**
 * @class Brain
 * @brief The central coordinator of the neural system (Phase 6: Integrated Artificial Brain)
 * 
 * This class implements real spiking neural computation with event-driven dynamics
 * and provides integration of multiple cognitive and biological systems:
 * - Memory systems (working, episodic, associative)
 * - Prediction and planning systems
 * - Development and neuromodulation systems
 * - Sensory and motor integration
 * 
 * The Brain class serves as the main controller for the NLM simulation,
 * managing regions, neurons, synapses, and coordinating between different
 * subsystems. It supports advanced biological-inspired neural computation
 * including spike-timing dependent plasticity (STDP), structural plasticity,
 * and developmental processes.
 * 
 * @note This is Phase 6 of the NLM (Neural Learning Machine) implementation,
 *       representing an integrated artificial brain with multiple interacting
 *       neural systems. The design supports real-time simulation with event-driven
 *       spike propagation and complex inter-regional connectivity.
 * 
 * @warning Memory management is handled internally with proper RAII patterns.
 *         Configuration must be valid and contain necessary parameters.
 * 
 * @see SpikeSystem, STDP, StructuralPlasticity, NeuralRegion
 * @see NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory
 * @see PredictionSystem, NeuralPlanner, ConceptFormation
 * @see DevelopmentSystem, Dopamine, Curiosity, PredictionError
 * 
 * @since Version 0.1.0 (Phase 6)
 */

class Brain {
public:
    /**
     * @brief Constructor - creates a new Brain instance with configuration
     * 
     * @param config Shared pointer to configuration object containing
     *               brain parameters, simulation settings, and system
     *               configuration options. The configuration must be valid
     *               and contain essential parameters like neuron_count,
     *               region_count, random_seed, and other simulation parameters.
     * 
     * @throws std::invalid_argument if config is nullptr
     * 
     * @post Brain is initialized but not yet configured/activated.
     *       Call initialize() to set up neurons, regions, and systems.
     * 
     * @note Configuration is stored as a shared pointer to allow sharing
     *       across multiple Brain instances and maintain consistency.
     * 
     * @see initialize()
     * @see getConfig()
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor - cleans up brain resources
     * 
     * Properly destroys the Brain instance and releases all internal
     * resources including regions, neurons, synapses, and integrated systems.
     * The destructor is noexcept to ensure proper cleanup during exceptions.
     */
    ~Brain();
    
    /**
     * @brief Copy constructor - disabled to prevent shallow copying
     * 
     * Brain instances cannot be copied to prevent multiple ownership of
     * shared resources and ensure proper resource management. Always use
     * move semantics instead.
     * 
     * @warning Undefined behavior if attempted
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Copy assignment operator - disabled
     * 
     * Brain instances cannot be assigned to prevent shallow copying and
     * resource management issues. The operation is marked delete to ensure
     * compliance with the rule of three/five.
     * 
     * @warning Undefined behavior if attempted
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor - enables efficient Brain transfer
     * 
     * Move constructor allows transferring ownership of the internal implementation
     * from one Brain instance to another, avoiding deep copy costs. After moving,
     * the source object is left in a valid but unspecified state.
     * 
     * @param other The Brain instance to move from (will be left in valid state)
     * 
     * @post Current instance takes ownership of other's internal resources.
     *       Other object is left in a valid but unspecified state.
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator - enables efficient Brain transfer
     * 
     * Move assignment allows transferring ownership of internal resources
     * from one Brain instance to another, similar to move constructor but
     * for assignment operations.
     * 
     * @param other The Brain instance to move from (will be left in valid state)
     * 
     * @return Reference to this Brain after assignment
     * 
     * @post Current instance takes ownership of other's internal resources.
     *       Other object is left in a valid but unspecified state.
     *       Self-assignment is handled correctly.
     */
    Brain& operator=(Brain&&) noexcept;
    
/**
 * @class Brain
 * @brief The central coordinator of the neural system (Phase 6: Integrated Artificial Brain)
 * 
 * This class implements real spiking neural computation with event-driven dynamics
 * and provides integration of multiple cognitive and biological systems:
 * - Memory systems (working, episodic, associative)
 * - Prediction and planning systems
 * - Development and neuromodulation systems
 * - Sensory and motor integration
 * 
 * The Brain class serves as the main controller for the NLM simulation,
 * managing regions, neurons, synapses, and coordinating between different
 * subsystems. It supports advanced biological-inspired neural computation
 * including spike-timing dependent plasticity (STDP), structural plasticity,
 * and developmental processes.
 * 
 * @note This is Phase 6 of the NLM (Neural Learning Machine) implementation,
 *       representing an integrated artificial brain with multiple interacting
 *       neural systems. The design supports real-time simulation with event-driven
 *       spike propagation and complex inter-regional connectivity.
 * 
 * @warning Memory management is handled internally with proper RAII patterns.
 *         Configuration must be valid and contain necessary parameters.
 * 
 * @see SpikeSystem, STDP, StructuralPlasticity, NeuralRegion
 * @see NeuralWorkingMemory, NeuralEpisodicMemory, NeuralAssociativeMemory
 * @see PredictionSystem, NeuralPlanner, ConceptFormation
 * @see DevelopmentSystem, Dopamine, Curiosity, PredictionError
 * 
 * @since Version 0.1.0 (Phase 6)
 */

class Brain {
public:
    /**
     * @brief Constructor - creates a new Brain instance with configuration
     * 
     * @param config Shared pointer to configuration object containing
     *               brain parameters, simulation settings, and system
     *               configuration options. The configuration must be valid
     *               and contain essential parameters like neuron_count,
     *               region_count, random_seed, and other simulation parameters.
     * 
     * @throws std::invalid_argument if config is nullptr
     * 
     * @post Brain is initialized but not yet configured/activated.
     *       Call initialize() to set up neurons, regions, and systems.
     * 
     * @note Configuration is stored as a shared pointer to allow sharing
     *       across multiple Brain instances and maintain consistency.
     * 
     * @see initialize()
     * @see getConfig()
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor - cleans up brain resources
     * 
     * Properly destroys the Brain instance and releases all internal
     * resources including regions, neurons, synapses, and integrated systems.
     * The destructor is noexcept to ensure proper cleanup during exceptions.
     */
    ~Brain();
    
    /**
     * @brief Copy constructor - disabled to prevent shallow copying
     * 
     * Brain instances cannot be copied to prevent multiple ownership of
     * shared resources and ensure proper resource management. Always use
     * move semantics instead.
     * 
     * @warning Undefined behavior if attempted
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Copy assignment operator - disabled
     * 
     * Brain instances cannot be assigned to prevent shallow copying and
     * resource management issues. The operation is marked delete to ensure
     * compliance with the rule of three/five.
     * 
     * @warning Undefined behavior if attempted
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor - enables efficient Brain transfer
     * 
     * Move constructor allows transferring ownership of the internal implementation
     * from one Brain instance to another, avoiding deep copy costs. After moving,
     * the source object is left in a valid but unspecified state.
     * 
     * @param other The Brain instance to move from (will be left in valid state)
     * 
     * @post Current instance takes ownership of other's internal resources.
     *       Other object is left in a valid but unspecified state.
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator - enables efficient Brain transfer
     * 
     * Move assignment allows transferring ownership of internal resources
     * from one Brain instance to another, similar to move constructor but
     * for assignment operations.
     * 
     * @param other The Brain instance to move from (will be left in valid state)
     * 
     * @return Reference to this Brain after assignment
     * 
     * @post Current instance takes ownership of other's internal resources.
     *       Other object is left in a valid but unspecified state.
     *       Self-assignment is handled correctly.
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * Sets up the brain with neurons, regions, and integrated systems based on
     * the configuration parameters. This must be called after construction before
     * starting simulation. Cannot be called multiple times on the same instance.
     * 
     * @return bool true if initialization succeeded, false if initialization failed
     * 
     * @throws std::runtime_error if initialization fails due to configuration errors
     * 
     * @post Brain is ready for simulation. Regions, neurons, and all integrated
     *       systems are created and configured.
     * 
     * @note This method sets up the brain with configurable parameters including
     *       neuron count, region count, connection probability, and system parameters.
     *       Default values are used if parameters are not specified in configuration.
     * 
     * @see Brain(std::shared_ptr<Config> config)
     * @see getConfig()
     */
    bool initialize();
    
    /**
     * @brief Execute a simulation step with step count only
     * 
     * Performs a single simulation step using the provided step count. Time is
     * incremented internally based on the configured timestep.
     * 
     * @param currentStep The simulation step number for this step
     * 
     * @post All neurons, synapses, and integrated systems have been updated
     *       for this step. Spike events have been processed and recorded.
     * 
     * @note This is the primary simulation loop method. It coordinates all systems
     *       including neuron dynamics, plasticity updates, development processes,
     *       and neuromodulation. Called once per simulation timestep.
     * 
     * @warning Multiple threads should not call this method simultaneously on
     *         the same Brain instance without proper synchronization.
     * 
     * @see step(SimulationStep currentStep, Timestamp currentTime)
     * @see updatePlasticity()
     * @see develop()
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Execute a simulation step with both step and time parameters
     * 
     * Performs a single simulation step using both step count and current time.
     * This provides more detailed time tracking for temporal processes.
     * 
     * @param currentStep The simulation step number for this step
     * @param currentTime The current simulation time in seconds
     * 
     * @post All systems have been updated with the new time information.
     *       Spike propagation and all biological processes have been executed.
     * 
     * @note This variant provides more accurate temporal simulation for systems
     *       that depend on precise timing, such as plasticity rules and
     *       developmental processes.
     * 
     * @see step(SimulationStep currentStep)
     * @see SimulationClock
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Injects current into sensory neurons based on the input pattern from
     * the environment. This simulates the brain's reception of external stimuli.
     * 
     * @param input The SensoryInput object containing the input pattern,
     *              type, and intensity. The input must be properly configured
     *              with valid values and neuron targets.
     * 
     * @throws std::invalid_argument if input is nullptr
     * @throws std::runtime_error if input processing fails
     * 
     * @post Sensory neurons receive the injected current, modifying their
     *       membrane potentials and potentially triggering spike activity.
     *       Input is recorded in working memory for later processing.
     * 
     * @note This method integrates external sensory information into the
     *       neural system, enabling perception and response generation.
     *       The input pattern can represent various sensory modalities.
     * 
     * @see SensoryInput
     * @see injectCurrent(NeuronId neuron, MembranePotential current)
     * @see getWorkingMemory()
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Directly injects current into a specific neuron, simulating synaptic
     * input or external stimulation. This is a low-level method for precise
     * neural control.
     * 
     * @param neuron The ID of the neuron to receive current
     * @param current The amount of current to inject (in nanoamperes)
     * 
     * @throws std::invalid_argument if neuron ID is invalid or current is negative
     * 
     * @post The specified neuron's membrane potential is increased by the injected
     *       current, potentially triggering an action potential if threshold is reached.
     * 
     * @note This method provides fine-grained control over individual neuron activity,
     *       useful for testing, debugging, or targeted neural stimulation experiments.
     * 
     * @see receiveSensoryInput(const SensoryInput& input)
     * @see injectCurrentToNeurons(NeuronType type, MembranePotential current)
     * @see Neuron::injectCurrent()
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Injects current into all neurons of a specified type (e.g., all sensory,
     * all motor, or all internal neurons). This simulates population-level input.
     * 
     * @param type The neuron type to target (Sensory, Motor, or Internal)
     * @param current The amount of current to inject to each neuron (nA)
     * 
     * @post All neurons of the specified type receive the injected current.
     *       This can synchronously activate neural populations.
     * 
     * @note This method is more efficient than individual neuron injections when
     *       targeting large neuron populations. Useful for simulating coordinated
     *       neural activity patterns.
     * 
     * @see injectCurrent(NeuronId neuron, MembranePotential current)
     * @see NeuronType
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get access to the spike system
     * 
     * @return SpikeSystem* Pointer to the spike system instance
     * 
     * @post Caller must not delete the returned pointer. Spike system is owned
     *       by this Brain instance and remains valid for the lifetime of the Brain.
     * 
     * @note The spike system manages all spike generation, propagation, and
     *       timing for the neural network. It implements the core event-driven
     *       neural computation.
     * 
     * @see getSpikeSystem() const
     * @see SpikeSystem
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get constant access to the spike system
     * 
     * @return const SpikeSystem* Const pointer to the spike system instance
     * 
     * @post Similar to non-const version but provides read-only access.
     * 
     * @see getSpikeSystem()
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get access to the Spike-Timing-Dependent Plasticity (STDP) system
     * 
     * @return STDP* Pointer to the STDP system instance
     * 
     * @post STDP system manages synaptic strengthening and weakening based on
     *       precise spike timing relationships between pre- and post-synaptic neurons.
     * 
     * @note STDP is a biological learning rule that implements Hebbian learning
     *       with temporal asymmetry. It enables the brain to learn temporal sequences
     *       and predictive relationships.
     * 
     * @see getHebbian()
     * @see getStructuralPlasticity()
     */
    STDP* getSTDP();
    
    /**
     * @brief Get access to the Hebbian plasticity system
     * 
     * @return Hebbian* Pointer to the Hebbian system instance
     * 
     * @post Hebbian system implements classic associative learning where
     *       simultaneous activation strengthens synaptic connections.
     * 
     * @note Hebbian plasticity implements the principle "neurons that fire together
     *       wire together". It's useful for pattern association and memory formation.
     * 
     * @see getSTDP()
     * @see getStructuralPlasticity()
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get access to the structural plasticity system
     * 
     * @return StructuralPlasticity* Pointer to the structural plasticity instance
     * 
     * @post Structural plasticity system manages synapse creation, elimination,
     *       and rewiring, affecting the network's topology over time.
     * 
     * @note This system implements biological-inspired structural changes including
     *       synaptogenesis (new synapse formation) and pruning (synapse elimination),
     *       enabling the network to adapt its structure based on activity patterns.
     * 
     * @see getSTDP()
     * @see getHebbian()
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation-inhibition ratio
     * 
     * Calculates the ratio of total excitatory current to total inhibitory current
     * across the entire network, providing a measure of network balance.
     * 
     * @return float The E/I ratio (typically ranges from 0.5 to 2.0 in stable networks)
     * 
     * @post Network stability can be assessed through this ratio. Values significantly
     *       different from 1.0 may indicate excitation dominance or inhibition weakness.
     * 
     * @note This metric is important for understanding network dynamics and stability.
     *       Extremely high ratios may lead to seizures (excitation dominance),
     *       while very low ratios may lead to network silencing (inhibition dominance).
     * 
     * @see getTotalNeuronCount()
     * @see getActiveNeuronCount()
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count since brain creation
     * 
     * @return size_t Total number of spikes generated by all neurons since brain
     *         was created (includes spikes from initialization if any).
     * 
     * @post Useful for monitoring brain activity levels and assessing learning progress.
     *       Used in experiments to track neural activation patterns over time.
     * 
     * @note This counter accumulates across the entire simulation lifetime and
     *       is not reset during normal operation (use reset() to clear it).
     * 
     * @see getTotalSpikeCount()
     * @see getFiringNeuronCount()
     * @see reset()
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of neurons with pending spike events
     * 
     * @return size_t Number of neurons that have spiked in the current step
     *         but haven't had their spike effects fully propagated yet.
     * 
     * @post Pending spikes are those that have been generated but are still
     *       waiting in the spike queue to be delivered to target neurons.
     * 
     * @note This count can help assess the timing and propagation of neural activity
     *       through the network. High values may indicate spike synchronization.
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * @return std::unique_ptr<Action> Action object representing the motor output
     *         generated by the brain's motor neurons. Returns nullptr if no action
     *         is generated or motor system is not active.
     * 
     * @post The action encodes the brain's motor response based on current neural
     *       activity patterns, sensory input, and cognitive planning.
     * 
     * @note This method integrates motor neuron activity into executable actions,
     *       bridging internal neural computation with external behavior. The action
     *       can be used to control agents, robots, or other output systems.
     * 
     * @see Action
     * @see injectCurrentToNeurons(NeuronType type, MembranePotential current)
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     * 
     * Applies neuromodulatory signals that influence neural activity and plasticity
     * in a globally coordinated manner. Neuromodulators like dopamine, serotonin,
     * and norepinephrine can alter learning rates, motivation, attention, etc.
     * 
     * @param signal The Neuromodulator object containing the type, intensity,
     *               and temporal characteristics of the neuromodulatory signal.
     * 
     * @throws std::invalid_argument if signal is nullptr
     * 
     * @post Neuromodulators affect multiple brain systems simultaneously,
     *       influencing learning, motivation, attention, and other cognitive functions.
     * 
     * @note Neuromodulation is crucial for adaptive behavior, enabling the brain
     *       to adjust its learning and performance based on environmental context,
     *       reward prediction, and other factors.
     * 
     * @see Neuromodulator
     * @see getDopamine()
     * @see getCuriosity()
     * @see getPredictionErrorSignal()
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * Updates synaptic weights and other plastic properties based on current neural
     * activity and plasticity rules (STDP, Hebbian, etc.). This is typically called
     * automatically during simulation steps.
     * 
     * @post Synaptic weights are adjusted according to plastic rules. The network
     *       connectivity and strength evolve based on neural activity patterns.
     * 
     * @note Plasticity is the mechanism by which the brain learns and adapts. This
     *       method implements the biological learning rules that enable the network
     *       to extract statistical regularities from experience and develop complex
     *       behaviors.
     * 
     * @see step(SimulationStep currentStep)
     * @see getSTDP()
     * @see getHebbian()
     * @see getStructuralPlasticity()
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * Applies developmental processes to the network, including structural changes,
     * maturation, and activity-dependent refinement. Called automatically during
     * simulation steps.
     * 
     * @post Network structure and properties evolve according to developmental rules.
     *       This includes synaptic pruning, structural plasticity, and activity-dependent
     *       refinement.
     * 
     * @note Development is the process by which the brain matures from a simple
     *       network into a complex, organized system. This method implements the
     *       biological developmental processes that shape the network's final architecture.
     * 
     * @see step(SimulationStep currentStep)
     * @see getDevelopmentSystem()
     * @see getStructuralPlasticity()
     */
    void develop();
    
    /**
     * @brief Reset brain state
     * 
     * Resets all internal brain state to initial conditions, clearing statistics,
     * resetting neurons to resting state, and clearing all temporary data structures.
     * Note that configuration and basic network structure remain intact.
     * 
     * @post All statistics are cleared, neurons are reset to resting state, and all
     *       temporary buffers and caches are cleared. The brain is returned to its
     *       initial post-initialization state.
     * 
     * @note Reset does not affect the brain's configuration, network structure,
     *       or connectivity. It only clears dynamic state information. Can be used
     *       for experiments requiring clean state or to recover from error states.
     * 
     * @see initialize()
     * @see getTotalSpikeCount()
     * @see getTotalNeuronCount()
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Saves the current brain state to a file for checkpointing and recovery. This
     * includes neural connectivity, weights, neuron states, and system parameters.
     * 
     * @param filepath Path to the file where the brain state will be saved
     * 
     * @return bool true if save operation succeeded, false if save failed (e.g.,
     *         file permission error, disk full)
     * 
     * @throws std::invalid_argument if filepath is empty or invalid
     * 
     * @post Brain state including neurons, synapses, weights, and system state
     *       is serialized to the specified file. The file can later be loaded using
     *       load() to restore the brain to this state.
     * 
     * @note Checkpointing enables recovery from errors, supports long-running
     *       simulations, and allows saving intermediate results for analysis.
     *       The saved format is binary for efficiency and compactness.
     * 
     * @see load(const std::string& filepath)
     * @see saveToFile(const std::string& filepath) in Config.hpp
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * Loads brain state from a file, restoring neurons, synapses, weights, and
     * system parameters to a previously saved state.
     * 
     * @param filepath Path to the file containing the saved brain state
     * 
     * @return bool true if load operation succeeded, false if load failed (e.g.,
     *         file not found, corrupted data, incompatible version)
     * 
     * @throws std::invalid_argument if filepath is empty or invalid
     * 
     * @post Brain state is restored from the file. All neurons, synapses, and
     *       system parameters are recreated with their saved values. The brain
     *       assumes the same configuration as when the state was saved.
     * 
     * @note Loading allows recovery from errors, continuation of long simulations,
     *       and replay of previously recorded brain states. The loaded state must
     *       be compatible with the current brain configuration.
     * 
     * @see save(const std::string& filepath) const
     * @see Config::loadFromFile()
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region
     * 
     * Creates and adds a new neural region to the brain. Regions are basic
     * organizational units that contain populations of neurons and local
     * connectivity. Each region has a unique ID and can be named for identification.
     * 
     * @param name Optional name for the region. If empty, a default name
     *             "Region_X" will be generated where X is the region ID.
     * 
     * @return RegionId The unique ID of the newly created region
     * 
     * @post A new region is created and added to the brain's region collection.
     *       The region can then be populated with neurons and synapses.
     * 
     * @note Regions provide modular organization of the neural network, allowing
     *       for spatially and functionally distinct areas. They're used for
     *       implementing complex brain architectures with specialized regions.
     * 
     * @see NeuralRegion
     * @see getRegion(RegionId id)
     * @see getRegionIds()
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a neural region by ID
     * 
     * @param id The unique identifier of the region to retrieve
     * 
     * @return NeuralRegion* Pointer to the requested region, or nullptr if
     *         region with given ID doesn't exist
     * 
     * @post The returned pointer is valid for the lifetime of the Brain instance.
     *       Caller should not delete the returned pointer.
     * 
     * @see getRegion(RegionId id) const
     * @see getRegionIds()
     * @see addRegion(const std::string& name)
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a neural region by ID (const version)
     * 
     * @param id The unique identifier of the region to retrieve
     * 
     * @return const NeuralRegion* Const pointer to the requested region, or nullptr
     *         if region with given ID doesn't exist
     * 
     * @post Similar to non-const version but provides read-only access.
     * 
     * @see getRegion(RegionId id)
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get the number of regions in the brain
     * 
     * @return size_t The current number of regions in the brain
     * 
     * @post Reflects the current state of region creation and destruction.
     *       Changes when regions are added or removed.
     * 
     * @note This count includes all regions created via addRegion().
     *       Regions are fundamental organizational units of the neural network.
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * 
     * @return std::vector<RegionId> Vector containing all region IDs currently
     *         in the brain
     * 
     * @post The returned vector contains all region IDs in no specific order.
     *       Order may change if regions are added or removed.
     * 
     * @note This provides a complete list of all regions for iteration and
     *       bulk operations. Use getRegionCount() for efficiency checks.
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions (const version)
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Reference to the
     *         vector containing all regions. The vector is owned by this Brain
     *         instance.
     * 
     * @post The returned reference remains valid for the lifetime of the Brain.
     *       The vector contains pointers to all regions in the brain.
     * 
     * @note This provides access to all regions for bulk operations and iteration.
     *       For single region access, use getRegion() for better performance.
     * 
     * @see getRegion(RegionId id)
     * @see getRegionCount()
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection
     * 
     * Creates a long-range connection between two regions, allowing neurons in
     * one region to influence neurons in another region. These connections are
     * used to implement inter-regional communication and integration.
     * 
     * @param source Source region ID
     * @param target Target region ID  
     * @param weight Initial synaptic weight for the connection (default: 0.0f)
     * @param delay Delay in steps for the connection (default: 1)
     * 
     * @throws std::invalid_argument if source or target region IDs are invalid
     * 
     * @post A new inter-region connection is created and added to the brain's
     *       connection collection. The connection will be used during spike
     *       propagation to transmit spikes between regions.
     * 
     * @note Inter-region connections enable communication between different brain
     *       regions, implementing long-range connectivity essential for complex
     *       brain function. They're used in implementing coordinated activity
     *       across multiple regions.
     * 
     * @see InterRegionConnection
     * @see removeInterRegionConnection(RegionId source, RegionId target)
     * @see getTotalSynapseCount()
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * 
     * Removes the inter-region connection between two regions.
     * 
     * @param source Source region ID
     * @param target Target region ID
     * 
     * @post The connection between source and target regions is removed from
     *       the brain's connection collection. The connection can be re-added
     *       later if needed.
     * 
     * @note This is useful for dynamically adjusting inter-region connectivity
     *       or removing ineffective connections during learning processes.
     * 
     * @see addInterRegionConnection(RegionId source, RegionId target, float weight, Delay delay)
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count
     * 
     * @return size_t Total number of neurons across all regions in the brain
     * 
     * @post This count includes all neurons in all regions, including sensory,
     *       motor, and internal neurons. It provides a measure of brain size.
     * 
     * @note This count is set during initialization and changes only when
     *       regions are modified or neurons are added/removed programmatically.
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count
     * 
     * @return size_t Total number of synapses across all regions in the brain.
     *         This includes both intra-region and inter-region connections.
     * 
     * @post This count includes all synaptic connections and provides a measure
     *       of brain connectivity complexity.
     * 
     * @note This count is updated dynamically as synapses are created and
     *       eliminated through structural plasticity and other processes.
     * 
     * @see addInterRegionConnection()
     * @see getStructuralPlasticity()
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count
     * 
     * @return size_t Number of neurons that are currently firing (membrane potential
     *         above threshold) in the current simulation step.
     * 
     * @post Active neurons are those that have crossed their firing threshold
     *       and are ready to generate spikes in the current step.
     * 
     * @note This count provides real-time information about neural activity levels
     *       and can be used to monitor brain dynamics and assess network state.
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * 
     * @return size_t Number of neurons that successfully fired spikes in the
     *         current simulation step.
     * 
     * @post Firing neurons are those that actually emitted spikes that will be
     *       propagated to their target neurons in the current step.
     * 
     * @note This count represents the actual output of the neural network in the
     *       current step and is closely related to information processing and
     *       transmission.
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * 
     * @return float Average firing rate across all neurons in the brain (in Hz),
     *         calculated as total spikes per second divided by number of neurons.
     * 
     * @post This provides a measure of overall brain activity and can be used to
     *       assess network state and performance.
     * 
     * @note This metric smooths out moment-to-moment fluctuations and provides
     *       a stable measure of network excitability and information processing capacity.
     * 
     * @see getTotalSpikeCount()
     * @see getTotalNeuronCount()
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Get working memory access
     * 
     * @return NeuralWorkingMemory* Pointer to the working memory system
     * 
     * @post Working memory provides transient storage for active information
     *       that is being processed or used for current decision-making.
     * 
     * @note Working memory is crucial for maintaining information online and
     *       integrating current sensory input with stored knowledge.
     * 
     * @see NeuralWorkingMemory
     * @see receiveSensoryInput()
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory access
     * 
     * @return NeuralEpisodicMemory* Pointer to the episodic memory system
     * 
     * @post Episodic memory stores past experiences and events for later recall.
     *       This enables the brain to learn from experience and generalize.
     * 
     * @note Episodic memory allows the brain to build a temporal narrative of
     *       experiences, supporting planning, prediction, and decision-making.
     * 
     * @see NeuralEpisodicMemory
     * @see getPredictionSystem()
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory access
     * 
     * @return NeuralAssociativeMemory* Pointer to the associative memory system
     * 
     * @post Associative memory stores patterns and their relationships, enabling
     *       pattern completion and association-based reasoning.
     * 
     * @note This system implements classical conditioning and pattern association,
     *       allowing the brain to form connections between related experiences
     *       and stimuli.
     * 
     * @see NeuralAssociativeMemory
     * @see getConceptFormation()
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get prediction system access
     * 
     * @return PredictionSystem* Pointer to the prediction system
     * 
     * @post The prediction system computes predictions about sensory input and
     *       generates prediction error signals, enabling learning and adaptation.
     * 
     * @note This system implements predictive coding and error-based learning,
     *       allowing the brain to minimize prediction errors and learn internal
     *       models of the environment.
     * 
     * @see PredictionSystem
     * @see getPredictionErrorSignal()
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get neural planner access
     * 
     * @return NeuralPlanner* Pointer to the neural planner
     * 
     * @post The neural planner generates action plans and sequences based on
     *       current goals, sensory input, and learned knowledge.
     * 
     * @note This system implements action planning and hierarchical control,
     *       enabling the brain to formulate and execute complex behaviors.
     * 
     * @see NeuralPlanner
     * @see getConceptFormation()
     * @see getAttention()
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation access
     * 
     * @return ConceptFormation* Pointer to the concept formation system
     * 
     * @post Concept formation extracts abstract patterns and concepts from
     *       sensory input and experiences, enabling generalization and reasoning.
     * 
     * @note This system implements pattern discovery and abstraction, allowing
     *       the brain to form high-level representations of the environment.
     * 
     * @see ConceptFormation
     * @see getAssociativeMemory()
     * @see getAttention()
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection access
     * 
     * @return AttentionalSelection* Pointer to the attentional selection system
     * 
     * @post Attentional selection determines which neural representations and
     *       processes receive priority for processing and resource allocation.
     * 
     * @note This system implements selective attention, enabling the brain to
     *       focus resources on relevant information and ignore irrelevant noise.
     * 
     * @see AttentionalSelection
     * @see getPlanner()
     * @see getConceptFormation()
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get development system access
     * 
     * @return DevelopmentSystem* Pointer to the development system
     * 
     * @post The development system manages the brain's maturation and structural
     *       development processes, including synaptogenesis and pruning.
     * 
     * @note This system implements developmental processes that shape the
     *       brain's structure and function over time, similar to biological brain
     *       development.
     * 
     * @see DevelopmentSystem
     * @see develop()
     * @see getStructuralPlasticity()
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * @return DevelopmentalStage The current developmental stage of the brain
     * 
     * @post Developmental stages represent different phases of brain maturation
     *       and organization, from initial to mature states.
     * 
     * @note The brain progresses through developmental stages during initialization
     *       and development processes. Each stage has characteristic properties
     *       and behaviors.
     * 
     * @see DevelopmentSystem
     * @see setDevelopmentalStage()
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * @param stage The developmental stage to set
     * 
     * @post The brain's developmental stage is changed to the specified value.
     *       This may trigger developmental processes and structural changes.
     * 
     * @note This method allows manual control over development, useful for
     *       experiments and testing. The brain should typically progress through
     *       stages naturally via development() method.
     * 
     * @see getDevelopmentalStage()
     * @see develop()
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get dopamine access
     * 
     * @return Dopamine* Pointer to the dopamine system
     * 
     * @post Dopamine is a neuromodulator involved in reward processing, reinforcement
     *       learning, and motivation. It influences synaptic plasticity and neural activity.
     * 
     * @note This system implements reward-based learning and motivation, enabling
     *       the brain to learn from rewards and pursue goals.
     * 
     * @see Dopamine
     * @see applyNeuromodulation()
     * @see getCuriosity()
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity access
     * 
     * @return Curiosity* Pointer to the curiosity system
     * 
     * @post Curiosity drives exploration and information seeking behavior,
     *       encouraging the brain to learn about novel and unpredictable environments.
     * 
     * @note This system implements intrinsic motivation and curiosity-driven learning,
     *       enabling the brain to discover new information and develop better models
     *       of the environment.
     * 
     * @see Curiosity
     * @see applyNeuromodulation()
     * @see getNovelty()
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty access
     * 
     * @return Novelty* Pointer to the novelty system
     * 
     * @post Novelty detection identifies unusual or unexpected sensory input,
     *       signaling the brain to pay attention to new information.
     * 
     * @note This system implements change detection and novelty processing,
     *       helping the brain to identify and respond to new environmental patterns.
     * 
     * @see Novelty
     * @see applyNeuromodulation()
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal access
     * 
     * @return PredictionError* Pointer to the prediction error system
     * 
     * @post Prediction error signals the difference between expected and actual
     *       sensory input, driving learning and adaptation.
     * 
     * @note This system implements error-based learning, providing the primary
     *       learning signal for predictive coding and reinforcement learning.
     * 
     * @see PredictionError
     * @see applyNeuromodulation()
     * @see getPredictionSystem()
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get current configuration
     * 
     * @return std::shared_ptr<const Config> Shared pointer to the configuration
     *         object used to create this Brain instance
     * 
     * @post The configuration remains constant throughout the Brain's lifetime
     *       and provides access to simulation parameters and settings.
     * 
     * @note This allows read-only access to the configuration without exposing
     *       the full configuration interface. For modifications, use the Config
     *       class directly before passing it to the Brain constructor.
     * 
     * @see Brain(std::shared_ptr<Config> config)
     * @see Config
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator access
     * 
     * @return RandomGenerator* Pointer to the random number generator
     * 
     * @post The random generator is used for stochastic processes in the brain,
     *       including initial conditions, noise injection, and probabilistic learning.
     * 
     * @note This provides deterministic random number generation when seeded
     *       with a specific seed, enabling reproducible simulations.
     * 
     * @see RandomGenerator
     * @see initialize()
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log brain status
     * 
     * @post Logs comprehensive information about the brain's current state,
     *       including statistics, configuration, and system status. Output is
     *       directed to the logging system configured for the application.
     * 
     * @note This is useful for monitoring brain state, debugging, and logging
     *       simulation progress. Logs include key metrics and system health information.
     * 
     * @see Logger
     * @see getConfig()
     * @see getTotalNeuronCount()
     * @see getTotalSpikeCount()
     */
    void logStatus() const;
    
private:
    /**
     * @brief Internal implementation structure (pImpl idiom)
     * 
     * This structure contains the Brain's implementation details and member variables.
     * It uses the pImpl idiom to hide implementation details from the header file
     * and improve compilation times.
     * 
     * @note This separation allows easier implementation changes without recompiling
     *       all dependent code. The implementation is compiled in Brain.cpp.
     * 
     * @see Brain.cpp
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     * 
     * Pointer to the Brain's internal implementation structure using the pImpl idiom.
     * This maintains abstraction and hides implementation details from the header file.
     * 
     * @note The pointer is lazily initialized and properly managed by the constructor,
     *       move operations, and destructor.
     * 
     * @see Impl
     */
    Impl* pImpl;
};

} // namespace nlm
};

} // namespace nlm
