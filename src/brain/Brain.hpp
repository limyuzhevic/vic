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

/**
 * @class Brain
 * @brief The central coordinator of the neural system implementing an integrated artificial brain
 * @details This class implements a complete spiking neural network with Phase 6: Integrated Artificial Brain architecture.
 * It combines real spiking neural computation with event-driven dynamics and integrated memory, prediction,
 * cognition, and neuromodulation systems. The brain serves as the main interface between higher-level cognitive
 * functions and low-level neural dynamics.
 * 
 * The Brain class follows a hierarchical architecture:
 * - **Core neural dynamics**: Spiking neurons, synapses, plasticity systems
 * - **Memory systems**: Working memory, episodic memory, associative memory
 * - **Prediction system**: Sensory prediction and error computation
 * - **Cognition systems**: Planning, concept formation, attention
 * - **Development system**: Behavioral development and maturation
 * - **Neuromodulation systems**: Dopamine, curiosity, novelty detection
 * 
 * Memory model: Uses pointer-to-implementation (PIMPL) for stable ABI and efficient memory management.
 * Thread safety: Not thread-safe; external synchronization required for concurrent access.
 * 
 * @warning Complex interaction between multiple integrated systems - requires careful configuration
 * @note Default parameters are biologically inspired but application-specific tuning may be required
 */
class Brain {
public:
    /**
     * @brief Constructor - creates brain with configuration
     * @param config Shared configuration object for brain parameters
     * @post Brain object created; initialization not complete until initialize() is called
     * @warning Configuration validation occurs during initialization, not construction
     * @throws std::bad_alloc if memory allocation fails
     * @see initialize() for complete brain setup
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor - cleans up brain resources
     * @details Performs comprehensive cleanup including:
     *         - Shutdown of all neural systems
     *         - Cleanup of memory systems
     *         - Finalization of plasticity systems
     *         - Resource deallocation
     * @post All resources released; brain object destroyed
     */
    ~Brain();
    
    /**
     * @brief Copy constructor - deleted to prevent copying and ensure unique brain identity
     * @details Brains are complex systems with unique identity and configuration
     * @warning Attempting to copy will result in compilation error
     */
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor - enables efficient brain transfer
     * @param other Brain to move from (will be invalidated after move)
     * @post this->config = other->config, other->pImpl = nullptr (source is invalid)
     * @note Move semantics provide efficient transfer of ownership without deep copying
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator - enables efficient brain reassignment
     * @param other Brain to move from (will be invalidated after move)
     * @post this->config = other->config, other->pImpl = nullptr (source is invalid)
     * @return Reference to this brain for chaining operations
     * @note Move assignment provides efficient transfer of ownership without deep copying
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * @return true if initialization successful, false if failed
     * @post Brain ready for simulation; all systems initialized
     * @details Performs comprehensive initialization including:
     *         - Region creation based on configuration
     *         - Neuron population setup across regions
     *         - Synapse connectivity initialization
     *         - Integration system startup
     *         - Memory system initialization
     *         - Plasticity system configuration
     * @warning May take significant time for large networks
     * @throws NLMException if initialization fails
     * @see step() for simulation execution
     */
    bool initialize();
    
    /**
     * @brief Main simulation step (1ms timestep)
     * @param currentStep Current simulation step number
     * @post All neural systems updated; spikes processed
     * @note Convenience method for fixed 1ms timestep simulation
     * @see step(SimulationStep, Timestamp) for variable timestep control
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step with precise time control
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in milliseconds
     * @post Complete brain integration and neural processing
     * @details Implements Phase 6: INTEGRATED ARTIFICIAL BRAIN LOOP with:
     *         - 1. Process pending delayed spike events
     *         - 2. Update all neuron membrane potentials
     *         - 3. Detect and schedule spike events
     *         - 4. Update working memory
     *         - 5. Apply neuromodulation effects
     *         - 6. Apply plasticity rules (STDP, Hebbian)
     *         - 7. Update episodic memory
     *         - 8. Update prediction system
     *         - 9. Update attention system
     *         - 10. Update concept formation
     *         - 11. Apply structural plasticity
     *         - 12. Replay important memories
     *         - 13. Apply development effects
     *         - 14. Collect statistics
     * @note Core computational method implementing complete brain dynamics
     * @see step(SimulationStep) for convenience method
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * @param input SensoryInput object containing sensory data
     * @post Current injected into sensory neurons; working memory updated
     * @details Processes environmental input through:
     *         - Normalizes input values to biological range (-10 to 10 mV)
     *         - Distributes input across all sensory neurons
     *         - Stores relevant patterns in working memory
     * @warning Input format must match brain's sensory encoding
     * @see injectCurrent() for direct neuron injection
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * @param neuron NeuronId of target neuron
     * @param current Current injection in mV
     * @post Current added to target neuron; health status invalid
     * @warning Direct injection bypasses natural processing
     * @throws NLMException if neuron not found
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * @param type NeuronType of target neurons
     * @param current Current injection in mV
     * @post Current added to all matching neurons
     * @note Useful for neuromodulatory signals affecting entire populations
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get spike system access
     * @return Pointer to spike system for spike management
     * @post None
     * @note Core system for spike generation and transmission
     * @see getSTDP() for synaptic plasticity access
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get const spike system access
     * @return Const pointer to spike system for read-only access
     * @post None
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get STDP (Spike-Timing-Dependent Plasticity) system access
     * @return Pointer to STDP system for plasticity management
     * @post None
     * @note Implements Hebbian-like learning based on spike timing
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity system access
     * @return Pointer to Hebbian system for synaptic learning
     * @post None
     * @note Implements activity-dependent synaptic strengthening
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity system access
     * @return Pointer to structural plasticity system for synapse creation/pruning
     * @post None
     * @note Manages synapse formation and elimination over time
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get excitation/inhibition balance ratio
     * @return Ratio of total excitatory weight to total inhibitory weight
     * @post None
     * @note Higher values indicate more excitatory network activity
     * @note Values near 0 indicate inhibitory dominance
     * @note Infinity indicates only excitation present
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count since brain creation
     * @return Total number of spikes generated by all neurons
     * @post Value remains unchanged
     * @note Used for performance monitoring and analysis
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of pending spike events
     * @return Count of spikes waiting for delayed delivery
     * @post None
     * @note Includes both immediate and delayed spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * @return Unique pointer to Action object
     * @post Action produced based on motor neuron analysis
     * @details Implements simple action selection:
     *         - Analyzes motor neuron firing patterns
     *         - Selects appropriate action type
     *         - Returns structured Action object
     * @note Current implementation is simplified for demonstration
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     * @param signal Neuromodulator object containing modulation data
     * @post Neuromodulation effects applied to plasticity systems
     * @details Affects learning dynamics:
     *         - Scales STDP learning rates
     *         - Modulates synaptic plasticity
     *         - Influences learning speed and direction
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (called automatically in step)
     * @post Plasticity rules applied to all synapses
     * @note This method is maintained for API compatibility
     * @see step() for automatic plasticity updates
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (called automatically in step)
     * @post Development effects applied to brain systems
     * @note Implements developmental stage transitions
     * @see step() for automatic development updates
     */
    void develop();
    
    /**
     * @brief Reset brain state to initial conditions
     * @post All neural systems reset; simulation restarted
     * @details Performs comprehensive reset including:
     *         - Reset all neurons and synapses
     *         - Clear spike system state
     *         - Reset developmental stage
     *         - Clear memory systems
     *         - Reset statistics
     * @note All simulation state is lost; equivalent to new brain instance
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * @param filepath Path to checkpoint file
     * @return true if save successful, false if failed
     * @post Brain state serialized and saved to file
     * @details Saves comprehensive brain state:
     *         - All neuron states and parameters
     *         - All synapse states and connections
     *         - System statistics
     *         - Memory system state
     * @warning File permissions must allow writing
     * @see load() for state restoration
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * @param filepath Path to checkpoint file
     * @return true if load successful, false if failed
     * @post Brain state restored from file
     * @details Restores brain state from checkpoint:
     *         - All neuron parameters and state
     *         - All synapse connections and weights
     *         - System statistics
     *         - Memory system state
     * @warning Will overwrite current brain state
     * @see save() for checkpoint creation
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Create new brain region
     * @param name Optional region name (defaults to "" if empty)
     * @return RegionId of newly created region
     * @post Region created; nextRegionId incremented
     * @warning Region IDs are sequential and immutable
     * @see getRegion() for region access
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get brain region by identifier
     * @param id Region identifier
     * @return Pointer to region if found, nullptr otherwise
     * @post Region accessed; const-qualified for read-only access
     * @note Returns nullptr if region doesn't exist
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get brain region by identifier (const version)
     * @param id Region identifier
     * @return Const pointer to region if found, nullptr otherwise
     * @post Region accessed; const-qualified for read-only access
     * @note Returns nullptr if region doesn't exist
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get total number of regions
     * @return Number of regions in brain
     * @post None
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region identifiers
     * @return Vector of all region IDs
     * @post None
     * @note Useful for iterating over all regions
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all brain regions
     * @return Const reference to vector of all regions
     * @post None
     * @note Provides direct access to all regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection (long-range connectivity)
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Connection weight (default: 0.0)
     * @param delay Connection delay in simulation steps (default: 1)
     * @post Inter-region connection added
     * @warning Connection durability across regions
     * @see removeInterRegionConnection() for removal
     */
    void addInterRegionConnection(RegionId source, RegionId target,
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * @param source Source region identifier
     * @param target Target region identifier
     * @post Connection removed if found
     * @note No effect if connection doesn't exist
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count across all regions
     * @return Total number of neurons in brain
     * @post None
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count across all regions
     * @return Total number of synapses in brain
     * @post None
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get count of active neurons
     * @return Number of neurons with elevated membrane potential
     * @post None
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get count of firing neurons in current step
     * @return Number of neurons that fired in current step
     * @post Value may change each step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * @return Average firing rate in Hz
     * @post None
     * @note Computed as total spikes / total time
     */
    float getAverageFiringRate() const;
    
    /**
     * @brief Get working memory system access
     * @return Pointer to working memory system
     * @post None
     * @note Transient memory system for active information
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory system access
     * @return Pointer to episodic memory system
     * @post None
     * @note Experience storage and replay system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory system access
     * @return Pointer to associative memory system
     * @post None
     * @note Pattern association and retrieval system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    /**
     * @brief Get prediction system access
     * @return Pointer to prediction system
     * @post None
     * @note Sensory prediction and error computation
     */
    PredictionSystem* getPredictionSystem();
    
    /**
     * @brief Get neural planner access
     * @return Pointer to neural planner system
     * @post None
     * @note Action planning and decision making
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation access
     * @return Pointer to concept formation system
     * @post None
     * @note Pattern discovery and abstraction
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection access
     * @return Pointer to attentional selection system
     * @post None
     * @note Focus and competition for resources
     */
    AttentionalSelection* getAttention();
    
    /**
     * @brief Get development system access
     * @return Pointer to development system
     * @post None
     * @note Behavioral development and maturation
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * @return Current developmental stage
     * @post None
     * @note Affects plasticity rates and learning capabilities
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * @param stage New developmental stage
     * @post Stage updated; may affect plasticity
     * @warning Stage changes affect learning capabilities
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    /**
     * @brief Get dopamine system access
     * @return Pointer to dopamine system
     * @post None
     * @note Reward and reinforcement processing
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity system access
     * @return Pointer to curiosity system
     * @post None
     * @note Exploration motivation and intrinsic drive
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty detection access
     * @return Pointer to novelty system
     * @post None
     * @note Novelty detection and change recognition
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal access
     * @return Pointer to prediction error system
     * @post None
     * @note Prediction error computation and signaling
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get current configuration
     * @return Const shared pointer to configuration
     * @post None
     * @note Read-only access to brain configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random number generator access
     * @return Pointer to random generator
     * @post None
     * @note Used for stochastic processes and initialization
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log current brain status
     * @post None
     * @details Logs comprehensive brain state:
     *         - Region and neuron counts
     *         - Synapse statistics
     *         - Memory system status
     *         - Neuromodulation levels
     *         - Developmental stage
     * @note Useful for debugging and monitoring
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
