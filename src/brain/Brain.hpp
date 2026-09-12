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
    
    /**
     * @brief Create an inter-regional connection for long-range neural communication
     * 
     * Inter-regional connections enable communication between different brain regions,
     * implementing higher-level cognitive functions like attention and planning.
     * 
     * @param src Source brain region ID
     * @param tgt Target brain region ID  
     * @param w Connection weight (positive for excitatory, negative for inhibitory)
     * @param d Communication delay in simulation steps
     * @return InterRegionConnection configured connection
     * 
     * @note Used for implementing attention, prediction, and planning systems
     * @example
     * // Connect attention region to planning region for goal-directed behavior
     * auto conn = InterRegionConnection(RegionId(1), RegionId(2), 0.5f, 2);
     */
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and **fully integrated memory, prediction, cognition, and neuromodulation systems**
// 
// Phase 6: The brain functions as a complete artificial cognitive system with all
// subsystems interconnected in a unified computational loop. Memory systems receive
// sensory input, prediction systems generate forward models, and neuromodulation
// coordinates learning across all systems.

class Brain {
public:
    /**
     * @brief Create brain with configuration
     * 
     * @param config Shared pointer to configuration object containing brain parameters
     * 
     * @note Configuration should include: neuron_count, region_count, connection_probability,
     *       working_memory_capacity, max_episodic_episodes, simulation_timestep
     * 
     * @example
     * auto config = std::make_shared<Config>();
     * config->set("neuron_count", 1000);
     * config->set("region_count", 2);
     * config->set("working_memory_capacity", 100);
     * Brain brain(config);
     * brain.initialize();
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor cleans up brain resources
     */
    ~Brain();
    
    /**
     * @brief Disable copying (brain state management)
     */
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Enable move operations
     */
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * Creates neural regions, neurons, synapses, and initializes all cognitive systems.
     * 
     * @return true if initialization successful, false otherwise
     * 
     * @note After initialization, the brain is ready for simulation steps
     * @throws std::runtime_error if configuration is invalid
     */
    bool initialize();
    
    /**
     * @brief Main simulation step (time-based)
     * 
     * Advances the brain by one simulation step with precise timing for neural dynamics.
     * 
     * @param currentStep Simulation step index
     * @param currentTime Current simulation time in milliseconds
     * 
     * @note Implements the complete integrated brain loop with all cognitive systems
     * @see step(SimulationStep) for step-based version
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Main simulation step (simplified interface)
     * 
     * Convenience wrapper for step() using currentTime = currentStep * timestep.
     * 
     * @param currentStep Simulation step index
     * 
     * @note Calculates currentTime based on configured timestep
     * @see step(SimulationStep, Timestamp) for time-based version
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Injects sensory data into sensory neurons, starting the perception cycle.
     * Sensory input is transformed into neural activity patterns that propagate
     * through the brain's cognitive systems.
     * 
     * @param input Sensory input from environment (vision, touch, internal, proprioception)
     * 
     * @note This is the entry point for experience into the brain
     * @example
     * SensoryInput percept({"vision": {0.8, 0.3, 0.9}, "position": {0.5, 0.5}});
     * brain.receiveSensoryInput(percept);
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Low-level current injection for targeted neural stimulation or debugging.
     * 
     * @param neuron Neuron ID to receive current
     * @param current Current magnitude in nA to inject
     * 
     * @note Useful for stimulating specific neurons or testing neural responses
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Mass current injection for modulating neural populations (e.g., all sensory neurons).
     * 
     * @param type Neuron type to target
     * @param current Current magnitude in nA to inject
     * 
     * @note Used for implementing external stimuli or neuromodulation effects
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Access the spike processing system
     * 
     * @return Pointer to spike system for event scheduling and processing
     * 
     * @note Handles spike events, synaptic transmission, and delayed spike delivery
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Constant access to the spike processing system
     * 
     * @return Constant pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access the Spike-Timing-Dependent Plasticity system
     * 
     * @return Pointer to STDP system implementing spike-timing learning rules
     * 
     * @note STDP is the primary mechanism for temporal sequence learning
     */
    STDP* getSTDP();
    
    /**
     * @brief Access the Hebbian learning system
     * 
     * @return Pointer to Hebbian system implementing co-activation learning
     * 
     * @note Hebbian learning complements STDP for activity-based strengthening
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Access the structural plasticity system
     * 
     * @return Pointer to structural plasticity for synapse formation/removal
     * 
     * @note Structural plasticity shapes the brain's architecture over time
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation/inhibition balance
     * 
     * @return Ratio of total excitatory to total inhibitory synaptic weights
     * 
     * @note Values > 1 indicate net excitation, < 1 indicate net inhibition
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count since initialization
     * 
     * @return Total number of spikes across all neurons
     * 
     * @note Used for monitoring and statistical analysis
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get current pending spike event count
     * 
     * @return Number of spike events waiting to be processed
     * 
     * @note Includes both immediate and delayed spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * 
     * @return Unique pointer to Action object representing motor output
     * 
     * @note Implements the output pathway from neural activity to behavior
     * @return nullptr if no motor neurons are active
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals to brain systems
     * 
     * @param signal Neuromodulator (dopamine, novelty, curiosity, etc.)
     * 
     * @note Neuromodulators coordinate learning, attention, and exploration
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * 
     * @note Plasticity is now applied continuously during brain steps
     * @deprecated This method is kept for API compatibility only
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * 
     * @note Development updates plasticity rates and neural properties based on age
     */
    void develop();
    
    /**
     * @brief Reset brain state
     * 
     * @note Resets neural activity, clears memories, and restores initial state
     * @example
     * brain.reset();  // After development or checkpoint
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * @param filepath Output file path for brain state
     * @return true if save successful, false otherwise
     * 
     * @note Saves complete brain state including neurons, synapses, memories, and development
     * @see load() for restoring saved state
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * @param filepath Input file path containing saved brain state
     * @return true if load successful, false otherwise
     * 
     * @note Restores all brain systems including memories and development state
     * @see save() for creating checkpoints
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new brain region
     * 
     * @param name Optional name for the region
     * @return RegionId of newly created region
     * 
     * @note Brain regions can represent different functional areas or modules
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get brain region by ID
     * 
     * @param id Region ID to retrieve
     * @return Pointer to region, nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Constant access to brain region
     * 
     * @param id Region ID to retrieve
     * @return Constant pointer to region, nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get total number of brain regions
     * 
     * @return Number of regions in brain
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * 
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all brain regions
     * 
     * @return Constant reference to regions vector
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-regional connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight (-1.0 to 1.0)
     * @param delay Communication delay in steps
     * 
     * @note Long-range connections enable integration between brain regions
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-regional connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count across all regions
     * 
     * @return Total number of neurons
     * 
     * @note Used for statistics and system monitoring
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count across all regions
     * 
     * @return Total number of synapses
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of active neurons (threshold exceeded)
     * 
     * @return Count of neurons with membrane potential above threshold
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of firing neurons (spiking)
     * 
     * @return Count of neurons currently firing spikes
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * 
     * @return Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Access working memory system
     * 
     * @return Pointer to working memory system
     * 
     * @note Working memory provides transient storage of active information
     *       with persistent neural activity patterns
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Constant access to working memory system
     * 
     * @return Constant pointer to working memory system
     */
    const NeuralWorkingMemory* getWorkingMemory() const;
    
    /**
     * @brief Access episodic memory system
     * 
     * @return Pointer to episodic memory system
     * 
     * @note Episodic memory stores experiences as neural patterns with temporal indexing
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Constant access to episodic memory system
     * 
     * @return Constant pointer to episodic memory system
     */
    const NeuralEpisodicMemory* getEpisodicMemory() const;
    
    /**
     * @brief Access associative memory system
     * 
     * @return Pointer to associative memory system
     * 
     * @note Associative memory links related concepts and patterns
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Constant access to associative memory system
     * 
     * @return Constant pointer to associative memory system
     */
    const NeuralAssociativeMemory* getAssociativeMemory() const;
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Access prediction system
     * 
     * @return Pointer to prediction system for forward models and error computation
     * 
     * @note Prediction system generates expectations and computes prediction errors
     *       that drive learning through neuromodulation
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Constant access to prediction system
     * 
     * @return Constant pointer to prediction system
     */
    const PredictionSystem* getPredictionSystem() const;
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Access neural planner system
     * 
     * @return Pointer to neural planner for action selection and planning
     * 
     * @note Neural planner uses predictions to select actions leading to goals
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Constant access to neural planner
     */
    const NeuralPlanner* getPlanner() const;
    
    /**
     * @brief Access concept formation system
     * 
     * @return Pointer to concept formation for pattern discovery
     * 
     * @note Concept formation discovers abstract patterns from experiences
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Constant access to concept formation
     */
    const ConceptFormation* getConceptFormation() const;
    
    /**
     * @brief Access attentional selection system
     * 
     * @return Pointer to attentional selection for focus allocation
     * 
     * @note Attentional selection uses competitive dynamics to select processing focus
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Constant access to attentional selection
     */
    const AttentionalSelection* getAttention() const;
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Access development system
     * 
     * @return Pointer to development system for stage progression
     * 
     * @note Development system orchestrates maturational changes across all systems
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Constant access to development system
     */
    const DevelopmentSystem* getDevelopmentSystem() const;
    
    /**
     * @brief Get current developmental stage
     * 
     * @return Current developmental stage (Initial, CriticalPeriod, Maturation, Adult)
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * @param stage New developmental stage
     * 
     * @note Changing developmental stage affects plasticity rates and learning capabilities
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Access dopamine system
     * 
     * @return Pointer to dopamine system for reward and reinforcement
     * 
     * @note Dopamine modulates neural excitability and synaptic plasticity
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Constant access to dopamine system
     */
    const Dopamine* getDopamine() const;
    
    /**
     * @brief Access curiosity system
     * 
     * @return Pointer to curiosity system for exploration motivation
     * 
     * @note Curiosity combines novelty detection and prediction error for exploration
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Constant access to curiosity system
     */
    const Curiosity* getCuriosity() const;
    
    /**
     * @brief Access novelty detection system
     * 
     * @return Pointer to novelty system for change detection
     * 
     * @note Novelty detection drives curiosity and exploration behavior
     */
    Novelty* getNovelty();
    
    /**
     * @brief Constant access to novelty system
     */
    const Novelty* getNovelty() const;
    
    /**
     * @brief Access prediction error signal system
     * 
     * @return Pointer to prediction error system
     * 
     * @note Prediction error drives dopaminergic learning signals
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Constant access to prediction error system
     */
    const PredictionError* getPredictionErrorSignal() const;
    
    /**
     * @brief Get current configuration
     * 
     * @return Shared pointer to constant configuration object
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator
     * 
     * @return Pointer to random number generator
     * 
     * @note Used for stochastic initialization and random connectivity
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log brain status and statistics
     * 
     * @note Outputs detailed status including neurons, synapses, memories, and development
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
