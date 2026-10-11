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
//
// This is the main artificial brain class that orchestrates all neural subsystems.
// It provides a complete integrated brain simulation with the following capabilities:
// - Real LIF (Leaky Integrate-and-Fire) neuron dynamics
// - Event-driven spike propagation with synaptic delays
// - Multiple plasticity mechanisms (STDP, Hebbian, structural)
// - Integrated memory systems (working, episodic, associative)
// - Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
// - Prediction and planning systems
// - Cognitive functions (attention, concept formation)
// - Development system with age-dependent plasticity
// - Checkpoint saving/loading and memory replay
// - Reward-based learning and exploration
//
// The brain operates as a closed-loop system where neural activity, plasticity,
// memory, neuromodulation, and behavior continuously influence each other.
// All systems are interconnected to form a coherent artificial brain.

class Brain {
public:
    /// @brief Creates a brain instance with the given configuration.
    ///
    /// @param config Shared configuration object containing brain parameters
    ///              such as neuron count, region count, plasticity settings, etc.
    explicit Brain(std::shared_ptr<Config> config);
    
    /// @brief Destructor - cleans up brain resources and connected subsystems.
    ~Brain();
    
    /// @brief Disables copying to prevent accidental duplication.
    Brain(const Brain&) = delete;
    
    /// @brief Disables assignment to prevent accidental copying.
    Brain& operator=(const Brain&) = delete;
    
    /// @brief Enables move construction for efficient resource transfer.
    Brain(Brain&&) noexcept;
    
    /// @brief Enables move assignment for efficient resource transfer.
    Brain& operator=(Brain&&) noexcept;
    
    /// @brief Initializes the brain with configuration parameters.
    ///
    /// This method creates and configures all neural subsystems including
    /// neurons, synapses, memory systems, neuromodulation, etc. based on
    /// the configuration settings.
    ///
    /// @return true if initialization succeeds, false if initialization fails
    bool initialize();
    
    /// @brief Advances the brain simulation by one time step.
    ///
    /// @param currentStep The current simulation step number (for logging)
    void step(SimulationStep currentStep);
    
    /// @brief Advances the brain simulation by one time step with explicit time.
    ///
    /// @param currentStep The current simulation step number
    /// @param currentTime The current simulation time in seconds
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /// @brief Receives sensory input from the environment.
    ///
    /// This method injects current into sensory neurons based on the input
    /// pattern, allowing the brain to perceive its environment.
    ///
    /// @param input The sensory input containing environmental data
    void receiveSensoryInput(const class SensoryInput& input);
    
    /// @brief Injects current directly into a specific neuron.
    ///
    /// This method allows direct control of individual neurons, useful for
    /// testing, demonstrations, or specific input patterns.
    ///
    /// @param neuron The ID of the neuron to receive current
    /// @param current The amount of current to inject (in nA)
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /// @brief Injects current into all neurons of a specific type.
    ///
    /// This method is useful for stimulating entire populations of neurons
    /// (e.g., all sensory neurons, all motor neurons, etc.).
    ///
    /// @param type The type of neurons to receive current
    /// @param current The amount of current to inject per neuron (in nA)
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /// @brief Returns the spike system used for event-driven spike processing.
    ///
    /// The spike system manages spike events, delayed spike propagation,
    /// and synaptic event timing.
    ///
    /// @return Pointer to the spike system, or nullptr if not initialized
    SpikeSystem* getSpikeSystem();
    
    /// @brief Returns the spike system used for event-driven spike processing (const version).
    ///
    /// @return Const pointer to the spike system, or nullptr if not initialized
    const SpikeSystem* getSpikeSystem() const;
    
    /// @brief Returns the spike-timing-dependent plasticity (STDP) system.
    ///
    /// STDP is a learning mechanism that strengthens synapses when
    /// pre-synaptic neurons fire shortly before post-synaptic neurons,
    /// and weakens synapses when the reverse order occurs.
    ///
    /// @return Pointer to the STDP system, or nullptr if not initialized
    STDP* getSTDP();
    
    /// @brief Returns the Hebbian plasticity system.
    ///
    /// Hebbian learning strengthens synapses based on simultaneous
    /// pre- and post-synaptic activity (neurons that fire together wire together).
    ///
    /// @return Pointer to the Hebbian system, or nullptr if not initialized
    Hebbian* getHebbian();
    
    /// @brief Returns the structural plasticity system.
    ///
    /// Structural plasticity handles the creation and elimination of
    /// synapses (synaptogenesis and pruning) based on activity patterns.
    ///
    /// @return Pointer to the structural plasticity system, or nullptr if not initialized
    StructuralPlasticity* getStructuralPlasticity();
    
    /// @brief Returns the excitation-to-inhibition ratio of the network.
    ///
    /// This ratio is calculated from the total synaptic weights, providing
    /// insight into the network's balance and stability.
    ///
    /// @return The excitation-inhibition ratio (excitatory weights / inhibitory weights)
    float getExcitationInhibitionRatio() const;
    
    /// @brief Returns the total number of spikes generated since initialization.
    ///
    /// This statistic is useful for monitoring brain activity and learning progress.
    ///
    /// @return Total spike count
    size_t getTotalSpikeCount() const;
    
    /// @brief Returns the number of pending spike events (spikes not yet processed).
    ///
    /// @return Number of pending spike events
    size_t getPendingSpikeEventCount() const;
    
    /// @brief Produces motor action based on motor neuron activity.
    ///
    /// This method analyzes motor neuron activity and generates appropriate
    /// motor commands for the agent's actions in the environment.
    ///
    /// @return Unique pointer to the generated action
    std::unique_ptr<class Action> produceAction();
    
    /// @brief Applies neuromodulatory signals to the brain.
    ///
    /// Neuromodulators like dopamine, acetylcholine, etc. affect neural
    /// excitability, plasticity, and learning rate.
    ///
    /// @param signal The neuromodulatory signal to apply
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /// @brief Updates plasticity rules (called automatically in step).
    ///
    /// This method applies plasticity rules to synapses based on recent
    /// neural activity. It's called automatically during each simulation step.
    void updatePlasticity();
    
    /// @brief Applies developmental changes (called automatically in step).
    ///
    /// This method applies age-dependent developmental changes to the brain,
    /// including changes in plasticity rates, structural changes, etc.
    /// It's called automatically during each simulation step.
    void develop();
    
    /// @brief Resets the brain state to initial conditions.
    ///
    /// This method resets all neural state variables, memory contents,
    /// and developmental stage to their initial values.
    void reset();
    
    /// @brief Saves the current brain state to a file (checkpointing).
    ///
    /// This method serializes the brain's neural state, synapse weights,
    /// memory contents, and other state variables to a file for later restoration.
    ///
    /// @param filepath The path to the checkpoint file to create
    /// @return true if save succeeds, false if save fails
    bool save(const std::string& filepath) const;
    
    /// @brief Loads a previously saved brain state from a file.
    ///
    /// This method deserializes brain state from a checkpoint file and
    /// restores the brain to that previous state.
    ///
    /// @param filepath The path to the checkpoint file to load
    /// @return true if load succeeds, false if load fails
    bool load(const std::string& filepath);
    
    /// @brief Adds a new neural region to the brain.
    ///
    /// Regions are groups of neurons that can have different properties
    /// and connectivity patterns. They allow for modular brain organization.
    ///
    /// @param name Optional name for the region (for identification)
    /// @return The ID of the newly created region
    RegionId addRegion(const std::string& name = "");
    
    /// @brief Returns a pointer to the specified neural region.
    ///
    /// @param id The ID of the region to retrieve
    /// @return Pointer to the region, or nullptr if region not found
    NeuralRegion* getRegion(RegionId id);
    
    /// @brief Returns a const pointer to the specified neural region.
    ///
    /// @param id The ID of the region to retrieve
    /// @return Const pointer to the region, or nullptr if region not found
    const NeuralRegion* getRegion(RegionId id) const;
    
    /// @brief Returns the number of regions in the brain.
    ///
    /// @return Number of regions
    size_t getRegionCount() const;
    
    /// @brief Returns the IDs of all regions in the brain.
    ///
    /// @return Vector of region IDs
    std::vector<RegionId> getRegionIds() const;
    
    /// @brief Returns a reference to all regions in the brain.
    ///
    /// @return Const reference to vector of unique pointers to neural regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /// @brief Adds a long-range connection between two regions.
    ///
    /// Inter-region connections allow communication between different neural regions,
    /// enabling distributed processing and coordination.
    ///
    /// @param source The source region ID
    /// @param target The target region ID
    /// @param weight The connection weight (default: 0.0)
    /// @param delay The transmission delay in time steps (default: 1)
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /// @brief Removes a long-range connection between two regions.
    ///
    /// @param source The source region ID
    /// @param target The target region ID
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /// @brief Returns the total number of neurons in the brain.
    ///
    /// @return Total neuron count
    size_t getTotalNeuronCount() const;
    
    /// @brief Returns the total number of synapses in the brain.
    ///
    /// @return Total synapse count
    size_t getTotalSynapseCount() const;
    
    /// @brief Returns the number of currently active neurons.
    ///
    /// Active neurons are those that are processing signals or have
    /// just fired (in refractory period).
    ///
    /// @return Number of active neurons
    size_t getActiveNeuronCount() const;
    
    /// @brief Returns the number of neurons firing in the current step.
    ///
    /// @return Number of firing neurons
    size_t getFiringNeuronCount() const;
    
    /// @brief Returns the average firing rate across all neurons.
    ///
    /// @return Average firing rate in Hz
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /// @brief Returns the working memory system.
    ///
    /// Working memory stores transient active information, maintaining
    /// recent neural activity patterns for short-term processing.
    ///
    /// @return Pointer to the working memory system, or nullptr if not initialized
    NeuralWorkingMemory* getWorkingMemory();
    
    /// @brief Returns the episodic memory system.
    ///
    /// Episodic memory stores experiences as sequences of events with
    /// temporal context, allowing the brain to recall past experiences.
    ///
    /// @return Pointer to the episodic memory system, or nullptr if not initialized
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /// @brief Returns the associative memory system.
    ///
    /// Associative memory stores pattern associations and relationships
    /// between different neural representations.
    ///
    /// @return Pointer to the associative memory system, or nullptr if not initialized
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /// @brief Returns the prediction system.
    ///
    /// The prediction system generates predictions about sensory input
    /// and computes prediction errors to drive learning.
    ///
    /// @return Pointer to the prediction system, or nullptr if not initialized
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /// @brief Returns the neural planner for action planning.
    ///
    /// The neural planner evaluates potential action sequences and selects
    /// optimal actions based on predicted outcomes.
    ///
    /// @return Pointer to the neural planner, or nullptr if not initialized
    NeuralPlanner* getPlanner();
    
    /// @brief Returns the concept formation system.
    ///
    /// Concept formation discovers abstract patterns and relationships
    /// from experience, forming higher-level cognitive representations.
    ///
    /// @return Pointer to the concept formation system, or nullptr if not initialized
    ConceptFormation* getConceptFormation();
    
    /// @brief Returns the attentional selection system.
    ///
    /// Attentional selection focuses processing resources on the most
    /// relevant information, implementing competitive selection dynamics.
    ///
    /// @return Pointer to the attentional selection system, or nullptr if not initialized
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /// @brief Returns the development system.
    ///
    /// The development system manages age-dependent changes in brain
    /// properties and plasticity rates throughout the brain's lifetime.
    ///
    /// @return Pointer to the development system, or nullptr if not initialized
    DevelopmentSystem* getDevelopmentSystem();
    
    /// @brief Returns the current developmental stage.
    ///
    /// The developmental stage determines the brain's current phase of
    /// maturation, affecting plasticity rates and behavior.
    ///
    /// @return Current developmental stage
    DevelopmentalStage getDevelopmentalStage() const;
    
    /// @brief Sets the current developmental stage.
    ///
    /// This method changes the brain's developmental phase, which can
    /// affect plasticity rates, learning speed, and behavioral patterns.
    ///
    /// @param stage The developmental stage to set
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /// @brief Returns the dopamine system.
    ///
    /// Dopamine encodes reward prediction errors and modulates learning,
    /// motivation, and motor function.
    ///
    /// @return Pointer to the dopamine system, or nullptr if not initialized
    Dopamine* getDopamine();
    
    /// @brief Returns the curiosity system.
    ///
    /// Curiosity drives exploration of novel or unpredictable situations,
    /// promoting learning through intrinsic motivation.
    ///
    /// @return Pointer to the curiosity system, or nullptr if not initialized
    Curiosity* getCuriosity();
    
    /// @brief Returns the novelty system.
    ///
    /// Novelty detection identifies new or unexpected sensory input,
    /// triggering attention and learning processes.
    ///
    /// @return Pointer to the novelty system, or nullptr if not initialized
    Novelty* getNovelty();
    
    /// @brief Returns the prediction error system.
    ///
    /// Prediction error signals the difference between expected and actual
    /// outcomes, driving learning and adaptation.
    ///
    /// @return Pointer to the prediction error system, or nullptr if not initialized
    PredictionError* getPredictionErrorSignal();
    
    /// @brief Returns the brain's configuration object.
    ///
    /// @return Const shared pointer to the configuration object
    std::shared_ptr<const Config> getConfig() const;
    
    /// @brief Returns the brain's random number generator.
    ///
    /// The random generator is used for stochastic processes in the brain,
    /// including synaptic weight initialization and some plasticity rules.
    ///
    /// @return Pointer to the random generator, or nullptr if not initialized
    RandomGenerator* getRandomGenerator();
    
    /// @brief Logs detailed status information about the brain.
    ///
    /// This method outputs comprehensive statistics about the brain's current
    /// state, including neuron counts, spike statistics, memory contents,
    /// and subsystem status. Useful for debugging and monitoring.
    void logStatus() const;
    
private:
    /// @brief Private implementation details for the PImpl pattern.
    ///
    /// This separates the Brain class interface from its implementation
    /// to reduce compilation dependencies and improve encapsulation.
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
