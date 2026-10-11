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
// Phase 6: Final Integration - This class represents the complete integrated artificial brain
// All major cognitive systems are integrated into this coherent neural loop
class Brain {
public:
    /**
     * @brief Create brain with configuration
     * @param config Shared configuration object containing brain parameters
     * @note Uses PImpl idiom for implementation hiding, constructor throws on allocation failure
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     * @note Cleans up Brain::Impl and any allocated resources
     */
    ~Brain();
    
    /**
     * @brief Disable copying (value semantics not supported)
     * @note Brain instances cannot be copied for safety and resource management
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Move constructor
     * @param other Brain instance to move from
     * @note Transfers ownership of implementation, leaves source in valid but unspecified state
     */
    Brain(Brain&&) noexcept;
    
    /**
     * @brief Move assignment operator
     * @param other Brain instance to move from
     * @return Reference to this instance
     * @note Transfers ownership of implementation, cleans up previous implementation
     */
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * @return true if initialization successful, false on configuration error
     * @note Creates neurons, regions, populations and initializes all integrated systems
     * @details This method sets up the complete brain architecture including:
     * - Neural computation systems (LIF neurons, spikes, plasticity)
     * - Memory systems (working, episodic, associative)
     * - Prediction system
     * - Cognitive systems (planner, concept formation, attention)
     * - Development system
     * - Neuromodulation systems (dopamine, curiosity, novelty, prediction error)
     */
    bool initialize();
    
    /**
     * @brief Main simulation step with step count only
     * @param currentStep Current simulation step number
     * @note Convenience wrapper for step(currentStep, currentStep * timestep)
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Main simulation step with time stamp
     * @param currentStep Current simulation step number
     * @param currentTime Current simulation time in milliseconds
     * @note Implements the complete Phase 6 integrated brain loop
     * @details Executes 15-step integrated brain loop:
     * 1. Process delayed spike events (synaptic input)
     * 2. Update all neurons (LIF dynamics)
     * 3. Detect spikes and schedule events
     * 4. Update working memory (maintenance, competition)
     * 5. Apply neuromodulation (novelty, curiosity, dopamine)
     * 6. Apply plasticity rules (STDP, Hebbian)
     * 7. Update episodic memory with current experience
     * 8. Update prediction system
     * 9. Update attention system
     * 10. Update concept formation
     * 11. Apply structural plasticity periodically
     * 12. Replay important memories (during rest/periodically)
     * 13. Apply development effects
     * 14. Periodic memory consolidation
     * 15. Checkpoint management
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Receive sensory input from environment
     * @param input Sensory input containing stimulus data
     * @note Injects current into sensory neurons based on input pattern
     * @details Simple mapping approach - sensory encoding is basic but functional
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * @param neuron Neuron identifier
     * @param current Current to inject in millivolts
     * @note Used for direct external stimulation or learning signals
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * @param type Neuron type (sensory, motor, internal, etc.)
     * @param current Current to inject in millivolts
     * @note Used for global modulation or stimulation of neuron populations
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // ========== SPIKE SYSTEM ACCESSORS ==========
    
    /**
     * @brief Get spike system instance
     * @return Pointer to spike system, nullptr if spike system not initialized
     * @note Spike system handles event-driven spike propagation and delays
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get spike system instance (const version)
     * @return Const pointer to spike system, nullptr if not initialized
     */
    const SpikeSystem* getSpikeSystem() const;
    
    // ========== PLASTICITY SYSTEM ACCESSORS ==========
    
    /**
     * @brief Get STDP plasticity instance
     * @return Pointer to STDP system, nullptr if not initialized
     * @note Spike-Timing-Dependent Plasticity for synaptic strengthening/weakening
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity instance
     * @return Pointer to Hebbian system, nullptr if not initialized
     * @note Hebbian learning based on correlated activity
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity instance
     * @return Pointer to structural plasticity system, nullptr if not initialized
     * @note Synaptogenesis (new connections) and pruning (old connections removal)
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // ========== STATISTICS ==========
    
    /**
     * @brief Get excitation/inhibition balance ratio
     * @return E/I ratio (excitatory/inhibitory), infinity if no inhibition, 0 if no excitation
     * @note Useful for monitoring network stability and behavior
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count since initialization
     * @return Cumulative number of spikes generated by all neurons
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get number of pending spike events
     * @return Count of spikes queued but not yet delivered to target neurons
     */
    size_t getPendingSpikeEventCount() const;
    
    /**
     * @brief Produce motor/action output based on motor neuron activity
     * @return Unique pointer to action object
     * @note Simple action selection based on current motor neuron firing patterns
     */
    std::unique_ptr<class Action> produceAction();
    
    /**
     * @brief Apply neuromodulatory signals to plasticity
     * @param signal Neuromodulator instance (dopamine, etc.)
     * @note Applies neuromodulation effects on plasticity rules
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules (legacy method)
     * @note Plasticity is now applied during each step, kept for API compatibility
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes (legacy method)
     * @note Development updates structural plasticity, kept for API compatibility
     */
    void develop();
    
    /**
     * @brief Reset brain state to initial conditions
     * @note Clears all neurons, memory systems, and statistics
     */
    void reset();
    
    /**
     * @brief Save brain state to file (checkpointing)
     * @param filepath Path to checkpoint file
     * @return true if save successful, false on error
     * @note Implements persistence using CheckpointSystem infrastructure
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * @param filepath Path to checkpoint file
     * @return true if load successful, false on error
     * @note Restores brain state including neurons, synapses, and memory
     */
    bool load(const std::string& filepath);
    
    // ========== REGION MANAGEMENT ==========
    
    /**
     * @brief Add new brain region
     * @param name Optional name for the region
     * @return Region identifier for the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get brain region by ID
     * @param id Region identifier
     * @return Pointer to region, nullptr if region not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get brain region by ID (const version)
     * @param id Region identifier
     * @return Const pointer to region, nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions in brain
     * @return Count of regions
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region identifiers
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * @return Const reference to vector of region pointers
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Synaptic weight for connection
     * @param delay Transmission delay in steps
     * @note Creates long-range connections between brain regions
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    /**
     * @brief Get total neuron count across all regions
     * @return Total number of neurons in brain
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count across all regions
     * @return Total number of synapses in brain
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get number of active neurons (firing or above threshold)
     * @return Count of active neurons
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get number of firing neurons (currently in refractory)
     * @return Count of firing neurons
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate across all neurons
     * @return Average firing rate in Hz
     */
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    /**
     * @brief Get working memory instance
     * @return Pointer to working memory, nullptr if not initialized
     * @note Working memory provides temporary storage of active information
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory instance
     * @return Pointer to episodic memory, nullptr if not initialized
     * @note Episodic memory stores experiences as episodes with timestamps
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory instance
     * @return Pointer to associative memory, nullptr if not initialized
     * @note Associative memory stores pattern associations and connections
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    /**
     * @brief Get prediction system instance
     * @return Pointer to prediction system, nullptr if not initialized
     * @note Prediction system learns temporal sequences and predicts next states
     */
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    /**
     * @brief Get neural planner instance
     * @return Pointer to neural planner, nullptr if not initialized
     * @note Neural planner evaluates potential action sequences and selects best
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation instance
     * @return Pointer to concept formation, nullptr if not initialized
     * @note Concept formation discovers patterns and forms abstractions from experience
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection instance
     * @return Pointer to attentional selection, nullptr if not initialized
     * @note Attentional selection provides competitive selection of information
     */
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    /**
     * @brief Get development system instance
     * @return Pointer to development system, nullptr if not initialized
     * @note Development system orchestrates brain maturation and stage transitions
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get current developmental stage
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * @param stage New developmental stage to set
     * @note Affects plasticity rates and learning capabilities
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    /**
     * @brief Get dopamine instance
     * @return Pointer to dopamine system, nullptr if not initialized
     * @note Dopamine signals reward prediction error and modulates plasticity
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity instance
     * @return Pointer to curiosity system, nullptr if not initialized
     * @note Curiosity drives exploration behavior based on novelty and prediction error
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty detection instance
     * @return Pointer to novelty system, nullptr if not initialized
     * @note Novelty detection identifies new or surprising sensory input
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error signal instance
     * @return Pointer to prediction error system, nullptr if not initialized
     * @note Prediction error signals discrepancy between expected and actual outcomes
     */
    PredictionError* getPredictionErrorSignal();
    
    /**
     * @brief Get current configuration
     * @return Const shared pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator instance
     * @return Pointer to random generator, nullptr if not initialized
     * @note Used for stochastic initialization and probabilistic algorithms
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log current brain status and statistics
     * @note Outputs comprehensive status information to log
     */
    void logStatus() const;
    
private:
    /**
     * @brief Internal implementation structure
     * @details PImpl idiom implementation details hidden from header
     */
    struct Impl;
    Impl* pImpl;  // Pointer to internal implementation
};

} // namespace nlm
