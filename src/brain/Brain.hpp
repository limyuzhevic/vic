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
// This is Phase 6: Integrated Artificial Brain - represents a complete computational model
// of brain function with multiple interacting subsystems

class Brain {
public:
    // Create brain with configuration
    // Initializes all integrated subsystems based on configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    // Creates neural regions, populations, establishes connectivity,
    // and initializes all integrated memory, prediction, cognition, and neuromodulation systems
    bool initialize();
    
    // Main simulation step - Phase 6: Complete brain integration
    // Implements the complete integrated brain simulation loop with:
    // 1. Process pending delayed spikes (deliver synaptic input)
    // 2. Update all neuron membrane potentials (LIF dynamics)
    // 3. Detect spikes and schedule outgoing spike events
    // 4. Update working memory (maintenance and competition)
    // 5. Apply neuromodulation effects on neural excitability
    // 6. Apply plasticity rules (STDP, Hebbian)
    // 7. Update episodic memory with current experience
    // 8. Update prediction system
    // 9. Update attention system
    // 10. Update concept formation
    // 11. Apply structural plasticity (synaptogenesis, pruning)
    // 12. Replay important memories (during rest or periodically)
    // 13. Apply development effects
    // 14. Collect statistics
    void step(SimulationStep currentStep);
    
    // Simulation time step with explicit timestamp
    // Allows for precise timing in event-driven neural simulation
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    // Enables interaction between brain and external environment
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    // Useful for testing and external control of neural activity
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    // Used for coordinated stimulation of neuron populations
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    // Provides access to spike timing and event management
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    // Provides access to synaptic modification rules
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    // Neural network statistics and metrics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    // Simple action selection based on motor neuron population activity
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    // Modulates neural plasticity based on reward or other signals
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    // Maintains compatibility with older code while being integrated into main loop
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    // Models brain development and maturation processes
    void develop();
    
    // Reset brain state
    // Clears all neural states and resets to initial configuration
    void reset();
    
    // Save brain state to file (checkpointing)
    // Persists neural state for later resumption
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // Restores previously saved neural state
    bool load(const std::string& filepath);
    
    // Region management
    // Creates neural regions (subsystems within the brain)
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    // Returns the complete collection of neural regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    // Manages long-range connections between neural regions
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    // Aggregate statistics across the entire brain
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    // Stores current neural activity patterns for immediate processing
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    // Stores recorded experiences for later recall and replay
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    // Stores learned associations between different patterns
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    // Generates predictions about sensory inputs and computes prediction errors
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    // Plans actions based on current neural state and goals
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    // Discovers and forms abstract concepts from neural activity patterns
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    // Selects important neural activity patterns for further processing
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    // Development system for brain maturation
    DevelopmentSystem* getDevelopmentSystem();
    
    // Get current developmental stage
    // Models brain development stages from initial to mature
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Set developmental stage
    // Allows control over brain development phase
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    // Mediates reward prediction and reinforcement learning
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    // Motivates exploration of novel environments and patterns
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    // Detects novel or unexpected events
    Novelty* getNovelty();
    
    // Prediction error signal
    // Computes difference between expected and actual sensory input
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    // Provides access to the brain's configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    // Provides access to random number generation for stochastic processes
    RandomGenerator* getRandomGenerator();
    
    // Logging
    // Logs detailed brain status and statistics
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
