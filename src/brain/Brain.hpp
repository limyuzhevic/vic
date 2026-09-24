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
 * @struct InterRegionConnection
 * @brief Represents long-range connectivity between neural regions
 * @details This structure defines connections that span multiple neural regions,
 * allowing for distributed processing and integration across different brain areas.
 */
struct InterRegionConnection {
    RegionId sourceRegion;      ///< Source neural region ID
    RegionId targetRegion;      ///< Target neural region ID
    float weight;               ///< Connection strength (typically 0.0-1.0)
    Delay delay;                ///< Transmission delay in timesteps
    PlasticityFlags plasticityFlags; ///< Flags controlling plasticity for this connection
    
    /**
     * @brief Default constructor
     * @details Creates a connection with zero weight, default delay, and empty plasticity flags
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Parameterized constructor
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Connection weight (default: 0.0f)
     * @param d Transmission delay (default: 1)
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief The central coordinator of the neural system
 * @details Implements real spiking neural computation with event-driven dynamics
 * and integrated memory, prediction, cognition, and neuromodulation systems.
 * This class serves as the main interface for the NLM neural simulation framework,
 * providing methods for brain initialization, simulation stepping, sensory input
 * processing, and motor output generation.
 * 
 * The Brain class follows the PImpl pattern for implementation hiding and provides
 * a clean interface to the complex underlying neural architecture.
 */
class Brain {
public:
    /**
     * @brief Constructs a new Brain instance
     * @param config Shared pointer to configuration object containing brain parameters
     * 
     * @pre config must be a valid, non-null pointer to a Config object
     * @post Brain is initialized but not yet activated (call initialize() to start simulation)
     * 
     * @throws std::runtime_error if configuration is invalid
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroys the Brain instance
     * @details Properly cleans up all neural components and memory management structures
     */
    ~Brain();
    
    /**
     * @brief Copy constructor (deleted)
     * @details Brain objects are not copyable to maintain proper memory management
     * and neural state consistency
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Copy assignment operator (deleted)
     * @details Brain objects are not copyable to maintain proper memory management
     * and neural state consistency
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor (defaulted)
     * @details Transfers ownership of the internal implementation from another Brain
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator (defaulted)
     * @details Transfers ownership of the internal implementation from another Brain
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initializes the brain with configuration parameters
     * @details Sets up all neural components including neurons, synapses, memory systems,
     * and integrated modules based on the provided configuration. This method must
     * be called before beginning simulation.
     * 
     * @return bool true if initialization was successful, false otherwise
     * @post Brain is ready for simulation (call step() to advance time)
     * 
     * @throws std::runtime_error if initialization fails due to invalid configuration
     */
    bool initialize();
    
    /**
     * @brief Performs a single simulation step
     * @details Advances the neural simulation by one time step, executing all neural
     * dynamics, plasticity updates, and cognitive processes according to the current
     * state and inputs.
     * 
     * @param currentStep Current simulation step index (used for developmental timing)
     * @post Neural state is updated for the next time step
     * 
     * @note This method handles automatic integration with memory, prediction, and
     * neuromodulation systems
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Performs a single simulation step with timestamp
     * @details Advances the neural simulation by one time step with explicit timing,
     * allowing for precise temporal control and coordination with external events.
     * 
     * @param currentStep Current simulation step index
     * @param currentTime Current simulation time (in milliseconds)
     * @post Neural state is updated for the next time step
     * 
     * @note This overload provides finer temporal control than the single-step version
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receives sensory input from the environment
     * @details Processes sensory data by injecting appropriate current into sensory
     * neurons, effectively encoding environmental information in neural activity.
     * 
     * @param input Reference to sensory input data containing modality-specific features
     * @post Sensory neurons have been activated according to the input pattern
     * 
     * @note This method is typically called by AgentBrain when processing world percepts
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Injects direct current into a specific neuron
     * @details Manually controls neural activity by adding current to a particular
     * neuron, bypassing the normal synaptic input mechanisms.
     * 
     * @param neuron ID of the target neuron
     * @param current Amount of current to inject (in nanoamps)
     * @post Target neuron receives additional current, potentially affecting its firing
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Injects current into all neurons of a specific type
     * @details Batch operation for injecting current into all neurons matching
     * a given type (e.g., all excitatory neurons, all sensory neurons, etc.).
     * 
     * @param type Neural type to target
     * @param current Amount of current to inject (in nanoamps)
     * @post All neurons of specified type receive additional current
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Gets access to the spike system
     * @return Pointer to the SpikeSystem instance (may be null if system not initialized)
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Gets read-only access to the spike system
     * @return Const pointer to the SpikeSystem instance (may be null if system not initialized)
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Gets access to the Spike-Timing-Dependent Plasticity (STDP) system
     * @return Pointer to the STDP instance (may be null if system not initialized)
     */
    STDP* getSTDP();
    
    /**
     * @brief Gets access to the Hebbian plasticity system
     * @return Pointer to the Hebbian instance (may be null if system not initialized)
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Gets access to the structural plasticity system
     * @return Pointer to the StructuralPlasticity instance (may be null if system not initialized)
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Gets the excitation/inhibition balance ratio
     * @return Current E/I ratio (excitatory current / inhibitory current)
     * @post Useful for monitoring network stability and dynamics
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Gets total spike count across all neurons
     * @return Total number of spikes generated by the brain
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Gets count of pending spike events
     * @return Number of neurons that have spiked but haven't been processed yet
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Gets the average firing rate across all neurons
     * @return Average firing rate in spikes per second
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Produces motor/action output based on motor neuron activity
     * @details Analyzes the current neural activity pattern and generates appropriate
     * motor commands based on learned associations and current state.
     * 
     * @return Unique pointer to an Action object (may be null if no action selected)
     * @post Action is selected based on neural dynamics and internal state
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Applies neuromodulatory signals
     * @details Processes neuromodulator signals (dopamine, curiosity, novelty, etc.)
     * that affect neural excitability, plasticity, and motivation.
     * 
     * @param signal Neuromodulator signal containing type and strength
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Updates plasticity rules
     * @details Computes and applies synaptic weight changes based on current neural
     * activity and plasticity rules (STDP, Hebbian, etc.).
     * 
     * @post Synaptic weights are updated according to plasticity rules
     * @note This method is typically called automatically during the simulation step
     */
    void updatePlasticity();
    
    /**
     * @brief Applies developmental changes
     * @details Modifies neural properties according to the current developmental stage,
     * including changes in plasticity rates, connectivity, and neural excitability.
     * 
     * @post Neural properties are updated for the current developmental stage
     * @note This method is typically called automatically during the simulation step
     */
    void develop();
    
    /**
     * @brief Resets brain state to initial conditions
     * @details Restores all neural components to their initial state, clearing activity,
     * memory, and developmental changes.
     * 
     * @post Brain is in initial state, ready for new simulation
     */
    void reset();
    
    /**
     * @brief Saves brain state to file (checkpointing)
     * @details Serializes the entire brain state including neural activity, weights,
     * memory contents, and developmental history to a file.
     * 
     * @param filepath Path to the output file
     * @return bool true if save was successful, false otherwise
     * @post Brain state can be restored using load()
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Loads brain state from file
     * @details Restores brain state from a previously saved checkpoint file.
     * 
     * @param filepath Path to the input file
     * @return bool true if load was successful, false otherwise
     * @post Brain state is restored, ready for continuation of simulation
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Adds a new neural region
     * @details Creates a new NeuralRegion instance with the specified name and adds it
     * to the brain's region collection.
     * 
     * @param name Optional name for the region (default: empty string)
     * @return RegionId ID of the newly created region
     * @post Region is initialized and added to the brain's region collection
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Gets a neural region by ID
     * @param id ID of the region to retrieve
     * @return Pointer to the NeuralRegion (may be null if region doesn't exist)
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Gets a neural region by ID (const version)
     * @param id ID of the region to retrieve
     * @return Const pointer to the NeuralRegion (may be null if region doesn't exist)
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Gets the number of regions in the brain
     * @return Count of neural regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Gets all region IDs
     * @return Vector of all region IDs in the brain
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Gets all regions
     * @return Const reference to the vector of all neural regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Adds an inter-region connection
     * @details Creates a long-range connection between two neural regions, enabling
     * distributed processing and integration across brain areas.
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection strength (default: 0.0f)
     * @param delay Transmission delay in timesteps (default: 1)
     * @post Connection is added to the brain's inter-region connectivity
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Removes an inter-region connection
     * @details Deletes the long-range connection between two neural regions.
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Gets total neuron count across all regions
     * @return Total number of neurons in the brain
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Gets total synapse count across all regions
     * @return Total number of synapses in the brain
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Gets count of active neurons
     * @return Number of neurons currently exhibiting spike activity
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Gets count of firing neurons
     * @return Number of neurons currently in the process of firing
     */
    size_t getFiringNeuronCount() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Gets access to working memory
     * @details Working memory provides transient storage of information for
     * cognitive processing, maintaining active representations of sensory input
     * and task-relevant information.
     * 
     * @return Pointer to the NeuralWorkingMemory instance
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Gets access to episodic memory
     * @details Episodic memory stores experiences as sequential episodes, enabling
     * the recall and replay of past events for learning and decision-making.
     * 
     * @return Pointer to the NeuralEpisodicMemory instance
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Gets access to associative memory
     * @details Associative memory forms pattern associations between different
     * neural representations, supporting concept formation and knowledge integration.
     * 
     * @return Pointer to the NeuralAssociativeMemory instance
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Gets access to the prediction system
     * @details The prediction system generates predictions about sensory inputs
     * and computes prediction errors that drive learning and adaptation.
     * 
     * @return Pointer to the PredictionSystem instance
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Gets access to the neural planner
     * @details The neural planner evaluates action sequences and selects optimal
     * behaviors based on predicted outcomes and current goals.
     * 
     * @return Pointer to the NeuralPlanner instance
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Gets access to concept formation
     * @details Concept formation discovers and abstracts patterns from experience,
     * enabling generalization and transfer of learned knowledge.
     * 
     * @return Pointer to the ConceptFormation instance
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Gets access to attentional selection
     * @details Attentional selection focuses processing resources on relevant
     * information through competitive neural dynamics and inhibition.
     * 
     * @return Pointer to the AttentionalSelection instance
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Gets access to the development system
     * @details The development system controls age-dependent changes in neural
     * properties, including plasticity rates and structural connectivity.
     * 
     * @return Pointer to the DevelopmentSystem instance
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Gets the current developmental stage
     * @return Current developmental stage (e.g., CriticalPeriod, Maturation, Adult)
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Sets the developmental stage
     * @param stage Target developmental stage
     * @post Neural properties are updated for the new developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Gets access to the dopamine system
     * @details Dopamine modulates neural excitability and plasticity based on
     * reward prediction and prediction errors.
     * 
     * @return Pointer to the Dopamine instance
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Gets access to the curiosity system
     * @details Curiosity drives exploration behavior by detecting novel or
     * unexpected stimuli and motivating investigation.
     * 
     * @return Pointer to the Curiosity instance
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Gets access to the novelty detection system
     * @details Novelty detection identifies statistically unexpected patterns
     * in sensory input, triggering learning and attention.
     * 
     * @return Pointer to the Novelty instance
     */
    Novelty* getNovelty();
    
    /**
     * @brief Gets access to the prediction error signal
     * @details Prediction error computes the difference between expected and
     * actual sensory input, driving reinforcement learning.
     * 
     * @return Pointer to the PredictionError instance
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Gets the current configuration
     * @return Shared pointer to the current configuration object
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Gets access to the random generator
     * @return Pointer to the RandomGenerator instance
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Logs brain status and statistics
     * @details Logs comprehensive information about the current brain state,
     * including neuron counts, firing rates, memory contents, and system statistics.
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm