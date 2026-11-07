#pragma once

#include "../core/Types/Types.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
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
 * @brief The central coordinator of the neural system
 * 
 * Implements real spiking neural computation with event-driven dynamics
 * and integrated memory, prediction, cognition, and neuromodulation systems.
 * 
 * The Brain class serves as the main controller for the NLM neural simulation,
 * coordinating all subsystems including neurons, synapses, plasticity rules,
 * memory systems, prediction, cognition, and neuromodulation. It manages
 * the overall simulation loop, sensory processing, motor output, and learning
 * through various mechanisms including STDP, Hebbian plasticity, and structural
 * plasticity.
 * 
 * The brain architecture follows a closed-loop processing pipeline:
 * 1. Sensory input from environment -> 2. Neural processing -> 3. Internal state
 * -> 4. Memory/prediction -> 5. Motivation/neuromodulation -> 6. Action selection
 * -> 7. Motor output -> 8. World consequences -> 9. Reward/error signals ->
 * 10. Plasticity updates -> 11. Changed future behavior
 * 
 * @note This is the main entry point for neural simulation and represents
 * Phase 2+ functionality (real neural computation with integrated systems).
 * 
 * @author NLM Project
 * @date 2026
 * @version 0.1.0
 * 
 * @see Neuron, Synapse, STDP, Brain, NeuralWorkingMemory
 * 
 * @ingroup core_systems
 */

class Brain {
public:
    /**
     * @brief Create brain with configuration
     * 
     * Constructs a new Brain instance with the specified configuration.
     * The configuration contains all parameters needed for brain initialization
     * including neuron count, connection probability, plasticity parameters,
     * and simulation settings.
     * 
     * @param config Shared pointer to configuration object. Must not be nullptr.
     *               Configuration will be copied internally.
     * 
     * @pre Configuration should be valid (contains all required parameters)
     * @post Brain object is constructed but not initialized
     * 
     * @throws std::runtime_error if config is nullptr
     * 
     * @example
     * @code
     * auto config = std::make_shared<Config>();
     * config->set("neuron_count", 1000);
     * auto brain = std::make_shared<Brain>(config);
     * brain->initialize();
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
     * Initializes the brain according to the configuration provided during construction.
     * This creates all neural components, sets up connectivity patterns, and prepares
     * the simulation environment for operation.
     * 
     * The initialization process includes:
     * - Creating neural populations and neurons
     * - Establishing synaptic connections
     * - Setting up plasticity rules
     * - Initializing memory systems
     * - Configuring neuromodulation systems
     * - Setting developmental stage
     * 
     * @return bool True if initialization succeeds, false otherwise
     * @retval true Initialization completed successfully
     * @retval false Initialization failed (typically due to configuration errors)
     * 
     * @pre Brain object must have been constructed with valid configuration
     * @post Brain is ready for simulation steps (can call step() methods)
     * 
     * @note This method should be called exactly once before simulation
     * @note Thread safety: Not thread-safe. Should be called from single thread
     * 
     * @see Brain::Brain, Brain::reset
     */
    bool initialize();
    
    /**
     * @brief Main simulation step
     * 
     * Performs one simulation step using the configured time step.
     * This advances the neural dynamics, processes spikes, updates plasticity,
     * and handles neuromodulation according to the brain's internal state.
     * 
     * The step operation includes:
     * - Event-driven spike propagation through neural network
     * - Membrane potential integration for neurons
     * - Synaptic transmission with delay effects
     * - Plasticity rule updates (STDP, Hebbian)
     * - Neuromodulation signal processing
     * - Action selection and motor output
     * - Memory system updates
     * - Development system updates
     * 
     * @param currentStep The current simulation step number (monotonically increasing)
     * 
     * @pre Brain must be initialized (brain->initialize() called)
     * @post Neural state updated for next step
     * 
     * @note Time step uses default timestep from configuration
     * @note Thread safety: Not thread-safe with respect to internal state
     * 
     * @example
     * @code
     * for (SimulationStep step = 0; step < maxSteps; ++step) {
     *     brain->step(step);
     * }
     * @endcode
     * 
     * @see Brain::step(Timestamp), Brain::receiveSensoryInput
     */
    void step(SimulationStep currentStep);

    /**
     * @brief Simulation time step
     * 
     * Performs one simulation step with explicit timestamp.
     * This method provides more control over timing than step(SimulationStep).
     * 
     * @param currentStep The current simulation step number
     * @param currentTime The absolute timestamp (seconds) for this step
     * 
     * @pre Brain must be initialized
     * @post Neural state updated with explicit timing
     * 
     * @note This is the preferred step method when precise timing is required
     * @note Thread safety: Not thread-safe
     * 
     * @see Brain::step(SimulationStep)
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Injects sensory current into appropriate neurons based on input pattern.
     * This method bridges the environment and the brain by processing external
     * or internal sensory signals and converting them into neural excitation.
     * 
     * The sensory input processing includes:
     * - Decoding input data into neural current patterns
     * - Mapping input to specific neuron populations
     * - Applying appropriate gain and scaling
     * - Integrating with existing neuromodulatory state
     * 
     * @param input The sensory input data structure containing:
     *             - Type: Vision, Audio, or InternalSignals
     *             - Data: Raw sensor readings or internal state values
     *             - Dimensions: Spatial or temporal extent of input
     *             - Timestamp: When the input was generated
     * 
     * @pre Brain must be initialized and have sensory neuron populations
     * @post Neural excitation injected according to input pattern
     * @post Sensory processing recorded in sensory buffers
     * 
     * @note Input is typically generated by world simulation or internal sensing
     * @note Thread safety: Should be called from simulation thread
     * 
     * @example
     * @code
     * Vision vision(320, 240, 3);
     * // Fill vision data with image pixels...
     * vision.setData(pixelData);
     * brain->receiveSensoryInput(vision);
     * @endcode
     * 
     * @see Action, MotorSystem, SensoryInput, SimpleWorld
     */
    void receiveSensoryInput(const class SensoryInput& input);

    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Applies external current directly to a single neuron's input.
     * This is useful for controlled stimulation experiments or direct
     * control of specific neurons.
     * 
     * @param neuron Neuron ID to receive current
     * @param current The membrane current to inject (positive = excitatory)
     * 
     * @pre Neuron with specified ID must exist
     * @post Neuron's membrane potential increases proportional to current
     * @post Current counted in total injected current statistics
     * 
     * @note Current is applied instantaneously (within timestep)
     * @note Can override natural input patterns for experimentation
     * 
     * @example
     * @code
     * // Stimulate a sensory neuron
     * brain->injectCurrent(sensoryNeuronId, 50.0f);
     * // Stimulate a motor neuron
     * brain->injectCurrent(motorNeuronId, 10.0f);
     * @endcode
     * 
     * @see Brain::injectCurrentToNeurons
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);

    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Applies uniform current to all neurons of a specified type.
     * This is useful for global stimulation or controlled experiments
     * where you want to activate all neurons of a certain function.
     * 
     * @param type The neuron type to stimulate (Sensory, Motor, etc.)
     * @param current The membrane current to inject to each neuron
     * 
     * @pre Brain must have neurons of specified type
     * @post All neurons of specified type receive current injection
     * @post Population-level activity increases
     * 
     * @note Different neuron types have different response characteristics
     * @note Useful for creating controlled input patterns
     * 
     * @example
     * @code
     * // Stimulate all sensory neurons
     * brain->injectCurrentToNeurons(NeuronType::Sensory, 30.0f);
     * // Stimulate all motor neurons
     * brain->injectCurrentToNeurons(NeuronType::Motor, 10.0f);
     * @endcode
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
/**
     * @brief Get excitation/inhibition balance ratio
     * 
     * Computes the ratio of excitatory to inhibitory neural activity.
     * This metric reflects the overall balance and stability of the network.
     * 
     * @return float The excitation/inhibition ratio (excitation/inhibition)
     * @retval > 1.0 Network is dominated by excitation
     * @retval = 1.0 Network is balanced
     * @retval < 1.0 Network is dominated by inhibition
     * 
     * @pre Brain must be initialized
     * @post Ratio computed from current neural activity
     * 
     * @note Used as a stability metric and for early warning of runaway excitation
     */
    float getExcitationInhibitionRatio() const;

    /**
     * @brief Get total spike count
     * 
     * Returns the cumulative number of spikes generated by all neurons
     * in the brain since initialization (resets when brain is reset).
     * This is a key measure of neural activity level.
     * 
     * @return size_t Total number of spikes generated (never decreases until reset)
     * 
     * @pre Brain must be initialized
     * @post Spike count returned
     * 
     * @note Reset when brain->reset() is called
     * @note Used for learning metrics and activity monitoring
     */
    size_t getTotalSpikeCount() const;

    /**
     * @brief Get pending spike event count
     * 
     * Returns the number of spike events currently scheduled for future delivery
     * due to synaptic delays. This reflects the temporal extent of neural activity.
     * 
     * @return size_t Number of pending spike events
     * 
     * @pre Brain must be initialized
     * @post Pending spike count returned
     * 
     * @note These spikes will be delivered in subsequent simulation steps
     * @note Important for understanding temporal dynamics
     */
    size_t getPendingSpikeEventCount() const;

    /**
     * @brief Produce motor/action output based on neural activity
     * 
     * Generates motor commands by interpreting neural activity patterns.
     * This method converts neural firing patterns into actionable outputs
     * that can drive the simulated environment or external systems.
     * 
     * The action production process includes:
     * - Reading motor neuron population activities
     * - Applying decision thresholds and inhibition
     * - Converting activity patterns to discrete actions
     * - Creating action parameter values
     * 
     * @return std::unique_ptr<Action> Pointer to generated action (nullptr if no action)
     * @retval Non-null Action when motor neuron activity exceeds threshold
     * @retval nullptr When no sufficient motor activity detected
     * 
     * @pre Brain must be initialized and have motor neuron populations
     * @post Action produced (or nullptr if none)
     * 
     * @note Action selection is based on winner-take-all dynamics
     * @note Action parameters are typically continuous values within ranges
     * 
     * @example
     * @code
     * auto action = brain->produceAction();
     * if (action) {
     *     // Execute the action (move, eat, rest, etc.)
     *     std::cout << "Action: " << action->getName() << std::endl;
     * }
     * @endcode
     * 
     * @see Action, MotorSystem
     */
    std::unique_ptr<class Action> produceAction();

    /**
     * @brief Apply neuromodulatory signals
     * 
     * Applies neuromodulatory signals that influence neural excitability,
     * plasticity, and learning rates. Neuromodulators like dopamine, curiosity,
     * and novelty affect how the brain responds to stimuli and learns from experience.
     * 
     * @param signal The neuromodulator signal containing:
     *             - Type: Dopamine, Curiosity, Novelty, PredictionError
     *             - Value: Signal strength/magnitude (-1.0 to +1.0)
     *             - Timestamp: When signal was generated
     *             - Context: Information about why signal was generated
     * 
     * @pre Brain must be initialized and have neuromodulation systems
     * @post Neuromodulator applied to appropriate brain regions
     * @post Plasticity rules affected if applicable
     * 
     * @note Neuromodulators can enhance or suppress learning
     * @note Can affect both short-term dynamics and long-term plasticity
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * Applies plasticity rules to modify synaptic strengths based on neural activity.
     * This includes STDP (Spike-Timing Dependent Plasticity), Hebbian plasticity,
     * and structural plasticity changes.
     * 
     * The plasticity update process:
     * - Detect pre-post spike pairs for STDP
     * - Apply Hebbian co-activation strengthening
     * - Update synaptic weights according to rules
     * - Modify synaptic connectivity (structural plasticity)
     * 
     * @pre Brain must be initialized and have plasticity systems enabled
     * @post Synaptic weights updated based on recent activity
     * @post Plasticity statistics updated
     * 
     * @note Called automatically by step() method
     * @note Can be called manually for controlled plasticity experiments
     * @note Thread safety: Should be called from simulation thread
     */
    void updatePlasticity();

    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * Applies developmental stage effects that modulate plasticity rates and
     * neural properties. During different developmental stages (Initial,
     * CriticalPeriod, Maturation, Adult, Aging), the brain exhibits different
     * plasticity characteristics and behavioral patterns.
     * 
     * @pre Brain must be initialized and have development system
     * @post Developmental changes applied (plasticity rates, excitability, etc.)
     * @post Neural properties modified according to current stage
     * 
     * @note Called automatically by step() method
     * @note Developmental stage affects learning capability and behavior
     */
    void develop();

    /**
     * @brief Reset brain state
     * 
     * Resets the brain to its initial state after initialization.
     * This clears spike counts, resets neuron states, and restores
     * synaptic weights to their initial values. Useful for running
     * multiple independent experiments or trials.
     * 
     * @pre Brain must be initialized
     * @post Brain state restored to initial conditions
     * @post All counters reset to zero
     * @post Spike history cleared
     * @post Synaptic weights restored to initial values
     * 
     * @note After reset(), must call initialize() again before next simulation
     * @note This is different from checkpoint save/load operations
     * 
     * @example
     * @code
     * brain->reset();  // Reset to initial state
     * brain->initialize();  // Ready for new simulation
     * @endcode
     */
    void reset();

    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Saves the complete brain state to a checkpoint file. This allows
     * resuming simulation from a previous state or analyzing learned behavior.
     * The saved state includes neural connectivity, weights, memory contents,
     * developmental stage, and learned preferences.
     * 
     * @param filepath Path to save checkpoint file
     * 
     * @return bool True if save succeeds, false otherwise
     * @retval true Checkpoint saved successfully
     * @retval false Save failed (file permissions, disk space, etc.)
     * 
     * @pre Brain must be initialized
     * @post Complete brain state saved to file
     * @post File can be loaded with load() method
     * 
     * @note Checkpoint format is binary and implementation-specific
     * @note File size can be large for complex brains
     * @note Use for long-running experiments or analysis
     * 
     * @example
     * @code
     * bool saved = brain->save("./brain_checkpoint.bin");
     * if (saved) {
     *     std::cout << "Checkpoint saved successfully" << std::endl;
     * }
     * @endcode
     * 
     * @see Brain::load, Phase6IntegratedExperiment::testCheckpointing
     */
    bool save(const std::string& filepath) const;

    /**
     * @brief Load brain state from file
     * 
     * Loads a brain state from a checkpoint file. This restores the complete
     * brain state including neural connectivity, synaptic weights, memory
     * contents, developmental stage, and learned preferences.
     * 
     * @param filepath Path to checkpoint file to load
     * 
     * @return bool True if load succeeds, false otherwise
     * @retval true State loaded successfully
     * @retval false Load failed (file not found, corrupted, incompatible)
     * 
     * @pre Brain should be reset and initialized
     * @post Brain state restored from file
     * @post All brain components recreated
     * @post Developmental stage and learned preferences restored
     * 
     * @note The brain will be in the same state as when saved
     * @note Requires matching brain configuration
     * @note File format is binary and implementation-specific
     * 
     * @example
     * @code
     * bool loaded = brain->load("./brain_checkpoint.bin");
     * if (loaded) {
     *     std::cout << "Checkpoint loaded successfully" << std::endl;
     *     brain->initialize();  // Ready to run simulation
     * }
     * @endcode
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region
     * 
     * Creates a new neural region with the specified name. Neural regions
     * are collections of interconnected neurons that perform specific functions
     * (e.g., sensory processing, motor control, memory storage). Regions
     * can be connected with long-range connections for inter-region communication.
     * 
     * @param name Optional name for the region (for debugging/logging)
     * 
     * @return RegionId Unique identifier for the new region
     * 
     * @pre Brain must be initialized
     * @post New region created and added to brain's region list
     * 
     * @note Region IDs are unique and sequentially assigned
     * @note Default region has ID 0 and is created during initialization
     * @note Can be null name (defaults to internal naming)
     */
    RegionId addRegion(const std::string& name = "");

    /**
     * @brief Get a region by ID
     * 
     * Retrieves the neural region with the specified identifier. This is the
     * primary method for accessing individual neural regions within the brain.
     * 
     * @param id Region ID to retrieve
     * @return NeuralRegion* Pointer to region (nullptr if not found)
     * 
     * @pre Brain must be initialized
     * @post Region pointer returned (may be nullptr)
     * 
     * @note Thread safety: Reading, not modifying region structure
     * @note Returns nullptr for invalid region IDs
     */
    NeuralRegion* getRegion(RegionId id);

    /**
     * @brief Get a region by ID (const version)
     * 
     * Const version of getRegion() for read-only access to regions.
     * This is the preferred method when you only need to read region data.
     * 
     * @param id Region ID to retrieve
     * @return const NeuralRegion* Const pointer to region (nullptr if not found)
     * 
     * @pre Brain must be initialized
     * @post Const region pointer returned
     * 
     * @note Thread safety: Reading, not modifying region structure
     * @note Returns nullptr for invalid region IDs
     * 
     * @see Brain::getRegion(RegionId)
     */
    const NeuralRegion* getRegion(RegionId id) const;

    /**
     * @brief Get the number of regions
     * 
     * Returns the total count of neural regions in the brain.
     * 
     * @return size_t Number of regions
     * 
     * @pre Brain must be initialized
     * @post Count returned
     */
    size_t getRegionCount() const;

    /**
     * @brief Get all region IDs
     * 
     * Returns the list of all region identifiers currently in the brain.
     * Useful for iterating over all regions or debugging.
     * 
     * @return std::vector<RegionId> List of region IDs
     * 
     * @pre Brain must be initialized
     * @post List of region IDs returned
     */
    std::vector<RegionId> getRegionIds() const;

    /**
     * @brief Get all regions
     * 
     * Returns references to all neural regions in the brain. This provides
     * access to the complete set of neural populations for comprehensive analysis
     * or manipulation.
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Reference to region list
     * 
     * @pre Brain must be initialized
     * @post Const reference to region list returned
     * 
     * @note This returns const references to avoid unnecessary copying
     * @note Region order is deterministic but implementation-dependent
     */
    const std billing<std::unique_ptr<NeuralRegion>>& getRegions() const;

    /**
     * @brief Add inter-region connection (long-range connectivity)
     * 
     * Creates a long-range synaptic connection between two different neural regions.
     * This enables communication between functionally distinct brain areas,
     * such as sensory to motor regions, or memory to decision-making regions.
     * 
     * @param source Region ID of the source region
     * @param target Region ID of the target region
     * @param weight Initial synaptic weight (default: 0.0)
     * @param delay Synaptic transmission delay in time steps (default: 1)
     * 
     * @pre Both source and target regions must exist
     * @post Inter-region connection created and stored
     * @post Connection will be used in spike propagation
     * 
     Viscosity
     * @note Connections have their own plasticity flags
     * @note Can be removed with removeInterRegionConnection()
     * @note Important for brain-wide integration and communication
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);

    /**
     * @brief Remove inter-region connection
     * 
     * Removes a long-range synaptic connection between two neural regions.
     * This effectively disconnects the regions for communication purposes.
     sporatio
     * @param source Region ID of source region
     * @param target Region ID of target region
     * 
     * @pre Both regions must exist
     * @post Inter-region connection removed (if exists)
     */
    void removeInterRegionConnection(RegionId source, RegionId target);

    /**
     * @brief Get total neuron count
     * 
     * Returns the total number/dry count of all neurons in the brain,
     * across all neural regions. This is a key measure of brain complexity.
     * 
     * @return size_t Total number of neurons
     * 
     * @pre Brain must be initialized
     * @post Count returned
     */
    size_t getTotalNeuronCount() const;

    /**
     * @brief Get total synapse count
     * 
     * Returns the total number of synapses in the brain, across all regions.
     * This reflects the connectivity and complexity of the neural network.
     * 
     * @return size_t Total number of synapses
     * 
     * @pre Brain must be initialized
     * @post Count returned
     */
    size_t getTotalSynapseCount() const;

    /**
     * @brief Get active neuron count
     * 
     * Returns the number of neurons currently in an active (firing) state.
     * This reflects the current level of neural engagement and processing.
     * 
     * @return size_t Number of active neurons
     * 
     * @pre Brain must be initialized
     * @post Count returned
     */
    size_t getActiveNeuronCount() const;

    /**
     * @brief Get firing neuron count
     * 
     * Returns the number of neurons currently in a firing (spiking) state.
     * This is different from active neurons (which includes non-spiking active states).
     * 
     * @return size_t Number Hicks-Firing neuron count
     * 
     * @pre Brain must be initialized
     * @post Count returned
     */
    size_t getFiringNeuronCount() const;

    /**
     * @brief Get average firing rate
     * 
     * Computes the average firing rate across all neurons in the brain.
     * This is a key metric of overall neural activity level.
     * 
     * @return float Average firing rate (spikes per time step)
     * 
     * @pre Brain must be initialized
     * @post Average rate computed and returned
     * 
     * @note Returns 0.0 if no neurons exist
     */
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

    // Learned preference trait (Phase 5): a single scalar, -1..1,
    // updated only by sandbox reward through AgentBrain, saved in the
    // checkpoint (SimulationState section) and used as a small bias in
    // later action choice. This is the measured "personality" step: a
    // preference learned from experience that survives a reload and
    // changes later behaviour. It is not a language-model persona and
    // it does not, on its own, make the brain a human brain.
    float getLearnedPreference() const;
    void setLearnedPreference(float preference);
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
