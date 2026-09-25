#pragma once

#include "../core/Types/Types.hpp"
#include "NeuralRegion.hpp"
#include "../dynamics/SpikeSystem.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "abstraction/AbstractBrain.hpp"
#include "di/DIContainer.hpp"
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
class PlasticityRuleFactory;
class NeuromodulatorFactory;
class MemorySystemFactory;
class DevelopmentSystemFactory;

// Inter-regional connection (long-range connectivity)
// Represents structural connections between brain regions for cross-regional communication
// Used for implementing long-range pathways and inter-regional coordination
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
    
    // Add comprehensive documentation for structural connections
    // Biological inspiration: Long-range connections in cortical hierarchies
    // Performance impact: O(1) lookup for inter-region communication
    void documentConnection() const {
        // Document connection properties for analysis
    }
};

// Brain: The central coordinator of the neural system
// Implements real spiking neural computation with event-driven dynamics
// and integrated memory, prediction, cognition, and neuromodulation systems
// Uses dependency injection for improved modularity and extensibility

class Brain : public AbstractBrain {
public:
    // Create brain with configuration and dependency injection container
    // This constructor uses dependency injection to provide all necessary components
    // without hard-coding dependencies, enabling testability and flexibility
    explicit Brain(std::shared_ptr<Config> config);
    
    // Create brain with both configuration and DI container for advanced scenarios
    // This allows direct container injection for more complex dependency graphs
    Brain(std::shared_ptr<Config> config, DIContainer& container);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    // Sets up all brain regions, neurons, synapses, and component factories
    // Complexity: O(n) where n is number of regions/neurons
    bool initialize();
    
    // Main simulation step - core computational loop
    // Implements the main neural computation engine using event-driven dynamics
    // Complexity: O(N log N) for efficient spike propagation and plasticity updates
    void step(SimulationStep currentStep);
    
    // Simulation time step with explicit timestamp
    // Provides temporal precision for spike timing-dependent processes
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    // This is the main interface for environmental interaction
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    // Allows precise control over individual neuron activation
    // O(1) using adjacency list lookup for fast neuron access
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    // Useful for implementing global neuromodulatory effects
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    // Provides access to the main spike processing system
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    // Access to different plasticity mechanisms (STDP, Hebbian, Structural)
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    // Provides metrics for monitoring brain state and performance
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    // Translates neural activity into behavioral outputs
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    // Integrates neuromodulation effects into brain dynamics
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    // Applies synaptic plasticity based on spike timing and neuromodulation
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    // Implements developmental plasticity and structural changes
    void develop();
    
    // Reset brain state
    // Clears all neural states and resets to initial conditions
    void reset();
    
    // Save brain state to file (checkpointing)
    // Persistent state management for long-running simulations
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // Restores previously saved brain state for recovery or continuation
    bool load(const std::string& filepath);
    
    // Region management
    // Brain is organized into regions for modular architecture
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    // Enables long-range connections between brain regions
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    // Comprehensive brain state statistics for monitoring and analysis
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
    
    // Plugin system support
    // Enable/disable component plugins dynamically
    void enablePlugin(const std::string& pluginName);
    void disablePlugin(const std::string& pluginName);
    bool hasPlugin(const std::string& pluginName) const;
    
    // Factory system support
    // Direct factory access for component creation
    void setPlasticityRuleFactory(std::shared_ptr<PlasticityRuleFactory> factory);
    void setNeuromodulatorFactory(std::shared_ptr<NeuromodulatorFactory> factory);
    void setMemorySystemFactory(std::shared_ptr<MemorySystemFactory> factory);
    void setDevelopmentSystemFactory(std::shared_ptr<DevelopmentSystemFactory> factory);
    
private:
    struct Impl;
    Impl* pImpl;
};

// Alias for convenience
using Brain = Brain;

} // namespace nlm
