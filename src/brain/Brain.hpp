#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>
#include <vector>

namespace nlm {

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems

/**
 * @brief Main brain implementation - Phase 6: Integrated Artificial Brain
 * 
 * This class implements a comprehensive spiking neural network that integrates
 * multiple specialized subsystems:
 * - Neural computation (spiking neurons, synaptic dynamics)
 * - Working memory (persistent activity, recurrent connections)
 * - Episodic memory (experience storage, replay)
 * - Associative memory (pattern associations)
 * - Prediction system (forward models, prediction errors)
 * - Cognitive systems (planning, concept formation, attention)
 * - Neuromodulation (dopamine, norepinephrine, serotonin, etc.)
 * - Development (plasticity, maturation)
 * 
 * The brain operates with event-driven dynamics, processing spikes in real-time
 * and maintaining complex internal states for learning and behavior.
 * 
 * @complexity O(N_neurons + N_synapses) for neural processing
 *            O(N_memories) for memory operations
 *            O(N_systems) for integrated system updates
 * 
 * @note This is Phase 6 of the Neural Learning Machine (NLM) system,
 *       representing the fully integrated artificial brain implementation.
 */
class Brain {
public:
    // Type aliases for improved readability and maintainability
    using RegionIdList = std::vector<RegionId>;
    using RegionPtr = std::unique_ptr<NeuralRegion>;
    using RegionPtrList = std::vector<RegionPtr>;
    using ConnectionId = std::pair<RegionId, RegionId>;
    using ConnectionList = std::vector<InterRegionConnection>;
    
    /**
     * @brief Creates a brain instance with the given configuration
     * 
     * @param config Shared pointer to configuration object containing
     *              simulation parameters (neuron count, region count, etc.)
     * @post Brain is created with configuration loaded into pImpl structure
     * 
     * @complexity O(1) - constant time constructor
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroys brain instance and cleans up allocated resources
     * 
     * @post All brain subsystems are properly shut down
     */
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Move constructor - transfers ownership of implementation
     * 
     * @param other Brain instance to move from
     * @post This brain takes ownership of other's implementation,
     *       other is left in valid but unspecified state
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator - transfers ownership of implementation
     * 
     * @param other Brain instance to move from
     * @return Reference to this brain instance
     * @post This brain takes ownership of other's implementation,
     *       other is left in valid but unspecified state
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initializes brain with configuration parameters
     * 
     * This method sets up the brain's structure and subsystems based on
     * configuration parameters. It must be called before any simulation steps.
     * 
     * @return true if initialization succeeds, false otherwise
     * @post Brain is ready for simulation if true
     * 
     * @complexity O(N_regions * neurons_per_region) - depends on network size
     */
    bool initialize();
    
    /**
     * @brief Main simulation step (time-based)
     * 
     * Performs a complete brain simulation step, processing neurons,
     * updating subsystems, and managing internal state. This is the primary
     * simulation method used for standard time-based stepping.
     * 
     * @param currentStep Current simulation step number
     * @post Brain state updated for this time step
     * 
     * @complexity O(N_neurons + N_synapses + N_systems) where N represents
     *            total counts of neurons, synapses, and systems
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step with time information
     * 
     * Enhanced simulation step that includes precise timing information
     * for more accurate neural dynamics and event scheduling.
     * 
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in milliseconds
     * @post Brain state updated with temporal precision
     * 
     * @complexity O(N_neurons + N_synapses + N_systems) - same as time-based step
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receives sensory input from environment
     * 
     * Injects current into sensory neurons based on the input pattern,
     * allowing the brain to perceive and respond to environmental stimuli.
     * 
     * @param input Sensory input containing pattern and associated information
     * @post Sensory neurons are updated with environmental input
     * 
     * @complexity O(N_sensory_neurons) - linear with number of sensory neurons
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Injects current directly into a specific neuron
     * 
     * Provides direct control over individual neuron activation, useful for
     * testing, debugging, or implementing external control signals.
     * 
     * @param neuron ID of the target neuron
     * @param current Amount of current to inject (microsiemens)
     * @post Target neuron membrane potential is modified
     * 
     * @complexity O(1) - constant time operation
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Injects current into all neurons of a specific type
     * 
     * Batch injection of current into all neurons of a given type (e.g., all
     * sensory, motor, or internal neurons), enabling targeted modulation of
     * neural populations.
     * 
     * @param type Type of neurons to target
     * @param current Amount of current to inject per neuron (microsiemens)
     * @post All neurons of specified type are updated
     * 
     * @complexity O(N_neurons_of_type) - linear with count of target neurons
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // ========== COMPUTATIONAL SYSTEM ACCESS ==========
    
    /**
     * @brief Access spike system for spike event management
     * 
     * @return Pointer to spike system, nullptr if not initialized
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Access spike system (const version)
     * 
     * @return Const pointer to spike system
     */
    const SpikeSystem* getSpikeSystem() const;
    
    /**
     * @brief Access spike-timing-dependent plasticity system
     * 
     * @return Pointer to STDP system
     */
    STDP* getSTDP();
    
    /**
     * @brief Access Hebbian plasticity system
     * 
     * @return Pointer to Hebbian system
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Access structural plasticity system
     * 
     * @return Pointer to structural plasticity system
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // ========== STATISTICS ==========
    
    /**
     * @brief Computes excitation-inhibition balance ratio
     * 
     * Measures the relative strength of excitatory versus inhibitory neural activity
     * in the system, providing insight into network stability and dynamics.
     * 
     * @return E/I ratio as float (typically >1.0 for healthy networks)
     * @complexity O(N_neurons) - linear with total neuron count
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Gets total spike count across all neurons
     * 
     * @return Total number of spikes generated during current simulation
     * @complexity O(1) - maintains running total
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Gets number of pending spike events
     * 
     * @return Count of scheduled spike events not yet processed
     * @complexity O(1) - maintains running count
     */
    size_t getPendingSpikeEventCount() const;
    
    // ========== OUTPUT PRODUCTION ==========
    
    /**
     * @brief Produces motor/action output based on motor neuron activity
     * 
     * Analyzes motor neuron firing patterns and produces appropriate action
     * signals for controlling external behavior or influencing the environment.
     * 
     * @return Unique pointer to action containing motor commands
     * @post Action is produced based on current neural state
     * 
     * @complexity O(N_motor_neurons) - linear with motor neuron count
     */
    std::unique_ptr<class Action> produceAction();
    
    // ========== SYSTEM INTEGRATION ==========
    
    /**
     * @brief Applies neuromodulatory signals to brain systems
     * 
     * Integrates neuromodulatory signals (dopamine, serotonin, etc.) into the
     * brain's control systems, affecting neural excitability, plasticity, and
     * behavioral tendencies.
     * 
     * @param signal Neuromodulatory signal containing type and intensity
     * @post Brain systems are modulated according to signal
     * 
     * @complexity O(N_systems) - updates all integrated systems
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Updates plasticity rules (called automatically in step)
     * 
     * Applies synaptic plasticity mechanisms (STDP, Hebbian, etc.) to update
     * synaptic weights based on neural activity patterns.
     * 
     * @post Synaptic weights are updated according to plasticity rules
     * 
     * @complexity O(N_synapses) - linear with synapse count
     */
    void updatePlasticity();
    
    /**
     * @brief Applies developmental changes (called automatically in step)
     * 
     * Applies developmental effects that influence neural organization,
     * synaptic structure, and system maturation.
     * 
     * @post Developmental changes are applied to brain systems
     * 
     * @complexity O(N_regions) - linear with region count
     */
    void develop();
    
    /**
     * @brief Resets brain state to initial conditions
     * 
     * Clears all neural activity, resets membrane potentials, and resets
     * internal system states to initial conditions.
     * 
     * @post Brain is in initial state, ready for new simulation
     * 
     * @complexity O(N_neurons + N_synapses) - linear with network size
     */
    void reset();
    
    // ========== PERSISTENCE ==========
    
    /**
     * @brief Saves brain state to file (checkpointing)
     * 
     * Serializes the current brain state (neural activity, synaptic weights,
     * memory content) to a file for persistence and restart.
     * 
     * @param filepath Path to output file
     * @return true if save succeeds, false on error
     * @post Brain state is saved to persistent storage
     * 
     * @complexity O(N_neurons + N_synapses + N_memories) - linear with system size
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Loads brain state from file
     * 
     * Restores brain state from a checkpoint file, allowing recovery from
     * persistence or continuation of previous simulations.
     * 
     * @param filepath Path to input file
     * @return true if load succeeds, false on error
     * @post Brain state is restored from persistent storage
     * 
     * @complexity O(N_neurons + N_synapses + N_memories) - linear with system size
     */
    bool load(const std::string& filepath);
    
    // ========== REGION MANAGEMENT ==========
    
    /**
     * @brief Creates a new brain region
     * 
     * @param name Optional name for the region (default: "")
     * @return Region ID of newly created region
     * @post Region is added to brain's region collection
     * 
     * @complexity O(1) - constant time region creation
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Gets region by ID
     * 
     * @param id Region identifier
     * @return Pointer to region, nullptr if not found
     * @complexity O(N_regions) - linear with region count
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Gets region by ID (const version)
     * 
     * @param id Region identifier
     * @return Const pointer to region, nullptr if not found
     * @complexity O(N_regions) - linear with region count
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Gets number of regions
     * 
     * @return Count of brain regions
     * @complexity O(1) - maintains running count
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Gets all region IDs
     * 
     * @return List of all region IDs
     * @complexity O(N_regions) - linear with region count
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Gets all regions
     * 
     * @return Const reference to all brain regions
     * @complexity O(1) - provides direct access
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // ========== INTER-REGION CONNECTIONS ==========
    
    /**
     * @brief Adds connection between two regions
     * 
     * Creates a long-range connection from source to target region,
     * allowing inter-regional communication and coordinated activity.
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Synaptic weight of connection
     * @param delay Transmission delay (in time steps)
     * @post Connection is added to inter-region connection list
     * 
     * @complexity O(1) - constant time connection addition
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Removes connection between two regions
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @post Connection is removed from list
     * 
     * @complexity O(N_connections) - linear with connection count
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // ========== GLOBAL STATISTICS ==========
    
    /**
     * @brief Gets total number of neurons in brain
     * 
     * @return Total neuron count across all regions
     * @complexity O(N_regions) - linear with region count
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Gets total number of synapses in brain
     * 
     * @return Total synapse count (intra-region + inter-region)
     * @complexity O(N_regions) - linear with region count
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Gets number of active neurons
     * 
     * @return Count of neurons currently firing or above threshold
     * @complexity O(N_regions) - linear with region count
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Gets number of firing neurons (current step)
     * 
     * @return Count of neurons that fired during current simulation step
     * @complexity O(1) - maintains running count
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Gets average firing rate across all neurons
     * 
     * @return Average firing rate (Hz)
     * @complexity O(N_regions) - linear with region count
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Access working memory system
     * 
     * @return Pointer to working memory system
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Access episodic memory system
     * 
     * @return Pointer to episodic memory system
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Access associative memory system
     * 
     * @return Pointer to associative memory system
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Access prediction system
     * 
     * @return Pointer to prediction system
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Access neural planner system
     * 
     * @return Pointer to neural planner
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Access concept formation system
     * 
     * @return Pointer to concept formation system
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Access attentional selection system
     * 
     * @return Pointer to attentional selection system
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Access development system
     * 
     * @return Pointer to development system
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Gets current brain developmental stage
     * 
     * @return Current developmental stage
     * @complexity O(1) - constant time access
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Sets brain developmental stage
     * 
     * @param stage New developmental stage
     * @post Brain developmental stage is updated
     * 
     * @complexity O(1) - constant time update
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Access dopamine system
     * 
     * @return Pointer to dopamine system
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Access norepinephrine system
     * 
     * @return Pointer to norepinephrine system
     */
    Norepinephrine* getNorepinephrine();
    
    /**
     * @brief Access curiosity system
     * 
     * @return Pointer to curiosity system
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Access novelty detection system
     * 
     * @return Pointer to novelty system
     */
    Novelty* getNovelty();
    
    /**
     * @brief Access prediction error signal system
     * 
     * @return Pointer to prediction error system
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Access serotonin system
     * 
     * @return Pointer to serotonin system
     */
    Serotonin* getSerotonin();
    
    /**
     * @brief Access acetylcholine system
     * 
     * @return Pointer to acetylcholine system
     */
    Acetylcholine* getAcetylcholine();
    
    // ========== SYSTEM CONFIGURATION ==========
    
    /**
     * @brief Gets current configuration
     * 
     * @return Shared pointer to configuration
     * @complexity O(1) - constant time access
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Gets random number generator
     * 
     * @return Pointer to random generator
     * @complexity O(1) - constant time access
     */
    RandomGenerator* getRandomGenerator();
    
    // ========== DEBUGGING AND MONITORING ==========
    
    /**
     * @brief Logs brain status and statistics
     * 
     * Outputs comprehensive status information including region counts,
     * neuron counts, synapse counts, firing statistics, and system state.
     * 
     * @post Status information is logged via NLM_LOG_INFO
     * 
     * @complexity O(N_regions) - linear with region count
     */
    void logStatus() const;
    
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
    
    // Forward declarations (now reduced to only essential ones)
    class Config;
    class RandomGenerator;
    class SimulationClock;
    class Logger;
    class SensoryInput;
    class Neuromodulator;
    class Action;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm

/**
 * @brief Type alias for enhanced Brain documentation
 * 
 * This provides modern C++ type aliases that improve code readability
 * and maintainability in the Brain class interface.
 */
using BrainPtr = std::shared_ptr<nlm::Brain>;
using BrainConstPtr = std::shared_ptr<const nlm::Brain>;
