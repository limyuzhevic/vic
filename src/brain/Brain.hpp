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
/**
 * @class Brain
 * @brief The core neural simulation engine implementing real spiking computation
 * 
 * The Brain class serves as the central coordinator of the NLM neural system,
 * implementing real spiking neural computation with event-driven dynamics and
 * integrated memory, prediction, cognition, and neuromodulation systems.
 * 
 * It manages:
 * - Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
 * - Synaptic connections with STDP, Hebbian, and structural plasticity
 * - Sensory input processing and motor output generation
 * - Memory systems (working, episodic, semantic, procedural, associative)
 * - Cognitive functions (planning, attention, concept formation)
 * - Neuromodulation (dopamine, curiosity, novelty, prediction error)
 * - Developmental processes (synaptogenesis, pruning, maturation)
 * 
 * The brain operates in discrete time steps and can be initialized with
 * configuration parameters via the Config system.
 * 
 * @see Config
 * @see ConfigValue
 * @see LogLevel
 * @see SimulationClock
 */
// Brain: The central coordinator of the neural system
/**
 * @class Brain
 * @brief The core neural simulation engine implementing real spiking computation
 * 
 * The Brain class serves as the central coordinator of the NLM neural system,
 * implementing real spiking neural computation with event-driven dynamics and
 * integrated memory, prediction, cognition, and neuromodulation systems.
 * 
 * It manages:
 * - Leaky Integrate-and-Fire (LIF) neurons with event-driven spike propagation
 * - Synaptic connections with STDP, Hebbian, and structural plasticity
 * - Sensory input processing and motor output generation
 * - Memory systems (working, episodic, semantic, procedural, associative)
 * - Cognitive functions (planning, attention, concept formation)
 * - Neuromodulation (dopamine, curiosity, novelty, prediction error)
 * - Developmental processes (synaptogenesis, pruning, maturation)
 * 
 * The brain operates in discrete time steps and can be initialized with
 * configuration parameters via the Config system.
 * 
 * @see Config
 * @see ConfigValue
 * @see LogLevel
 * @see SimulationClock
 */
class Brain {
public:
    /**
     * @brief Constructor - creates brain with configuration
     * 
     * @param config Shared pointer to configuration object containing brain parameters
     *        such as neuron count, synaptic properties, and simulation settings.
     *        Must not be null.
     * 
     * @note Configuration can be modified after brain creation using the set()
     *       method before calling initialize().
     * 
     * @see Config::set()
     * @see Brain::initialize()
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     * 
     * Cleans up brain resources including neurons, synapses, and all integrated
     * memory, prediction, cognition, and neuromodulation systems.
     */
    ~Brain();
    
    /**
     * @brief Copy constructor is deleted (non-copyable)
     * 
     * The Brain class manages dynamic resources and cannot be safely copied.
     * Use move operations or create new brain instances instead.
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Copy assignment operator is deleted (non-assignable)
     * 
     * The Brain class manages dynamic resources and cannot be safely assigned.
     * Use move operations or create new brain instances instead.
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor
     * 
     * @param other The brain instance to move from (will be left in valid but empty state)
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * @param other The brain instance to move from (will be left in valid but empty state)
     * @return Reference to this brain instance
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration and start neural computation
     * 
     * This method sets up the brain's internal systems including neurons, synapses,
     * memory systems, and all plasticity mechanisms. After initialization, the brain
     * can process sensory input, maintain internal state, and generate motor output.
     * 
     * @return bool True if initialization succeeded, false otherwise
     * 
     * @note Initialize must be called before any simulation steps or input processing.
     * @note Multiple calls to initialize() will reset the brain to its initial state.
     * 
     * @see Brain::reset()
     * @see Brain::step()
     * @see Config
     */
    bool initialize();
    
    /**
     * @brief Perform one simulation time step
     * 
     * @param currentStep Current simulation step number (monotonically increasing)
     * 
     * @note This method should be called regularly to advance the neural computation.
     * Each call processes all neurons, updates synaptic connections, and advances
     * the brain's internal state according to the current configuration and input.
     * 
     * @see Brain::step(SimulationStep, Timestamp)
     * @see Brain::receiveSensoryInput()
     * @see Brain::produceAction()
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Perform one simulation time step with precise timing
     * 
     * @param currentStep Current simulation step number (monotonically increasing)
     * @param currentTime Current simulation time in seconds (more precise than step)
     * 
     * @note This method provides more accurate timing control than the step() method,
     * allowing for non-uniform time steps and precise synchronization with external
     * events or real-time systems.
     * 
     * @see Brain::step(SimulationStep)
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive and process sensory input from environment
     * 
     * This method injects the given sensory input into the brain's sensory neurons,
     * activating the appropriate neural pathways for perception and processing.
     * 
     * @param input Sensory input data containing visual, auditory, tactile, or other
     *             sensory modalities. The input format depends on the specific
     *             sensory system implementation.
     * 
     * @note Received input will affect neural firing rates in the next simulation steps.
     * @note Input should be received before calling step() to ensure it affects computation.
     * 
     * @see Brain::step()
     * @see SensoryInput
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * This method directly excites or inhibits a specific neuron by injecting
     * membrane current, bypassing the normal sensory processing pathway.
     * 
     * @param neuron ID of the target neuron
     * @param current Amount of current to inject (positive for excitation, negative for inhibition)
     * 
     * @note Can be used for testing, debugging, or providing external control signals.
     * @note Injection takes effect immediately in the next processing cycle.
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * This method provides broad control over neural population activity by
     * applying current to all neurons of a given type (e.g., all excitatory neurons).
     * 
     * @param type Type of neurons to receive current (Excitatory, Inhibitory, etc.)
     * @param current Amount of current to inject (positive for excitation, negative for inhibition)
     * 
     * @note Useful for global neuromodulation or testing network responses.
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get access to the spike system for spike-related operations
     * 
     * @return Pointer to the spike system implementation
     * 
     * @note The spike system manages LIF neuron dynamics, spike generation,
     *       and spike propagation through synaptic connections.
     * 
     * @see SpikeSystem
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get constant access to the spike system for spike-related operations
     * 
     * @return Constant pointer to the spike system implementation
     */
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
    
    // Logging
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
