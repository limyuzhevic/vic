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

/**
 * @brief Inter-regional connection representing long-range connectivity
 * 
 * This structure defines a connection between two neural regions, enabling
 * communication and integration across different brain areas. Long-range
 * connections are important for coordinating activity between regions
 * and supporting functions like attention and working memory.
 */
struct InterRegionConnection {
    RegionId sourceRegion;          ///< Source region identifier
    RegionId targetRegion;          ///< Target region identifier
    float weight;                   ///< Connection strength (-1.0 to 1.0)
    Delay delay;                    ///< Transmission delay in simulation steps
    PlasticityFlags plasticityFlags; ///< Plasticity configuration for this connection
    
    /// Default constructor
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /// Parameterized constructor
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @brief The central coordinator of the neural system (Phase 6: Complete Artificial Brain)
 * 
 * This class implements the complete integrated brain simulation as described in
 * Phase 6: FINAL INTEGRATION. The Brain class orchestrates all neural components
 * including memory systems, prediction systems, cognition systems, development
 * systems, and neuromodulation systems.
 * 
 * ## Brain Architecture:
 * 
 * ### Core Neural Computation:
 * - **Real LIF neurons**: Leaky Integrate-and-Fire dynamics with spike generation
 * - **Event-driven processing**: Delayed spike events and synaptic transmission
 * - **Plasticity integration**: STDP, Hebbian, and structural plasticity mechanisms
 * - **Neuromodulation**: Dopamine, curiosity, novelty, and prediction error signals
 * 
 * ### Integrated Systems:
 * 
 * #### Memory Systems (Access via getWorkingMemory(), getEpisodicMemory(), etc.)
 * - **Working memory**: Transient active information with competitive dynamics
 * - **Episodic memory**: Experience encoding and replay during rest periods
 * - **Associative memory**: Pattern associations and concept formation
 * - **Procedural memory**: Action sequence learning and motor programs
 * 
 * #### Prediction System
 * - **Forward model**: Sensory prediction and error computation
 * - **Confidence tracking**: Uncertainty estimation and prediction reliability
 * 
 * #### Cognition Systems
 * - **Neural planner**: Action sequence evaluation with forward models
 * - **Concept formation**: Pattern discovery and abstraction
 * - **Attentional selection**: Competitive focus mechanism
 * 
 * #### Development System
 * - **Developmental stages**: Initial -> Critical period -> Maturation -> Adult
 * - **Structural plasticity**: Synaptogenesis and pruning modulated by age
 * - **Neural excitability**: Changing responsiveness over time
 * 
 * #### Neuromodulation Systems
 * - **Dopamine**: Reward prediction error and reinforcement learning
 * - **Curiosity**: Exploration motivation and intrinsic reward
 * - **Novelty**: Surprise detection and novelty-based learning
 * - **Prediction error**: Forward model error signals
 * 
 * ### Simulation Loop (15-Step Integration Process):
 * 
 * 1. **Process delayed spikes**: Deliver synaptic input with transmission delays
 * 2. **Update neurons**: LIF dynamics and spike detection
 * 3. **Schedule spikes**: Queue immediate and delayed spike events
 * 4. **Update working memory**: Store active neurons and maintain competition
 * 5. **Apply neuromodulation**: Update novelty, curiosity, dopamine levels
 * 6. **Apply plasticity**: STDP, Hebbian, and reward-modulated learning
 * 7. **Update episodic memory**: Store current experience as new episode
 * 8. **Update prediction system**: Process sensory predictions and errors
 * 9. **Update attention system**: Select and focus on competing representations
 * 10. **Update concept formation**: Extract patterns and form concepts
 * 11. **Apply structural plasticity**: Synaptogenesis and pruning
 * 12. **Replay memories**: Replay important episodes during consolidation
 * 13. **Apply development effects**: Age-dependent plasticity modulation
 * 14. **Periodic consolidation**: Strengthen important memories, discard weak ones
 * 15. **Checkpoint management**: Save state periodically
 * 
 * ## Key Features:
 * 
 * - **Closed-loop operation**: World → Sensory → Neural → Action → World
 * - **Self-organizing**: System develops its own structure through experience
 * - **Persistent activity**: Working memory maintains information over time
 * - **Replay capability**: Experiences are replayed during rest/consolidation
 * - **Developmental trajectory**: System matures and stabilizes over time
 * - **Modulatory control**: Multiple neuromodulators coordinate learning and behavior
 * - **Checkpointing**: Can save/load brain state for persistence and recovery
 * - **Scalable architecture**: Supports different region counts and connectivity patterns
 * 
 * ## Usage:
 * 
 * The Brain class is designed to be used as follows:
 * 
 * @code
 * // Create brain with configuration
 * auto config = std::make_shared<Config>();
 * auto brain = std::make_shared<Brain>(config);
 * 
 * // Initialize with random neurons and connections
 * brain->initialize();
 * 
 * // Main simulation loop
 * for (SimulationStep step = 0; step < max_steps; ++step) {
 *     Timestamp time = step * timestep;
 *     
 *     // Process sensory input from environment
 *     brain->receiveSensoryInput(sensoryInput);
 *     
 *     // Run neural computation
 *     brain->step(step, time);
 *     
 *     // Generate action based on motor activity
 *     auto action = brain->produceAction();
 *     
 *     // Apply action to environment
 *     // Update world state based on action
 * }
 * @endcode
 * 
 * @note Uses PIMPL pattern to hide implementation details and reduce compile times.
 *       All member functions are implemented in Brain.cpp.
 */
class Brain {
public:
    /**
     * @brief Create brain with configuration
     * 
     * @param config Configuration for brain initialization (neuron count,
     *               connectivity, plasticity parameters, etc.)
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    /**
     * @brief Disable copying (PIMPL pattern)
     */
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Enable move operations (PIMPL pattern)
     */
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * This method creates the neural architecture, sets up all integrated systems,
     * and initializes neurons, synapses, and memory systems according to the
     * configuration parameters.
     * 
     * @return bool True if initialization succeeded, false on error
     */
    bool initialize();
    
    /**
     * @brief Main simulation step with time parameter
     * 
     * This method implements the complete integrated brain loop as described
     * in the class documentation. It processes all neural dynamics, plasticity,
     * memory updates, and neuromodulation for one simulation timestep.
     * 
     * @param currentStep Current simulation step (integer time)
     * @param currentTime Current simulation time in milliseconds
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Main simulation step (convenience wrapper)
     * 
     * @param currentStep Current simulation step
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * This method processes sensory input by injecting current into sensory
     * neurons according to the input pattern. The sensory input drives the
     * brain-behavior cycle and provides the interface to the external world.
     * 
     * @param input Sensory input containing observations and signals
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * This method directly injects current into a specific neuron, bypassing
     * synaptic transmission. Used for direct control or external stimulation.
     * 
     * @param neuron NeuronId to inject current into
     * @param current Current amount to inject (nS * mV)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * This method distributes current across all neurons of a given type,
     * typically used to implement global modulatory signals or population-level
     * effects.
     * 
     * @param type Neuron type to target (Sensory, Internal, Motor)
     * @param current Current amount to inject per neuron (nS * mV)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    /**
     * @brief Get access to the spike system
     * 
     * @return SpikeSystem* Pointer to the spike system
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get const access to the spike system
     * 
     * @return const SpikeSystem* Const pointer to the spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Get access to the STDP plasticity system
     * 
     * @return STDP* Pointer to the spike-timing dependent plasticity system
     */
    STDP* getSTDP();
    
    /**
     * @brief Get access to the Hebbian plasticity system
     * 
     * @return Hebbian* Pointer to the Hebbian plasticity system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get access to the structural plasticity system
     * 
     * @return StructuralPlasticity* Pointer to the structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    /**
     * @brief Get the excitation-inhibition balance ratio
     * 
     * This ratio indicates the balance between excitatory and inhibitory
     * influence in the network, which is crucial for stability and function.
     * 
     * @return float E/I ratio (higher means more excitation)
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get the total number of spikes generated
     * 
     * @return size_t Total spike count since initialization
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get the number of pending spike events
     * 
     * Includes both immediate and delayed spike events waiting to be processed.
     * 
     * @return size_t Number of pending spike events
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor action based on motor neuron activity
     * 
     * This method implements action selection based on the activity of motor
     * neurons in the network. It typically selects the motor neuron population
     * with the highest average activity.
     * 
     * @return std::unique_ptr<Action> Generated action for the environment
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals
     * 
     * This method applies neuromodulatory signals (e.g., dopamine, curiosity)
     * to influence neural excitability and plasticity throughout the network.
     * 
     * @param signal Neuromodulatory signal to apply
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (maintained for API compatibility)
     * 
     * Plasticity is now applied automatically during each step, so this method
     * is kept for API compatibility but does nothing in Phase 6.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes
     * 
     * This method applies age-dependent developmental changes to the network,
     * including changes in plasticity rates and structural modifications.
     * 
     * @note Development is typically applied every 1000 simulation steps
     */
    void develop();
    
    /**
     * @brief Reset brain state to initial conditions
     * 
     * This method resets all neural components, memory systems, and statistics
     * to their initial state. Used for restarting simulations or experiments.
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * 
     * This method serializes the current brain state including neuron states,
     * synapse states, and integrated system states to a file for persistence
     * and recovery.
     * 
     * @param filepath Path to checkpoint file
     * @return bool True if save succeeded, false on error
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * This method deserializes a previously saved brain state and restores
     * all neural components and memory systems to that state.
     * 
     * @param filepath Path to checkpoint file
     * @return bool True if load succeeded, false on error
     */
    bool load(const std::string& filepath);
    
    /**
     * @brief Add a new neural region
     * 
     * @param name Optional name for the region
     * @return RegionId Unique identifier for the new region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get a region by ID
     * 
     * @param id Region identifier
     * @return NeuralRegion* Pointer to region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get a region by ID (const version)
     * 
     * @param id Region identifier
     * @return const NeuralRegion* Const pointer to region, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get the number of regions in the brain
     * 
     * @return size_t Number of regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * 
     * @return std::vector<RegionId> Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * 
     * @return const std::vector<std::unique_ptr<NeuralRegion>>& Vector of all regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add an inter-region connection
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     * @param weight Connection strength (default: 0.0)
     * @param delay Transmission delay in steps (default: 1)
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove an inter-region connection
     * 
     * @param source Source region identifier
     * @param target Target region identifier
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get the total number of neurons in the brain
     * 
     * @return size_t Total neuron count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get the total number of synapses in the brain
     * 
     * @return size_t Total synapse count (includes inter-region connections)
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get the number of neurons currently active
     * 
     * @return size_t Number of neurons with membrane potential above threshold
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get the number of neurons firing in this step
     * 
     * @return size_t Number of neurons that spiked this step
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get the average firing rate across all neurons
     * 
     * @return float Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Get access to the working memory system
     * 
     * @return NeuralWorkingMemory* Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get access to the episodic memory system
     * 
     * @return NeuralEpisodicMemory* Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get access to the associative memory system
     * 
     * @return NeuralAssociativeMemory* Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get access to the prediction system
     * 
     * @return PredictionSystem* Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get access to the neural planner
     * 
     * @return NeuralPlanner* Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get access to the concept formation system
     * 
     * @return ConceptFormation* Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get access to the attentional selection system
     * 
     * @return AttentionalSelection* Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get access to the development system
     * 
     * @return DevelopmentSystem* Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get the current developmental stage
     * 
     * @return DevelopmentalStage Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set the developmental stage
     * 
     * @param stage New developmental stage
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get access to the dopamine system
     * 
     * @return Dopamine* Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get access to the curiosity system
     * 
     * @return Curiosity* Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get access to the novelty system
     * 
     * @return Novelty* Pointer to novelty system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get access to the prediction error system
     * 
     * @return PredictionError* Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get the current configuration
     * 
     * @return std::shared_ptr<const Config> Const configuration pointer
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get access to the random number generator
     * 
     * @return RandomGenerator* Pointer to random number generator
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log detailed brain status
     * 
     * This method logs comprehensive information about the current brain state
     * including statistics, memory system status, and neuromodulation levels.
     */
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
