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
// This class represents the complete artificial brain, integrating all neural
// subsystems into a coherent developmental system. It implements the main
// simulation loop and coordinates communication between all components.
// 
// Key features:
// - Leaky Integrate-and-Fire (LIF) neurons with realistic dynamics
// - Event-driven spike propagation with synaptic delays
// - Spike-Timing-Dependent Plasticity (STDP) and Hebbian learning
// - Structural plasticity (synaptogenesis and pruning)
// - Working, episodic, and associative memory systems
// - Prediction systems for temporal sequence learning
// - Cognitive planning and attentional selection
// - Neuromodulation (dopamine, curiosity, novelty, prediction error)
// - Developmental systems for synaptic growth and pruning
// - Environment interaction and motor output generation
// and integrated memory, prediction, cognition, and neuromodulation systems

// 
class Brain {
public:
    /**
     * @brief Create brain with configuration
     * 
     * Constructs a new Brain instance with the specified configuration.
     * The configuration contains simulation parameters, neural properties,
     * and system settings that control the behavior of the artificial brain.
     * 
     * @param config Shared pointer to configuration object
     * @note Configuration must be valid and contain all required parameters
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Default destructor
     * 
     * Performs cleanup of all brain resources, including neural regions,
     * synapses, and integrated subsystems. Ensures proper memory management
     * and cleanup of all allocated resources.
     */
    ~Brain();
    
    // Move operations for brain transfer between instances
    /**
     * @brief Move constructor
     * 
     * Transfers ownership of brain resources from another brain instance
     * to this one. The source brain is left in a valid but unspecified state.
     * 
     * @param other The brain instance to move from
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * 
     * Transfers ownership of brain resources from another brain instance
     * to this one. Properly cleans up existing resources first.
     * 
     * @param other The brain instance to move from
     * @return Reference to this brain instance for method chaining
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * Performs the initialization of all neural subsystems, creates the
     * initial neural architecture, and sets up all integrated systems.
     * Must be called before simulation can begin.
     * 
     * @return true if initialization succeeded, false otherwise
     * @note Initialization failure indicates a problem with configuration
     *       or resource allocation that prevents brain operation
     */
    bool initialize();
    
    /**
     * @brief Main simulation step with default timing
     * 
     * Advances the neural simulation by one time step using a default
     * time scale appropriate for the neural dynamics.
     * 
     * @param currentStep The current simulation step number
     * @note This method calls step(currentStep, currentTime) with
     *       currentTime calculated from currentStep
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step with explicit timing
     * 
     * Advances the neural simulation by one time step, coordinating all
     * neural subsystems including neurons, synapses, plasticity, memory,
     * cognition, and neuromodulation.
     * 
     * @param currentStep The current simulation step number
     * @param currentTime The current simulation time in seconds
     * @note This is the core simulation method that implements the
     *       main neural computation loop
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Processes sensory input from the environment and converts it into
     * neural current injections in sensory neurons. This enables the brain
     * to perceive and interact with its environment.
     * 
     * @param input The sensory input data containing environmental information
     * @note This method is called by the environment during simulation to
     *       provide external stimuli to the brain
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * Applies an external current directly to a specific neuron, bypassing
     * the normal sensory pathway. Used for testing, debugging, or direct
     * stimulation of specific neural elements.
     * 
     * @param neuron The ID of the neuron to receive the current
     * @param current The amount of current to inject (positive for excitation)
     * @note This method is useful for controlled experiments and testing
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Applies external current to all neurons of a specified type (e.g.,
     * sensory, motor, or association neurons). This is useful for bulk
     * stimulation of neural populations.
     * 
     * @param type The type of neurons to receive the current
     * @param current The amount of current to inject (positive for excitation)
     * @note Commonly used for sensory stimulation or population-level effects
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Access spike processing system
     * 
     * Provides direct access to the spike processing system for monitoring
     * and debugging spike-related activity. The spike system handles all
     * event-driven spike propagation and timing.
     * 
     * @return Pointer to the spike system instance
     * @note Returns nullptr if spike system has not been initialized
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Access spike processing system (const version)
     * 
     * Provides read-only access to the spike processing system for monitoring
     * and debugging spike-related activity.
     * 
     * @return Const pointer to the spike system instance
     * @note Returns nullptr if spike system has not been initialized
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access spike-timing-dependent plasticity system
     * 
     * Provides access to the STDP system, which implements the classic
     * spike-timing-dependent plasticity learning rule. This system
     * strengthens synapses when presynaptic spikes precede postsynaptic
     * spikes, and weakens them when the order is reversed.
     * 
     * @return Pointer to the STDP instance
     * @note Returns nullptr if STDP has not been initialized
     */
    STDP* getSTDP();
    
    /**
     * @brief Access Hebbian plasticity system
     * 
     * Provides access to the Hebbian plasticity system, which implements
     * basic Hebbian learning. This system strengthens synapses based on
     * correlated activity between presynaptic and postsynaptic neurons.
     * 
     * @return Pointer to the Hebbian instance
     * @note Returns nullptr if Hebbian plasticity has not been initialized
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Access structural plasticity system
     * 
     * Provides access to the structural plasticity system, which manages
     * the formation and elimination of synapses. This system implements
     * synaptogenesis (formation of new connections) and pruning (elimination
     * of unused connections).
     * 
     * @return Pointer to the structural plasticity instance
     * @note Returns nullptr if structural plasticity has not been initialized
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation-inhibition ratio
     * 
     * Computes the ratio of total excitatory input to total inhibitory input
     * across all synapses in the brain. This metric indicates the balance
     * between excitation and inhibition in the neural network.
     * 
     * @return Excitation-inhibition ratio (excitation/inhibition)
     * @note A ratio of 1.0 indicates perfect balance. Values > 1.0 indicate
     *       excitation dominance, values < 1.0 indicate inhibition dominance
     */
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
