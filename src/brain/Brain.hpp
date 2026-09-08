#pragma once

// Brain.hpp - Central neural simulation brain class (Phase 6: Integrated Artificial Brain)
// This header defines the Brain class interface using the pImpl pattern for implementation
// The Brain class integrates all neural systems: memory, prediction, cognition, neuromodulation

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"
#include "../core/SimulationClock/SimulationClock.hpp"
#include "../brain/NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../sensory/SensoryInput.hpp"
#include "../motor/Action.hpp"
#include "../environment/Environment.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../neuromodulation/Neuromodulator.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/Novelty.hpp"
#include "../memory/WorkingMemory.hpp"
#include "../memory/EpisodicMemory.hpp"
#include "../memory/AssociativeMemory.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../memory/AssociativeMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/NeuralPlanner.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../performance/CheckpointSystem.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations for system components
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
class PredictionError;
class Novelty;
class CheckpointManager;

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
 * @brief The central coordinator of the neural system implementing Phase 6 integrated artificial brain
 * 
 * This class integrates all neural systems for Phase 6 operation:
 * - Memory systems (working, episodic, associative)
 * - Neuromodulation (dopamine, curiosity, novelty, prediction error)
 * - Prediction system
 * - Cognition systems (planning, attention, concept formation)
 * - Development system
 * - Structural plasticity
 * - Checkpointing and persistence
 * 
 * The Brain uses real LIF neuron dynamics with event-driven spike propagation
 * and maintains all internal state in the pImpl implementation for efficiency.
 * 
 * **Phase 6 Integration Features**:
 * - All memory systems connected to neural processing
 * - Neuromodulation affects plasticity and dynamics
 * - Prediction integrated with learning
 * - Development affects plasticity rates
 * - Checkpoint save/load working
 * - Replay and consolidation functional
 */
class Brain {
public:
    /**
     * @brief Create brain with configuration
     * @param config Shared configuration for the brain
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     */
    ~Brain();
    
    // **Disable copying, enable moving**
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * @return true if initialization succeeded, false otherwise
     * 
     * Creates neural regions, sets up populations, initializes all integrated
     * memory, prediction, cognition, and neuromodulation systems.
     */
    bool initialize();
    
    /**
     * @brief Perform a simulation step with current simulation time
     * @param currentStep The current simulation step number
     * @param currentTime The current time in seconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Alternative step method for backward compatibility
     * @param currentStep The current simulation step number
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Receive sensory input from environment
     * @param input Sensory input to process
     * 
     * Injects current into sensory neurons based on input pattern.
     * This is the brain's interface to the external world.
     */
    void receiveSensoryInput(const SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * @param neuron ID of neuron to inject current into
     * @param current Current to inject (in mV)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * @param type Neuron type to inject into
     * @param current Current to inject (in mV)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // **Spike system access**
    
    /**
     * @brief Get spike system for spike event handling
     * @return Pointer to spike system
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get spike system (const version)
     * @return Const pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    // **Plasticity system access**
    
    /**
     * @brief Get STDP plasticity system
     * @return Pointer to STDP system
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity system
     * @return Pointer to Hebbian system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity system
     * @return Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // **Statistics**
    
    /**
     * @brief Get excitation/inhibition balance ratio
     * @return E/I ratio (excitation divided by inhibition)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count across all neurons
     * @return Total number of spikes generated
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get count of pending spike events
     * @return Number of spike events waiting to be processed
     */
    size_t getPendingSpikeEventCount() const;
    
    // **Motor action production**
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * @return Unique pointer to action object (nullptr if no action selected)
     */
    std::unique_ptr<Action> produceAction();
    
    // **Neuromodulation**
    
    /**
     * @brief Apply neuromodulatory signals
     * @param signal Neuromodulator signal to apply
     */
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // **Plasticity**
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * This method is kept for API compatibility. Plasticity is now applied
     * during each step in the main step function.
     */
    void updatePlasticity();
    
    // **Development**
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * Updates development system and modulates plasticity rates based on
     * current developmental stage.
     */
    void develop();
    
    // **State management**
    
    /**
     * @brief Reset brain state to initial conditions
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * @param filepath Path to checkpoint file
     * @return true if save succeeded, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * @param filepath Path to checkpoint file
     * @return true if load succeeded, false otherwise
     */
    bool load(const std::string& filepath);
    
    // **Region management**
    
    /**
     * @brief Add a new neural region
     * @param name Optional name for the region
     * @return ID of the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get region by ID
     * @param id Region ID to lookup
     * @return Pointer to region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get region by ID (const version)
     * @param id Region ID to lookup
     * @return Const pointer to region, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions
     * @return Number of neural regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * @return Const reference to vector of region pointers
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection (long-range connectivity)
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight
     * @param delay Connection delay in steps
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                 float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // **Global statistics**
    
    /**
     * @brief Get total neuron count across all regions
     * @return Total number of neurons
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count across all regions
     * @return Total number of synapses
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count (neurons in active state)
     * @return Number of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count (neurons that fired this step)
     * @return Number of firing neurons this step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * @return Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    // **Memory systems**
    
    /**
     * @brief Get working memory system
     * @return Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory system
     * @return Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory system
     * @return Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // **Prediction system**
    
    /**
     * @brief Get prediction system
     * @return Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    // **Cognition systems**
    
    /**
     * @brief Get neural planner
     * @return Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation system
     * @return Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection system
     * @return Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();
    
    // **Development system**
    
    /**
     * @brief Get development system
     * @return Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // **Neuromodulation systems**
    
    /**
     * @brief Get dopamine system
     * @return Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity system
     * @return Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty detection system
     * @return Pointer to novelty system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal
     * @return Pointer to prediction error signal
     */
    PredictionError* getPredictionErrorSignal();
    
    // **System accessors**
    
    /**
     * @brief Get current configuration
     * @return Shared pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator
     * @return Pointer to random number generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log brain status to logger
     */
    void logStatus() const;
    
private:
    // Private pImpl implementation
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm