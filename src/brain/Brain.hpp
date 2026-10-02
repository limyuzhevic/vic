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

class Brain {
public:
    /**
     * @brief Constructor with configuration
     * 
     * Creates a new Brain instance with the specified configuration.
     * The configuration contains all parameters needed to initialize
     * the brain, including neural properties, plasticity settings, and
     * simulation parameters.
     * 
     * @param config Shared pointer to the configuration object
     * 
     * @see initialize
     */
    explicit Brain(std::shared_ptr<Config> config);

    /**
     * @brief Destructor
     * 
     * Cleans up the brain instance and releases all associated resources.
     * The brain uses the Pimpl idiom to hide implementation details and
     * ensure proper cleanup.
     */
    ~Brain();

    /**
     * @brief Copy constructor - disabled to prevent copying
     * 
     * The Brain class manages complex neural resources and state,
     * so copying is not allowed. Use move semantics instead for efficient
     * transfer of brain instances.
     */
    Brain(const Brain&) = delete;

    /**
     * @brief Copy assignment operator - disabled to prevent copying
     * @return Reference to this object (not used)
     */
    Brain& operator=(const Brain&) = delete;

    /**
     * @brief Move constructor - enables efficient transfer of brain instances
     * @param other The Brain instance to move from
     */
    Brain(Brain&&) noexcept;

    /**
     * @brief Move assignment operator - enables efficient transfer of brain instances
     * @param other The Brain instance to move from
     * @return Reference to this object for method chaining
     */
    Brain& operator=(Brain&&) noexcept;

    /**
     * @brief Initialize the brain with configuration
     * 
     * This method performs the complete initialization of the brain,
     * including:
     * - Creating neural regions
     * - Setting up neuron types and properties
     * - Establishing synaptic connections
     * - Initializing all subsystems
     * - Setting up random number generators
     * - Configuring simulation parameters
     * 
     * Initialization must be completed successfully before running
     * the simulation. This method should be called after constructing
     * the Brain instance.
     * 
     * @return true if initialization succeeded, false otherwise
     * 
     * @see reset
     * @see step
     */
    bool initialize();

    /**
     * @brief Main simulation step (time-based)
     * 
     * This method performs a single simulation time step using the
     * specified current step and time. It updates the entire neural
     * system according to the brain simulation model.
     * 
     * The step method orchestrates the complete neural computation cycle:
     * 1. Update neural membrane potentials
     * 2. Process spike events
     * 3. Apply plasticity rules
     * 4. Update neuromodulatory signals
     * 5. Apply developmental changes
     * 6. Generate motor outputs
     * 7. Log system state
     * 
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (in seconds)
     * 
     * @see step(SimulationStep)
     */
    void step(SimulationStep currentStep);

    /**
     * @brief Main simulation step (step-based)
     * 
     * This method performs a single simulation time step using only the
     * step number. The actual time is calculated internally based on
     * the simulation timestep configuration.
     * 
     * @param currentStep The current simulation step number
     * 
     * @see step(SimulationStep, Timestamp)
     */
    void step(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Receive sensory input from the environment
     * 
     * This method processes sensory input from the environment and injects
     * current into sensory neurons based on the input pattern. It serves
     * as the interface between the external environment and the neural system.
     * 
     * The sensory input is converted to neural current injection patterns
     * that drive the activity of sensory neurons. This input can represent
     * external stimuli, rewards, punishments, or other environmental signals.
     * 
     * @param input Reference to the sensory input object
     * 
     * @see SensoryInput
     * @see injectCurrent
     * @see injectCurrentToNeurons
     */
    void receiveSensoryInput(const class SensoryInput& input);

    /**
     * @brief Inject current directly into a specific neuron
     * 
     * This method directly injects current into a specific neuron, causing
     * an immediate change in its membrane potential. This is useful for
     * targeted stimulation of individual neurons or for implementing
     * specific input patterns.
     * 
     * @param neuron The ID of the neuron to inject current into
     * @param current The amount of current to inject (in nanoamperes)
     * 
     * @see receiveSensoryInput
     * @see injectCurrentToNeurons
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);

    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * This method injects current into all neurons of the specified type,
     * creating a coordinated input pattern across a population of neurons.
     * This is useful for implementing input patterns that affect multiple
     * neurons simultaneously.
     * 
     * @param type The type of neurons to inject current into
     * @param current The amount of current to inject (in nanoamperes)
     * 
     * @see receiveSensoryInput
     * @see injectCurrent
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);

    /**
     * @brief Get access to the spike system
     * 
     * This method returns a pointer to the spike system, which manages
     * all spike events in the neural system. The spike system handles:
     * - Spike generation and propagation
     * - Synaptic delay management
     * - Spike event scheduling
     * - Spike recording and statistics
     * 
     * @return Pointer to the spike system (non-owning)
     * @return nullptr if the spike system hasn't been initialized
     */
    SpikeSystem* getSpikeSystem();

    /**
     * @brief Get read-only access to the spike system
     * @return Const pointer to the spike system (non-owning)
     */
    const SpikeSystem* getSpikeSystem() const;

    /**
     * @brief Get access to the STDP plasticity system
     * 
     * This method returns a pointer to the Spike-Timing-Dependent Plasticity
     * (STDP) system, which implements synaptic learning based on the precise
     * timing of pre- and postsynaptic spikes. STDP is a biologically plausible
     * learning rule that strengthens synapses when presynaptic spikes
     * precede postsynaptic spikes (LTP) and weakens them when the order is
     * reversed (LTD).
     * 
     * @return Pointer to the STDP system (non-owning)
     */
    STDP* getSTDP();

    /**
     * @brief Get read-only access to the STDP plasticity system
     * @return Const pointer to the STDP system (non-owning)
     */
    const STDP* getSTDP() const;

    /**
     * @brief Get access to the Hebbian plasticity system
     * 
     * This method returns a pointer to the Hebbian plasticity system,
     * which implements synaptic strengthening based on correlated activity
     * between pre- and postsynaptic neurons. Hebbian plasticity follows
     * the principle "neurons that fire together, wire together" and is
     * often used to implement pattern learning and association formation.
     * 
     * @return Pointer to the Hebbian system (non-owning)
     */
    Hebbian* getHebbian();

    /**
     * @brief Get read-only access to the Hebbian plasticity system
     * @return Const pointer to the Hebbian system (non-owning)
     */
    const Hebbian* getHebbian() const;

    /**
     * @brief Get access to the structural plasticity system
     * 
     * This method returns a pointer to the structural plasticity system,
     * which implements changes to the neural network's connectivity
     * structure, including synaptogenesis (formation of new synapses)
     * and pruning (elimination of existing synapses). Structural plasticity
     * allows the network to adapt its architecture based on activity.
     * 
     * @return Pointer to the structural plasticity system (non-owning)
     */
    StructuralPlasticity* getStructuralPlasticity();

    /**
     * @brief Get read-only access to the structural plasticity system
     * @return Const pointer to the structural plasticity system (non-owning)
     */
    const StructuralPlasticity* getStructuralPlasticity() const;

    /**
     * @brief Get excitation/inhibition balance ratio
     * 
     * This method calculates and returns the ratio of excitatory to inhibitory
     * neural activity in the brain. This ratio is an important indicator of
     * network stability and dynamics:
     * - Ratios close to 1.0 indicate balanced networks
     * - Higher ratios indicate excitation-dominated networks (potentially unstable)
     * - Lower ratios indicate inhibition-dominated networks (potentially quiescent)
     * 
     * @return Excitation/inhibition ratio (typically between 0.1 and 10.0)
     */
    float getExcitationInhibitionRatio() const;

    /**
     * @brief Get the total number of spikes generated
     * 
     * This method returns the cumulative count of all spikes generated
     * throughout the simulation. It includes spikes from all neurons
     * and can be used to measure neural activity levels and learning progress.
     * 
     * @return Total number of spikes generated
     */
    size_t getTotalSpikeCount() const;

    /**
     * @brief Get the number of pending spike events
     * 
     * This method returns the number of spike events that have been
     * scheduled but not yet processed. Pending spikes are typically
     * due to synaptic delays and are processed in future time steps.
     * 
     * @return Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;

    /**
     * @brief Generate a motor/action output based on motor neuron activity
     * 
     * This method analyzes the current state of motor neurons and generates
     * an action output. The action represents the brain's behavioral
     * response to its internal state and external sensory input.
     * 
     * @return Unique pointer to the generated action
     * @return nullptr if no action is generated
     */
    std::unique_ptr<class Action> produceAction();

    /**
     * @brief Apply neuromodulatory signals to the brain
     * 
     * This method applies neuromodulatory signals that influence neural
     * processing and learning. Neuromodulators include:
     * - Dopamine: reward prediction and reinforcement learning
     * - Curiously: exploration motivation
     * - Novelty: novelty detection
     * - Prediction error: mismatch between expected and actual outcomes
     * 
     * These signals modulate neural plasticity, attention, and decision-making
     * processes, allowing the brain to adapt its behavior based on experience.
     * 
     * @param signal Reference to the neuromodulator signal
     */
    void applyNeuromodulation(const class Neuromodulator& signal);

    /**
     * @brief Update plasticity rules based on current neural activity
     * 
     * This method is called during each simulation step to update all
     * plasticity rules based on the current neural activity patterns.
     * It includes:
     * - STDP updates based on spike timing
     * - Hebbian updates based on correlated activity
     * - Structural plasticity updates based on activity-dependent rules
     * 
     * Plasticity updates are essential for learning and adaptation, allowing
     * the brain to change its synaptic connections based on experience.
     */
    void updatePlasticity();

    /**
     * @brief Apply developmental changes to the brain
     * 
     * This method applies developmental changes to the brain's structure
     * and function, based on the current developmental stage. Development
     * includes processes such as:
     * - Synaptogenesis: formation of new synaptic connections
     * - Pruning: elimination of weak synaptic connections
     * - Maturation: refinement of neural properties
     * - Migration: movement of neurons to their correct positions
     * 
     * Developmental changes are typically applied during specific
     * developmental stages and can significantly impact the brain's
     * functionality and learning capabilities.
     */
    void develop();

    /**
     * @brief Reset the brain to its initial state
     * 
     * This method resets the brain to its initial state, including:
     * - Resetting all neuron membrane potentials
     * - Clearing spike events
     * - Resetting plasticity accumulators
     * - Resetting neuromodulatory signals
     * - Resetting developmental timers
     * - Reinitializing random number generators
     * 
     * The reset operation is useful for running multiple independent
     * simulation trials or experiments with the same brain configuration.
     * 
     * @see initialize
     */
    void reset();

    /**
     * @brief Save brain state to a file (checkpointing)
     * 
     * This method saves the current brain state to a file, allowing for
     * checkpointing and restoration of simulation state. The saved state
     * includes:
     * - All neuron states
     * - Synaptic weights
     * - Plasticity accumulators
     * - Neuromodulatory signals
     * - Spike events
     * - Configuration parameters
     * 
     * Checkpointing is useful for:
     * - Saving progress in long simulations
     * - Restoring from crashes or errors
     * - Running multiple experiments with the same initial conditions
     * - Analyzing brain states at specific points in time
     * 
     * @param filepath Path to the output file
     * @return true if the state was successfully saved, false otherwise
     */
    bool save(const std::string& filepath) const;

    /**
     * @brief Load brain state from a file
     * 
     * This method loads a previously saved brain state from a file,
     * restoring the brain to a previous configuration. The loaded state
     * should be compatible with the current brain configuration.
     * 
     * @param filepath Path to the input file
     * @return true if the state was successfully loaded, false otherwise
     */
    bool load(const std::string& filepath);

    /**
     * @brief Add a new neural region to the brain
     * 
     * This method creates a new neural region with the specified name.
     * Neural regions are independent areas of the brain that can contain
     * different types of neurons and perform different functions. New
     * regions can be added dynamically during simulation.
     * 
     * @param name Optional name for the new region
     * @return ID of the newly created region
     */
    RegionId addRegion(const std::string& name = "");

    /**
     * @brief Get a neural region by ID
     * 
     * This method returns a pointer to the neural region with the specified
     * ID. Neural regions are the basic building blocks of the brain's
     * architecture, containing neurons and defining functional areas.
     * 
     * @param id The ID of the region to retrieve
     * @return Pointer to the region (non-owning)
     * @return nullptr if no region with the specified ID exists
     */
    NeuralRegion* getRegion(RegionId id);

    /**
     * @brief Get a const neural region by ID
     * 
     * This method returns a const pointer to the neural region with the
     * specified ID. This version is used for read-only access to region
     * information.
     * 
     * @param id The ID of the region to retrieve
     * @return Const pointer to the region (non-owning)
     */
    const NeuralRegion* getRegion(RegionId id) const;

    /**
     * @brief Get the number of regions in the brain
     * @return Number of neural regions
     */
    size_t getRegionCount() const;

    /**
     * @brief Get all region IDs
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;

    /**
     * @brief Get all neural regions
     * 
     * This method returns all neural regions in the brain. The returned
     * vector contains unique pointers to all regions, which can be used
     * for iterating over all regions or for specific region operations.
     * 
     * @return Vector of unique pointers to all neural regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;

    /**
     * @brief Add an inter-regional connection
     * 
     * This method creates a connection between two neural regions,
     * enabling long-range communication and coordination. Inter-regional
     * connections are used to integrate information across different
     * areas of the brain and implement complex cognitive functions.
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Initial synaptic weight (default: 0.0f)
     * @param delay Transmission delay in time steps (default: 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);

    /**
     * @brief Remove an inter-regional connection
     * 
     * This method removes the connection between two neural regions,
     * breaking their long-range communication.
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);

    /**
     * @brief Get the total number of neurons in the brain
     * @return Total number of neurons
     */
    size_t getTotalNeuronCount() const;

    /**
     * @brief Get the total number of synapses in the brain
     * @return Total number of synapses
     */
    size_t getTotalSynapseCount() const;

    /**
     * @brief Get the number of currently active neurons
     * 
     * This method returns the number of neurons that are currently
     * above threshold and have generated spikes in the current time step.
     * Active neurons are those that are participating in the current
     * neural computation process.
     * 
     * @return Number of active neurons
     */
    size_t getActiveNeuronCount() const;

    /**
     * @brief Get the number of neurons that fired in the current step
     * @return Number of firing neurons
     */
    size_t getFiringNeuronCount() const;

    /**
     * @brief Get the average firing rate of all neurons
     * 
     * This method calculates and returns the average firing rate of all
     * neurons in the brain, based on their spike activity over time. The
     * firing rate is typically measured in spikes per second.
     * 
     * @return Average firing rate (spikes per second)
     */
    float getAverageFiringRate() const;

    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Get access to the working memory system
     * 
     * The working memory system stores transient active information
     * used in ongoing cognitive processes. It includes:
     * - Short-term memory traces
     * - Active representations
     * - Attention allocations
     * - Chunked information
     * 
     * Working memory is essential for:
     * - Maintaining information during complex tasks
     * - Integrating multiple sensory inputs
     * - Planning and decision-making
     * - Learning and forming associations
     * 
     * @return Pointer to the working memory system (non-owning)
     */
    NeuralWorkingMemory* getWorkingMemory();

    /**
     * @brief Get read-only access to the working memory system
     * @return Const pointer to the working memory system (non-owning)
     */
    const NeuralWorkingMemory* getWorkingMemory() const;

    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get access to the prediction system
     * 
     * The prediction system implements predictive coding and inference,
     * including:
     * - Predictive models of sensory input
     * - Prediction error computation
     * - Temporal sequence prediction
     * - Expectation formation
     * 
     * Prediction systems are essential for:
     * - Understanding environmental regularities
     * - Anticipating future events
     * - Reducing uncertainty
     * - Guiding attention and learning
     * 
     * @return Pointer to the prediction system (non-owning)
     */
    PredictionSystem* getPredictionSystem();

    /**
     * @brief Get read-only access to the prediction system
     * @return Const pointer to the prediction system (non-owning)
     */
    const PredictionSystem* getPredictionSystem() const;

    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get access to the neural planner
     * 
     * The neural planner implements action planning and goal-directed
     * behavior, including:
     * - Action sequence generation
     * - Goal representation and selection
     * - Policy evaluation
     * - Reinforcement learning integration
     * 
     * Planning systems are essential for:
     * - Goal-directed behavior
     * - Sequential decision-making
     * - Action selection
     * - Strategic planning
     * 
     * @return Pointer to the neural planner (non-owning)
     */
    NeuralPlanner* getPlanner();

    /**
     * @brief Get read-only access to the neural planner
     * @return Const pointer to the neural planner (non-owning)
     */
    const NeuralPlanner* getPlanner() const;

    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get access to the development system
     * 
     * The development system manages neural development and maturation,
     * including:
     * - Synaptogenesis: formation of new synaptic connections
     * - Pruning: elimination of weak synaptic connections
     * - Maturation: refinement of neural properties
     * - Migration: positioning of neurons
     * 
     * The development system is essential for:
     * - Neural network maturation
     * - Activity-dependent development
     * - Adaptive brain development
     * - Experience-dependent plasticity
     * 
     * @return Pointer to the development system (non-owning)
     */
    DevelopmentSystem* getDevelopmentSystem();

    /**
     * @brief Get the current developmental stage
     * 
     * The developmental stage represents the current phase of neural
     * development. Different stages may have different characteristics:
     * - Stage 1: Initial formation
     * - Stage 2: Synaptogenesis
     * - Stage 3: Pruning
     * - Stage 4: Maturation
     * - Stage 5: Refinement
     * - Stage 6: Final integration
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;

    /**
     * @brief Set the developmental stage
     * 
     * This method changes the brain's developmental stage, which can
     * trigger different developmental processes and enable specific
     * functionality for different phases of development.
     * 
     * @param stage The new developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);

    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get access to the dopamine system
     * 
     * The dopamine system implements reward prediction and reinforcement
     * learning, including:
     * - Reward prediction error computation
     * - Reward-based learning
     * - Motivation and reinforcement
     * - Reward anticipation and valuation
     * 
     * Dopamine systems are essential for:
     * - Reinforcement learning
     * - Reward-based decision-making
     * - Motivation and drive
     * - Expectation formation
     * 
     * @return Pointer to the dopamine system (non-owning)
     */
    Dopamine* getDopamine();

    /**
     * @brief Get read-only access to the dopamine system
     * @return Const pointer to the dopamine system (non-owning)
     */
    const Dopamine* getDopamine() const;

    /**
     * @brief Get access to the curiosity system
     * 
     * The curiosity system implements exploration motivation and
     * intrinsic motivation, including:
     * - Intrinsic reward signals
     * - Exploration drive
     * - Novelty seeking
     * - Information-seeking behavior
     * 
     * Curiosity systems are essential for:
     * - Exploration-exploitation balance
     * - Intrinsic motivation
     * - Discovery and innovation
     * - Adaptive behavior
     * 
     * @return Pointer to the curiosity system (non-owning)
     */
    Curiosity* getCuriosity();

    /**
     * @brief Get read-only access to the curiosity system
     * @return Const pointer to the curiosity system (non-owning)
     */
    const Curiosity* getCuriosity() const;

    /**
     * @brief Get access to the novelty detection system
     * 
     * The novelty detection system identifies novel or unexpected events,
     * including:
     * - Novelty computation
     * - Prediction error detection
     * - Change detection
     * - Surprise signals
     * 
     * Novelty systems are essential for:
     * - Change detection
     * - Anomaly detection
     * - Learning from unexpected events
     * - Adaptive responding to change
     * 
     * @return Pointer to the novelty system (non-owning)
     */
    Novelty* getNovelty();

    /**
     * @brief Get read-only access to the novelty detection system
     * @return Const pointer to the novelty system (non-owning)
     */
    const Novelty* getNovelty() const;

    /**
     * @brief Get access to the prediction error system
     * 
     * The prediction error system computes the difference between
     * expected and actual outcomes, including:
     * - Prediction error computation
     * - Expectation updating
     * - Error-based learning
     * - Predictive coding
     * 
     * Prediction error systems are essential for:
     * - Learning from prediction errors
     * - Adaptive behavior
     * - Model-based learning
     * - Hierarchical reinforcement learning
     * 
     * @return Pointer to the prediction error system (non-owning)
     */
    PredictionError* getPredictionErrorSignal();

    /**
     * @brief Get read-only access to the prediction error system
     * @return Const pointer to the prediction error system (non-owning)
     */
    const PredictionError* getPredictionErrorSignal() const;

    /**
     * @brief Get the current configuration
     * 
     * This method returns the configuration object used to initialize
     * the brain. The configuration contains all parameters that were
     * used during brain initialization.
     * 
     * @return Const shared pointer to the configuration
     */
    std::shared_ptr<const Config> getConfig() const;

    /**
     * @brief Get access to the random number generator
     * 
     * This method returns a pointer to the random number generator,
     * which is used for:
     * - Initializing neural weights
     * - Generating random connections
     * - Random sampling in learning rules
     * - Stochastic processes
     * 
     * The random number generator ensures reproducibility when a
     * specific seed is set in the configuration.
     * 
     * @return Pointer to the random generator (non-owning)
     */
    RandomGenerator* getRandomGenerator();

    /**
     * @brief Log brain status and statistics
     * 
     * This method logs comprehensive information about the current
     * state of the brain, including:
     * - Network statistics (neuron and synapse counts)
     * - Activity statistics (spike counts, firing rates)
     * - System configuration
     * - Performance metrics
     * - Subsystems status
     * 
     * This method is useful for monitoring brain behavior and debugging.
     */
    void logStatus() const;

private:
    /**
     * @brief Private implementation details (Pimpl idiom)
     * 
     * The Brain class uses the Pimpl idiom to hide implementation details
     * and reduce compilation dependencies. All actual brain implementation
     * is contained in this private struct.
     */
    struct Impl;

    /**
     * @brief Pointer to the private implementation
     */
    Impl* pImpl;
};

} // namespace nlm
