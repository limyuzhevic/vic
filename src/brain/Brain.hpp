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

class Brain {
public:
    // Create brain with configuration
    /**
     * @brief Constructor that initializes the brain with the given configuration
     * 
     * @param config Shared pointer to the configuration object containing brain parameters
     *               such as neuron count, connection probability, and plasticity rules.
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destructor that cleans up brain resources and performs finalization
     */
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    /**
     * @brief Initialize the brain with current configuration
     * 
     * Creates neurons, regions, and synapses based on configuration parameters.
     * Must be called before the brain can be used for simulation.
     * 
     * @return true if initialization succeeded, false if configuration was invalid
     */
    bool initialize();
    
    // Main simulation step
    /**
     * @brief Perform a simulation step
     * 
     * Executes one time step of neural simulation. Advances all neuron states,
     * processes spikes, applies plasticity rules, and updates development system.
     * Uses default timestamp equal to currentStep * default_timestep.
     * 
     * @param currentStep The simulation step number (0, 1, 2, ...)
     */
    void step(SimulationStep currentStep);
    
    // Simulation time step
    /**
     * @brief Perform a simulation step with explicit timestamp
     * 
     * Executes one time step of neural simulation with explicit time coordinate.
     * Advances all neuron states, processes spikes, applies plasticity rules,
     * and updates development system using the given timestamp.
     * 
     * @param currentStep The simulation step number (0, 1, 2, ...)
     * @param currentTime The absolute timestamp in milliseconds (or other time units)
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    /**
     * @brief Inject sensory input into the brain
     * 
     * Converts environmental sensory data into neural current injections.
     * The input is processed and injected into appropriate sensory neurons
     * based on the brain's sensory mapping configuration.
     * 
     * @param input Reference to sensory input containing environmental data
     */
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    /**
     * @brief Inject current into a specific neuron
     * 
     * Injects electrical current into an individual neuron, causing its membrane
     * potential to increase. Used for targeted stimulation or external input.
     * 
     * @param neuron The ID of the neuron to inject current into
     * @param current The amount of current to inject (positive values excite)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    /**
     * @brief Inject current into all neurons of a specific type
     * 
     * Injects electrical current into all neurons of the specified type,
     * typically used for processing sensory input or driving network activity.
     * 
     * @param type The neuron type (e.g., Sensory, Excitatory, Inhibitory)
     * @param current The amount of current to inject into each neuron
     */
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    /**
     * @brief Get the excitation/inhibition balance ratio
     * 
     * Returns the ratio of excitatory to inhibitory influence in the network,
     * which affects network stability and dynamics.
     * 
     * @return E/I ratio where values > 1.0 indicate excitation bias,
     *         < 1.0 indicate inhibition bias, = 1.0 indicate balance
     */
    float getExcitationInhibitionRatio() const;
    
    /**
     * @brief Get total spike count across all neurons
     * 
     * @return Total number of spikes generated by the brain since initialization
     */
    size_t getTotalSpikeCount() const;
    
    /**
     * @brief Get count of pending spike events
     * 
     * Returns the number of spikes that have been generated but not yet
     * processed by postsynaptic neurons due to transmission delays.
     * 
     * @return Number of pending spike events in the network
     */
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    /**
     * @brief Generate motor action based on neural activity
     * 
     * Analyzes the current state of motor neurons and produces an appropriate
     * motor command that can be executed in the environment. This is the
     * primary interface for the brain to control external actions.
     * 
     * @return Unique pointer to an action object representing the selected motor command
     */
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    /**
     * @brief Apply neuromodulatory signal to the brain
     * 
     * Applies neuromodulator signals (e.g., dopamine, curiosity) to influence
     * neural processing, plasticity, and behavior. Neuromodulators can enhance
     * learning, modulate attention, or drive exploratory behavior.
     * 
     * @param signal Reference to neuromodulator containing type, strength, and timing
     */
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    /**
     * @brief Update synaptic plasticity based on current state
     * 
     * Applies all configured plasticity rules (STDP, Hebbian, structural) to
     * synaptic weights based on recent neural activity. This function is called
     * automatically during each simulation step.
     */
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    /**
     * @brief Update developmental processes
     * 
     * Advances developmental stage and applies age-dependent changes to
     * brain properties such as plasticity rates, connectivity, and structure.
     * This function is called automatically during each simulation step.
     */
    void develop();
    
    // Reset brain state
    /**
     * @brief Reset brain state to initial conditions
     * 
     * Resets all neuron states, memory contents, and developmental stage
     * to their initial values based on current configuration. Can be used
     * to start a new simulation episode or recover from errors.
     */
    void reset();
    
    // Save brain state to file (checkpointing)
    /**
     * @brief Save brain state to persistent storage
     * 
     * Serializes the complete brain state (neurons, synapses, memories,
     * development stage) to a file for later restoration. Enables saving
     * long-running simulations and resuming from checkpoints.
     * 
     * @param filepath Path to the file where state will be saved
     * @return true if save succeeded, false if file could not be written
     */
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    /**
     * @brief Load brain state from persistent storage
     * 
     * Restores a previously saved brain state from file. Enables resuming
     * simulations from checkpoints and loading expert demonstrations.
     * 
     * @param filepath Path to the file containing saved brain state
     * @return true if load succeeded, false if file could not be read or was invalid
     */
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
