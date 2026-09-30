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
 * @class Brain
 * @brief Main neural simulation interface and central coordinator of the neural system.
 * 
 * The Brain class implements real spiking neural computation with event-driven dynamics,
 * integrating memory, prediction, cognition, and neuromodulation systems. It serves as
 * the primary interface for users and developers to control and observe neural simulation
 * processes, from sensory input processing to action generation and learning.
 * 
 * The Brain class manages multiple neural regions, coordinates inter-regional connections,
 * maintains various brain systems (memory, prediction, cognition, development, neuromodulation),
 * and provides a complete interface for neural simulation from perception to action.
 * 
 * @note This is the main entry point for neural simulation and should be instantiated
 *       first before creating other neural components. The Brain object maintains ownership
 *       of all internal neural systems and manages their coordination during simulation.
 * 
 * @see AgentBrain for the interface between neural computation and world interaction
 * @see SimpleWorld for the simulated environment
 * @see Neuron for the basic neural computation unit
 */

class Brain {
public:
    /**
     * @brief Create brain with configuration
     * 
     * Initializes the neural system with the provided configuration object. The configuration
     * contains all simulation parameters including neural architecture, plasticity rules,
     * memory system settings, and neuromodulation parameters.
     * 
     * @param config Shared pointer to configuration object containing simulation parameters.
     *              The configuration must remain valid throughout the Brain's lifetime.
     * 
     * @note The configuration is stored as a constant reference for the lifetime of the Brain.
     *       Any modifications to the configuration after initialization will not be reflected
     *       in the simulation.
     * 
     * @throws std::bad_alloc if memory allocation fails during initialization
     * 
     * @see Config for configuration options
     * @example
     * @code
     * auto config = std::make_shared<Config>();
     * config->set("brain.regionCount", 3);
     * config->set("brain.plasticity.enableSTDP", true);
     * config->set("brain.memory.enableEpisodic", true);
     * 
     * auto brain = std::make_unique<Brain>(config);
     * if (!brain->initialize()) {
     *     std::cerr << "Failed to initialize brain" << std::endl;
     * }
     * @endcode
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the Brain object
     * 
     * Cleans up all neural resources, memory systems, and simulation state.
     * The destructor ensures proper cleanup of all allocated resources including
     * neural regions, connections, and internal brain systems.
     */
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * Initializes all neural regions, systems, and computational components based on
     * the configuration provided during construction. This method must be called before
     * any simulation can occur.
     * 
     * @return bool true if initialization succeeded, false otherwise
     * 
     * @retval true Initialization successful - brain is ready for simulation
     * @retval false Initialization failed - brain cannot be used for simulation
     * 
     * @note This method should be called after Brain construction but before any
     *       simulation steps. Multiple calls to initialize() after the first are
     *       generally not supported and may reset internal state.
     * 
     * @warning If initialization fails, the brain object should not be used and
     *         should be destroyed or reset before attempting re-initialization.
     * 
     * @see Brain(std::shared_ptr<Config>) constructor
     * 
     * @example
     * @code
     * auto brain = std::make_unique<Brain>(config);
     * if (!brain->initialize()) {
     *     std::cerr << "Brain initialization failed!" << std::endl;
     *     return -1;
     * }
     * std::cout << "Brain initialized successfully" << std::endl;
     * @endcode
     */
    bool initialize();
    
    /**
     * @brief Main simulation step
     * 
     * Advances the neural simulation by one time step. This method should be called
     * in a loop for each simulation step to update neural dynamics, process spikes,
     * update plasticity, apply neuromodulation, and generate motor actions.
     * 
     * @param currentStep Current simulation step number (integer)
     * 
     * @note This method performs the core neural computation for each simulation step,
     *       including spike generation, synaptic transmission, plasticity updates,
     *       and cognitive system updates.
     * 
     * @warning The step method should be called consistently throughout the simulation.
     *         Skipping steps or calling it with out-of-order step numbers may lead
     *         to incorrect neural dynamics.
     * 
     * @see step(SimulationStep, Timestamp) overload for time-synchronized stepping
     * 
     * @example
     * @code
     * for (SimulationStep step = 0; step < maxSteps; ++step) {
     *     brain->step(step);
     *     // Process sensory input, generate action, etc.
     * }
     * @endcode
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Simulation time step
     * 
     * Advances the neural simulation by one time step with explicit timestamp information.
     * This method provides more detailed temporal control and is useful for time-sensitive
     * simulation tasks such as reward timing, developmental changes, or episodic memory.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time (in simulation-specific units)
     * 
     * @note The timestamp parameter enables time-aware computations and allows the simulation
     *       to correlate neural events with real-world time when interfacing with the environment.
     * 
     * @see step(SimulationStep) for simpler stepping without temporal information
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Injects current into sensory neurons based on the input pattern provided by the
     * agent or environment. This method should be called each simulation step to provide
     * the neural system with sensory data from the world.
     * 
     * @param input Sensory input data containing sensory patterns and values. The input
     *             should represent the current environmental state as perceived by the agent.
     * 
     * @note This method typically interfaces with the AgentBrain class which transduces
     *       environmental data into neural current inputs. The sensory neurons then
     *       process this input and propagate it through the neural network.
     * 
     * @warning The sensory input should be provided each simulation step to maintain
     *         consistent sensory input to the neural system. Missing input may lead to
     *         incorrect neural processing.
     * 
     * @see AgentBrain::processSensoryInput for the method that prepares sensory data
     * @see injectCurrent for direct neuron current injection
     * @see injectCurrentToNeurons for batch current injection
     * 
     * @example
     * @code
     * SensoryPercept percept = world->getSensoryPercept();
     * brain->receiveSensoryInput(percept);
     * @endcode
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Directly injects current into a specific neuron by its ID. This method provides
     * fine-grained control over neural activation and is useful for targeted stimulation
     * or testing specific neural pathways.
     * 
     * @param neuron Neuron ID to receive the current injection
     * @param current Membrane potential current to inject (in nA)
     * 
     * @note This method bypasses the normal synaptic transmission pathway and directly
     *       affects the neuron's membrane potential. It's commonly used for testing or
     *       implementing external control of neural activity.
     * 
     * @warning Injected current directly affects the neuron's firing threshold and may
     *         cause unexpected firing patterns if too large. Use with caution.
     * 
     * @see receiveSensoryInput for sensory-driven current injection
     * @see injectCurrentToNeurons for batch current injection to multiple neurons
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Injects current into all neurons of a specified type, enabling population-level
     * modulation and coordinated activation of neural ensembles.
     * 
     * @param type Neuron type (e.g., sensory, motor, interneuron) to receive current
     * @param current Membrane potential current to inject (in nA)
     * 
     * @note This method is useful for implementing neuromodulatory signals that affect
     *       entire neural populations, such as attention, arousal, or learning signals.
     * 
     * @warning Injecting current to large populations can lead to synchronized firing
     *         and may cause instability. Monitor neural dynamics carefully.
     * 
     * @see injectCurrent for single neuron current injection
     * @see getExcitationInhibitionRatio for monitoring the resulting neural balance
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get spike system pointer
     * 
     * Returns a pointer to the spike system that manages spike generation, transmission,
     * and event-driven neural dynamics. The spike system is responsible for detecting
     * neuron firing and propagating spike events through synaptic connections.
     * 
     * @return SpikeSystem* Pointer to the spike system object. Returns nullptr if the
     *         spike system has not been initialized yet.
     * 
     * @retval nullptr Spike system not available (brain not initialized or spike system not created)
     * @retval non-null Pointer to the spike system instance
     * 
     * @note The spike system is a core component of event-driven neural computation.
     *       It allows for efficient simulation of sparse neural activity by only processing
     *       spike events rather than updating all neurons at each time step.
     * 
     * @see getSTDP for synaptic plasticity management
     * @see getHebbian for Hebbian plasticity management
     * @see getStructuralPlasticity for structural connection changes
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get spike system pointer (const version)
     * 
     * Returns a constant pointer to the spike system for read-only access to spike
     * system state and configuration.
     * 
     * @return const SpikeSystem* Constant pointer to the spike system object
     * 
     * @retval nullptr Spike system not available (brain not initialized or spike system not created)
     * 
     * @see getSpikeSystem() for mutable access
     */
    const SpikeSystem* getSpikeSystem() const;
    
    // Statistics
    /**
     * @brief Get excitation-inhibition balance ratio
     * 
     * Returns the ratio of excitation to inhibition in the neural network, providing
     * insight into the network's excitation-inhibition balance, which is crucial for
     * stable neural dynamics and appropriate firing patterns.
     * 
     * @return float Excitation-inhibition ratio (EI ratio). Values > 1.0 indicate
     *         predominant excitation, values < 1.0 indicate predominant inhibition,
     *         and values near 1.0 indicate balanced excitation and inhibition.
     * 
     * @note A healthy neural network typically maintains EI ratio near 1.0 to prevent
     *       runaway excitation or excessive inhibition. Monitoring this ratio helps
     *       ensure stable simulation dynamics.
     * 
     * @see getTotalSpikeCount for overall activity metrics
     * @see getPendingSpikeEventCount for spike event backlog
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * 
     * Returns the cumulative number of spikes generated by all neurons since
     * initialization or last reset. This metric quantifies overall neural activity
     * and can be used to monitor learning progress and network dynamics.
     * 
     * @return size_t Total number of spikes generated by the brain. This count
     *         includes spikes from all neural regions and neuron types.
     * 
     * @note Spike count is a fundamental metric for measuring neural activity and
     *       can be used to assess learning, memory formation, and network dynamics.
     *       The count persists until reset() is called or the brain object is destroyed.
     * 
     * @see getTotalSpikeCount for related activity metrics
     * @see reset() to reset this counter
     * @see getAverageFiringRate to compute average firing rate
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * Returns the number of spike events that are currently queued but not yet
     * processed. This metric indicates the backlog of spike processing and can
     * help identify potential performance bottlenecks in the simulation.
     * 
     * @return size_t Number of pending spike events waiting to be processed.
     *         This value should typically be small in a well-tuned simulation.
     * 
     * @note Pending spike events represent spikes that have been detected but not
     *       yet propagated through the network. A large pending spike count may
     *       indicate performance issues or numerical instability.
     * 
     * @warning High pending spike counts may indicate simulation instability or
     *         performance bottlenecks. Consider reducing network complexity or
     *         checking for numerical issues.
     * 
     * @see getTotalSpikeCount for total spike production metrics
     * @see getSpikeSystem for detailed spike system information
     */
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * Generates motor commands based on the current activity of motor neurons in the
     * brain. This method should be called each simulation step to translate neural
     * activity into actionable behavior for the agent or system being controlled.
     * 
     * @return std::unique_ptr<Action> Smart pointer to the generated action. Returns
     *         nullptr if no motor activity is detected or if action generation fails.
     * 
     * @retval nullptr No action generated - motor neurons are inactive
     * @retval non-null Pointer to the generated action with motor commands
     * 
     * @note This method typically interfaces with the AgentBrain class which decodes
     *       neural activity into motor commands. The generated action contains the
     *       actual movement or behavior commands to be executed.
     * 
     * @see AgentBrain::decodeMotorCommand for the motor decoding implementation
     * @see SimpleWorld::applyMotorCommand for action execution in the environment
     * @see injectCurrentToNeurons for influencing motor neuron activity
     * 
     * @example
     * @code
     * std::unique_ptr<Action> action = brain->produceAction();
     * if (action) {
     *     world->applyMotorCommand(*action, currentTime);
     * }
     * @endcode
     */
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    /**
     * @brief Apply neuromodulatory signals
     * 
     * Applies neuromodulatory signals (dopamine, serotonin, acetylcholine, etc.) to the
     * neural network. Neuromodulators influence neural plasticity, firing patterns, and
     * overall brain state, implementing motivational, arousal, and learning signals.
     * 
     * @param signal Neuromodulator object containing the neuromodulatory signal. This
     *              includes the type of modulator, concentration, timing, and other
     *              relevant parameters.
     * 
     * @note Neuromodulation is essential for adaptive behavior, learning, and motivation.
     *       Different neuromodulators have different effects on neural processing and plasticity.
     * 
     * @see getDopamine() for reward-related neuromodulation
     * @see getCuriosity() for exploration motivation
     * @see getNovelty() for novelty detection
     * @see getPredictionError() for prediction error signals
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    /**
     * @brief Update plasticity rules
     * 
     * Updates synaptic plasticity rules based on recent neural activity and neuromodulatory
     * signals. This method implements the actual synaptic weight changes according to
     * configured plasticity mechanisms (STDP, Hebbian, reward-modulated, etc.).
     * 
     * @note This method is typically called automatically during each simulation step
     *       by the brain simulation engine, but can also be called manually for
     *       specific timing requirements or testing.
     * 
     * @warning Plasticity updates modify synaptic weights permanently. Incorrect timing
     *         or inappropriate plasticity settings can lead to unstable network dynamics
     *         or runaway synaptic strengthening.
     * 
     * @see getSTDP() for detailed STDP mechanism
     * @see getHebbian() for detailed Hebbian mechanism
     * @see getStructuralPlasticity() for structural connection changes
     * @see applyNeuromodulation() for neuromodulation effects on plasticity
     */
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    /**
     * @brief Apply developmental changes
     * 
     * Applies developmental changes to the neural network architecture and parameters
     * based on the current developmental stage. This includes synaptic pruning,
     * myelination, connectivity changes, and other developmental processes that
     * shape the mature neural network.
     * 
     * @note Development typically occurs early in the simulation and affects long-term
     *       network structure and function. Developmental stages are managed by the
     *       DevelopmentSystem and can be controlled externally via setDevelopmentalStage().
     * 
     * @see getDevelopmentSystem() for developmental process management
     * @see getDevelopmentalStage() for current developmental stage
     * @see setDevelopmentalStage() for external stage control
     */
    void develop();
    
    // Reset brain state
    /**
     * @brief Reset brain state
     * 
     * Resets the brain to its initial state as defined by the configuration.
     * This clears all neural activity, spike history, synaptic weights, and memory
     * contents. After reset, the brain can be re-initialized or continue simulation
     * from a clean state.
     * 
     * @note Reset is useful for:
     *       - Starting a new simulation episode
     *       - Testing with a clean neural state
     *       - Clearing accumulated learning for exploration
     *       - Debugging simulation issues
     * 
     * @warning Reset clears all learned knowledge and memory. For checkpointing and
     *         saving learned behavior, use save() instead of reset().
     * 
     * @see save() for saving learned state
     * @see load() for restoring saved state
     * @see initialize() for initial configuration-based setup
     * 
     * @example
     * @code
     * // Save learned behavior
     * brain->save("learned_state.bin");
     * 
     * // Reset for new episode
     * brain->reset();
     * 
     * // Continue simulation
     * for (step = 0; step < episodeLength; ++step) {
     *     brain->step(step);
     * }
     * @endcode
     */
    void reset();
    
    // Save brain state to file (checkpointing)
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Saves the complete brain state including neural connectivity, synaptic weights,
     * memory contents, and developmental parameters to a file. This enables
     * checkpointing, resuming simulation from saved states, and preserving learned
     * behavior.
     * 
     * @param filepath Path to the file where brain state will be saved.
     * 
     * @return bool true if save operation succeeded, false otherwise.
     * 
     * @retval true Save operation completed successfully
     * @retval false Save operation failed - file may not be writable, disk full, etc.
     * 
     * @note Saved brain state can be loaded later using load() to resume simulation
     *       with the same neural architecture and learned behavior. This is essential
     *       for long-running simulations and preserving learned skills.
     * 
     * @warning Saved files contain binary data and configuration information. Loading
     *         saved state requires the exact same brain configuration and version.
     *         Attempting to load incompatible saved states may result in undefined behavior.
     * 
     * @see load() for restoring saved brain state
     * @see reset() for starting from initial configuration
     * @see getConfig() for configuration information
     * 
     * @example
     * @code
     * // Save after significant learning
     * if (brain->save("agent_state_checkpoint_001.bin")) {
     *     std::cout << "State saved successfully" << std::endl;
     * } else {
     *     std::cerr << "Failed to save state" << std::endl;
     * }
     * @endcode
     */
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
