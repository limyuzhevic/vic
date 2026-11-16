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

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    void develop();
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
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
    
    // Debugging and monitoring
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize the brain with the given configuration
     * 
     * This method sets up the complete neural substrate including neurons,
     * synapses, memory systems, and all integrated modules. It performs
     * extensive validation of configuration parameters and throws exceptions
     * for invalid configurations.
     * 
     * @return true if initialization succeeded, false otherwise
     * @throws std::invalid_argument if configuration is invalid
     */
    bool initialize();
    
    /**
     * @brief Execute one simulation step
     * 
     * This is the main integration point for the complete brain simulation.
     * It implements the full integrated brain loop including:
     * - Spike processing and neural dynamics
     * - Memory updates and consolidation
     * - Neuromodulation and plasticity
     * - Prediction and cognition
     * - Action selection and behavior
     * 
     * @param currentStep The current simulation step
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Execute one simulation step with explicit timing
     * 
     * Alternative form of step() that accepts both step and time parameters.
     * This allows for more precise control over the simulation timeline.
     * 
     * @param currentStep The current simulation step
     * @param currentTime The current simulation time in seconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from the environment
     * 
     * This method injects current into sensory neurons based on the provided
     * sensory input. The current is normalized and distributed across the
     * available sensory neurons to encode the environmental state.
     * 
     * @param input The sensory input containing environmental observations
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * This method allows for direct current injection into a specific neuron
     * by its ID. If the neuron is found in the neuronById lookup table,
     * the current is injected immediately. Otherwise, a linear scan is
     * performed across all neurons in all regions.
     * 
     * @param neuron The ID of the neuron to receive current
     * @param current The amount of current to inject (in mV)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * This method injects the same current into all neurons of the specified
     * type (Sensory, Internal, or Motor). This is useful for applying
     * global modulatory signals or testing neural responses.
     * 
     * @param type The type of neurons to receive current
     * @param current The amount of current to inject (in mV)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get access to the spike system
     * 
     * @return Pointer to the SpikeSystem instance
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get const access to the spike system
     * 
     * @return Const pointer to the SpikeSystem instance
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get access to the STDP plasticity system
     * 
     * @return Pointer to the STDP instance
     */
    STDP* getSTDP();
    
    /**
     * @brief Get access to the Hebbian plasticity system
     * 
     * @return Pointer to the Hebbian instance
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get access to the structural plasticity system
     * 
     * @return Pointer to the StructuralPlasticity instance
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Calculate the excitation/inhibition ratio
     * 
     * This method computes the total excitatory weight divided by the
     * total inhibitory weight across all synapses in the network. This
     * provides a measure of the network's E/I balance.
     * 
     * @return The E/I ratio, or infinity if there's only excitation
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get the total number of spikes generated by the brain
     * 
     * @return Total number of spikes since brain initialization
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get the number of pending spike events
     * 
     * This includes both immediate and delayed spike events that are
     * waiting to be processed.
     * 
     * @return Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce an action based on motor neuron activity
     * 
     * This method analyzes the activity of motor neuron populations and
     * selects an appropriate action. The selection is based on the motor
     * group with the highest average firing rate.
     * 
     * @return A unique pointer to an Action instance
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals to the brain
     * 
     * This method applies neuromodulatory signals (like dopamine) to
     * influence neural excitability and plasticity. Neuromodulators
     * can affect multiple aspects of brain function including attention,
     * learning, and behavior.
     * 
     * @param signal The neuromodulator signal to apply
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * This method applies plasticity rules including STDP and Hebbian
     * learning. It is called automatically during each simulation step.
     * This method is kept for API compatibility with older versions.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * This method applies developmental changes to the brain's structure
     * and function. Developmental stages affect plasticity rates,
     * neural excitability, and other developmental processes.
     * This method is called automatically during each simulation step.
     */
    void develop();
    
    /**
     * @brief Reset the brain to its initial state
     * 
     * This method resets the brain's state including neurons, synapses,
     * and all internal systems. It also clears memory systems and
     * resets the developmental stage.
     */
    void reset();
    
    /**
     * @brief Save the current brain state to a file (checkpointing)
     * 
     * This method saves the complete brain state including neuron states,
     * synapse weights, spike events, and all integrated systems to a
     * checkpoint file. This allows for saving and reloading brain state.
     * 
     * @param filepath The path to the checkpoint file
     * @return true if the save succeeded, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load the brain state from a file (checkpointing)
     * 
     * This method loads a previously saved brain state from a checkpoint
     * file. It restores neuron states, synapse weights, spike events,
     * and all integrated systems.
     * 
     * @param filepath The path to the checkpoint file
     * @return true if the load succeeded, false otherwise
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region to the brain
     * 
     * This method creates a new neural region with the specified name.
     * Regions organize neurons into functional groups and provide
     * a hierarchical structure for the neural network.
     * 
     * @param name Optional name for the region (defaults to empty string)
     * @return The ID of the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a pointer to a neural region by its ID
     * 
     * @param id The ID of the region to retrieve
     * @return Pointer to the NeuralRegion, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a const pointer to a neural region by its ID
     * 
     * @param id The ID of the region to retrieve
     * @return Const pointer to the NeuralRegion, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get the number of regions in the brain
     * 
     * @return Number of regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs in the brain
     * 
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions in the brain
     * 
     * @return Const reference to the vector of neural regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add an inter-regional connection
     * 
     * This method adds a connection between two regions, allowing for
     * long-range communication between different parts of the brain.
     * 
     * @param source The source region ID
     * @param target The target region ID
     * @param weight The initial weight of the connection
     * @param delay The transmission delay in simulation steps
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove an inter-regional connection
     * 
     * @param source The source region ID
     * @param target The target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get the total number of neurons in the brain
     * 
     * @return Total number of neurons
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the total number of synapses in the brain
     * 
     * @return Total number of synapses
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get the number of active neurons in the brain
     * 
     * @return Number of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the number of firing neurons in the brain
     * 
     * This returns the number of neurons that fired during the current
     * simulation step.
     * 
     * @return Number of firing neurons
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the average firing rate of the brain
     * 
     * This returns the average firing rate across all neurons in the
     * brain (in Hz).
     * 
     * @return Average firing rate
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Get access to the working memory system
     * 
     * Working memory is a short-term memory system that maintains
     * active neural representations and supports working memory functions
     * like attention and planning.
     * 
     * @return Pointer to the NeuralWorkingMemory instance
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get access to the episodic memory system
     * 
     * Episodic memory stores experience as episodes, allowing the brain
     * to recall past events and learn from experience.
     * 
     * @return Pointer to the NeuralEpisodicMemory instance
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get access to the associative memory system
     * 
     * Associative memory stores associations between patterns of neural
     * activity and their consequences, supporting pattern completion
     * and expectation generation.
     * 
     * @return Pointer to the NeuralAssociativeMemory instance
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get access to the prediction system
     * 
     * The prediction system generates predictions about sensory input
     * and computes prediction errors that drive learning.
     * 
     * @return Pointer to the PredictionSystem instance
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get access to the neural planner
     * 
     * The neural planner evaluates sequences of actions and selects
     * the most promising ones based on learned action values.
     * 
     * @return Pointer to the NeuralPlanner instance
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get access to the concept formation system
     * 
     * Concept formation extracts abstract patterns from experience
     * and forms concepts that can be used for generalization and
     * reasoning.
     * 
     * @return Pointer to the ConceptFormation instance
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get access to the attentional selection system
     * 
     * Attentional selection focuses attention on relevant information
     * and filters out irrelevant inputs, allowing for efficient processing
     * of the environment.
     * 
     * @return Pointer to the AttentionalSelection instance
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get access to the development system
     * 
     * The development system manages developmental stages and processes
     * that affect brain development and plasticity.
     * 
     * @return Pointer to the DevelopmentSystem instance
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get the current developmental stage
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set the developmental stage
     * 
     * @param stage The developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get access to the dopamine neuromodulator
     * 
     * Dopamine is involved in reward prediction error, reinforcement learning,
     * and action selection. It affects both neural excitability and plasticity.
     * 
     * @return Pointer to the Dopamine instance
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get access to the curiosity neuromodulator
     * 
     * Curiosity drives exploration behavior and encourages the agent to
     * seek novel and informative experiences.
     * 
     * @return Pointer to the Curiosity instance
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get access to the novelty neuromodulator
     * 
     * Novelty detects novel or unexpected sensory input and signals
     * the importance of new information for learning.
     * 
     * @return Pointer to the Novelty instance
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get access to the prediction error signal
     * 
     * Prediction error signals the difference between expected and actual
     * outcomes, driving learning and adaptation.
     * 
     * @return Pointer to the PredictionError instance
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get access to the current configuration
     * 
     * @return Const pointer to the configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get access to the random number generator
     * 
     * @return Pointer to the RandomGenerator instance
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Get the learned preference trait
     * 
     * This trait represents the agent's learned preferences and biases,
     * which influence action selection and behavior. It survives checkpoint
     * reloads and changes based on experienced rewards.
     * 
     * @return The learned preference trait (-1 to 1)
     */
    float getLearnedPreference() const;
    
    /**
     * @brief Set the learned preference trait
     * 
     * @param preference The learned preference trait (-1 to 1)
     */
    void setLearnedPreference(float preference);
    
    // Logging
    /**
     * @brief Log the current status of the brain
     * 
     * This method logs important information about the current state of
     * the brain including region counts, neuron counts, synapse counts,
     * and other statistics.
     */
    void logStatus() const;
    
    // Debugging and monitoring
    /**
     * @brief Enable or disable debug logging
     * 
     * When enabled, debug logging provides detailed information about
     * the brain's operation including step-by-step processing, memory
     * updates, and system states.
     * 
     * @param enable true to enable debug logging, false to disable
     */
    void enableDebugging(bool enable);
    
    /**
     * @brief Enable or disable performance monitoring
     * 
     * When enabled, performance monitoring tracks key performance metrics
     * including firing rates, step processing times, and resource usage.
     * 
     * @param enable true to enable performance monitoring, false to disable
     */
    void enablePerformanceMonitoring(bool enable);
    
    /**
     * @brief Set the maximum number of steps to record for debugging
     * 
     * This limits the amount of historical data that is kept for debugging
     * purposes, helping to manage memory usage.
     * 
     * @param maxSteps Maximum number of steps to record
     */
    void setDebugStepLimit(size_t maxSteps);
    
    /**
     * @brief Check if debug logging is enabled
     * 
     * @return true if debug logging is enabled, false otherwise
     */
    bool isDebuggingEnabled() const;
    
    /**
     * @brief Get the current performance report
     * 
     * This method generates a report of current performance metrics
     * including firing rates, step counts, and other statistics.
     * 
     * @return Performance report as a string
     */
    std::string getPerformanceReport() const;
    
    /**
     * @brief Log the current debug state
     * 
     * This method logs detailed information about the current state of
     * the brain when debug logging is enabled.
     */
    void logDebugState() const;
    
    /**
     * @brief Log a summary of the current debug state
     * 
     * This method logs a summary of the current debug state when debug
     * logging is enabled.
     */
    void logDebugSummary() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
