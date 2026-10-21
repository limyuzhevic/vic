#pragma once

namespace nlm {

/**
 * @file Brain.hpp
 * @brief Brain class header with improved documentation and organization
 * 
 * This file contains the main Brain class declaration with comprehensive
 * documentation for all public methods and member functions. The Brain class
 * serves as the central coordinator of the neural system, integrating all
 * subsystems for intelligent behavior.
 * 
 * The Brain class uses the PIMPL pattern for implementation to maintain
 * a clean header and separate implementation from interface.
 */

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>
#include <string>

// Forward declarations for all integrated systems
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
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;
class SensoryInput;
class Action;
class Neuromodulator;
class CheckpointManager;
class CheckpointWriter;
class CheckpointReader;
class FiringState;

// Inter-regional connection structure for long-range connectivity
struct InterRegionConnection {
    RegionId sourceRegion;
    RegionId targetRegion;
    float weight;
    Delay delay;
    PlasticityFlags plasticityFlags;
    
    /**
     * @brief Default constructor
     */
    InterRegionConnection()
        : sourceRegion(), targetRegion(), weight(0.0f), delay(1), plasticityFlags() {}
    
    /**
     * @brief Constructor with parameters
     * 
     * @param src Source region ID
     * @param tgt Target region ID
     * @param w Initial weight
     * @param d Delay in steps
     */
    InterRegionConnection(RegionId src, RegionId tgt, float w = 0.0f, Delay d = 1)
        : sourceRegion(src), targetRegion(tgt), weight(w), delay(d), plasticityFlags() {}
};

/**
 * @class Brain
 * @brief Central coordinator of the neural system
 * 
 * The Brain class implements real spiking neural computation with event-driven
 * dynamics and integrated memory, prediction, cognition, and neuromodulation
 * systems. It serves as the main interface for controlling and observing the
 * artificial brain system.
 * 
 * The Brain class uses the PIMPL pattern for implementation, keeping the header
 * clean and separating implementation details from the interface. This improves
 * compilation times and reduces coupling between components.
 * 
 * Key features:
 * - Phase 6: Fully integrated artificial brain with all systems
 * - Event-driven spike processing for efficient simulation
 * - Modular architecture with clear separation of concerns
 * - Comprehensive checkpoint/save/load functionality
 * - Advanced neuromodulation and plasticity systems
 * 
 * Usage:
 * @code
 * auto config = std::make_shared<Config>();
 * config->loadFromFile("brain_config.cfg");
 * auto brain = std::make_unique<Brain>(config);
 * brain->initialize();
 * 
 * for (int step = 0; step < 1000; ++step) {
 *     brain->step(step);
 *     // Process sensory input, check actions, etc.
 * }
 * @endcode
 */
class Brain {
public:
    /**
     * @brief Create brain with configuration
     * 
     * @param config Configuration for the brain system
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor
     */
    ~Brain();
    
    /**
     * @brief Disable copying (PIMPL pattern)
     */
    Brain(const Brain&) = delete;
    
    /**
     * @brief Disable assignment (PIMPL pattern)
     */
    Brain& operator=(const Brain&) = delete;
    
    /**
     * @brief Enable move operations (PIMPL pattern)
     */
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    /**
     * @brief Initialize brain with configuration
     * 
     * This function sets up the complete brain architecture including:
     * - Region creation and neuron population setup
     * - Random connectivity initialization
     * - Integrated system initialization (memory, prediction, cognition, etc.)
     * - Spike handler registration for event-driven processing
     * - Configuration loading and parameter setup
     * 
     * @return true if initialization succeeds, false otherwise
     */
    bool initialize();
    
    /**
     * @brief Main simulation step with time parameter
     * 
     * This is the main simulation loop that processes one timestep of brain activity.
     * It implements the complete integrated brain simulation with 15+ steps:
     * 1. Process pending delayed spike events
     * 2. Update all neuron membrane potentials (LIF dynamics)
     * 3. Detect spikes and schedule outgoing spike events
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
     * 15. Checkpoint management
     * 
     * @param currentStep Current simulation step
     * @param currentTime Current simulation time
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    /**
     * @brief Main simulation step (convenience overload)
     * 
     * This overload automatically calculates the current time based on the step
     * and configured timestep.
     * 
     * @param currentStep Current simulation step
     */
    void step(SimulationStep currentStep);
    
    /**
     * @brief Receive sensory input from environment
     * 
     * Injects current into sensory neurons based on input pattern. This represents
     * the brain's perception of the external environment and drives prediction
     * and learning processes.
     * 
     * @param input Sensory input from environment
     */
    void receiveSensoryInput(const SensoryInput& input);
    
    /**
     * @brief Inject current directly into a specific neuron
     * 
     * This function allows direct control over specific neurons, useful for
     * testing, control experiments, or targeted stimulation.
     * 
     * @param neuron Neuron ID to inject current into
     * @param current Current value to inject (in mV)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * This function applies current to all neurons of a given type (sensory,
     * internal, or motor), useful for broad control or experimental conditions.
     * 
     * @param type Neuron type to target
     * @param current Current value to inject (in mV)
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // ===== SYSTEM ACCESS METHODS =====
    
    /**
     * @brief Get spike system pointer
     * 
     * @return Pointer to spike system, or nullptr if not initialized
     */
    SpikeSystem* getSpikeSystem();
    
    /**
     * @brief Get spike system pointer (const version)
     * 
     * @return Const pointer to spike system, or nullptr if not initialized
     */
    const SpikeSystem* getSpikeSystem() const;
    
    // ===== PLASTICITY SYSTEM ACCESS =====
    
    /**
     * @brief Get STDP plasticity system
     * 
     * @return Pointer to STDP system, or nullptr if not initialized
     */
    STDP* getSTDP();
    
    /**
     * @brief Get Hebbian plasticity system
     * 
     * @return Pointer to Hebbian system, or nullptr if not initialized
     */
    Hebbian* getHebbian();
    
    /**
     * @brief Get structural plasticity system
     * 
     * @return Pointer to structural plasticity system, or nullptr if not initialized
     */
    StructuralPlasticity* getStructuralPlasticity();
    
    // ===== STATISTICS AND METRICS =====
    
    /**
     * @brief Get excitation/inhibition ratio
     * 
     * @return Ratio of total excitatory weight to total inhibitory weight
     *         Returns infinity if no inhibitory connections exist
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count
     * 
     * @return Total number of spikes recorded since brain initialization
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get pending spike event count
     * 
     * @return Total number of pending spike events (immediate + delayed)
     */
    size_t getPendingSpikeEventCount() const;
    
    // ===== BEHAVIOR AND ACTION =====
    
    /**
     * @brief Produce motor/action output
     * 
     * This function selects an action based on motor neuron activity patterns.
     * It implements simple action selection where the most active motor neuron
     * group determines the output action.
     * 
     * @return Unique pointer to action object
     */
    std::unique_ptr<Action> produceAction();
    
    // ===== NEUROMODULATION =====
    
    /**
     * @brief Apply neuromodulatory signals
     * 
     * This function applies neuromodulatory signals to brain systems,
     * affecting neural excitability, plasticity rates, and learning dynamics.
     * 
     * @param signal Neuromodulator signal to apply
     */
    void applyNeuromodulation(const Neuromodulator& signal);
    
    /**
     * @brief Update plasticity rules
     * 
     * This function applies all plasticity rules to current synaptic states.
     * It's called automatically during each simulation step.
     */
    void updatePlasticity();
    
    /**
     * @brief Apply developmental changes
     * 
     * This function applies developmental effects to the brain systems,
     * including regulation of plasticity rates and structural maturation.
     */
    void develop();
    
    // ===== STATE MANAGEMENT =====
    
    /**
     * @brief Reset brain state
     * 
     * This function resets all brain systems to their initial state:
     * - Resets simulation counters and timers
     * - Resets all neurons and synapses
     * - Clears memory systems
     * - Resets neuromodulation systems
     * - Resets developmental stage to Initial
     */
    void reset();
    
    /**
     * @brief Save brain state to file
     * 
     * This function saves the complete brain state to a checkpoint file,
     * including neuron states, synaptic weights, memory contents, and
     * configuration parameters.
     * 
     * @param filepath Path to checkpoint file
     * @return true if save succeeds, false otherwise
     */
    bool save(const std::string& filepath) const;
    
    /**
     * @brief Load brain state from file
     * 
     * This function loads a previously saved brain state from a checkpoint file,
     * restoring all neuron states, synaptic weights, memory contents, and
     * configuration parameters.
     * 
     * @param filepath Path to checkpoint file
     * @return true if load succeeds, false otherwise
     */
    bool load(const std::string& filepath);
    
    // ===== REGION MANAGEMENT =====
    
    /**
     * @brief Add a new region
     * 
     * @param name Optional name for the region
     * @return Region ID of the newly created region
     */
    RegionId addRegion(const std::string& name = "");
    
    /**
     * @brief Get region by ID
     * 
     * @param id Region ID
     * @return Pointer to region, or nullptr if not found
     */
    NeuralRegion* getRegion(RegionId id);
    
    /**
     * @brief Get region by ID (const version)
     * 
     * @param id Region ID
     * @return Const pointer to region, or nullptr if not found
     */
    const NeuralRegion* getRegion(RegionId id) const;
    
    /**
     * @brief Get number of regions
     * 
     * @return Number of regions in the brain
     */
    size_t getRegionCount() const;
    
    /**
     * @brief Get all region IDs
     * 
     * @return Vector of all region IDs
     */
    std::vector<RegionId> getRegionIds() const;
    
    /**
     * @brief Get all regions
     * 
     * @return Const reference to vector of regions
     */
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    /**
     * @brief Add inter-region connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     * @param weight Connection weight
     * @param delay Connection delay in steps
     */
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    /**
     * @brief Remove inter-region connection
     * 
     * @param source Source region ID
     * @param target Target region ID
     */
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // ===== GLOBAL STATISTICS =====
    
    /**
     * @brief Get total neuron count
     * 
     * @return Total number of neurons in all regions
     */
    size_t getTotalNeuronCount() const;
    
    /**
     * @brief Get total synapse count
     * 
     * @return Total number of synapses in all regions
     */
    size_t getTotalSynapseCount() const;
    
    /**
     * @brief Get active neuron count
     * 
     * @return Number of neurons currently firing or highly active
     */
    size_t getActiveNeuronCount() const;
    
    /**
     * @brief Get firing neuron count
     * 
     * @return Number of neurons currently in refractory state
     */
    size_t getFiringNeuronCount() const;
    
    /**
     * @brief Get average firing rate
     * 
     * @return Average firing rate across all neurons (spikes per second)
     */
    float getAverageFiringRate() const;
    
    // ===== MEMORY SYSTEMS =====
    
    /**
     * @brief Get working memory system
     * 
     * @return Pointer to working memory system, or nullptr if not initialized
     */
    NeuralWorkingMemory* getWorkingMemory();
    
    /**
     * @brief Get episodic memory system
     * 
     * @return Pointer to episodic memory system, or nullptr if not initialized
     */
    NeuralEpisodicMemory* getEpisodicMemory();
    
    /**
     * @brief Get associative memory system
     * 
     * @return Pointer to associative memory system, or nullptr if not initialized
     */
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ===== PREDICTION SYSTEM =====
    
    /**
     * @brief Get prediction system
     * 
     * @return Pointer to prediction system, or nullptr if not initialized
     */
    PredictionSystem* getPredictionSystem();
    
    // ===== COGNITION SYSTEMS =====
    
    /**
     * @brief Get neural planner
     * 
     * @return Pointer to neural planner, or nullptr if not initialized
     */
    NeuralPlanner* getPlanner();
    
    /**
     * @brief Get concept formation system
     * 
     * @return Pointer to concept formation system, or nullptr if not initialized
     */
    ConceptFormation* getConceptFormation();
    
    /**
     * @brief Get attentional selection system
     * 
     * @return Pointer to attentional selection system, or nullptr if not initialized
     */
    AttentionalSelection* getAttention();
    
    // ===== DEVELOPMENT SYSTEM =====
    
    /**
     * @brief Get development system
     * 
     * @return Pointer to development system, or nullptr if not initialized
     */
    DevelopmentSystem* getDevelopmentSystem();
    
    /**
     * @brief Get developmental stage
     * 
     * @return Current developmental stage
     */
    DevelopmentalStage getDevelopmentalStage() const;
    
    /**
     * @brief Set developmental stage
     * 
     * @param stage Developmental stage to set
     */
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ===== NEUROMODULATION SYSTEMS =====
    
    /**
     * @brief Get dopamine system
     * 
     * @return Pointer to dopamine system, or nullptr if not initialized
     */
    Dopamine* getDopamine();
    
    /**
     * @brief Get curiosity system
     * 
     * @return Pointer to curiosity system, or nullptr if not initialized
     */
    Curiosity* getCuriosity();
    
    /**
     * @brief Get novelty detection system
     * 
     * @return Pointer to novelty system, or nullptr if not initialized
     */
    Novelty* getNovelty();
    
    /**
     * @brief Get prediction error system
     * 
     * @return Pointer to prediction error system, or nullptr if not initialized
     */
    PredictionError* getPredictionErrorSignal();
    
    // ===== UTILITY SYSTEMS =====
    
    /**
     * @brief Get configuration
     * 
     * @return Const pointer to configuration
     */
    std::shared_ptr<const Config> getConfig() const;
    
    /**
     * @brief Get random generator
     * 
     * @return Pointer to random generator, or nullptr if not initialized
     */
    RandomGenerator* getRandomGenerator();
    
    /**
     * @brief Log brain status
     * 
     * This function logs detailed information about the current brain state,
     * including neuron counts, synapse statistics, memory system status, and
     * overall system health.
     */
    void logStatus() const;
    
private:
    /**
     * @brief PIMPL implementation structure
     * 
     * This structure contains all private implementation details of the Brain class.
     * It includes all member variables, internal system pointers, and implementation
     * specific data structures. This keeps the Brain class header clean and allows
     * for better encapsulation.
     */
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
