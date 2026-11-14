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
    /**
     * @brief Source region of the connection
     */
    RegionId sourceRegion;
    
    /**
     * @brief Target region of the connection
     */
    RegionId targetRegion;
    
    /**
     * @brief Connection weight
     */
    float weight;
    
    /**
     * @brief Transmission delay in simulation steps
     */
    Delay delay;
    
    /**
     * @brief Plasticity flags for this connection
     */
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Default constructor
     * 
     * Creates an invalid connection with default values.
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Constructor with parameters
     * 
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Connection weight
     * @param d Transmission delay
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief Central neural simulation brain class
 * 
 * This class implements the core neural simulation system, providing:
 * - Real spiking neural computation with event-driven dynamics
 * - Integrated memory systems (working, episodic, associative)
 * - Prediction and cognition systems
 * - Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
 * - Developmental system for lifelong learning
 * - Structured plasticity (Hebbian, STDP, structural)
 * - Sensory input and motor output interfaces
 * 
 * The Brain class is the main entry point for neural simulation, coordinating
 * all subsystems and maintaining the neural network's state.
 * 
 * @note Uses the pimpl idiom for hiding implementation details
 * @see SimulationStep for discrete time steps
 * @see Timestamp for continuous time
 * @see Config for configuration management
 * 
 * @warning This is a complex, research-oriented implementation designed for
 * scientific investigation of artificial developmental systems and neural computation.
 */
class Brain {
public:
/**
     * @brief Constructor
     * 
     * Creates a Brain instance with the specified configuration.
     * 
     * @param config Shared pointer to configuration object
     * 
     * @note The configuration is stored and used for initialization and simulation parameters.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     * 
     * Cleans up Brain resources and performs shutdown procedures.
     */
    ~Brain();
    
    /**
     * @brief Disable copying
     * 
     * Brain objects cannot be copied to prevent inconsistent states.
     * Use move semantics instead.
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disable copy assignment
     * 
     * Brain objects cannot be assigned to prevent inconsistent states.
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * Enables efficient transfer of ownership between Brain objects.
     * 
     * @param other Source Brain object to move from
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Enables efficient transfer of ownership.
     * 
     * @param other Source Brain object to move from
     * @return Reference to this Brain object
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * Sets up the brain's internal structures, creates neural regions,
     * initializes neurons and synapses, and configures all subsystems.
     * 
     * @return true if initialization succeeded, false otherwise
     * 
     * @note This must be called before any simulation steps or operations.
     * @see step() for the main simulation loop
     * @see Config for configuration parameters
     */
    bool initialize();
    
    /**
     * @brief Perform main simulation step (discrete time version)
     * 
     * Advances the neural simulation by one time step using discrete simulation steps.
     * This is the primary simulation method for most use cases.
     * 
     * @param currentStep Current simulation step number (increments each call)
     * 
     * @note Uses the brain's configured timestep duration and processes:
     * - Spike propagation and synaptic transmission
     * - Neuron dynamics and plasticity updates
     * - Memory system updates
     * - Neuromodulation
     * - Developmental changes
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Perform main simulation step (continuous time version)
     * 
     * Advances the neural simulation by one time step with explicit timestamp.
     * Use this method when you need continuous time simulation or when the time
     * step is not constant.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Absolute simulation time in seconds
     * 
     * @note More flexible than the discrete version but requires more careful
     * time management in the calling code.
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Injects current into sensory neurons based on the provided input pattern.
     * This simulates the effect of external stimuli on the neural system.
     * 
     * @param input Sensory input data containing perception information
     * 
     * @note Typical usage: world.getSensoryPercept() -> brain.receiveSensoryInput()
     * @see SensoryInput for input types
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Applies external current to a single neuron, simulating direct stimulation.
     * Used for targeted stimulation experiments or testing specific neurons.
     * 
     * @param neuron Neuron ID to inject current into
     * @param current Current value in nanoamperes (or simulation units)
     * 
     * @note Positive current typically excites neurons, negative inhibits them.
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Applies current to all neurons matching the specified type (e.g., all sensory neurons).
     * Useful for testing network responses to broad stimulation patterns.
     * 
     * @param type Neuron type to stimulate
     * @param current Current value in nanoamperes (or simulation units)
     * 
     * @see NeuronType for available neuron types
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get pointer to spike system
     * 
     * @return Pointer to the SpikeSystem instance, or nullptr if not initialized
     * 
     * @note The spike system handles spike events and synaptic transmission
     * with precise timing and delay management.
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get constant pointer to spike system
     * 
     * @return Constant pointer to the SpikeSystem instance, or nullptr if not initialized
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get pointer to STDP plasticity system
     * 
     * @return Pointer to the STDP instance, or nullptr if not initialized
     */
    STDP* getSTDP();
    
    /**
     * @brief Get pointer to Hebbian plasticity system
     * 
     * @return Pointer to the Hebbian instance, or nullptr if not initialized
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get pointer to structural plasticity system
     * 
     * @return Pointer to the StructuralPlasticity instance, or nullptr if not initialized
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation/inhibition ratio
     * 
     * @return Ratio of excitatory to inhibitory neural activity (0.0 to ∞)
     * Higher values indicate more excitatory activity
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * 
     * @return Total number of spikes generated since initialization
     * @see step() for spike counting details
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * @return Number of spike events waiting to be delivered
     * 
     * @note Spike events are scheduled with delays and count here until delivered.
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * Analyzes motor neuron activity and produces an appropriate Action object
     * for the environment. This is the interface between the neural system
     * and the external world.
     * 
     * @return Unique pointer to Action object, or nullptr if no action produced
     * 
     * @note Uses motor neuron populations and action decoding systems.
     * @see Action for available action types
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     * 
     * Applies neuromodulatory signals that affect neural plasticity, learning,
     * and behavior. Signals are processed by neuromodulation systems (dopamine,
     * curiosity, novelty, prediction error).
     * 
     * @param signal Neuromodulator signal to apply
     * 
     * @see Neuromodulator for signal types
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * Applies plasticity rules to update synaptic weights based on neural activity.
     * This method is called automatically during simulation steps unless
     * disabled. Can also be called manually for specific experiments.
     * 
     * @note Updates STDP, Hebbian, and structural plasticity based on neural activity
     * patterns and neuromodulatory signals.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * Applies developmental processes to the neural system, including maturation,
     * synaptic pruning, and connectivity refinement. This method is called
     * automatically during simulation steps unless disabled.
     */
    void develop();
    
    /**
     * @brief Reset brain state
     * 
     * Resets all neural components to their initial state as defined by configuration.
     * This is useful for running multiple independent trials or experiments.
     * 
     * @note Preserves configuration but resets neural activity, memory, and development.
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Saves the complete neural state including:
     * - Neural connectivity (synapses, neurons)
     * - Memory contents
     * - Development state
     * - Learned preferences
     * - Other persistent state
     * 
     * @param filepath Path where the brain state will be saved
     * @return true if save succeeded, false otherwise
     * 
     * @note Used for checkpointing, experiment reproducibility, and state persistence.
     * @see load() for loading saved state
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * Restores the neural state from a previously saved checkpoint.
     * 
     * @param filepath Path to the saved brain state file
     * @return true if load succeeded, false otherwise
     * 
     * @note Restores neural connectivity, memory, development, and preferences.
     * Configuration remains unchanged (use setConfig() to update if needed).
     * @see save() for saving state
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region
     * 
     * Creates a new neural region with the specified name. Regions organize
     * neurons hierarchically and can have different properties and functions.
     * 
     * @param name Optional name for the region
     * @return ID of the newly created region
     * 
     * @note Region IDs are unique and can be used to access specific regions.
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get neural region by ID
     * 
     * @param id Region ID to retrieve
     * @return Pointer to NeuralRegion, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get neural region by ID (constant version)
     * 
     * @param id Region ID to retrieve
     * @return Constant pointer to NeuralRegion, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions
     * 
     * @return Current number of neural regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * 
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * 
     * @return Constant reference to vector of all neural regions
     * 
     * @note Returns regions in no guaranteed order.
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection
     * 
     * Creates a long-range connection between two neural regions.
     * These connections enable communication between different regions.
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight
     * @param delay Transmission delay in simulation steps
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count
     * 
     * @return Total number of neurons across all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count
     * 
     * @return Total number of synapses across all regions
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count
     * 
     * @return Number of neurons currently firing (Active state)
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * 
     * @return Number of neurons currently in FiringState::Active
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * 
     * @return Average firing rate across all neurons (Hz)
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get working memory pointer
     * 
     * @return Pointer to NeuralWorkingMemory, or nullptr if not initialized
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory pointer
     * 
     * @return Pointer to NeuralEpisodicMemory, or nullptr if not initialized
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory pointer
     * 
     * @return Pointer to NeuralAssociativeMemory, or nullptr if not initialized
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Get prediction system pointer
     * 
     * @return Pointer to PredictionSystem, or nullptr if not initialized
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Get neural planner pointer
     * 
     * @return Pointer to NeuralPlanner, or nullptr if not initialized
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation pointer
     * 
     * @return Pointer to ConceptFormation, or nullptr if not initialized
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection pointer
     * 
     * @return Pointer to AttentionalSelection, or nullptr if not initialized
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Get development system pointer
     * 
     * @return Pointer to DevelopmentSystem, or nullptr if not initialized
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * 
     * @return Current developmental stage of the brain
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * @param stage Developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @brief Get dopamine neuromodulation pointer
     * 
     * @return Pointer to Dopamine, or nullptr if not initialized
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity neuromodulation pointer
     * 
     * @return Pointer to Curiosity, or nullptr if not initialized
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty neuromodulation pointer
     * 
     * @return Pointer to Novelty, or nullptr if not initialized
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal pointer
     * 
     * @return Pointer to PredictionError, or nullptr if not initialized
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get current configuration
     * 
     * @return Shared pointer to configuration object
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator pointer
     * 
     * @return Pointer to RandomGenerator, or nullptr if not initialized
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Get learned preference trait
     * 
     * @return Learned preference trait value (-1.0 to 1.0)
     * 
     * @note This is a Phase 5 feature representing a single scalar preference
     * learned from experience that survives reloads and biases action choice.
     */
    float getLearnedPreference() const;
    
    /**
     * @brief Set learned preference trait
     * 
     * @param preference New preference value (-1.0 to 1.0)
     */
    void setLearnedPreference(float preference);
    
    /**
     * @brief Log brain status
     * 
     * Outputs diagnostic information about the current brain state,
     * including neuron counts, spike statistics, and subsystem status.
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
