#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "MemorySystems.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;
class PredictionSystem;
class NeuralPlanner;
class ConceptFormation;
class DevelopmentSystem;
class Dopamine;
class Curiosity;
class Novelty;
class PredictionError;

/**
 * @brief Inter-regional connection (long-range connectivity)
 * 
 * Represents a connection between two brain regions with specific properties.
 * Used for modeling long-range projections and inter-regional communication.
 */
struct InterRegionConnection {
    RegionId sourceRegion;  ///< Source region of the connection
    RegionId targetRegion;  ///< Target region of the connection
    float weight;           ///< Connection strength (positive = excitatory, negative = inhibitory)
    Delay delay;            ///< Transmission delay in simulation steps
    PlasticityFlags plasticityFlags;  ///< Flags indicating plasticity mechanisms for this connection
    
    /**
     * @brief Default constructor
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Parameterized constructor
     * 
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Connection strength
     * @param d Transmission delay
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief The central coordinator of the neural system
 * 
 * Implements real spiking neural computation with event-driven dynamics
 * and integrated memory, prediction, cognition, and neuromodulation systems.
 * 
 * The Brain class manages:
 * - Neural regions and their populations
 * - Spike generation and propagation
 * - Synaptic plasticity mechanisms
 * - Memory systems (working, episodic, associative)
 * - Prediction and cognition systems
 * - Neuromodulation for learning and motivation
 * - Development and structural changes
 * 
 * This class follows the PIMPL pattern to hide implementation details
 * and provides a clean interface for neural system control and observation.
 */
class Brain {
public:
    /**
     * @brief Create brain with configuration
     * 
     * @param config Shared pointer to configuration object
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     */
    ~Brain();
    
    /**
     * @brief Disable copying
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disable assignment
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Enable move construction
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Enable move assignment
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * Sets up all neural subsystems, regions, and initializes components.
     * 
     * @return true if initialization succeeded, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Main simulation step (using current time)
     * 
     * Advances the neural simulation by one timestep using the current system time.
     * 
     * @param currentStep Current simulation step
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step (with explicit time)
     * 
     * Advances the neural simulation by one timestep using explicit current time.
     * 
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Injects current into sensory neurons based on input pattern.
     * 
     * @param input Sensory input from environment
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * @param neuron Neuron ID to inject current into
     * @param current Amount of current to inject
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type Neuron type to target
     * @param current Amount of current to inject
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Accessor methods for various subsystems
    
    /**
     * @brief Get spike system pointer
     * 
     * @return Pointer to spike system
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get constant spike system pointer
     * 
     * @return Constant pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get STDP plasticity system pointer
     * 
     * @return Pointer to STDP system
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity system pointer
     * 
     * @return Pointer to Hebbian system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity system pointer
     * 
     * @return Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get working memory pointer
     * 
     * @return Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory pointer
     * 
     * @return Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory pointer
     * 
     * @return Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Get prediction system pointer
     * 
     * @return Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Get neural planner pointer
     * 
     * @return Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation pointer
     * 
     * @return Pointer to concept formation
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection pointer
     * 
     * @return Pointer to attentional selection
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Get development system pointer
     * 
     * @return Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get dopamine neuromodulator pointer
     * 
     * @return Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity neuromodulator pointer
     * 
     * @return Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty neuromodulator pointer
     * 
     * @return Pointer to novelty system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal pointer
     * 
     * @return Pointer to prediction error signal
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get current configuration
     * 
     * @return Shared pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator pointer
     * 
     * @return Pointer to random generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log brain status and statistics
     */
    void logStatus() const;
    
    // Region management methods
    
    /**
     * @brief Add a new neural region
     * 
     * @param name Optional name for the region
     * @return New region ID
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get neural region pointer by ID
     * 
     * @param id Region ID
     * @return Pointer to neural region
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get constant neural region pointer by ID
     * 
     * @param id Region ID
     * @return Constant pointer to neural region
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions
     * 
     * @return Number of neural regions
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
     * @return Reference to vector of neural region pointers
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    
    /**
     * @brief Add inter-region connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection strength
     * @param delay Transmission delay
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
    
    // Statistics methods
    
    /**
     * @brief Get excitation/inhibition ratio
     * 
     * @return E/I ratio (positive = excitation dominant, negative = inhibition dominant)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * 
     * @return Total number of spikes generated
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * @return Number of spikes pending delivery
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Get total neuron count
     * 
     * @return Total number of neurons
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count
     * 
     * @return Total number of synapses
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count
     * 
     * @return Number of neurons currently active
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * 
     * @return Number of neurons currently firing
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * 
     * @return Average firing rate across all neurons
     */
    float getAverageFiringRate() const;
    
    // Action and neuromodulation
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * @return Unique pointer to action produced
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     * 
     * @param signal Neuromodulator signal (e.g., dopamine, reward)
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules
     * 
     * Called automatically in step() to update synaptic plasticity
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes
     * 
     * Called automatically in step() to apply developmental processes
     */
    void develop();
    
    /**
     * @brief Reset brain state
     * 
     * Clears all memory, resets neuron states, and prepares for new simulation
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * @param filepath Path to save file
     * @return true if save succeeded, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * @param filepath Path to load file
     * @return true if load succeeded, false otherwise
     */
    bool load(const std::string& filepath);
    
    // Development stage management
    
    /**
     * @brief Get current developmental stage
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
private:
    struct Impl;
    Impl* pImpl;  ///< Pointer to private implementation (PIMPL pattern)
};

} // namespace nlm
