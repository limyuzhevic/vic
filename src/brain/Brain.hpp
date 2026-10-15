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

/**
 * @brief Inter-regional connection representing long-range connectivity
 * 
 * This structure defines connections between different neural regions,
 * enabling communication and integration across distributed brain areas.
 * 
 * @note Used for implementing inter-regional integration in multi-regional networks
 */
struct InterRegionConnection {
    /** Source region identifier */
    RegionId sourceRegion;
    
    /** Target region identifier */
    RegionId targetRegion;
    
    /** Connection weight */
    float weight;
    
    /** Transmission delay in simulation steps */
    Delay delay;
    
    /** Plasticity flags indicating which plasticity rules apply */
    PlasticityFlags plasticityFlags;
    
    /** Default constructor creates zero-weight connection */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Construct an inter-region connection
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
 * @file Brain.hpp
 * @brief Brain: The central coordinator of the neural system
 * 
 * Implements real spiking neural computation with event-driven dynamics and
 * integrated memory, prediction, cognition, and neuromodulation systems.
 * 
 * The Brain class serves as the main interface for controlling the neural simulation,
 * managing neural populations, processing sensory input, and producing motor output.
 * It coordinates all subsystems including spike dynamics, plasticity, memory,
 * prediction, cognition, development, and neuromodulation.
 * 
 * @author NLM Development Team
 * @version 1.0.0
 * @date 2026
 * 
 * @section brain_usage Basic Usage Example
 * 
 * @code{.cpp}
 * #include <nlm/Brain.hpp>
 * #include <nlm/Config.hpp>
 * #include <memory>
 * 
 * int main() {
 *     // Create configuration
 *     auto config = std::make_shared<nlm::Config>();
 *     config->set("neuron_count", 1000);
 *     config->set("region_count", 2);
 *     
 *     // Create brain
 *     auto brain = std::make_shared<nlm::Brain>(config);
 *     
 *     // Initialize
 *     brain->initialize();
 *     
 *     // Run simulation
 *     for (size_t step = 0; step < 1000; ++step) {
 *         brain->step(step);
 *         
 *         // Add some random input for demonstration
 *         if (step % 100 == 0) {
 *             // Create simple sensory input
 *             auto sensoryInput = std::make_shared<nlm::Vision>(10, 10, 3);
 *             brain->receiveSensoryInput(*sensoryInput);
 *         }
 *     }
 *     
 *     // Get statistics
 *     std::cout << "Total spikes: " << brain->getTotalSpikeCount() << std::endl;
 *     std::cout << "Average firing rate: " << brain->getAverageFiringRate() << std::endl;
 *     
 *     return 0;
 * }
 * @endcode
 * 
 * @section brain_performance Performance Considerations
 * 
 * The Brain class is designed for high-performance neural simulation:
 * - Event-driven spike propagation for efficiency
 * - Memory pools for allocation reduction
 * - SIMD optimizations for vectorized operations
 * - Parallel processing for multi-core systems
 * - Checkpointing for long-running simulations
 * 
 * @see nlm::Config Configuration system
 * @see nlm::Neuron LIF neuron implementation
 * @see nlm::SpikeSystem Spike dynamics
 * @see nlm::STDP Spike-timing-dependent plasticity
 * @see nlm::Dopamine Neuromodulation system
 * @see nlm::PredictionSystem Predictive coding
 * @see nlm::NeuralPlanner Planning and decision making
 * @see nlm::NeuralWorkingMemory Working memory
 * @see nlm::NeuralEpisodicMemory Episodic memory
 * @see nlm::DevelopmentSystem Neural development
 */

/**
 * @class Brain
 * @brief Brain: The central coordinator of the neural system
 * 
 * Implements real spiking neural computation with event-driven dynamics and
 * integrated memory, prediction, cognition, and neuromodulation systems.
 * 
 * The Brain class serves as the main interface for controlling the neural simulation,
 * managing neural populations, processing sensory input, and producing motor output.
 * It coordinates all subsystems including spike dynamics, plasticity, memory,
 * prediction, cognition, development, and neuromodulation.
 * 
 * The brain architecture follows the neural constructivist approach, implementing
 * biologically-inspired development and learning mechanisms.
 * 
 * @section brain_system_overview System Architecture
 * 
 * The brain implements a complete computational neuroscience framework:
 * 
 * 1. **Cortical Regions**: Hierarchical organization of neural populations
 * 2. **Neuronal Computation**: Leaky integrate-and-fire dynamics with spike generation
 * 3. **Synaptic Plasticity**: STDP, Hebbian, and structural learning rules
 * 4. **Memory Systems**: Working, episodic, and associative memory
 * 5. **Prediction**: Forward models and prediction error computation
 * 6. **Cognition**: Planning, attention, and concept formation
 * 7. **Development**: Maturation, synaptogenesis, and pruning
 * 8. **Neuromodulation**: Reward, curiosity, novelty, and prediction error signals
 * 
 * @section brain_thread_safety Thread Safety
 * 
 * The Brain class is thread-safe for read operations. For write operations,
 * external synchronization is required.
 * 
 * @section brain_memory_model Memory Management
 * 
 * Uses a Pimpl idiom for efficient memory management and implementation hiding.
 * All memory management is handled internally with proper cleanup.
 * 
 * @note Deep understanding of neural computation principles is recommended
 * before using this API. The brain implements complex neuroscience concepts.
 * 
 * @author NLM Development Team
 * @version 1.0.0
 * @date 2026
 */
public:
    /**
     * @brief Create brain with configuration
     * 
     * Constructs a new Brain instance with the specified configuration.
     * The configuration determines all aspects of the brain's architecture,
     * including neuron count, regions, plasticity rules, and simulation parameters.
     * 
     * @param config Shared pointer to configuration object. Must not be null.
     *               Configuration is copied and modified internally.
     * @throws std::runtime_error if configuration is invalid or initialization fails.
     * 
     * @example
     * @code{.cpp}
     * auto config = std::make_shared<nlm::Config>();
     * config->set("neuron_count", 1000);
     * config->set("region_count", 2);
     * auto brain = std::make_shared<nlm::Brain>(config);
     * @endcode
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * Performs the complete initialization sequence for the brain,
     * including: creating neural regions, neurons, and connections;
     * initializing memory systems; and setting up all subsystems.
     * 
     * @return true if initialization was successful, false otherwise
     * 
     * @note Must be called before any other operations. Can be called multiple
     *       times to reset the brain to its initial state.
     * 
     * @see addRegion() For creating neural regions
     * @see getRegion() For accessing regions
     */
    bool initialize();
    
    /**
     * @brief Main simulation step
     * 
     * Performs a single simulation step with discrete time updates.
     * This method handles the complete neural computation cycle for one timestep,
     * including spike generation, propagation, plasticity, and development.
     * 
     * @param currentStep Current simulation step number (must be non-negative)
     * 
     * @note This method does not update system time - use step(step, time) for that.
     * @note Thread-safe for read operations on brain state
     * 
     * @see step(SimulationStep, Timestamp) For time-aware stepping
     * @see updatePlasticity() For plasticity update details
     * @see develop() For developmental updates
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Simulation time step
     * 
     * Performs a single simulation step with precise time tracking.
     * This is the preferred method for simulations requiring temporal accuracy,
     * such as those involving time-dependent processes or continuous dynamics.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     * 
     * @note This method provides both step and time information for all subsystems
     *       that require temporal context.
     * 
     * @see step(SimulationStep) For step-only updates
     * @see SimulationClock For time management
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Injects current into sensory neurons based on the provided input pattern.
     * This implements the sensory transduction pathway, converting external
     * stimuli into neural activity patterns.
     * 
     * @param input Sensory input data containing the stimulus pattern
     * 
     * @note The input is processed by sensory neurons in the appropriate brain regions.
     *       Input type determines which neural populations receive the stimulation.
     * 
     * @see Vision For visual input processing
     * @see Audio For auditory input processing
     * @see InternalSignals For internal state input processing
     * @see SensoryInput Base class for all sensory inputs
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Directly injects current into a single neuron, bypassing normal synaptic inputs.
     * Useful for controlled experiments, teaching demonstrations, or targeted stimulation.
     * 
     * @param neuron Identifier of the neuron to inject current into
     * @param current Amount of current to inject in nanoamps (or appropriate units)
     * 
     * @note This method provides direct access to neural control, allowing for
     *       precise experimental manipulation of neural activity patterns.
     * 
     * @see injectCurrentToNeurons() For injecting into all neurons of a type
     * @see Neuron::getMembranePotential() For reading neuron state
     * @see Neuron::step() For neuron dynamics
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Injects current into all neurons of a specified type across the entire brain.
     * This provides population-level control for studying collective neural dynamics.
     * 
     * @param type Type of neurons to inject current into (Excitatory, Inhibitory, etc.)
     * @param current Amount of current to inject in nanoamps (or appropriate units)
     * 
     * @note This is more efficient than individual neuron injection when stimulating
     *       large populations of neurons. Useful for studying population-level effects.
     * 
     * @see injectCurrent() For single neuron injection
     * @see NeuronType For available neuron types
     * @see getRegion() For region-specific operations
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Access to spike system
     * 
     * Provides access to the central spike processing system, which handles
     * all spike generation, propagation, and event scheduling.
     * 
     * @return Pointer to the spike system object
     * 
     * @note The spike system is a core component responsible for the timing
     *       and coordination of all neural activity. It manages the event-driven
     *       computation model.
     * 
     * @see SpikeSystem For detailed spike processing documentation
     * @see getTotalSpikeCount() For spike statistics
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Access to spike system (const version)
     * 
     * Provides read-only access to the spike system. Useful for monitoring
     * spike processing without modifying state.
     * 
     * @return Const pointer to the spike system object
     * 
     * @note This method is thread-safe for read operations.
     * 
     * @see getSpikeSystem() Non-const version for modification
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access to spike-timing-dependent plasticity system
     * 
     * Provides access to the STDP plasticity implementation, which implements
     * the fundamental Hebbian learning rule based on spike timing.
     * 
     * @return Pointer to the STDP object
     * 
     * @note STDP is the primary mechanism for synaptic learning in the brain model.
     *       It enables spike-timing-dependent synaptic strengthening and weakening.
     * 
     * @see Hebbian() For alternative plasticity mechanism
     * @see StructuralPlasticity() For structural connectivity changes
     * @see PlasticityFlags For plasticity control
     */
    STDP* getSTDP();
    
    /**
     * @brief Access to Hebbian plasticity system
     * 
     * Provides access to the Hebbian plasticity implementation, which implements
     * activity-dependent synaptic strengthening.
     * 
     * @return Pointer to the Hebbian object
     * 
     * @note Hebbian plasticity provides an alternative learning mechanism
     *       to STDP, focusing on correlation-based synaptic strengthening.
     * 
     * @see STDP() For spike-timing-dependent plasticity
     * @see updatePlasticity() For plasticity update coordination
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Access to structural plasticity system
     * 
     * Provides access to the structural plasticity implementation, which handles
     * synaptogenesis (formation of new connections) and pruning (elimination of
     * weak or unused connections).
     * 
     * @return Pointer to the StructuralPlasticity object
     * 
     * @note Structural plasticity implements developmental and experience-dependent
     *       changes to the brain's connectivity pattern.
     * 
     * @see Synaptogenesis() For new connection formation
     * @see Pruning() For connection elimination
     * @see DevelopmentSystem() For developmental coordination
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation/inhibition ratio
     * 
     * Computes the balance between excitatory and inhibitory neural activity,
     * which is crucial for maintaining stable neural dynamics and preventing
     * runaway excitation.
     * 
     * @return Excitation/inhibition ratio as a floating-point value
     *         (typically between 0.0 and 1.0 for balanced networks)
     * 
     * @note A healthy brain typically maintains an excitation/inhibition ratio
     *       close to 1:1. Deviations can lead to instability or pathological states.
     * 
     * @see getActiveNeuronCount() For individual population counts
     * @see getFiringNeuronCount() For firing statistics
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * 
     * Returns the cumulative number of spikes generated by all neurons
     * since initialization or last reset.
     * 
     * @return Total spike count since initialization
     * 
     * @note This is a global statistic useful for monitoring brain activity
     *       and performance analysis. Can be reset by calling reset().
     * 
     * @see reset() For clearing spike statistics
     * @see getTotalStepCount() For total simulation steps
     * @see SpikeSystem::getTotalSpikeCount() For spike system details
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * Returns the number of spike events scheduled for future delivery
     * but not yet processed.
     * 
     * @return Number of pending spike events
     * 
     * @note Pending spikes represent the event-driven nature of the brain model.
     *       This count helps monitor the computational load and can be used
     *       to detect scheduling issues.
     * 
     * @see step() For processing pending spikes
     * @see SpikeSystem::getPendingSpikeEventCount() For spike system details
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on neural activity
     * 
     * Analyzes motor neuron activity patterns and produces appropriate motor
     * actions based on the brain's learned policies and current sensory context.
     * 
     * @return Unique pointer to an action object. Returns nullptr if no action
     *         should be taken.
     * 
     * @note This method implements the final output pathway, converting neural
     *       activity into meaningful actions in the simulated environment.
     * 
     * @see Action For action representation
     * @see MotorSystem For motor output processing
     * @see getSensoryInput() For sensory input processing
     * @see produceAction() For action generation
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     * 
     * Applies neuromodulatory signals (dopamine, curiosity, novelty, etc.)
     * to the brain, influencing learning, motivation, and attention.
     * 
     * @param signal Neuromodulator signal containing type and magnitude
     * 
     * @note Neuromodulation is crucial for adaptive behavior, learning rate
     *       modulation, and exploration-exploitation trade-offs.
     * 
     * @see Dopamine For reward-based neuromodulation
     * @see Curiosity For exploration motivation
     * @see Novelty For novelty detection
     * @see PredictionError For learning signals
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules
     * 
     * Updates all plasticity rules based on current neural activity patterns.
     * This method is called automatically during each simulation step.
     * 
     * @note Plasticity updates include:
     *       - STDP synaptic weight changes
     *       - Hebbian synaptic strengthening
     *       - Structural synaptogenesis and pruning
     *       - Neuromodulation-dependent plasticity
     * 
     * @see STDP::update() For STDP specifics
     * @see Hebbian::update() For Hebbian specifics
     * @see StructuralPlasticity::update() For structural changes
     * @see step() For automatic plasticity updates
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes
     * 
     * Applies developmental changes to the brain's structure and function.
     * This includes maturation of neural properties, synaptic refinement,
     * and experience-dependent developmental processes.
     * 
     * @note Development is controlled by the DevelopmentalSystem and includes:
     *       - Neural maturation and myelination
     *       - Synaptogenesis (connection formation)
     *       - Pruning (elimination of weak connections)
     *       - Critical period modulation
     * 
     * @see DevelopmentSystem::update() For developmental details
     * @see develop() For automatic developmental updates
     * @see getDevelopmentalStage() For current developmental state
     */
    void develop();
    
    /**
     * @brief Reset brain state
     * 
     * Resets the brain to its initial state as defined by the configuration.
     * This clears all neural activity, memory content, and developmental state,
     * effectively starting a new simulation episode.
     * 
     * @note Reset affects:
     *       - All neural populations and their activity states
     *       - Memory systems (working, episodic, associative)
     *       - Developmental stage and progress
     *       - Spike statistics and counters
     *       - Structural connectivity
     * 
     * @note This is equivalent to re-initializing the brain with the same
     *       configuration. Use save() and load() for checkpointing.
     * 
     * @see save() For saving brain state to file
     * @see load() For restoring brain state from file
     * @see initialize() For initial brain setup
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Saves the complete brain state to a file for checkpointing and
     * long-running simulations. This includes neural weights, memory content,
     * developmental state, and all other persistent brain state.
     * 
     * @param filepath Path to the file where brain state will be saved
     * @return true if save was successful, false otherwise
     * 
     * @note The saved state can be restored later using load(). This is essential
     *       for long-running simulations that need to be interrupted and resumed.
     * 
     * @see load() For restoring brain state from file
     * @see reset() For resetting to initial state
     * @see CheckpointSystem For checkpoint management details
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * Loads brain state from a checkpoint file, restoring the brain to a
     * previous state. This is used to resume interrupted simulations or
     * load previously saved brain configurations.
     * 
     * @param filepath Path to the checkpoint file
     * @return true if load was successful, false otherwise
     * 
     * @note Loading overwrites the current brain state completely.
     *       The configuration remains unchanged, but all neural state is restored.
     * 
     * @see save() For saving brain state to file
     * @see reset() For resetting to initial state
     * @see CheckpointSystem For checkpoint management details
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a neural region
     * 
     * Creates a new neural region in the brain with the specified name.
     * Regions are hierarchical containers for neural populations and enable
     * modular organization of brain function.
     * 
     * @param name Optional name for the region. If empty, a unique name is generated.
     * @return RegionId Identifier of the newly created region
     * 
     * @note Regions are the primary organizational unit of the brain. Each region
     *       contains one or more neural populations that perform specific functions.
     * 
     * @see getRegion() For accessing regions by ID
     * @see getRegionCount() For getting number of regions
     * @see getRegionIds() For getting all region IDs
     * @see NeuralRegion For region implementation details
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a neural region
     * 
     * Retrieves a neural region by its identifier. This provides access to the
     * neural populations and properties of that region.
     * 
     * @param id Region identifier
     * @return Pointer to the neural region object, or nullptr if not found
     * 
     * @note Thread-safe for read operations. External synchronization required
     *       for concurrent modification of region contents.
     * 
     * @see addRegion() For creating new regions
     * @see getRegionCount() For getting number of regions
     * @see getRegions() For getting all regions
     * @see NeuralRegion For region implementation details
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a neural region (const version)
     * 
     * Read-only access to a neural region for inspection and monitoring.
     * 
     * @param id Region identifier
     * @return Const pointer to the neural region object, or nullptr if not found
     * 
     * @note This method is thread-safe for read operations.
     * 
     * @see getRegion() Non-const version for modification
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get region count
     * 
     * Returns the total number of neural regions in the brain.
     * 
     * @return Number of regions
     * 
     * @note This provides a quick overview of brain architecture and organization.
     * 
     * @see addRegion() For creating regions
     * @see getRegionIds() For getting all region IDs
     * @see getRegions() For getting all region objects
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region identifiers
     * 
     * Returns a vector containing all region identifiers currently in the brain.
     * Useful for iteration and region management.
     * 
     * @return Vector of all region identifiers
     * 
     * @note The order of region IDs is not guaranteed to be consistent.
     * 
     * @see addRegion() For creating regions
     * @see getRegionCount() For getting number of regions
     * @see getRegion() For accessing specific regions
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * 
     * Returns a vector of all neural regions in the brain. This provides access
     * to the complete brain architecture and organization.
     * 
     * @return Vector of all neural region objects
     * 
     * @note This includes regions that may have been added during initialization
     *       or runtime. The regions are owned by the brain and will be cleaned up
     *       when the brain is destroyed.
     * 
     * @see addRegion() For creating new regions
     * @see getRegion() For accessing specific regions
     * @see getRegionCount() For getting number of regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection
     * 
     * Creates a connection between two neural regions, enabling communication
     * and integration across distributed brain areas. This implements long-range
     * connectivity essential for global brain function.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Connection weight (default: 0.0f)
     * @param delay Transmission delay in simulation steps (default: 1)
     * 
     * @note Inter-region connections enable integration of information across
     *       different brain areas, supporting complex cognitive functions.
     * 
     * @see removeInterRegionConnection() For removing connections
     * @see InterRegionConnection For connection structure details
     * @see getRegion() For region-specific operations
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * 
     * Removes a connection between two neural regions, disabling communication
     * between those regions.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * 
     * @note This does not affect the regions themselves, only their connection.
     * 
     * @see addInterRegionConnection() For adding connections
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count
     * 
     * Returns the total number of neurons across all regions in the brain.
     * This is a global statistic useful for monitoring brain size and complexity.
     * 
     * @return Total neuron count
     * 
     * @note This includes all neuron types (excitatory, inhibitory, etc.)
     *       across all brain regions.
     * 
     * @see getRegionCount() For number of regions
     * @see getTotalSynapseCount() For synaptic statistics
     * @see getActiveNeuronCount() For active neuron count
     * @see getFiringNeuronCount() For firing neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count
     * 
     * Returns the total number of synapses across all neurons in the brain.
     * This measures the connectivity and complexity of the brain network.
     * 
     * @return Total synapse count
     * 
     * @note This is a key metric of brain connectivity and complexity.
     *       Higher values typically indicate more sophisticated neural processing.
     * 
     * @see getTotalNeuronCount() For neuron statistics
     * @see getTotalSpikeCount() For spike statistics
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count
     * 
     * Returns the number of neurons currently firing or in an active state.
     * This measures the current level of brain activity.
     * 
     * @return Number of active neurons
     * 
     * @note This is a dynamic statistic that changes with neural activity.
     *       Can be used to monitor brain activity levels and detect anomalies.
     * 
     * @see getFiringNeuronCount() For firing statistics
     * @see getTotalNeuronCount() For total neuron count
     * @see getExcitationInhibitionRatio() For activity balance
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * 
     * Returns the number of neurons currently in the firing (active) state.
     * This is a subset of active neurons that are actually generating spikes.
     * 
     * @return Number of firing neurons
     * 
     * @note Firing neurons are those that have crossed the threshold for spike generation.
     *       This differs from active neurons which may be subthreshold but still influential.
     * 
     * @see getActiveNeuronCount() For broader activity statistics
     * @see getFiringRate() For firing rate statistics
     * @see Neuron::isFiring() For individual neuron firing status
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * 
     * Computes the average firing rate across all neurons in the brain.
     * This is a key metric of neural activity and computational performance.
     * 
     * @return Average firing rate in Hz (spikes per second)
     * 
     * @note This is calculated as total spikes divided by total simulation time
     *       and number of neurons. Useful for monitoring brain activity levels
     *       and comparing different brain configurations.
     * 
     * @see getTotalSpikeCount() For total spike statistics
     * @see getFiringNeuronCount() For firing statistics
     * @see getExcitationInhibitionRatio() For activity balance
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Access to working memory system
     * 
     * Provides access to the working memory system, which holds transient
     * information needed for ongoing cognitive tasks and immediate processing.
     * 
     * @return Pointer to the working memory object
     * 
     * @note Working memory is crucial for maintaining information online
     *       during cognitive tasks. It has limited capacity but rapid access times.
     * 
     * @see NeuralWorkingMemory For working memory implementation details
     * @see getEpisodicMemory() For long-term memory access
     * @see getAssociativeMemory() For associative memory access
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Access to episodic memory system
     * 
     * Provides access to the episodic memory system, which stores
     * experience-based knowledge and learned behaviors.
     * 
     * @return Pointer to the episodic memory object
     * 
     * @note Episodic memory supports replay and consolidation, allowing
     *       learned experiences to influence future behavior and performance.
     * 
     * @see NeuralEpisodicMemory For episodic memory implementation details
     * @see getWorkingMemory() For working memory access
     * @see getAssociativeMemory() For associative memory access
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Access to associative memory system
     * 
     * Provides access to the associative memory system, which supports
     * pattern completion and association-based retrieval.
     * 
     * @return Pointer to the associative memory object
     * 
     * @note Associative memory enables recall based on partial activation
     *       and supports pattern completion for robust information retrieval.
     * 
     * @see NeuralAssociativeMemory For associative memory implementation details
     * @see getWorkingMemory() For working memory access
     * @see getEpisodicMemory() For episodic memory access
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Access to prediction system
     * 
     * Provides access to the prediction system, which implements forward models
     * for sensory prediction and error computation.
     * 
     * @return Pointer to the prediction system object
     * 
     * @note The prediction system is essential for predictive coding, error-based
     *       learning, and adaptive behavior. It enables the brain to anticipate
     *       future states and reduce prediction errors.
     * 
     * @see PredictionSystem For prediction implementation details
     * @see getDopamine() For reward-based modulation
     * @see getCuriosity() For exploration motivation
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Access to neural planner
     * 
     * Provides access to the neural planner, which implements action planning
     * and decision making based on learned policies and current goals.
     * 
     * @return Pointer to the neural planner object
     * 
     * @note The neural planner supports hierarchical planning, action selection,
     *       and goal-directed behavior. It integrates predictions and memory
     *       to select optimal actions.
     * 
     * @see NeuralPlanner For planning implementation details
     * @see getConceptFormation() For concept-based planning
     * @see getAttention() For attentional control
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Access to concept formation system
     * 
     * Provides access to the concept formation system, which discovers and
     * learns abstract concepts and patterns from sensory input.
     * 
     * @return Pointer to the concept formation object
     * 
     * @note Concept formation enables abstraction, categorization, and
     *       generalization. It supports higher-level cognitive functions
     *       like reasoning and problem solving.
     * 
     * @see ConceptFormation For concept formation implementation details
     * @see getPlanner() For planning with concepts
     * @see getAttention() For attentional control
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Access to attentional selection system
     * 
     * Provides access to the attentional selection system, which controls
     * the allocation of cognitive resources and focus of attention.
     * 
     * @return Pointer to the attentional selection object
     * 
     * @note Attentional selection is crucial for efficient information processing,
     *       preventing cognitive overload, and focusing resources on relevant stimuli.
     * 
     * @see AttentionalSelection For attentional control implementation details
     * @see getPlanner() For planning with attentional control
     * @see getConceptFormation() For concept-based attention
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Access to development system
     * 
     * Provides access to the development system, which controls neural
     * maturation, synaptogenesis, and experience-dependent development.
     * 
     * @return Pointer to the development system object
     * 
     * @note Development is a critical component of the brain's architecture,
     *       implementing biologically-inspired learning and maturation processes.
     * 
     * @see DevelopmentSystem For development implementation details
     * @see getDevelopmentalStage() For current developmental state
     * @see getDopamine() For developmental modulation
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * Returns the current developmental stage of the brain, which determines
     * which developmental processes are active and how neural properties
     * are being modified.
     * 
     * @return Current developmental stage
     * 
     * @note Developmental stages include:
     *       - Initial: Brain initialization
     *       - CriticalPeriod: Sensitive period for learning
     *       - Maturation: Neural maturation and refinement
     *       - Adult: Stable adult brain
     *       - Aging: Age-related changes
     * 
     * @see setDevelopmentalStage() For setting developmental stage
     * @see DevelopmentSystem::getStage() For development system details
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * Manually sets the developmental stage of the brain, allowing control
     * over developmental processes. This can be useful for experiments
     * and testing different developmental scenarios.
     * 
     * @param stage Developmental stage to set
     * 
     * @note Changing developmental stage may trigger significant changes
     *       in neural properties and behavior. Use with caution.
     * 
     * @see getDevelopmentalStage() For getting current stage
     * @see DevelopmentSystem::setStage() For development system details
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @brief Access to dopamine system
     * 
     * Provides access to the dopamine system, which implements reward-based
     * neuromodulation and reinforcement learning.
     * 
     * @return Pointer to the dopamine object
     * 
     * @note Dopamine is crucial for reward prediction, reinforcement learning,
     *       and motivation. It modulates plasticity and attention based on
     *       reward prediction errors.
     * 
     * @see Dopamine For dopamine implementation details
     * @see getCuriosity() For exploration motivation
     * @see getNovelty() For novelty detection
     * @see getPredictionError() For learning signals
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Access to curiosity system
     * 
     * Provides access to the curiosity system, which implements exploration
     * motivation and intrinsic reward signals.
     * 
     * @return Pointer to the curiosity object
     * 
     * @note Curiosity drives exploration of novel states and promotes
     *       learning in uncertain environments. It complements extrinsic
     *       reward signals with intrinsic motivation.
     * 
     * @see Curiosity For curiosity implementation details
     * @see getDopamine() For extrinsic reward
     * @see getNovelty() For novelty detection
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Access to novelty system
     * 
     * Provides access to the novelty system, which detects unexpected changes
     * and generates novelty signals that drive learning and exploration.
     * 
     * @return Pointer to the novelty object
     * 
     * @note Novelty detection is crucial for adaptive behavior in dynamic
     *       environments. It helps the brain identify important changes
     *       and focus learning resources on novel information.
     * 
     * @see Novelty For novelty implementation details
     * @see getDopamine() For reward-based signals
     * @see getCuriosity() For exploration motivation
     */
    Novelty* getNovelty();
    
    /**
     * @brief Access to prediction error signal
     * 
     * Provides access to the prediction error signal system, which computes
     * and delivers prediction errors that drive learning and adaptation.
     * 
     * @return Pointer to the prediction error object
     * 
     * @note Prediction error is a fundamental learning signal in the brain,
     *       indicating the difference between expected and actual outcomes.
     *       It drives error-based learning and policy optimization.
     * 
     * @see PredictionError For prediction error implementation details
     * @see getDopamine() For reward prediction error
     * @see getCuriosity() For intrinsic prediction error
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get current configuration
     * 
     * Returns the configuration object associated with this brain, which
     * contains all parameters that determine brain behavior and architecture.
     * 
     * @return Shared pointer to the configuration object
     * 
     * @note The configuration is essential for brain initialization and
     *       determines all aspects of brain behavior, including neuron count,
     *       plasticity rules, and simulation parameters.
     * 
     * @see Config For configuration details
     * @see initialize() For brain initialization
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator
     * 
     * Provides access to the random number generator used by the brain
     * for stochastic processes, noise, and randomized behavior.
     * 
     * @return Pointer to the random generator object
     * 
     * @note The random generator is used for:
     *       - Initial neuron weight randomization
     *       - Random connectivity patterns
     *       - Stochastic plasticity rules
     *       - Exploration behavior
     *       - Noise injection for realistic dynamics
     * 
     * @see RandomGenerator For random number generation details
     * @see getConfig() For random seed configuration
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Logging
     * 
     * Logs the current status of the brain, including statistics, configuration,
     * and performance metrics. Useful for debugging, monitoring, and analysis.
     * 
     * @note The log output includes:
     *       - Current simulation step and time
     *       - Neural statistics (spikes, firing rates, etc.)
     *       - Configuration summary
     *       - System performance metrics
     *       - Memory usage information
     * 
     * @see Logger For logging implementation details
     * @see getConfig() For configuration logging
     * @see getTotalSpikeCount() For spike statistics
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
