// brain/Brain.hpp - Main Brain interface header
// Forward declarations and public interface for the NLM Brain class

#pragma once

#include "core/Types/Types.hpp"
#include "core/Config/Config.hpp"
#include <memory>
#include <string>
#include <vector>
#include <optional>

namespace nlm {

// Forward declarations for Brain's implementation details
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
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class CheckpointManager;

// Forward declarations for related types
class NeuralRegion;
class SensoryInput;
class Action;
class Neuromodulator;

// Struct for inter-region connections (long-range connectivity)
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
 * @brief The central coordinator of the neural system
 * 
 * Implements real spiking neural computation with event-driven dynamics
 * and integrated memory, prediction, cognition, and neuromodulation systems.
 * 
 * The Brain class is the core component of the NLM framework, providing:
 * - Real LIF neuron dynamics with event-driven spike propagation
 * - Integrated memory systems (working, episodic, associative)
 * - Prediction systems for sensory prediction and error computation
 * - Cognition systems (neural planning, concept formation, attention)
 * - Development systems for plasticity modulation over time
 * - Neuromodulation systems (dopamine, curiosity, novelty)
 * - Structural and functional plasticity mechanisms
 * 
 * The brain operates as a closed-loop system:
 * 1. Receives sensory input from environment
 * 2. Processes through neural networks
 * 3. Generates motor actions
 * 4. Receives feedback from environment
 * 5. Updates synaptic weights through learning
 * 
 * @note This implementation represents Phase 6 (Final Integration) of NLM,
 * where all previously developed systems are integrated into a coherent
 * artificial brain architecture.
 * 
 * @see nlm::Config for configuration options
 * @see nlm::Brain::Impl for implementation details
 */
class Brain {
public:
    /**
     * @brief Create a new Brain instance with the specified configuration
     * 
     * This static factory method creates a Brain instance with the given configuration.
     * It provides better error handling and logging than the constructor.
     * 
     * @param config Configuration to use for the brain. If nullptr, a default
     *              configuration will be created.
     * @return Unique pointer to the newly created Brain instance
     * 
     * @throws std::invalid_argument if config is invalid
     * @throws std::runtime_error if brain creation fails
     * 
     * @note This method is preferred over the constructor for better error handling
     *       and logging. The configuration is copied internally and can be modified
     *       after brain creation without affecting the brain's operation.
     * 
     * @param config Configuration for the brain
     * @return Pointer to new Brain instance
     * 
     * @see nlm::Config::createDefault() for creating default configurations
     * @see nlm::Brain::initialize() for initialization requirements
     */
    static std::unique_ptr<Brain> create(std::shared_ptr<Config> config);
    
    /**
     * @brief Create a new Brain instance with default configuration
     * 
     * This static factory method provides a convenient way to create Brain instances
     * without having to manually create configuration objects. It creates a default
     * configuration and uses it to construct the brain.
     * 
     * @return Unique pointer to the newly created Brain instance
     * 
     * @note This is the recommended way to create Brain instances in user code:
     * ```cpp
     * #include "brain/Brain.hpp"
     * 
     * int main() {
     *     // Create brain with default configuration
     *     auto brain = nlm::Brain::createDefault();
     *     
     *     // Initialize brain
     *     brain->initialize();
     *     
     *     // Run simulation
     *     for (int step = 0; step < 100; ++step) {
     *         brain->step(step);
     *     }
     *     
     *     return 0;
     * }
     * ```
     * 
     * @see nlm::Brain::create(std::shared_ptr<Config> config)
     *      for custom configuration
     */
    static std::unique_ptr<Brain> createDefault();
    
    /**
     * @brief Create a new Brain instance with the given configuration
     * 
     * @param config Shared configuration for the brain. If nullptr, default
     *              configuration will be used. The configuration should contain
     *              parameters for neuron count, region count, connection probability,
     *              and other simulation parameters.
     * 
     * @note The configuration will be copied internally and can be modified
     *       after brain creation without affecting the brain's operation.
     * 
     * @throws std::invalid_argument if config is invalid
     * @see nlm::Config for available configuration parameters
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept = default;
    Brain& operator=(Brain&&) noexcept = default;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * This method performs the complete brain initialization sequence:
     * 1. Creates neural regions based on configuration
     * 2. Populates regions with neurons of appropriate types
     * 3. Establishes connectivity between neurons
     * 4. Initializes all integrated systems (memory, prediction, cognition, etc.)
     * 5. Configures plasticity systems
     * 6. Sets up system parameters and statistics
     * 
     * @return true if initialization succeeded, false otherwise
     * @throws std::runtime_error if critical initialization fails
     * 
     * @see nlm::Brain::Impl::initialize()
     * @see nlm::Config for initialization parameters
     * 
     * Configuration parameters used:
     * - "neuron_count": Total number of neurons to create (default: 1000)
     * - "region_count": Number of neural regions (default: 1)
     * - "connection_probability": Probability of synapse formation (default: 0.1)
     * - "stdp_ltp_weight": STDP long-term potentiation weight (default: 0.01)
     * - "stdp_ltd_weight": STDP long-term depression weight (default: 0.012)
     * - "stdp_tau": STDP time constant (default: 20.0)
     * - "synaptogenesis_rate": Structural plasticity rate (default: 0.0001)
     * - "pruning_rate": Structural plasticity pruning rate (default: 0.00001)
     */
    bool initialize();
    
    /**
     * @brief Perform a single simulation step of the integrated brain
     * 
     * This method implements the complete 15-step integrated brain simulation loop:
     * Each step represents a distinct computational process in the artificial brain.
     * 
     * @param currentStep Current simulation step number
     * 
     * @note The 15-step process represents a single moment of brain activity, where:
     *       - Steps 1-3: Neural dynamics (spike generation and propagation)
     *       - Steps 4-5: Memory and modulation (working memory, neuromodulation)
     *       - Steps 6-7: Learning and memory (plasticity, episodic memory)
     *       - Steps 8-10: Higher cognition (prediction, attention, concept formation)
     *       - Steps 11-13: Structural adaptation (plasticity, replay, development)
     *       - Step 14: Statistics collection (performance monitoring)
     * 
     * @see nlm::Brain::Impl::step(SimulationStep, Timestamp) for implementation details
     * 
     * The step processing order is critical for proper brain operation:
     * 1. Delayed spike processing → 2. Neuron potential updates → 3. Spike detection
     * 4. Working memory updates → 5. Neuromodulation → 6. Plasticity rules
     * 7. Episodic memory → 8. Prediction system → 9. Attention system
     * 10. Concept formation → 11. Structural plasticity → 12. Memory replay
     * 13. Development effects → 14. Statistics collection
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Perform a single simulation step with timing information
     * 
     * This method implements the complete 15-step integrated brain simulation loop
     * with precise timing information for accurate biological modeling.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time (seconds)
     * 
     * @note This provides more detailed timing information than the step(SimulationStep)
     *       overload and is used for precise temporal dynamics and accurate modeling
     *       of synaptic delays and neural processing times.
     * 
     * @see nlm::Brain::Impl::step(SimulationStep, Timestamp) for implementation details
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive and process sensory input from the environment
     * 
     * This method injects sensory information into the brain's neural network:
     * - Converts sensory data into neuronal currents
     * - Injects currents into sensory neurons
     * - Updates working memory with significant sensory inputs
     * - Handles input normalization and validation
     * 
     * @param input Sensory input data from the environment. This should contain
     *             sensory measurements (vision, audio, internal states, etc.) as
     *             floating-point values in the range [-1.0, 1.0].
     * 
     * @throws std::runtime_error if sensory processing fails
     * 
     * @note This is a core input processing method that connects the brain to
     *       the external world. The input represents sensory data that the brain
     *       must process to understand its environment and determine appropriate actions.
     * 
     * Input processing flow:
     * 1. Validate input data (non-empty, reasonable size)
     * 2. Normalize input values to biological current ranges (-10 to 10 mV)
     * 3. Inject currents into appropriate sensory neurons
     * 4. Update working memory with significant inputs
     * 5. Handle errors and log for debugging
     * 
     * @see nlm::SensoryInput for input data formats
     * @see nlm::Brain::Impl::receiveSensoryInput()
     */
    void receiveSensoryInput(const SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * This method injects a current value into a specific neuron in the network,
     * directly modifying its membrane potential and potentially causing it to fire.
     * 
     * @param neuron ID of the target neuron to inject current into
     * @param current Current to inject in membrane potential units (mV)
     * 
     * @throws std::runtime_error if neuron not found or injection fails
     * 
     * @note This is a low-level method for direct neural manipulation. It's useful
     *       for targeted stimulation or for implementing specific input patterns
     *       that bypass the sensory system. For environmental input, use
     *       receiveSensoryInput() instead.
     * 
     * @see nlm::Brain::Impl::injectCurrent()
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * This method injects current into all neurons in the brain of the specified
     * type, which is useful for implementing global neuromodulatory signals or
     * targeted stimulation of specific neuron populations.
     * 
     * @param type Type of neurons to inject current into (Sensory, Motor, Excitatory, etc.)
     * @param current Current to inject in membrane potential units (mV)
     * 
     * @throws std::runtime_error if no neurons of the specified type are found
     * 
     * @note This is useful for implementing global signals like neuromodulation,
     *       attention signals, or for testing system-wide effects. The number of
     *       neurons injected is logged for monitoring.
     * 
     * @see nlm::Brain::Impl::injectCurrentToNeurons()
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
/**
     * @brief Get the spike system used for spike propagation and recording
     * 
     * This method provides access to the spike system, which manages the generation,
     * recording, and propagation of action potentials across the neural network.
     * 
     * @return Shared pointer to the spike system
     * 
     * @note The spike system is essential for neural communication. It tracks all
     *       spike events, handles synaptic propagation delays, and provides
     *       statistics on neural activity.
     * 
     * @see nlm::Brain::Impl::getSpikeSystem()
     */
    std::shared_ptr<SpikeSystem> getSpikeSystem();
    
    std::shared_ptr<const SpikeSystem> getSpikeSystem() const;

    /**
     * @brief Get the STDP (Spike-Timing Dependent Plasticity) system
     * 
     * This method provides access to the STDP system, which implements
     * spike-timing dependent plasticity - a learning mechanism where synaptic
     * strength changes depend on the precise timing between pre- and post-
     * synaptic spikes.
     * 
     * @return Shared pointer to the STDP system
     * 
     * @note STDP is a fundamental learning mechanism in biological neural
     *       networks. It enables the network to learn temporal patterns and
     *       correlations between neuron activities.
     * 
     * @see nlm::Brain::Impl::getSTDP()
     */
    std::shared_ptr<STDP> getSTDP();
    
    /**
     * @brief Get the Hebbian learning system
     * 
     * This method provides access to the Hebbian learning system, which
     * implements Hebbian learning - synaptic strengthening based on the
     * correlation of pre- and post-synaptic activity (often summarized as
     * "neurons that fire together, wire together").
     * 
     * @return Shared pointer to the Hebbian learning system
     * 
     * @note Hebbian learning is a classical learning rule that reinforces
     *       connections between neurons that fire together, enabling the
     *       formation of patterns and associations.
     * 
     * @see nlm::Brain::Impl::getHebbian()
     */
    std::shared_ptr<Hebbian> getHebbian();
    
    /**
     * @brief Get the structural plasticity system
     * 
     * This method provides access to the structural plasticity system, which
     * manages the formation and pruning of synapses (synaptogenesis and pruning).
     * This represents changes in the brain's connectivity architecture rather
     * than just synaptic weight changes.
     * 
     * @return Shared pointer to the structural plasticity system
     * 
     * @note Structural plasticity allows the brain to adapt its connectivity
     *       over time, enabling learning of new patterns and the elimination
     *       of unused connections.
     * 
     * @see nlm::Brain::Impl::getStructuralPlasticity()
     */
    std::shared_ptr<StructuralPlasticity> getStructuralPlasticity();
    
    /**
     * @brief Calculate the excitation/inhibition ratio of the network
     * 
     * This ratio provides insight into the overall balance of the neural network:
     * - Ratio > 1.0: Excitatory influence dominates
     * - Ratio < 1.0: Inhibitory influence dominates
     * - Ratio = 1.0: Balanced excitation and inhibition
     * - Ratio = infinity: Only excitatory connections present
     * - Ratio = 0.0: Only inhibitory connections present or no connections
     * 
     * The E/I ratio is important for:
     * - Network stability analysis
     * - Understanding computational capabilities
     * - Predicting dynamic behaviors
     * - Monitoring learning progress
     * 
     * @return Excitation/inhibition ratio. Returns infinity if only excitatory
     *         connections exist, 0.0 if only inhibitory connections exist or no
     *         connections, or the actual ratio otherwise.
     * 
     * @note This metric is used to monitor network health and computational state.
     *       Healthy networks typically have ratios between 0.8 and 2.0, though
     *       this varies by task and network architecture.
     * 
     * @see nlm::Brain::Impl::getExcitationInhibitionRatio()
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get the total spike count for the entire brain
     * 
     * This method returns the cumulative number of spikes generated by all neurons
     * in the brain since initialization or last reset.
     * 
     * @return Total number of spikes recorded
     * 
     * @note This statistic is useful for monitoring brain activity levels and
     *       learning progress. The spike count can be used to track network
     *       engagement and computational workload.
     * 
     * @see nlm::Brain::Impl::getTotalSpikeCount()
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get the number of pending spike events
     * 
     * This method returns the number of spike events that are currently scheduled
     * but not yet delivered (including delayed spikes). This helps monitor the
     * spike propagation load in the system.
     * 
     * @return Number of pending spike events
     * 
     * @note Pending spike events include both immediate and delayed spikes.
     *       This count helps monitor system performance and identify potential
     *       bottlenecks in spike processing.
     * 
     * @see nlm::Brain::Impl::getPendingSpikeEventCount()
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on neural activity
     * 
     * This method generates motor commands or actions based on the current neural
     * activity patterns. It analyzes the brain's state and produces appropriate
     * outputs to control behavior.
     * 
     * @return Unique pointer to the generated action. Returns nullptr if no action
     *         should be taken based on current neural state.
     * 
     * @note This is the brain's output mechanism - it converts internal neural
     *       processing into external actions. The action type and parameters depend
     *       on the specific brain implementation and task requirements.
     * 
     * @see nlm::Brain::Impl::produceAction()
     */
    std::unique_ptr<Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals to influence neural activity and plasticity
     * 
     * This method applies neuromodulatory signals (like dopamine, acetylcholine, etc.)
     * to influence neural excitability and plasticity. Neuromodulators can enhance or
     * suppress neural activity and modulate learning rates.
     * 
     * @param signal Neuromodulator containing type and strength of the signal
     * 
     * @note Neuromodulation is crucial for adaptive behavior. It allows the brain
     *       to adjust its operating parameters based on behavioral context, reward,
     *       or other factors. Common neuromodulators include dopamine (reward),
     *       acetylcholine (attention), and norepinephrine (arousal).
     * 
     * @see nlm::Brain::Impl::applyNeuromodulation()
     */
    void applyNeuromodulation(const Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * This method applies all plasticity rules (STDP, Hebbian, structural plasticity)
     * to modify synaptic strengths based on neural activity. This is a core learning
     * mechanism that allows the brain to adapt to its environment.
     * 
     * @note This method is typically called automatically during the step processing
     *       loop. It can also be called manually for manual control of learning.
     * 
     * @see nlm::Brain::Impl::updatePlasticity()
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * This method applies developmental changes to the brain, including plasticity
     * modulation and other developmental processes. The brain progresses through
     * developmental stages that influence its learning capabilities and behavior.
     * 
     * @note This method is typically called automatically during the step processing
     *       loop. It can also be called manually for controlled developmental processes.
     * 
     * @see nlm::Brain::Impl::develop()
     */
    void develop();
    
    /**
     * @brief Reset brain state to initial conditions
     * 
     * This method resets all neural components of the brain to their initial state,
     * including neuron membrane potentials, spike systems, working memory, and
     * statistical counters. This effectively restarts the brain from a clean state.
     * 
     * @note This method can be used to restart the simulation or to clear the
     *       brain's state between tasks. It preserves the brain's configuration
     *       and connectivity structure but resets dynamic state variables.
     * 
     * @see nlm::Brain::Impl::reset()
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * This method saves the brain's current state to a binary file for later
     * restoration. This is useful for checkpointing long-running simulations,
     * saving progress, or implementing learning across sessions.
     * 
     * @param filepath Path to the file where brain state will be saved
     * @return true if the state was saved successfully, false otherwise
     * 
     * @note Saved state includes simulation progress (step count, time, spikes)
     *       and can be loaded later to resume from the same point. The saved
     *       file contains the brain's dynamic state but not its configuration.
     * 
     * @see nlm::Brain::Impl::save()
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * This method loads a previously saved brain state from a binary file.
     * This allows the brain to resume from a checkpoint or load a previously
     * saved brain state.
     * 
     * @param filepath Path to the file containing saved brain state
     * @return true if the state was loaded successfully, false otherwise
     * 
     * @note Loaded state includes simulation progress (step count, time, spikes)
     *       and can be used to resume from the same point. The loaded state
     *       will override the brain's current dynamic state but not its configuration.
     * 
     * @see nlm::Brain::Impl::load()
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region to the brain
     * 
     * This method creates a new neural region with the specified name and returns
     * its ID. Regions are logical groupings of neurons that can be used to
     * organize neural computation and implement modular architectures.
     * 
     * @param name Optional name for the new region. If empty, a default name
     *              ("Region_1", "Region_2", etc.) will be generated
     * @return Region ID of the newly created region, or std::nullopt if region
     *         creation failed
     * 
     * @note Regions help organize neural computation into functional modules.
     *       Each region contains neurons of specific types and is used to
     *       implement different aspects of brain function.
     * 
     * @see nlm::Brain::Impl::addRegion()
     */
    std::optional<RegionId> addRegion(const std::string& name = "");
    /**
     * @brief Get a specific neural region by ID
     * 
     * This method retrieves a neural region from the brain by its unique ID.
     * This is useful for accessing specific regions to modify their properties
     * or examine their contents.
     * 
     * @param id ID of the region to retrieve
     * @return Pointer to the neural region, or nullptr if region not found
     * 
     * @note This method provides access to specific regions for direct manipulation
     *       or analysis. For iteration over all regions, use getRegions().
     * 
     * @see nlm::Brain::Impl::getRegion(RegionId id)
     */
    NeuralRegion* getRegion(RegionId id);
    
    const NeuralRegion* getRegion(RegionId id) const;
    /**
     * @brief Get the number of neural regions in the brain
     * 
     * This method returns the total count of neural regions currently in the brain.
     * This is useful for monitoring brain architecture and complexity.
     * 
     * @return Number of neural regions
     * 
     * @note The region count is determined by the brain configuration and
     *       initialization process. It's a useful metric for brain architecture
     *       monitoring and analysis.
     * 
     * @see nlm::Brain::Impl::getRegionCount()
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all neural regions in the brain
     * 
     * This method returns a const reference to the collection of all neural regions
     * in the brain. This is useful for iterating over all regions or analyzing the
     * brain's architecture.
     * 
     * @return Const reference to the vector of neural regions
     * 
     * @note This provides access to all regions for comprehensive analysis or
     *       modification of multiple regions. For specific regions, use getRegion().
     * 
     * @see nlm::Brain::Impl::getRegions()
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add an inter-region connection to the brain
     * 
     * This method establishes a long-range connection between two neural regions,
     * allowing for communication and coordination between different brain modules.
     * Inter-region connections are essential for implementing complex neural
     * architectures with functional specialization.
     * 
     * @param source Region ID of the source region
     * @param target Region ID of the target region
     * @param weight Initial synaptic weight for the connection (default: 0.0)
     * @param delay Transmission delay for the connection in simulation steps (default: 1)
     * 
     * @note Inter-region connections are used to implement global signals and
     *       coordination between different brain modules. They can represent
     *       myelinated axons or other long-range communication pathways.
     * 
     * @see nlm::Brain::Impl::addInterRegionConnection()
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove an inter-region connection from the brain
     * 
     * This method removes an existing long-range connection between two neural regions.
     * This is used to disconnect regions or modify the brain's connectivity
     * architecture.
     * 
     * @param source Region ID of the source region
     * @param target Region ID of the target region
     * 
     * @see nlm::Brain::Impl::removeInterRegionConnection()
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get the total number of neurons in the brain
     * 
     * This method returns the total count of neurons across all regions in the brain.
     * This is a fundamental metric for brain size and computational capacity.
     * 
     * @return Total number of neurons in the brain
     * 
     * @note This includes all neuron types (Sensory, Motor, Excitatory, Inhibitory)
     *       and all regions. It's a key metric for brain architecture analysis.
     * 
     * @see nlm::Brain::Impl::getTotalNeuronCount()
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the total number of synapses in the brain
     * 
     * This method returns the total count of synapses across all neurons in the brain.
     * This measures the brain's connectivity and is important for understanding its
     * computational complexity and capacity.
     * 
     * @return Total number of synapses in the brain
     * 
     * @note This includes all types of synapses (excitatory, inhibitory) and
     *       represents the brain's connectivity structure. It's a key metric
     *       for brain complexity analysis.
     * 
     * @see nlm::Brain::Impl::getTotalSynapseCount()
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get the number of active neurons in the brain
     * 
     * This method returns the count of neurons that are currently active (firing)
     * in the brain. This is a measure of brain activity and engagement.
     * 
     * @return Number of active neurons
     * 
     * @note Active neurons are those that have recently fired or are currently
     *       above their firing threshold. This is a key metric for monitoring
     *       brain activity levels.
     * 
     * @see nlm::Brain::Impl::getActiveNeuronCount()
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the number of firing neurons in the brain
     * 
     * This method returns the count of neurons that are currently firing (generating
     * action potentials). This is a direct measure of neural output and activity.
     * 
     * @return Number of firing neurons
     * 
     * @note Firing neurons are those that have crossed their firing threshold
     *       and are generating action potentials. This is a key metric for
     *       understanding brain dynamics and information processing.
     * 
     * @see nlm::Brain::Impl::getFiringNeuronCount()
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the average firing rate of the brain
     * 
     * This method returns the average firing rate across all neurons in the brain,
     * calculated as total spikes divided by total simulation time.
     * 
     * @return Average firing rate in spikes per time step
     * 
     * @note This is a global measure of brain activity and engagement. It's
     *       calculated from the total spike count and simulation progress. This
     *       metric is useful for monitoring brain dynamics and performance.
     * 
     * @see nlm::Brain::Impl::getAverageFiringRate()
     */
    float getAverageFiringRate() const;
    
// ========== MEMORY SYSTEMS ==========

    // Working memory - transient active information
    std::shared_ptr<NeuralWorkingMemory> getWorkingMemory();
    
    // Episodic memory - experience storage
    std::shared_ptr<NeuralEpisodicMemory> getEpisodicMemory();
    
    // Associative memory - pattern associations
    std::shared_ptr<NeuralAssociativeMemory> getAssociativeMemory();
    
// ========== PREDICTION SYSTEM ==========

    // Prediction system for sensory prediction and error computation
    std::shared_ptr<PredictionSystem> getPredictionSystem();
    
// ========== COGNITION SYSTEMS ==========

    // Neural planner for action planning
    std::shared_ptr<NeuralPlanner> getPlanner();
    
    // Concept formation for pattern discovery
    std::shared_ptr<ConceptFormation> getConceptFormation();
    
    // Attentional selection for focus
    std::shared_ptr<AttentionalSelection> getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    // Development system for plasticity modulation
    std::shared_ptr<DevelopmentSystem> getDevelopmentSystem();
    
    // Current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    std::shared_ptr<Dopamine> getDopamine();
    
    // Curiosity - exploration motivation
    std::shared_ptr<Curiosity> getCuriosity();
    
    // Novelty - novelty detection
    std::shared_ptr<Novelty> getNovelty();
    
    // Prediction error signal
    std::shared_ptr<PredictionError> getPredictionErrorSignal();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    std::shared_ptr<RandomGenerator> getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
private:
    // Private implementation class to reduce compilation dependencies
    class Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm