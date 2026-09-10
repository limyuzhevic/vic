#pragma once

/**
 * @file Brain.hpp
 * @brief Neural brain implementation - central coordinator of the neural system
 * @author NLM Brain Team
 * @date 2026
 * @version 1.0
 * 
 * This file implements the main Brain class, which serves as the central coordinator
 * for all neural computation. The Brain class integrates real spiking neural computation
 * with event-driven dynamics and incorporates memory, prediction, cognition, and
 * neuromodulation systems. It manages the overall simulation including neurons,
 * synapses, spikes, plasticity, development, and various neural subsystems.
 */

#include "../core/Types/Types.hpp"
#include "../core/ErrorHandling/ErrorHandling.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <stdexcept>

namespace nlm {

// Forward declarations
/** @brief Neural network configuration management */
class Config;
/** @brief Deterministic random number generator for reproducible experiments */
class RandomGenerator;
/** @brief Simulation clock for deterministic time tracking */
class SimulationClock;
/** @brief Logging system for diagnostic and monitoring output */
class Logger;
/** @brief Working memory - transient active information storage */
class NeuralWorkingMemory;
/** @brief Episodic memory - storage of experience sequences */
class NeuralEpisodicMemory;
/** @brief Associative memory - pattern association storage */
class NeuralAssociativeMemory;
/** @brief Prediction system for sensory prediction and error computation */
class PredictionSystem;
/** @brief Neural planner for action planning and decision making */
class NeuralPlanner;
/** @brief Concept formation for pattern discovery and abstraction */
class ConceptFormation;
/** @brief Attentional selection mechanism for focus management */
class AttentionalSelection;
/** @brief Neural development system for growth and maturation */
class DevelopmentSystem;
/** @brief Dopamine neuromodulator for reward and reinforcement signaling */
class Dopamine;
/** @brief Curiosity neuromodulator for exploration motivation */
class Curiosity;
/** @brief Novelty detection neuromodulator */
class Novelty;
/** @brief Prediction error signal for learning and adaptation */
class PredictionError;

// Inter-regional connection (long-range connectivity)
/**
 * @struct InterRegionConnection
 * @brief Represents a connection between different neural regions
 * 
 * Contains information about source and target regions, synaptic weight,
 * transmission delay, and plasticity flags for long-range connections.
 * 
 * @param sourceRegion ID of the source region
 * @param targetRegion ID of the target region
 * @param weight Synaptic weight of the connection (0.0 = no connection)
 * @param delay Transmission delay in simulation steps
 * @param plasticityFlags Flags determining which plasticity rules apply
 */
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Default constructor - creates an invalid connection
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Constructor with all parameters
     * 
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Synaptic weight
     * @param d Transmission delay
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

// Validation helper functions

/**
 * @brief Check if a region ID is valid (not the invalid sentinel value)
 * 
 * @param id The region ID to check
 * @return true if the region ID is valid, false otherwise
 */
inline bool isValidRegionId(RegionId id) {
    return id != INVALID_REGION_ID;
}

/**
 * @brief Check if a neuron ID is valid (not the invalid sentinel value)
 * 
 * @param id The neuron ID to check
 * @return true if the neuron ID is valid, false otherwise
 */
inline bool isValidNeuronId(NeuronId id) {
    return id != INVALID_NEURON_ID;
}

/**
 * @brief Check if a synapse ID is valid (not the invalid sentinel value)
 * 
 * @param id The synapse ID to check
 * @return true if the synapse ID is valid, false otherwise
 */
inline bool isValidSynapseId(SynapseId id) {
    return id != INVALID_SYNAPSE_ID;
}

/**
 * @brief Check if a population ID is valid (not the invalid sentinel value)
 * 
 * @param id The population ID to check
 * @return true if the population ID is valid, false otherwise
 */
inline bool isValidPopulationId(PopulationId id) {
    return id != INVALID_POPULATION_ID;
}

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

/**
 * @class Brain
 * @brief Central coordinator of the neural system with integrated computation capabilities
 * 
 * The Brain class implements the core neural simulation engine, managing all aspects
 * of brain computation including neuron dynamics, synaptic transmission, spike processing,
 * plasticity learning, developmental processes, and high-level cognitive functions.
 * 
 * This class uses the Pimpl idiom for implementation hiding, providing a clean interface
 * while maintaining efficient memory usage and allowing implementation changes without
 * affecting the public API.
 * 
 * The brain is organized into several key subsystems:
 * - Neural dynamics (neurons, synapses, spikes)
 * - Plasticity (learning rules including STDP, Hebbian, and reward-modulated learning)
 * - Memory (working, episodic, and associative memory systems)
 * - Prediction (forward models and prediction error computation)
 * - Cognition (planning, attention, concept formation)
 * - Development (structural changes and maturation)
 * - Neuromodulation (dopamine, curiosity, novelty signals)
 * 
 * Key features:
 * - Event-driven spike processing for computational efficiency
 * - Configurable timestep for simulation control
 * - Comprehensive error handling and validation
 * - Checkpoint/restore functionality
 * - Real-time execution support
 * 
 * @note This is a core component of the NLM (Neural Learning Machine) framework.
 * The brain serves as the primary interface for users to control and interact with
 * the neural simulation system.
 * 
 * @warning The Brain class is not thread-safe. Concurrent access requires external
 * synchronization mechanisms.
 * 
 * @tparam Allocator Memory allocator type (not used, kept for future extensions)
 */
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
    
    // Region management with validation
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
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
