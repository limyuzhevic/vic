#pragma once

/**
 * @file Brain.hpp
 * @brief NLM Brain - Central neural coordinator with integrated cognitive systems
 * @author NLM Project
 * @date Phase 6: FINAL INTEGRATION
 * 
 * This file defines the Brain class, which serves as the central coordinator
 * of the entire NLM (Neural Learning Machine) system. The Brain implements
 * real spiking neural computation with fully integrated cognitive, memory,
 * prediction, and neuromodulation systems.
 * 
 * The Brain class represents the culmination of Phase 6 development,
 * featuring:
 * - Event-driven spike propagation with synaptic delays
 * - Multiple memory systems (working, episodic, associative)
 * - Advanced prediction and cognition systems
 * - Neuromodulation for learning and behavior
 * - Development and plasticity mechanisms
 * - Checkpoint/save-load functionality
 * 
 * @note This is Phase 6: FINAL INTEGRATION of the NLM project
 * @see Brain.cpp Implementation details
 * @see ../performance/OptimizedBrain.hpp Alternative optimized implementation
 */

#include "../core/Types/Types.hpp"
#include "../core/Constants/Constants.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Forward declarations for integrated systems
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
 * @brief Long-range connectivity between neural regions
 * 
 * Represents structural and functional connections between different
 * NeuralRegion instances, enabling distributed processing and
 * communication across brain areas.
 */
struct InterRegionConnection {
    RegionId sourceRegion;          ///< Source region identifier
    RegionId targetRegion;          ///< Target region identifier  
    float weight;                   ///< Connection strength (-1.0 to 1.0)
    Delay delay;                    ///< Transmission delay (simulation steps)
    PlasticityFlags plasticityFlags; ///< Plasticity rules applicable to this connection
    
    /**
     * @brief Default constructor initializes with invalid region IDs
     */
    InterRegionConnection()
        : sourceRegion()
        , targetRegion()
        , weight(0.0f)
        , delay(1)
        , plasticityFlags() {}
    
    /**
     * @brief Constructs connection with specified parameters
     * 
     * @param src Source region identifier
     * @param tgt Target region identifier
     * @param w Connection weight
     * @param d Transmission delay
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src)
        , targetRegion(tgt)
        , weight(w)
        , delay(d)
        , plasticityFlags() {}
};

/**
 * @class Brain
 * @brief Central neural processor implementing integrated artificial intelligence
 * 
 * The Brain class represents a complete artificial brain simulation with:
 * - Millions of spiking neurons organized into functional regions
 * - Multiple memory systems for different types of information
 * - Advanced prediction and cognitive processing capabilities
 * - Neuromodulation for adaptive behavior and learning
 * - Developmental plasticity and structural remodeling
 * - Event-driven computation with spike propagation
 * - Checkpoint persistence for state saving/loading
 * 
 * Key Features:
 * - Event-driven architecture for efficient simulation
 * - Real-time cognitive processing
 * - Memory integration across multiple timescales
 * - Neuromodulatory control of learning and attention
 * - Developmental stages affecting plasticity
 * - Comprehensive statistics and monitoring
 * 
 * @note The class uses the pImpl pattern for efficient compilation and
 * maintains forward references to all integrated subsystems.
 */
class Brain {
public:
    /**
     * @brief Constructs brain with configuration
     * 
     * @param config Shared configuration for brain parameters
     * @throws std::bad_alloc if memory allocation fails
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Virtual destructor with pImpl cleanup
     */
    virtual ~Brain();
    
    /**
     * @brief Copy constructor is deleted (brains are not copied)
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor transfers pImpl ownership
     * 
     * @param other Brain to move from
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator transfers pImpl ownership
     * 
     * @param other Brain to move from
     * @return Reference to this brain
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initializes brain with configuration parameters
     * 
     * Creates neural regions, populations, and initializes all
     * integrated subsystems (memory, cognition, neuromodulation,
     * plasticity, development).
     * 
     * @return bool true if initialization succeeded, false otherwise
     * @retval true Brain ready for simulation
     * @retval false Initialization failed (invalid config, memory error)
     */
    bool initialize();
    
    /**
     * @brief Performs one simulation step with automatic time calculation
     * 
     * Convenience method that calculates time based on simulation step
     * and timestep.
     * 
     * @param currentStep Current simulation step
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Performs one simulation step with explicit time parameters
     * 
     * Implements the complete integrated brain loop executing all
     * subsystems in the correct order:
     * 1. Process spike events
     * 2. Update neuron membrane potentials
     * 3. Detect and queue spikes
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
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time (ms)
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receives sensory input from environment
     * 
     * Injects current into sensory neurons based on input pattern,
     * enabling the brain to perceive and respond to its environment.
     * 
     * @param input Sensory input from environment
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Injects current directly into specific neuron
     * 
     * @param neuron Neuron identifier
     * @param current Current to inject (nA)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Injects current into all neurons of specific type
     * 
     * Used for bulk input application to sensory, internal, or motor neurons.
     * 
     * @param type Neuron type to target
     * @param current Current to inject per neuron (nA)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Gets spike system for event-driven processing
     * 
     * @return SpikeSystem* Pointer to spike system
     * @retval nullptr if system not initialized
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Gets const spike system reference
     * 
     * @return const SpikeSystem* Const pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Gets STDP plasticity system
     * 
     * @return STDP* Pointer to STDP system
     */
    STDP* getSTDP();
    
    /**
     * @brief Gets Hebbian plasticity system
     * 
     * @return Hebbian* Pointer to Hebbian system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Gets structural plasticity system
     * 
     * @return StructuralPlasticity* Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Calculates excitation-to-inhibition ratio
     * 
     * @return float E/I ratio (positive = excitation dominant,
     *         negative = inhibition dominant, infinite = no inhibition)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Gets total spike count since brain creation
     * 
     * @return size_t Total number of spikes generated
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Gets count of pending spike events
     * 
     * Includes both immediate and delayed spike events.
     * 
     * @return size_t Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produces motor action based on motor neuron activity
     * 
     * Implements simple action selection based on which motor
     * neuron population has highest activity.
     * 
     * @return std::unique_ptr<class Action> Selected action
     * @retval nullptr if no action selected
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Applies neuromodulatory signal to brain
     * 
     * Modulates plasticity and neuron excitability based on
     * neuromodulatory signals like dopamine, serotonin, etc.
     * 
     * @param signal Neuromodulator signal
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Explicitly updates plasticity rules
     * 
     * Called automatically during step() if plasticity is enabled,
     * but can be called manually for specific timing control.
     */
    void updatePlasticity();
    
    /**
     * @brief Applies developmental changes to brain
     * 
     * Modifies structural plasticity rates based on current
     * developmental stage (Initial, CriticalPeriod, Maturation, Adult).
     */
    void develop();
    
    /**
     * @brief Resets brain to initial state
     * 
     * Restores all state variables to initial values as if
     * the brain were just created or loaded from checkpoint.
     */
    void reset();
    
    /**
     * @brief Saves brain state to checkpoint file
     * 
     * Persists current brain state including neuron membrane
     * potentials, synaptic weights, and system parameters.
     * 
     * @param filepath Path to checkpoint file
     * @return bool true if save succeeded, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Loads brain state from checkpoint file
     * 
     * Restores brain state from persistent storage.
     * 
     * @param filepath Path to checkpoint file
     * @return bool true if load succeeded, false otherwise
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Creates new neural region
     * 
     * @param name Human-readable name for the region
     * @return RegionId Unique identifier for new region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Gets neural region by identifier
     * 
     * @param id Region identifier
     * @return NeuralRegion* Pointer to region
     * @retval nullptr if region not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Gets const neural region reference
     * 
     * @param id Region identifier
     * @return const NeuralRegion* Const pointer to region
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Gets count of regions
     * 
     * @return size_t Number of regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Gets all region identifiers
     * 
     * @return std::vector<RegionId> List of region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Gets all neural regions
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Adds connection between regions
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Connection weight (-1.0 to 1.0)
     * @param delay Transmission delay (steps)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Removes connection between regions
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Gets total neuron count across all regions
     * 
     * @return size_t Total number of neurons
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Gets total synapse count across all regions
     * 
     * @return size_t Total number of synapses (including inter-region connections)
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Gets count of neurons currently active
     * 
     * @return size_t Number of neurons with non-zero membrane potential
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Gets count of neurons firing in current step
     * 
     * @return size_t Number of neurons that spiked in current step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Gets average firing rate across all regions
     * 
     * @return float Average firing rate (Hz)
     */
    float getAverageFiringRate() const;
    
    /**
     * @name Memory System Accessors
     * Access methods for integrated memory systems
     */
    ///@{ 
    
    /**
     * @brief Gets working memory system
     * 
     * Working memory maintains temporary activation patterns
     * for immediate cognitive processing and decision making.
     * 
     * @return NeuralWorkingMemory* Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Gets episodic memory system
     * 
     * Episodic memory stores experiences as temporal sequences
     * for replay and consolidation.
     * 
     * @return NeuralEpisodicMemory* Pointer to episodic memory
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Gets associative memory system
     * 
     * Associative memory maintains pattern relationships and
     * associations between concepts and experiences.
     * 
     * @return NeuralAssociativeMemory* Pointer to associative memory
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    ///@}
    
    /**
     * @name Prediction System Accessors
     * Access methods for prediction and error systems
     */
    ///@{
    
    /**
     * @brief Gets prediction system
     * 
     * Prediction system generates forward models and prediction errors
     * for adaptive behavior and learning.
     * 
     * @return PredictionSystem* Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    ///@}
    
    /**
     * @name Cognition System Accessors
     * Access methods for cognitive processing systems
     */
    ///@{
    
    /**
     * @brief Gets neural planner
     * 
     * Neural planner generates action plans and sequences based on
     * current state and goals.
     * 
     * @return NeuralPlanner* Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Gets concept formation system
     * 
     * Concept formation extracts abstract patterns and concepts from
     * neural activity patterns.
     * 
     * @return ConceptFormation* Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Gets attentional selection system
     * 
     * Attentional selection focuses processing resources on relevant
     * information and suppresses irrelevant activity.
     * 
     * @return AttentionalSelection* Pointer to attentional selection
     */
    AttentionalSelection* getAttention();
    
    ///@}
    
    /**
     * @name Development System Accessors
     * Access methods for developmental processes
     */
    ///@{
    
    /**
     * @brief Gets development system
     * 
     * Development system controls synaptogenesis, pruning, and
     * maturation processes across the lifespan.
     * 
     * @return DevelopmentSystem* Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Gets current developmental stage
     * 
     * @return DevelopmentalStage Current stage (Initial, CriticalPeriod, etc.)
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Sets developmental stage
     * 
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    ///@}
    
    /**
     * @name Neuromodulation System Accessors
     * Access methods for neuromodulatory systems
     */
    ///@{
    
    /**
     * @brief Gets dopamine system
     * 
     * Dopamine system implements reward prediction error and
     * reinforcement learning.
     * 
     * @return Dopamine* Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Gets curiosity system
     * 
     * Curiosity system drives exploration and novelty detection.
     * 
     * @return Curiosity* Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Gets novelty detection system
     * 
     * Novelty system detects unexpected events and novel patterns.
     * 
     * @return Novelty* Pointer to novelty system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Gets prediction error signal
     * 
     * Prediction error signal measures discrepancy between
     * predicted and actual outcomes.
     * 
     * @return PredictionError* Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    ///@}
    
    /**
     * @brief Gets brain configuration
     * 
     * @return std::shared_ptr<const Config> Configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Gets random number generator
     * 
     * @return RandomGenerator* Pointer to RNG
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Logs comprehensive brain status
     * 
     * Outputs detailed statistics about brain state including
     * neuron counts, spike activity, memory states, and system metrics.
     */
    void logStatus() const;
    
private:
    /**
     * @brief PImpl implementation details (hidden from header)
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

