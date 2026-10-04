#pragma once

/**
 * @file Brain.hpp
 * @brief The central coordinator of the neural system implementing real spiking neural computation
 * @author NLM Team
 * @date 2024
 * @version 1.0
 */

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

namespace nlm {

/**
 * @namespace nlm
 * @brief Neural Machine Learning (NLM) - A spiking neural network framework for cognitive computing
 *
 * This namespace encompasses the entire NLM ecosystem, from low-level neuron dynamics to
 * high-level cognitive functions including memory, prediction, attention, and development.
 */

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
class SensoryInput;
class Action;
class Neuromodulator;

// Helper classes for refactoring complex functions
// Helper classes for refactoring complex functions

/**
 * @class SimulationStepCoordinator
 * @brief Coordinates the execution of all 15 simulation steps for the brain
 * @details This class encapsulates the complex 15-step simulation process into a clean,
 *          reusable interface. Each step represents a logical phase of neural computation
 *          from spike detection to plasticity updates and memory consolidation.
 * @note This class is used internally by Brain and is not part of the public API
 * @see Brain::step()
 */

class SimulationStepCoordinator {
public:
    /**
     * @brief Constructs a simulation step coordinator
     * @param brain Pointer to the parent Brain instance
     * @pre The Brain instance must remain valid for the lifetime of this coordinator
     */
    SimulationStepCoordinator(Brain* brain) : brain_(brain) {}

    /**
     * @brief Executes the complete 15-step simulation process
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @note This method processes all neural dynamics, plasticity, memory, and output
     *       in the correct sequence. Call this from Brain::step().
     * @see executeStep1()
     * @see executeStep2()
     */
    void execute(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 1: Process pending delayed spike events
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Delivers synaptic input that was queued during previous steps.
     *          This completes the synaptic transmission chain.
     */
    void executeStep1(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 2: Update all neurons (LIF dynamics)
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Updates each neuron's membrane potential using the Leaky
     *          Integrate-and-Fire (LIF) dynamics equation.
     */
    void executeStep2(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 3: Detect spikes and schedule spike events
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Identifies neurons that have crossed their firing threshold,
     *          schedules immediate and delayed spike events, and records spike history.
     */
    void executeStep3(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 4: Process immediate spikes
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Processes queued spike events, delivering synaptic input to
     *          postsynaptic neurons.
     */
    void executeStep4(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 5: Update working memory
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Updates the neural working memory system with current neural activity.
     */
    void executeStep5(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 6: Apply neuromodulation effects
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Applies neuromodulatory signals (dopamine, curiosity, novelty)
     *          that modulate neural excitability and plasticity.
     */
    void executeStep6(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 7: Apply plasticity rules (STDP and Hebbian)
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Applies spike-timing-dependent plasticity (STDP) and Hebbian
     *          learning rules to modify synaptic weights based on neural activity.
     */
    void executeStep7(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 8: Update episodic memory
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Stores current neural activity patterns in episodic memory
     *          for later recall and consolidation.
     */
    void executeStep8(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 9: Update prediction system
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Updates the prediction system for sensory prediction and error
     *          computation. (Phase 2 enhancement needed)
     */
    void executeStep9(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 10: Update attention system
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Updates attentional selection based on working memory content
     *          and relevance signals.
     */
    void executeStep10(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 11: Update concept formation
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Updates concept formation system for pattern discovery and
     *          abstraction. (Phase 2 enhancement needed)
     */
    void executeStep11(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 12: Apply structural plasticity periodically
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Applies structural plasticity (synaptogenesis and pruning)
     *          every 100 simulation steps to modify network topology.
     */
    void executeStep12(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 13: Replay important memories
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Replays recently stored episodic memories to strengthen
     *          memory traces and support consolidation.
     */
    void executeStep13(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 14: Apply development effects
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Applies developmental changes to the neural system every
     *          1000 simulation steps, adjusting plasticity based on
     *          developmental stage.
     */
    void executeStep14(SimulationStep currentStep, Timestamp currentTime);

    /**
     * @brief Executes simulation step 15: Checkpoint management
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Manages checkpoint saving and loading for fault tolerance
     *          and reproducibility.
     */
    void executeStep15(SimulationStep currentStep, Timestamp currentTime);

private:
    Brain* brain_;
};

/**
 * @class BrainStatisticsCalculator
 * @brief Provides static utility methods for calculating brain-wide statistics
 * @details Computes various statistics across all regions, neurons, and synapses in the brain.
 *          These statistics are useful for monitoring brain state, debugging, and analysis.
 * @note All methods are static and can be called on the class itself (no Brain instance needed).
 */

class BrainStatisticsCalculator {
public:
    /**
     * @brief Calculates the excitation/inhibition (E/I) ratio across the brain
     * @param brain Pointer to the Brain instance to analyze
     * @return float The excitation/inhibition ratio. Returns infinity if no inhibitory synapses,
     *               or 0 if no excitatory synapses.
     * @details Sums the weights of all excitatory (positive weight) and inhibitory (negative weight)
     *          synapses across all regions. The ratio is total excitatory weight divided by
     *          total absolute inhibitory weight.
     * @warning High E/I ratios may indicate instability or hyperexcitability.
     */
    static float calculateExcitationInhibitionRatio(const Brain* brain);

    /**
     * @brief Counts the total number of spikes across all neurons in the brain
     * @param brain Pointer to the Brain instance to analyze
     * @return size_t Total spike count since brain initialization
     * @note Includes spikes from all regions and all time steps.
     */
    static size_t calculateTotalSpikeCount(const Brain* brain);

    /**
     * @brief Counts the number of neurons currently in refractory state
     * @param brain Pointer to the Brain instance to analyze
     * @return size_t Number of neurons in refractory period
     * @note Neurons in refractory state cannot fire during this period.
     */
    static size_t calculatePendingSpikeEventCount(const Brain* brain);

    /**
     * @brief Counts the total number of neurons in the brain
     * @param brain Pointer to the Brain instance to analyze
     * @return size_t Total neuron count across all regions
     */
    static size_t calculateTotalNeuronCount(const Brain* brain);

    /**
     * @brief Counts the total number of synapses in the brain
     * @param brain Pointer to the Brain instance to analyze
     * @return size_t Total synapse count across all regions and inter-region connections
     */
    static size_t calculateTotalSynapseCount(const Brain* brain);

    /**
     * @brief Counts neurons that are currently firing or active
     * @param brain Pointer to the Brain instance to analyze
     * @return size_t Number of neurons with active membrane potential
     * @details Includes neurons that are currently firing and those in the
     *          active state but not yet refractory.
     */
    static size_t calculateActiveNeuronCount(const Brain* brain);

    /**
     * @brief Calculates the average firing rate across all brain regions
     * @param brain Pointer to the Brain instance to analyze
     * @return float Average firing rate (Hz) across all regions
     * @details Computes the mean of each region's average firing rate, not
     *          a global average of individual neuron firing rates.
     */
    static float calculateAverageFiringRate(const Brain* brain);
};

/**
 * @class CheckpointManager
 * @brief Manages brain state checkpointing for fault tolerance and reproducibility
 * @details Handles saving and loading brain state (neurons, synapses, parameters) to/from
 *          persistent storage. Includes compression, error recovery, and validation.
 * @note Thread-safe for concurrent checkpoint operations.
 * @see CheckpointSystem
 */

class CheckpointManager {
public:
    /**
     * @brief Configures the checkpoint manager
     * @param directory Directory path for checkpoint files (must exist and be writable)
     * @param maxCheckpoints Maximum number of checkpoint files to retain
     * @param maxFileSize Maximum size per checkpoint file in bytes
     * @param compressionEnabled Whether to enable compression for checkpoint files
     * @pre directory must be a valid, writable path
     * @note Checkpoints older than maxCheckpoints or larger than maxFileSize may be auto-deleted.
     */
    void configure(const std::string& directory, size_t maxCheckpoints, 
                   size_t maxFileSize, bool compressionEnabled);

    /**
     * @brief Saves the current brain state to a checkpoint file
     * @param brain Pointer to the Brain instance to save
     * @param filepath Path to the checkpoint file to create
     * @return bool true if save was successful, false on error
     * @throws NeuralException if save operation fails
     * @note Saves neuron states (membrane potential, refractory periods), synapse weights,
     *       plasticity traces, and brain configuration parameters.
     * @warning Large brains may take significant time to checkpoint.
     */
    bool save(const Brain* brain, const std::string& filepath) const;

    /**
     * @brief Loads brain state from a checkpoint file
     * @param brain Pointer to the Brain instance to restore
     * @param filepath Path to the checkpoint file to load
     * @return bool true if load was successful, false on error
     * @throws NeuralException if load operation fails
     * @note Restores neuron states and synaptic weights. Network topology is preserved.
     * @warning Loading a checkpoint resets simulation time and state.
     */
    bool load(Brain* brain, const std::string& filepath);

    /**
     * @brief Gets the maximum number of checkpoints to retain
     * @return size_t Maximum checkpoints configured
     */
    size_t getMaxCheckpoints() const { return maxCheckpoints_; }

    /**
     * @brief Gets the maximum file size per checkpoint
     * @return size_t Maximum file size in bytes
     */
    size_t getMaxFileSize() const { return maxFileSize_; }

    /**
     * @brief Checks if compression is enabled
     * @return bool true if compression is enabled, false otherwise
     */
    bool isCompressionEnabled() const { return compressionEnabled_; }

private:
    std::string directory_;
    size_t maxCheckpoints_ = 10000;
    size_t maxFileSize_ = 5000000;  // 5MB
    bool compressionEnabled_ = true;
};

/**
 * @class SensoryInputProcessor
 * @brief Processes sensory input from the environment and injects it into neurons
 * @details Converts external sensory data (e.g., audio, visual, tactile) into membrane
 *          current injected into sensory neurons. Supports multiple sensory modalities.
 * @note Static class with no instance state. Used by Brain::receiveSensoryInput().
 */

class SensoryInputProcessor {
public:
    /**
     * @brief Processes sensory input and injects current into appropriate neurons
     * @param processor Pointer to this processor (unused, kept for API consistency)
     * @param input The sensory input data from the environment
     * @param brain Pointer to the parent Brain instance
     * @note Routes sensory data to the appropriate neural populations based on modality.
     *       Modality-specific processing happens in specialized processors.
     */
    static void process(SensoryInputProcessor* processor, const SensoryInput& input, Brain* brain);

    /**
     * @brief Injects sensory data directly into a set of sensory neurons
     * @param sensoryNeurons Vector of sensory neurons to receive input
     * @param input Sensory input containing the data to inject
     * @details Extracts the input data and distributes it across sensory neurons
     *          according to their receptive fields and tuning properties.
     * @warning Requires sensory neurons to be properly configured and registered.
     */
    static void injectToSensoryNeurons(const std::vector<Neuron*>& sensoryNeurons, 
                                        const class SensoryInput& input);

    /**
     * @brief Normalizes and injects raw sensor values into neurons
     * @param values Raw sensor values (typically in range [-1, 1] or [0, 255])
     * @param neurons Vector of neurons to receive the normalized current
     * @details Normalizes input values to biologically plausible range (-10 to 10 mV)
     *          and injects them into target neurons. Handles value clamping and
     *          distribution according to neural sensitivity.
     */
    static void normalizeAndInject(const std::vector<float>& values, 
                                  const std::vector<Neuron*>& neurons);
};

/**
 * @class ActionSelectionSystem
 * @brief Selects appropriate actions based on motor neuron activity
 * @details Converts neural activity patterns into discrete actions for the agent.
 *          Implements simple action selection based on motor neuron firing rates.
 * @note Static class used by Brain to produce motor output. Integration with
 *       more sophisticated action selection policies can be added in future phases.
 */

class ActionSelectionSystem {
public:
    /**
     * @brief Selects an action based on motor neuron activity
     * @param motorNeurons Vector of motor neurons to analyze
     * @return std::unique_ptr<Action> Selected action, or nullptr if no action selected
     * @details Analyzes firing patterns across motor neurons and maps them to
     *          appropriate action types. Currently implements simple majority voting.
     * @note Returns a Wait action if no motor neurons are firing or if input is empty.
     */
    static std::unique_ptr<Action> selectAction(const std::vector<Neuron*>& motorNeurons);

    /**
     * @brief Calculates motor neuron activity statistics
     * @param motorNeurons Vector of motor neurons to analyze
     * @param firingMotor Reference to output parameter that will receive the count
     *                    of firing motor neurons
     * @details Counts how many motor neurons are currently in firing state.
     *          Used for activity monitoring and action selection.
     */
    static void calculateMotorActivity(const std::vector<Neuron*>& motorNeurons, 
                                       size_t& firingMotor);

    /**
     * @brief Maps motor activity count to action type
     * @param firingMotor Number of motor neurons that are firing
     * @return ActionType Corresponding action type based on activity level
     * @details Simple threshold-based mapping: any firing motor neurons produce
     *          MoveForward action, otherwise Wait action.
     * @warning This is a placeholder implementation for Phase 2.
     */
    static ActionType activityToActionType(size_t firingMotor);
};

/**
 * @class NeuronStepProcessor
 * @brief Processes individual neurons and regions for one simulation step
 * @details Contains the low-level functions that update neuron states, handle spikes,
 *          and manage synaptic transmission. These are the core computational
 *          primitives used by the simulation step coordinator.
 * @note Static class with no instance state. Used internally by SimulationStepCoordinator.
 */

class NeuronStepProcessor {
public:
    /**
     * @brief Processes a single neuron for one LIF integration step
     * @param neuron Pointer to the neuron to process
     * @param currentTime The current simulation time (seconds)
     * @param dt The time step duration (seconds)
     * @return bool true if the neuron fired (crossed threshold), false otherwise
     * @details Updates the neuron's membrane potential using the Leaky Integrate-and-Fire
     *          dynamics equation: dV/dt = (V_rest - V)/tau + I/C
     * @note Handles refractory periods, spike detection, and reset.
     */
    static bool processNeuronStep(Neuron* neuron, Timestamp currentTime, TimestepDuration dt);

    /**
     * @brief Processes all neurons in a specific region
     * @param brain Pointer to the parent Brain instance
     * @param regionId ID of the region to process
     * @param currentTime The current simulation time (seconds)
     * @param dt The time step duration (seconds)
     * @details Iterates through all populations in the region and updates each neuron.
     *          Used by SimulationStepCoordinator::executeStep2().
     */
    static void processRegionNeurons(Brain* brain, RegionId regionId, 
                                      Timestamp currentTime, TimestepDuration dt);

    /**
     * @brief Processes spike events and their effects
     * @param brain Pointer to the parent Brain instance
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Processes all neurons that fired during this step, scheduling
     *          immediate and delayed spike events and updating spike history.
     * @note Called by SimulationStepCoordinator::executeStep3().
     */
    static void processSpikes(Brain* brain, SimulationStep currentStep, 
                              Timestamp currentTime);

    /**
     * @brief Delivers synaptic input from delayed spike events
     * @param brain Pointer to the parent Brain instance
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @details Processes all delayed spike events that are now ready to deliver
     *          their synaptic input to postsynaptic neurons.
     * @note Called by SimulationStepCoordinator::executeStep1().
     */
    static void deliverDelayedSpikes(Brain* brain, SimulationStep currentStep, 
                                      Timestamp currentTime);
};

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
 * @struct InterRegionConnection
 * @brief Represents a connection between two brain regions (long-range connectivity)
 * @details Models anatomical connections that span multiple regions, enabling
 *          inter-regional communication and integration.
 * @note Used by Brain for long-range connectivity management.
 */

struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Default constructor creates an unconnected connection
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}

    /**
     * @brief Constructs a connection with specified parameters
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Synaptic weight (-1.0 to 1.0)
     * @param d Delay in simulation steps (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief The central coordinator of the neural system implementing real spiking neural computation
 * @details The Brain class is the main interface for the NLM framework. It coordinates all neural
 *          computation, including neuron dynamics, synaptic plasticity, memory systems, prediction,
 *          cognition, neuromodulation, and motor output. The Brain manages multiple regions, each
 *          containing populations of neurons with complex connectivity and dynamics.
 *
 * The Brain implements the 15-step simulation process for each time step, coordinating all
 * neural subsystems including:
 * - Low-level neuron dynamics (LIF integration, spiking)
 * - Synaptic transmission and plasticity (STDP, Hebbian, reward-modulated)
 * - Working and episodic memory
 * - Prediction and cognitive systems
 * - Neuromodulatory signals (dopamine, curiosity, novelty)
 * - Developmental processes
 * - Motor output generation
 *
 * @note This class uses the Pimpl idiom for forward declaration and compilation efficiency.
 *       Brain is move-constructible and move-assignable, but not copyable.
 * @see SimulationStepCoordinator for step execution details
 * @see BrainStatisticsCalculator for utility functions
 * @see CheckpointManager for persistence
 */

class Brain {
public:
    /**
     * @brief Constructs a brain with the given configuration
     * @param config Shared pointer to configuration parameters
     * @throws NeuralException if configuration is invalid or initialization fails
     * @note Configuration should include simulation parameters, connection probabilities,
     *       learning rates, and system thresholds.
     * @see Config
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor cleans up brain resources and performs shutdown
     */
    ~Brain();
    
    /**
     * @brief Disables copying (Brain is not copyable)
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disables copy assignment
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Enables move construction (efficient transfer of ownership)
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Enables move assignment (efficient transfer of ownership)
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initializes the brain with configuration parameters
     * @return bool true if initialization succeeds, false on error
     * @throws NeuralException if critical initialization fails
     * @note Initializes all neural systems, memory, plasticity, and configuration.
     *       Must be called before any other operations.
     */
    bool initialize();
    
    /**
     * @brief Main simulation entry point (simplified interface)
     * @param currentStep The current simulation step number
     * @note Executes the 15-step simulation process. Internal time tracking is used.
     *       Prefer the overload that takes both step and time for precise control.
     * @see step(SimulationStep, Timestamp)
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation entry point (full interface)
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time (seconds)
     * @note This is the primary simulation method. It executes the complete 15-step
     *       process, updating neural dynamics, plasticity, memory, and outputs.
     *       Must be called with monotonically increasing time values.
     * @see execute()
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receives sensory input from the environment and injects it into neurons
     * @param input The sensory input data from the environment
     * @details Processes sensory data (audio, visual, tactile, etc.) and injects
     *          appropriate current into sensory neurons based on input patterns.
     *          Sensory neurons must be properly registered in the brain.
     * @see SensoryInputProcessor
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Injects current directly into a specific neuron
     * @param neuron Neuron ID to receive current
     * @param current Current to inject (mV)
     * @note Direct current injection bypasses synaptic transmission, useful for
     *       external control or stimulation.
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Injects current into all neurons of a specific type
     * @param type Neuron type to target (e.g., Sensory, Motor, Excitatory)
     * @param current Current to inject per neuron (mV)
     * @note Efficient for large-scale modulation of neural populations.
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Accesses the spike system for low-level spike event management
     * @return SpikeSystem* Pointer to the spike system instance
     * @note Used for advanced spike event scheduling and delivery control.
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Accesses the spike system (const version)
     * @return const SpikeSystem* Const pointer to the spike system instance
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Accesses the spike-timing-dependent plasticity system
     * @return STDP* Pointer to the STDP system instance
     */
    STDP* getSTDP();
    
    /**
     * @brief Accesses the Hebbian plasticity system
     * @return Hebbian* Pointer to the Hebbian system instance
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Accesses the structural plasticity system
     * @return StructuralPlasticity* Pointer to the structural plasticity system instance
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Calculates the excitation/inhibition ratio across the brain
     * @return float E/I ratio (total excitatory weight / total absolute inhibitory weight)
     * @see BrainStatisticsCalculator::calculateExcitationInhibitionRatio()
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Gets the total spike count since brain initialization
     * @return size_t Total number of spikes fired by all neurons
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Gets the number of neurons currently in refractory period
     * @return size_t Number of neurons unable to fire due to refractory state
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produces a motor/action output based on motor neuron activity
     * @return std::unique_ptr<Action> Selected action, or nullptr if no action selected
     * @note Calls ActionSelectionSystem to convert neural activity into discrete actions.
     *       Used for agent behavior control.
     * @see ActionSelectionSystem
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Applies neuromodulatory signals to the neural system
     * @param signal Neuromodulator containing dopamine, curiosity, novelty signals
     * @details Modulates neural excitability, plasticity, and learning based on
     *          neuromodulatory signals from cognitive and affective systems.
     * @see Neuromodulator
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Updates plasticity rules (called automatically in step)
     * @note Applies STDP, Hebbian, and reward-modulated plasticity based on recent activity.
     *       Called by SimulationStepCoordinator::executeStep7().
     */
    void updatePlasticity();
    
    /**
     * @brief Applies developmental changes to the neural system
     * @note Adjusts network properties based on developmental stage (critical period, maturation).
     *       Called periodically by SimulationStepCoordinator::executeStep14().
     */
    void develop();
    
    /**
     * @brief Resets the brain to its initial state
     * @note Clears all neuron states, synaptic weights, memory contents, and statistics.
     *       Preserves configuration and neural structure.
     */
    void reset();
    
    /**
     * @brief Saves brain state to a checkpoint file
     * @param filepath Path to checkpoint file
     * @return bool true if save successful, false on error
     * @see CheckpointManager
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Loads brain state from a checkpoint file
     * @param filepath Path to checkpoint file
     * @return bool true if load successful, false on error
     * @see CheckpointManager
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Creates a new neural region
     * @param name Optional name for the region
     * @return RegionId ID of the newly created region
     * @note Regions organize neurons into functional groups with local connectivity.
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Accesses a neural region by ID
     * @param id Region ID
     * @return NeuralRegion* Pointer to the region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Accesses a neural region by ID (const version)
     * @param id Region ID
     * @return const NeuralRegion* Const pointer to the region, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Gets the total number of regions in the brain
     * @return size_t Number of neural regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Gets all region IDs
     * @return std::vector<RegionId> Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Gets all regions (non-const version)
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Reference to internal regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Adds a connection between two regions
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Synaptic weight (-1.0 to 1.0)
     * @param delay Delay in simulation steps (default: 1)
     * @note Creates an InterRegionConnection for long-range connectivity.
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Removes a connection between two regions
     * @param source Source region ID
     * @param target Target region ID
     * @note Removes both the connection and any associated plastic changes.
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Gets the total number of neurons across all regions
     * @return size_t Total neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Gets the total number of synapses across all regions
     * @return size_t Total synapse count
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Gets the number of neurons currently active (firing or processing)
     * @return size_t Number of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Gets the number of neurons currently firing
     * @return size_t Number of neurons in firing state
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Calculates the average firing rate across all regions
     * @return float Average firing rate (Hz)
     */
    float getAverageFiringRate() const;
    
    /**
     * @name Memory System Accessors
     * @brief Accessors for integrated memory systems
     */
    ///@{ 
    
    /**
     * @brief Accesses the working memory system
     * @return NeuralWorkingMemory* Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Accesses the episodic memory system
     * @return NeuralEpisodicMemory* Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Accesses the associative memory system
     * @return NeuralAssociativeMemory* Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    ///@}
    
    /**
     * @name Prediction System Accessors
     * @brief Accessors for prediction and cognitive systems
     */
    ///@{ 
    
    /**
     * @brief Accesses the prediction system for sensory prediction and error
     * @return PredictionSystem* Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    ///@}
    
    /**
     * @name Cognition System Accessors
     * @brief Accessors for cognitive functions
     */
    ///@{ 
    
    /**
     * @brief Accesses the neural planner for action planning
     * @return NeuralPlanner* Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Accesses the concept formation system for pattern discovery
     * @return ConceptFormation* Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Accesses the attentional selection system for focus control
     * @return AttentionalSelection* Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();
    ///@}
    
    /**
     * @name Development System Accessors
     * @brief Accessors for developmental processes
     */
    ///@{ 
    
    /**
     * @brief Accesses the development system for maturational changes
     * @return DevelopmentSystem* Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Gets the current developmental stage
     * @return DevelopmentalStage Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Sets the developmental stage
     * @param stage Developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    ///@}
    
    /**
     * @name Neuromodulation System Accessors
     * @brief Accessors for neuromodulatory signals
     */
    ///@{ 
    
    /**
     * @brief Accesses the dopamine system for reward and reinforcement
     * @return Dopamine* Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Accesses the curiosity system for exploration motivation
     * @return Curiosity* Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Accesses the novelty detection system
     * @return Novelty* Pointer to novelty system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Accesses the prediction error signal system
     * @return PredictionError* Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    ///@}
    
    /**
     * @brief Accesses the current configuration
     * @return std::shared_ptr<const Config> Const shared pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Accesses the random number generator
     * @return RandomGenerator* Pointer to random number generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Logs brain status and statistics for debugging
     * @note Outputs information about neuron counts, spike statistics, memory states,
     *       and system health to the logging system.
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Helper classes for refactored functionality
    friend class SimulationStepCoordinator;
    friend class BrainStatisticsCalculator;
    friend class CheckpointManager;
    friend class SensoryInputProcessor;
    friend class ActionSelectionSystem;
    friend class NeuronStepProcessor;
};

} // namespace nlm
