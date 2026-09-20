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
    /**
     * @brief Create brain with configuration
     * 
     * Initializes the brain system with the given configuration. The brain creates
     * a BrainCore instance internally to handle the actual neural computation.
     * This is the main entry point for creating a brain with specific parameters.
     * 
     * @param config Shared pointer to the configuration object containing brain
     *               parameters such as neuron count, region count, and system settings
     */
    explicit Brain(std::shared_ptr<Config> config);
    
    /**
     * @brief Destroy the Brain object
     * 
     * Cleans up the brain system and all its internal resources. This includes
     * destroying the BrainCore instance and releasing any allocated memory.
     */
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    /**
     * @brief Initializes the brain with configuration settings
     * 
     * This method initializes the brain core with the given configuration.
     * It delegates the actual initialization to BrainCore and performs
     * additional logging and statistics collection after successful initialization.
     * 
     * @return bool true if initialization succeeds, false otherwise
     */
    bool initialize();
    
    // Main simulation step
    /**
     * @brief Executes one simulation step with current step number
     * 
     * Advances the neural simulation by one time step using the current step number
     * to calculate the current time. This is a convenience overload that automatically
     * uses the brain's timestep from BrainCore.
     * 
     * @param currentStep The current simulation step number (discrete time)
     */
    void step(SimulationStep currentStep);
    /**
     * @brief Executes one simulation step with explicit time
     * 
     * Advances the neural simulation by one complete time step, including all neural
     * computations, plasticity updates, memory updates, and neuromodulation. This
     * is the core method that drives the entire brain simulation.
     * 
     * @param currentStep The current simulation step number (discrete time)
     * @param currentTime The absolute timestamp for this simulation step
     */
    void step(SimulationStep currentStep, Timestamp currentTime);
    /**
     * @brief Receives and processes sensory input from the environment
     * 
     * Injects sensory input current into the brain's sensory neurons based on the
     * provided SensoryInput object. This input represents external stimuli that
     * will be processed by the neural network and influence behavior.
     * 
     * @param input The SensoryInput object containing environmental stimuli
     */
    void receiveSensoryInput(const class SensoryInput& input);
    /**
     * @brief Injects current directly into a specific neuron
     * 
     * Directly applies membrane potential current to a specific neuron, bypassing
     * the usual synaptic input pathway. This is useful for testing or for providing
     * external control signals to specific neurons.
     * 
     * @param neuron Neuron ID of the target neuron
     * @param current Membrane potential current to inject (positive for excitation,
     *                   negative for inhibition)
     */
    void injectCurrent(NeuronId neuron, MembranePotential current);
    /**
     * @brief Injects current into all neurons of a specific type
     * 
     * Applies the specified current to all neurons of the given type (Sensory,
     * Internal, or Motor). This is useful for bulk control of neural populations.
     * 
     * @param type Neuron type to target
     * @param current Membrane potential current to inject to all neurons of this type
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
    
    // Connect to BrainCore for basic operations
    void setBrainCore(BrainCore* core);
    
private:
    BrainCore* brainCore;
};

} // namespace nlm
