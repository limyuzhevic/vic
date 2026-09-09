#pragma once
/**
 * @file Brain.hpp
 * @brief Central brain controller for the NLM neural simulation system
 * 
 * This file contains the Brain class, which serves as the central coordinator
 * of the neural simulation system. The Brain implements real spiking neural
 * computation with event-driven dynamics and integrates multiple specialized
 * systems for memory, prediction, cognition, and neuromodulation.
 * 
 * The Brain class provides a unified interface for managing the complete
 * neural network simulation, from low-level neuron dynamics to high-level
 * cognitive functions. It orchestrates interactions between all subsystems
 * and provides a complete simulation environment for studying neural computation.
 * 
 * @author NLM Development Team
 * @version 1.0
 * @date 2026
 * 
 * @note This class uses the Pimpl idiom for efficient memory management and
 *       hides implementation details from the interface.
 * 
 * @see Config.hpp, NeuralRegion.hpp, Neuron.hpp, Synapse.hpp
 * @see SimulationClock.hpp, Random.hpp, Logger.hpp
 * @see InteractiveVisualizer.hpp, Phase6IntegratedExperiment.hpp
 */

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include "../experiments/Exporters.hpp"

namespace nlm {

/**
 * @namespace nlm
 * @brief Neural Simulation Library namespace
 * 
 * Contains all types, classes, and functions for the Neural Simulation Library,
 * including the central Brain class and all neural system components.
 * 
 * The nlm namespace provides:
 * - Core neural data types and identifiers
 * - Configuration management
 * - Logging and debugging utilities
 * - Memory systems (working, episodic, associative)
 * - Prediction and cognitive systems
 * - Neuromodulation systems
 * - Development and plasticity systems
 * 
 * @see Config.hpp, Types.hpp, Logger.hpp
 */

/**
 * @brief Forward declaration of interdependent classes
 * 
 * These classes are forward-declared here to avoid circular dependencies
 * when Brain.hpp includes other brain components that may need to reference
 * the Brain class.
 */
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
 * @struct InterRegionConnection
 * @brief Represents a long-range synaptic connection between neural regions
 * 
 * This structure defines connections that span multiple neural regions, enabling
 * inter-regional communication and coordination. These connections are crucial
 * for integrating information across different brain areas and implementing
 * complex neural computations.
 * 
 * Inter-region connections support plasticity mechanisms and can have varying
 * strengths and delays, allowing for sophisticated temporal and spatial patterns
 * of neural activity.
 * 
 * @note Used by the Brain class for global network connectivity and information
 *       integration across regions.
 */
struct InterRegionConnection {
    /** Identifier of the source region */
    RegionId sourceRegion;
    
    /** Identifier of the target region */
    RegionId targetRegion;
    
    /** Synaptic weight (strength of connection) */
    float weight;
    
    /** Transmission delay in simulation steps */
    Delay delay;
    
    /** Plasticity mechanism flags for this connection */
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Default constructor creates an inactive connection
     * 
     * Creates an InterRegionConnection with default values (zero weights and delays)
     * and all plasticity flags disabled.
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Construct an inter-region connection with specified parameters
     * 
     * @param src Source region identifier
     * @param tgt Target region identifier  
     * @param w Connection weight (default: 0.0f)
     * @param d Transmission delay in steps (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief Central coordinator of the neural simulation system
 * 
 * The Brain class implements a comprehensive spiking neural network simulation
 * with event-driven dynamics and integrated cognitive systems. It serves as the
 * main entry point for the NLM library and provides a complete interface for
 * managing neural computation, memory, prediction, and action selection.
 * 
 * The Brain class orchestrates interactions between multiple specialized subsystems:
 * - Neural dynamics (spike generation and propagation)
 * - Memory systems (working, episodic, associative)
 * - Plasticity mechanisms (STDP, Hebbian, structural)
 * - Cognitive systems (planning, concept formation, attention)
 * - Neuromodulation (dopamine, curiosity, novelty)
 * - Development and adaptation
 * 
 * The implementation uses the Pimpl idiom for efficient memory management and
 * thread-safe operations. The Brain is not copyable but is movable.
 * 
 * @note This class is the primary interface for the NLM neural simulation library.
 *       All high-level operations should be performed through this class.
 * 
 * @see Config.hpp Configuration management
 * @see NeuralRegion.hpp Regional organization
 * @see InteractiveVisualizer.hpp Visualization interface
 */
class Brain {
public:
    /**
     * @brief Constructs a Brain instance with configuration
     * 
     * Creates a new Brain instance configured with the provided configuration.
     * The Brain is initialized in an uninitialized state and must be explicitly
     * initialized using the initialize() method before simulation can begin.
     * 
     * @param config Shared pointer to configuration object
     * @throw std::invalid_argument if config is nullptr
     * @see initialize()
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     * 
     * Cleans up all resources and performs any necessary cleanup operations.
     * This includes saving any unsaved state and releasing allocated memory.
     */
    ~Brain();
    
    /**
     * @brief Copy constructor (deleted)
     * 
     * Brain objects cannot be copied due to internal pImpl structure.
     * @throw std::logic_error if attempted
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     * 
     * Brain objects cannot be assigned due to internal pImpl structure.
     * @throw std::logic_error if attempted
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor ( noexcept)
     * 
     * Transfers ownership of the internal implementation from another Brain object.
     * @param other Brain to move from
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator ( noexcept)
     * 
     * Transfers ownership of the internal implementation from another Brain object.
     * @param other Brain to move from
     * @return Reference to this object
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initializes the brain with configuration
     * 
     * Performs the initialization sequence required to set up the brain for simulation.
     * This includes creating all neural regions, initializing subsystems, setting up
     * connections, and allocating memory structures.
     * 
     * @return true if initialization completed successfully, false on error
     * @throw std::runtime_error if initialization fails
     * @see Config::get()
     */
    bool initialize();
    
    /**
     * @brief Main simulation step (step count only)
     * 
     * Advances the neural simulation by one discrete time step. This performs all
     * neural dynamics computations, including spike generation, synaptic transmission,
     * plasticity updates, and cognitive processing.
     * 
     * @param currentStep Current simulation step number
     * @see step(SimulationStep, Timestamp)
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step (with time information)
     * 
     * Advances the neural simulation by one discrete time step with precise timing.
     * This performs all neural dynamics computations using the current simulation time.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     * @see step(SimulationStep)
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Injects current into sensory neurons based on the input pattern, simulating
     * external stimulus processing. This method processes sensory inputs that drive
     * the neural network's response to environmental changes.
     * 
     * @param input Sensory input pattern from environment
     * @see SensoryInput class
     * @see injectCurrent()
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Applies current to a specific neuron's membrane, directly influencing its
     * membrane potential and potentially triggering spike generation. Used for
     * targeted neural stimulation.
     * 
     * @param neuron Target neuron identifier
     * @param current Current to inject (can be positive or negative)
     * @see injectCurrentToNeurons()
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Applies current to all neurons of the specified type throughout the network,
     * useful for global modulation or targeted population control.
     * 
     * @param type Neuron type to target
     * @param current Current to inject
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get spike system reference
     * 
     * Returns a pointer to the SpikeSystem object, which manages neural spike generation
     * and propagation. The spike system is responsible for the core neural dynamics.
     * 
     * @return SpikeSystem* Pointer to the spike system, or nullptr if not initialized
     * @see SpikeSystem.hpp
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get spike system reference (const version)
     * 
     * Returns a const pointer to the SpikeSystem for read-only access.
     * 
     * @return const SpikeSystem* Const pointer to the spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get STDP plasticity system reference
     * 
     * Returns a pointer to the Spike-Timing-Dependent Plasticity (STDP) system,
     * which implements spike-timing-dependent synaptic modification rules.
     * 
     * @return STDP* Pointer to the STDP system
     * @see STDP.hpp
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity system reference
     * 
     * Returns a pointer to the Hebbian plasticity system, which implements
     * activity-dependent synaptic strengthening rules.
     * 
     * @return Hebbian* Pointer to the Hebbian system
     * @see Hebbian.hpp
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity system reference
     * 
     * Returns a pointer to the structural plasticity system, which manages
     * changes in neural connectivity and structure.
     * 
     * @return StructuralPlasticity* Pointer to the structural plasticity system
     * @see StructuralPlasticity.hpp
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation-inhibition ratio
     * 
     * Calculates the balance between excitatory and inhibitory neural activity
     * in the network, which is crucial for stable network operation.
     * 
     * @return float E/I ratio (excitatory/inhibitory), or 1.0 if no activity
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * 
     * Returns the cumulative number of spikes generated by all neurons in the network.
     * This is a global statistic useful for measuring network activity levels.
     * 
     * @return size_t Total number of spikes generated
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * Returns the number of spike events that are scheduled but not yet delivered,
     * which reflects the transmission load and future activity in the network.
     * 
     * @return size_t Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * Analyzes motor neuron activity to generate an appropriate action or behavior
     * based on the current neural state. The produced action reflects the network's
     * decision or response to the current situation.
     * 
     * @return std::unique_ptr<class Action> Generated action, or nullptr if no action
     * @see Action class
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     * 
     * Applies neuromodulatory signals (dopamine, serotonin, etc.) that affect
     * neural excitability, plasticity, and learning. Neuromodulation is crucial
     * for adaptive behavior and reinforcement learning.
     * 
     * @param signal Neuromodulator signal to apply
     * @see Neuromodulator class
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules
     * 
     * Updates synaptic plasticity based on recent neural activity. This method
     * is called automatically during each simulation step and implements
     * learning rules like STDP and Hebbian plasticity.
     * 
     * @note This method is called automatically in step().
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes
     * 
     * Applies developmental changes to the neural system based on the current
     * developmental stage. This includes pruning, myelination, and other
     * maturational processes.
     * 
     * @note This method is called automatically in step().
     */
    void develop();
    
    /**
     * @brief Reset brain state to initial conditions
     * 
     * Resets all neural state variables to their initial values, effectively
     * returning the brain to its starting configuration. This is useful for
     * running controlled experiments or restarting simulations.
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Saves the current brain state to a file for checkpointing or later loading.
     * This allows resuming simulations from a saved state, which is useful for
     * long-running simulations or reproducibility.
     * 
     * @param filepath Path where brain state should be saved
     * @return true if save completed successfully, false on error
     * @see load()
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * Restores a previously saved brain state from a file. This allows resuming
     * simulations from checkpoint files or loading pretrained networks.
     * 
     * @param filepath Path to file containing saved brain state
     * @return true if load completed successfully, false on error
     * @see save()
     */
    bool load(const std::string& filepath);

    /**
     * @brief Rollback brain initialization on failure
     * 
     * This internal method handles rollback when brain initialization fails.
     * It cleans up all resources that were successfully initialized before
     * the failure occurred, ensuring the brain is left in a consistent
     * uninitialized state.
     * 
     * @param initializedComponents List of successfully initialized components
     */
    void rollbackInitialization(const std::vector<std::string>& initializedComponents);

    /**
     * @name Region Management
     * @brief Methods for managing neural regions
     */
    
    /**
     * @brief Add a new neural region
     * 
     * Creates and adds a new neural region to the brain. Regions are used for
     * organizing the neural network into functionally or anatomically distinct
     * areas with specialized properties.
     * 
     * @param name Optional name for the region (default: "")
     * @return RegionId Identifier of the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get neural region by ID
     * 
     * Returns a pointer to the neural region with the specified ID.
     * 
     * @param id Region identifier
     * @return NeuralRegion* Pointer to region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get neural region by ID (const version)
     * 
     * Returns a const pointer to the neural region with the specified ID.
     * 
     * @param id Region identifier
     * @return const NeuralRegion* Const pointer to region, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions in the brain
     * 
     * @return size_t Number of neural regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * 
     * @return std::vector<RegionId> Vector of all region identifiers
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * 
     * Returns a reference to the vector containing all neural regions in the brain.
     * This provides read-only access to the complete regional organization.
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Reference to regions vector
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @name Inter-Region Connection Management
     * @brief Methods for managing long-range connections
     */
    
    /**
     * @brief Add an inter-region connection
     * 
     * Creates a long-range connection between two neural regions. Such connections
     * enable information flow and integration across different brain areas.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Connection strength (default: 0.0f)
     * @param delay Transmission delay in steps (default: 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove an inter-region connection
     * 
     * Removes a previously established connection between two regions.
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @name Global Statistics
     * @brief Methods for network-wide statistics
     */
    
    /**
     * @brief Get total neuron count
     * 
     * Returns the total number of neurons in the entire network.
     * 
     * @return size_t Total number of neurons
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count
     * 
     * Returns the total number of synaptic connections in the network.
     * 
     * @return size_t Total number of synapses
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count
     * 
     * Returns the number of neurons currently active (firing or in active states).
     * 
     * @return size_t Number of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * 
     * Returns the number of neurons currently firing action potentials.
     * 
     * @return size_t Number of firing neurons
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * 
     * Calculates the average firing rate across all neurons in the network.
     * 
     * @return float Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    /**
     * @name Memory Systems
     * @brief Access to memory subsystem components
     */
    
    /**
     * @brief Get working memory reference
     * 
     * Returns a pointer to the working memory system, which stores transient
     * active information used in current processing tasks.
     * 
     * @return NeuralWorkingMemory* Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory reference
     * 
     * Returns a pointer to the episodic memory system, which stores experienced
     * events and episodes for later retrieval.
     * 
     * @return NeuralEpisodicMemory* Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory reference
     * 
     * Returns a pointer to the associative memory system, which stores pattern
     * associations and relationships between neural representations.
     * 
     * @return NeuralAssociativeMemory* Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @name Prediction System
     * @brief Access to prediction and cognition systems
     */
    
    /**
     * @brief Get prediction system reference
     * 
     * Returns a pointer to the prediction system, which generates predictions
     * about sensory inputs and computes prediction errors for learning.
     * 
     * @return PredictionSystem* Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @name Cognition Systems
     * @brief Access to higher-level cognitive systems
     */
    
    /**
     * @brief Get neural planner reference
     * 
     * Returns a pointer to the neural planner, which generates action plans
     * based on current state and goals.
     * 
     * @return NeuralPlanner* Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation reference
     * 
     * Returns a pointer to the concept formation system, which discovers and
     * abstracts patterns to form higher-level concepts.
     * 
     * @return ConceptFormation* Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection reference
     * 
     * Returns a pointer to the attentional selection system, which selects
     * and prioritizes information for processing.
     * 
     * @return AttentionalSelection* Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();
    
    /**
     * @name Development System
     * @brief Access to development and maturation systems
     */
    
    /**
     * @brief Get development system reference
     * 
     * Returns a pointer to the development system, which manages neural
     * development and maturation processes.
     * 
     * @return DevelopmentSystem* Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * Returns the current developmental stage of the brain, which affects
     * plasticity, learning capabilities, and behavioral patterns.
     * 
     * @return DevelopmentalStage Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * Changes the developmental stage of the brain, affecting plasticity
     * rules and behavioral characteristics.
     * 
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @name Neuromodulation Systems
     * @brief Access to neuromodulation and learning systems
     */
    
    /**
     * @brief Get dopamine system reference
     * 
     * Returns a pointer to the dopamine system, which implements reward-based
     * reinforcement learning and motivation.
     * 
     * @return Dopamine* Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity system reference
     * 
     * Returns a pointer to the curiosity system, which drives exploration
     * and intrinsic motivation.
     * 
     * @return Curiosity* Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty detection reference
     * 
     * Returns a pointer to the novelty detection system, which identifies
     * unexpected events and patterns.
     * 
     * @return Novelty* Pointer to novelty detection system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal reference
     * 
     * Returns a pointer to the prediction error signal system, which computes
     * and provides error signals for learning.
     * 
     * @return PredictionError* Pointer to prediction error signal system
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get current configuration
     * 
     * Returns a shared pointer to the configuration object used by this brain.
     * The configuration contains parameters that control brain behavior and simulation.
     * 
     * @return std::shared_ptr<const Config> Shared pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;

    /**
     * @brief Get random generator reference
     * 
     * Returns a pointer to the random number generator used by the brain for
     * stochastic processes and probabilistic computations.
     * 
     * @return RandomGenerator* Pointer to random generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @name Data Export
     * @brief Methods for exporting neural data in various formats
     */
    
    /**
     * @brief Export brain data as CSV
     * 
     * Exports neural data in CSV format for analysis. Supports selective export
     * of different data types including neurons, synapses, spike history, membrane
     * potentials, and weights.
     * 
     * @param filepath Output file path
     * @param options Export options controlling what to export
     * @return true if export completed successfully, false on error
     * @see ExportOptions, CSVExporter
     */
    bool exportCSV(const std::string& filepath, const ExportOptions& options = ExportOptions());
    
    /**
     * @brief Export brain data as JSON
     * 
     * Exports neural data in JSON format with metadata for machine learning
     * integration and interoperability with other tools.
     * 
     * @param filepath Output file path
     * @param options Export options controlling what to export
     * @return true if export completed successfully, false on error
     * @see ExportOptions, JSONExporter
     */
    bool exportJSON(const std::string& filepath, const ExportOptions& options = ExportOptions());
    
    /**
     * @brief Export brain data as binary
     * 
     * Exports neural data in binary format for large datasets and efficient storage.
     * Supports compression for reduced storage requirements.
     * 
     * @param filepath Output file path
     * @param options Export options controlling what to export
     * @return true if export completed successfully, false on error
     * @see ExportOptions, BinaryExporter
     */
    bool exportBinary(const std::string& filepath, const ExportOptions& options = ExportOptions());
    
    /**
     * @brief Export time series data
     * 
     * Exports time series data for analysis, including membrane potentials,
     * firing rates, and synaptic weights over time.
     * 
     * @param filepath Output file path
     * @param startStep Starting simulation step
     * @param endStep Ending simulation step
     * @param options Export options
     * @return true if export completed successfully, false on error
     */
    bool exportTimeSeries(const std::string& filepath, SimulationStep startStep, 
                         SimulationStep endStep, const ExportOptions& options = ExportOptions());
    
    /**
     * @brief Export snapshot data
     * 
     * Exports a snapshot of the current brain state at a specific time.
     * 
     * @param filepath Output file path
     * @param step Simulation step to export
     * @param options Export options
     * @return true if export completed successfully, false on error
     */
    bool exportSnapshot(const std::string& filepath, SimulationStep step, 
                       const ExportOptions& options = ExportOptions());
    
    /**
     * @brief Export batch of time series data
     * 
     * Exports multiple time series snapshots for batch analysis.
     * 
     * @param filepath Output file path
     * @param steps Vector of simulation steps to export
     * @param options Export options
     * @return true if export completed successfully, false on error
     */
    bool exportBatch(const std::string& filepath, const std::vector<SimulationStep>& steps,
                    const ExportOptions& options = ExportOptions());
    
    /**
     * @brief Log brain status information
     * 
     * Logs comprehensive status information about the brain, including network
     * statistics, configuration parameters, and subsystem states. Useful for
     * debugging and monitoring.
     * 
     * @note This method uses the Logger class for output.
     */
    void logStatus() const;
    
private:
    /**
     * @brief Internal implementation structure
     * 
     * The Brain class uses the Pimpl idiom for efficient memory management and
     * to hide implementation details. All actual implementation is stored in
     * this private structure.
     */
    struct Impl;
    
    /**
     * @brief Pointer to internal implementation
     * 
     * Stores a pointer to the Brain's internal implementation structure.
     * The pointer is never null after successful construction.
     */
    Impl* pImpl;
};

} // namespace nlm

/**
 * @defgroup brain_group Brain System
 * @brief Group of Brain-related classes and functions
 * 
 * This group contains the Brain class and related components for the neural
 * simulation system. These components form the core of the NLM simulation
 * environment.
 * 
 * @see Brain.hpp, NeuralRegion.hpp, Neuron.hpp
 * @see InteractiveVisualizer.hpp, Phase6IntegratedExperiment.hpp
 */

/**
 * @defgroup types_group Core Types
 * @brief Group of fundamental type definitions
 * 
 * This group contains the core type definitions used throughout the NLM library,
 * including strongly-typed identifiers, enumerations, and fundamental data types.
 * 
 * @see Types.hpp, ConfigValue, ConfigSource
 */

/**
 * @defgroup logging_group Logging System
 * @brief Group of logging-related classes and utilities
 * 
 * This group contains the logging infrastructure for the NLM library, including
 * the Logger class, log levels, and convenience macros for logging.
 * 
 * @see Logger.hpp, ILogger, NLM_LOG_*, LogStream
 */
