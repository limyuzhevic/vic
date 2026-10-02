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

class Brain {
public:
    /**
     * @brief Create a new NLM Brain with the specified configuration
     * 
     * Initializes the brain with all integrated systems (memory, prediction, 
     * cognition, neuromodulation, development) based on the provided configuration.
     * 
     * @param config Shared pointer to configuration object containing brain parameters
     *              such as neuron count, region count, connection probability, etc.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the Brain object and clean up all resources
     */
    ~Brain();
    
    /**
     * @brief Copy constructor - explicitly deleted to prevent copying
     * 
     * The Brain class uses PIMPL pattern and contains complex internal state
     * that should not be copied. Use move semantics instead.
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Copy assignment operator - explicitly deleted to prevent copying
     * 
     * The Brain class uses PIMPL pattern and contains complex internal state
     * that should not be copied. Use move semantics instead.
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor - transfer ownership of internal implementation
     * 
     * Safely moves the internal implementation from another brain object,
     * leaving the source brain in a valid but empty state.
     * 
     * @param other Brain to move from
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfer ownership of internal implementation
     * 
     * Safely moves the internal implementation from another brain object,
     * cleaning up the current implementation first.
     * 
     * @param other Brain to move from
     * @return Reference to this brain after the move
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize the brain with neurons, regions, and connectivity
     * 
     * Creates the neural architecture: regions, populations, and neurons.
     * Initializes random connectivity patterns and sets up all subsystems
     * (memory, prediction, cognition, neuromodulation, development).
     * 
     * @return true if initialization succeeds, false on error
     */
    bool initialize();
    
    /**
     * @brief Perform one simulation step with the specified timestep
     * 
     * Executes the complete integrated brain loop for one timestep:
     * 1. Process delayed spike events
     * 2. Update neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing events
     * 4. Update working memory
     * 5. Apply neuromodulation effects
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity
     * 12. Replay important memories
     * 13. Apply development effects
     * 14. Collect statistics
     * 
     * @param currentStep Current simulation step number
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Perform one simulation step with explicit timestamp
     * 
     * Convenience overload of step() that computes timestep from step
     * and current time.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in seconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Inject sensory input into the brain's sensory neurons
     * 
     * Processes sensory data from the environment and injects appropriate
     * currents into sensory neurons, encoding the sensory information
     * into neural activity patterns.
     * 
     * @param input Sensory input containing sensor data (vision, audio, etc.)
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Overrides or augments synaptic input to a specific neuron with
     * additional current, useful for testing or targeted stimulation.
     * 
     * @param neuron ID of the neuron to inject current into
     * @param current Current to inject in microamps
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Applies current to all neurons with the specified type (e.g., all
     * sensory neurons, all motor neurons, etc.).
     * 
     * @param type Neuron type to inject current into
     * @param current Current to inject in microamps
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Generate motor action based on current neural activity
     * 
     * Performs action selection based on the current state of motor neurons
     * and active neural patterns, producing a motor command for the agent.
     * 
     * @return Unique pointer to generated action
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Reset the brain to its initial state
     * 
     * Clears all neural activity, resets neurons to resting state,
     * clears memory systems, and prepares the brain for a new episode.
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * Persists the complete brain state including neuron states,
     * synaptic weights, memory contents, and developmental state
     * to a checkpoint file for later restoration.
     * 
     * @param filepath Path to the checkpoint file
     * @return true if save succeeds, false on error
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * Restores brain state from a checkpoint file, replacing the current
     * state with saved neuron states, synaptic weights, and memory contents.
     * 
     * @param filepath Path to the checkpoint file
     * @return true if load succeeds, false on error
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region to the brain
     * 
     * Creates a new brain region with the specified name. Regions represent
     * spatially segregated neural groups with their own populations of neurons.
     * 
     * @param name Optional name for the region (defaults to empty string)
     * @return Region ID of the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a region by ID
     * 
     * Retrieves the neural region with the specified ID.
     * 
     * @param id Region ID to look up
     * @return Pointer to the region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a region by ID (const version)
     * 
     * Retrieves the neural region with the specified ID (const overload).
     * 
     * @param id Region ID to look up
     * @return Const pointer to the region, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get the number of regions in the brain
     * 
     * @return Total number of neural regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * 
     * @return Vector of all region IDs in the brain
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * 
     * @return Reference to vector of all regions in the brain
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add an inter-region connection (long-range connectivity)
     * 
     * Creates a connection from one brain region to another, allowing
     * long-range communication between regions.
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight (positive for excitation)
     * @param delay Delay in simulation steps
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove an inter-region connection
     * 
     * Removes the connection between source and target regions.
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total number of neurons in the brain
     * 
     * @return Total neuron count across all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total number of synapses in the brain
     * 
     * @return Total synapse count across all regions
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of active neurons
     * 
     * @return Number of neurons currently firing or above threshold
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of firing neurons in current step
     * 
     * @return Number of neurons that fired during the current simulation step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * 
     * @return Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get spike system
     * 
     * @return Pointer to spike system
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get spike system (const version)
     * 
     * @return Const pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get STDP plasticity system
     * 
     * @return Pointer to STDP system
     */
    STDP* getSTDP();
    
    /**
     * @brief Get STDP plasticity system (const version)
     * 
     * @return Const pointer to STDP system
     */
    const STDP* getSTDP() const;
    
    /**
     * @brief Get Hebbian plasticity system
     * 
     * @return Pointer to Hebbian system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get Hebbian plasticity system (const version)
     * 
     * @return Const pointer to Hebbian system
     */
    const Hebbian* getHebbian() const;
    
    /**
     * @brief Get structural plasticity system
     * 
     * @return Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get structural plasticity system (const version)
     * 
     * @return Const pointer to structural plasticity system
     */
    const StructuralPlasticity* getStructuralPlasticity() const;
    
    /**
     * @brief Get excitation/inhibition ratio
     * 
     * @return Ratio of total excitation to total inhibition
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * 
     * @return Total number of spikes generated by this brain
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * @return Number of pending spike events (immediate + delayed)
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Get neuromodulation system
     * 
     * @return Pointer to neuromodulation system
     */
    Neuromodulator* getNeuromodulator();
    
    /**
     * @brief Get neuromodulation system (const version)
     * 
     * @return Const pointer to neuromodulation system
     */
    const Neuromodulator* getNeuromodulator() const;
    
    /**
     * @brief Apply neuromodulatory signal
     * 
     * @param signal Neuromodulatory signal (dopamine, curiosity, etc.)
     */
    void applyNeuromodulation(const Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules
     * 
     * This method is called automatically during simulation steps.
     * Updates all plasticity systems based on recent neural activity.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes
     * 
     * This method is called automatically during simulation steps.
     * Updates developmental parameters and affects plasticity rates.
     */
    void develop();
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Get working memory system
     * 
     * @return Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get working memory system (const version)
     * 
     * @return Const pointer to working memory system
     */
    const NeuralWorkingMemory* getWorkingMemory() const;
    
    /**
     * @brief Get episodic memory system
     * 
     * @return Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get episodic memory system (const version)
     * 
     * @return Const pointer to episodic memory system
     */
    const NeuralEpisodicMemory* getEpisodicMemory() const;
    
    /**
     * @brief Get associative memory system
     * 
     * @return Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Get associative memory system (const version)
     * 
     * @return Const pointer to associative memory system
     */
    const NeuralAssociativeMemory* getAssociativeMemory() const;
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get prediction system
     * 
     * @return Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Get prediction system (const version)
     * 
     * @return Const pointer to prediction system
     */
    const PredictionSystem* getPredictionSystem() const;
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get neural planner system
     * 
     * @return Pointer to neural planner system
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get neural planner system (const version)
     * 
     * @return Const pointer to neural planner system
     */
    const NeuralPlanner* getPlanner() const;
    
    /**
     * @brief Get concept formation system
     * 
     * @return Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get concept formation system (const version)
     * 
     * @return Const pointer to concept formation system
     */
    const ConceptFormation* getConceptFormation() const;
    
    /**
     * @brief Get attentional selection system
     * 
     * @return Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Get attentional selection system (const version)
     * 
     * @return Const pointer to attentional selection system
     */
    const AttentionalSelection* getAttention() const;
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get development system
     * 
     * @return Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get development system (const version)
     * 
     * @return Const pointer to development system
     */
    const DevelopmentSystem* getDevelopmentSystem() const;
    
    /**
     * @brief Get current developmental stage
     * 
     * @return Current developmental stage (Initial, CriticalPeriod, etc.)
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * Manually advances or regresses the brain's developmental stage,
     * affecting plasticity rates and other developmental parameters.
     * 
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get dopamine system
     * 
     * @return Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get dopamine system (const version)
     * 
     * @return Const pointer to dopamine system
     */
    const Dopamine* getDopamine() const;
    
    /**
     * @brief Get curiosity system
     * 
     * @return Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get curiosity system (const version)
     * 
     * @return Const pointer to curiosity system
     */
    const Curiosity* getCuriosity() const;
    
    /**
     * @brief Get novelty system
     * 
     * @return Pointer to novelty system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get novelty system (const version)
     * 
     * @return Const pointer to novelty system
     */
    const Novelty* getNovelty() const;
    
    /**
     * @brief Get prediction error system
     * 
     * @return Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get prediction error system (const version)
     * 
     * @return Const pointer to prediction error system
     */
    const PredictionError* getPredictionErrorSignal() const;
    
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
