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
// This class represents the complete artificial brain architecture for NLM Phase 6.
// It integrates all subsystems including:
// - Real spiking neural computation (LIF neurons, event-driven spike propagation)
// - Memory systems (working, episodic, associative memory)
// - Prediction systems (temporal and action-consequence prediction)
// - Cognition systems (neural planning, concept formation, attention)
// - Neuromodulation systems (dopamine, curiosity, novelty)
// - Development system (maturation, plasticity modulation)
// - Structural plasticity (synaptogenesis, pruning)
// - Replay and consolidation mechanisms
// 
// The brain operates as a closed-loop system with:
// 1. Sensory input from environment
// 2. Neural processing (spiking dynamics, memory updates)
// 3. Action selection (motor output)
// 4. Reward prediction error and neuromodulation
// 5. Plasticity updates (STDP, Hebbian, structural)
// 6. Development effects (plasticity modulation)
// 7. Memory replay and consolidation
class Brain {
public:
    // Create brain with configuration
    /**
     * @brief Construct a new Brain object with configuration
     * 
     * @param config Shared pointer to configuration object containing brain parameters
     * 
     * @details The configuration typically includes:
     * - Number of neurons and regions
     * - Connection probabilities
     * - Plasticity rule parameters
     * - Simulation timestep
     * - Random seed for reproducibility
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the Brain object
     * 
     * @details Destructor cleans up all brain resources and subsystems
     */
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * @param other Another Brain object to move from
     * 
     * @details Transfers ownership of the internal implementation (pImpl) from other to this object.
     * After the move, the source object will have null pImpl pointer.
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * @param other Another Brain object to move from
     * 
     * @return Reference to this Brain object
     * 
     * @details Transfers ownership of the internal implementation (pImpl) from other to this object.
     * After the move, the source object will have null pImpl pointer.
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize the brain with configuration
     * 
     * @return true if initialization succeeded, false otherwise
     * 
     * @details This method creates the neural architecture:
     * - Creates brain regions with neurons
     * - Sets up initial connectivity
     * - Initializes all integrated subsystems
     * - Configures plasticity systems
     * - Sets up random number generator
     * 
     * @throw std::runtime_error if initialization fails
     */
    bool initialize();
    
    /**
     * @brief Perform a simulation step using default timestamp calculation
     * 
     * @param currentStep Current simulation step number (integer)
     * 
     * @details This performs one time step of the simulation:
     * - Processes delayed spike events
     * - Updates neuron membrane potentials
     * - Detects and queues spikes
     * - Updates working memory
     * - Applies neuromodulation
     * - Updates plasticity rules
     * - Updates episodic memory
     * - Applies structural plasticity
     * - Triggers memory replay
     * - Applies developmental effects
     * - Collects statistics
     * 
     * @note This is the main loop of the integrated brain simulation
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Perform a simulation step with explicit timestamp
     * 
     * @param currentStep Current simulation step number (integer)
     * @param currentTime Current simulation time (seconds, double)
     * 
     * @details This performs one time step of the simulation with precise timing:
     * See details for step(SimulationStep currentStep) for full algorithm
     * 
     * @note Use this method when you need precise timing control for multiple clocks
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * @param input SensoryInput object containing sensory data (vision, touch, etc.)
     * 
     * @details Injects current into sensory neurons based on the input pattern:
     * - Maps sensory values to neural currents
     * - Updates working memory with current sensory information
     * - Triggers prediction error computation if prediction system is active
     * - Updates novelty detection
     * 
     * @note This is called by AgentBrain::processSensoryInput()
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * @param neuron Neuron ID to inject current into
     * @param current Current value to inject (microsiemens * millivolts)
     * 
     * @details Finds the neuron with the given ID across all brain regions and injects
     * the specified current. This is useful for direct neural control or testing.
     * 
     * @throw std::runtime_error if neuron with given ID is not found
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * @param type Neuron type (Sensory, Motor, Internal, etc.)
     * @param current Current value to inject (microsiemens * millivolts)
     * 
     * @details Injects the specified current into all neurons of the given type across
     * all brain regions. This is useful for global modulation effects (e.g., neuromodulation).
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get access to the spike system for advanced control
     * 
     * @return Pointer to the SpikeSystem object
     * 
     * @details The SpikeSystem manages event-driven spike propagation and timing.
     * Advanced users can access it for custom spike event handling or testing.
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get read-only access to the spike system
     * 
     * @return Const pointer to the SpikeSystem object
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get access to the STDP plasticity system
     * 
     * @return Pointer to the STDP object
     * 
     * @details Spike-Timing-Dependent Plasticity (STDP) is a learning rule that
     * modifies synaptic weights based on the relative timing of pre- and post-synaptic spikes.
     */
    STDP* getSTDP();
    
    /**
     * @brief Get access to the Hebbian plasticity system
     * 
     * @return Pointer to the Hebbian object
     * 
     * @details Hebbian learning strengthens synapses that are simultaneously active,
     * implementing the principle "neurons that fire together, wire together".
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get access to the structural plasticity system
     * 
     * @return Pointer to the StructuralPlasticity object
     * 
     * @details Structural plasticity manages the creation and elimination of synaptic
     * connections over longer timescales, implementing synaptogenesis and pruning.
     */
    StructuralPlasticity* getStructuralPlasticity();
    
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
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
