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
    // Initializes the brain's internal systems based on provided configuration
    // This must be called before any other operations
    // @param config Configuration object containing brain parameters and settings
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    // Performs initialization of all internal brain systems including:
    // - Neural regions and populations
    // - Plasticity systems (STDP, Hebbian, structural)
    // - Memory systems (working, episodic, semantic, associative)
    // - Prediction and cognition systems
    // - Neuromodulation systems (dopamine, curiosity, novelty)
    // Returns true if initialization succeeds, false on failure
    // @return bool true on success, false if initialization fails
    bool initialize();
    
    // Main simulation step
    // Advances the brain state by one simulation step
    // Integrates neural dynamics, plasticity, memory, and cognition systems
    // Must be called with sequential step numbers for proper time integration
    // @param currentStep Current simulation step number (must be sequential)
    void step(SimulationStep currentStep);
    
    // Simulation time step
    // Advanced the brain state by one simulation step with absolute time
    // Provides both step number and precise time for time-dependent processes
    // @param currentStep Current simulation step number
    // @param currentTime Current absolute time (typically milliseconds)
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
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
    // Generates an action based on the current neural state and accumulated sensory information
    // This method performs the motor planning and decoding process
    // @return std::unique_ptr<Action> Pointer to generated action, nullptr if no action produced
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    // Applies neuromodulatory signals to the brain's systems
    // Neuromodulators like dopamine, acetylcholine, etc. influence plasticity, attention, and learning
    // @param signal Neuromodulator signal to apply
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    // Manually updates synaptic plasticity rules (STDP, Hebbian, structural)
    // This can be called to trigger plasticity independently of the regular simulation step
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    // Manually applies developmental changes to neural systems
    // This can be called to trigger development independently of the regular simulation step
    void develop();
    
    // Reset brain state
    // Resets all neural systems to their initial state
    // Clears spike history, memory, and development state
    void reset();
    
    // Save brain state to file (checkpointing)
    // Saves the entire brain state to a file for persistence or loading
    // Serializes all neural systems, memories, and plasticity state
    // @param filepath Path to file where brain state will be saved
    // @return bool true if save succeeds, false if save fails
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // Restores brain state from a saved file
    // Deserializes all neural systems, memories, and plasticity state
    // @param filepath Path to file containing saved brain state
    // @return bool true if load succeeds, false if load fails
    bool load(const std::string& filepath);
    
    // Region management
    // Adds a new neural region to the brain
    // Regions are containers for populations and handle local connectivity
    // @param name Optional name for the region (for identification)
    // @return RegionId ID of the newly created region
    RegionId addRegion(const std::string& name = "");
    
    // Get a specific region by ID
    // @param id Region ID to retrieve
    // @return NeuralRegion* Pointer to the region (nullptr if not found)
    NeuralRegion* getRegion(RegionId id);
    
    // Get a specific region by ID (const version)
    // @param id Region ID to retrieve
    // @return const NeuralRegion* Pointer to the region (nullptr if not found)
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Get number of regions in the brain
    // @return size_t Number of regions
    size_t getRegionCount() const;
    
    // Get all region IDs
    // @return std::vector<RegionId> Vector of all region IDs
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    // @return const std::vector<std::unique_ptr<NeuralRegion>>& Vector of all regions
    
    // Inter-region connection management
    // Add a connection between two regions (long-range connectivity)
    // These connections enable communication between different brain regions
    // @param source Source region ID
    // @param target Target region ID
    // @param weight Synaptic weight of the connection (default: 0.0f)
    // @param delay Transmission delay in time steps (default: 1)
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    // Remove inter-region connection
    // @param source Source region ID
    // @param target Target region ID
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
    // @return std::shared_ptr<const Config> Pointer to the current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    // @return RandomGenerator* Pointer to the random number generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    // Logs the current brain status including statistics, developmental stage, and key metrics
    // Useful for debugging and monitoring brain development
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
