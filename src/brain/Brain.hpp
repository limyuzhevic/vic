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
// This is the core of the NLM architecture, implementing Phase 6's
// integrated artificial brain. The Brain class manages:
// - Neural computation (LIF neurons, spike propagation)
// - Memory systems (working, episodic, associative)
// - Prediction systems (sensory prediction and error)
// - Cognitive systems (planning, concept formation, attention)
// - Neuromodulation (dopamine, curiosity, novelty)
// - Development systems (synaptogenesis, pruning, maturation)
// - Plasticity rules (STDP, Hebbian, structural)
//
// The architecture is event-driven, using spike events to propagate
// information and trigger plasticity updates. This approach closely
// mimics biological neural computation.

class Brain {
public:
    // Create brain with configuration
    // 
    // Args:
    //   config: Configuration object containing neural parameters
    //           (neuron count, region count, plasticity parameters, etc.)
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    // 
    // Creates neural regions, assigns neurons to populations, establishes
    // connectivity, and initializes all integrated systems.
    // 
    // Returns:
    //   true if initialization succeeds, false otherwise
    bool initialize();
    
    // Main simulation step
    // 
    // Advances the neural simulation by one time step.
    // 
    // Args:
    //   currentStep: Current simulation step (integer timestamp)
    void step(SimulationStep currentStep);
    
    // Simulation time step
    // 
    // Advances the neural simulation by one time step with explicit timing.
    // 
    // Args:
    //   currentStep: Current simulation step (integer timestamp)
    //   currentTime: Current simulation time (float, in seconds)
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // 
    // Injects current into sensory neurons based on input pattern.
    // This models how biological neurons receive and encode sensory information.
    // 
    // Args:
    //   input: Semantic sensory input (e.g., vision, audio, internal signals)
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    // 
    // Low-level current injection for testing and direct manipulation.
    // 
    // Args:
    //   neuron: ID of the target neuron
    //   current: Current to inject (in millivolts)
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    // 
    // Bulk current injection for system-wide modulation.
    // 
    // Args:
    //   type: Type of neurons to target (Sensory, Internal, Motor)
    //   current: Current to inject (in millivolts)
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    // 
    // Provides access to the spike processing system that handles
    // event-driven neural computation.
    // 
    // Returns:
    //   Pointer to the SpikeSystem instance (may be null if not initialized)
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    // 
    // Provides access to different plasticity mechanisms.
    // 
    // Returns:
    //   Pointers to STDP, Hebbian, and StructuralPlasticity instances
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    // 
    // Provides global statistics about brain state.
    // 
    // Returns:
    //   Various statistics about network state
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    // 
    // Selects the most active motor neuron group and produces a corresponding
    // action. This implements the agent's motor output.
    // 
    // Returns:
    //   Unique pointer to an Action object
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    // 
    // Applies neuromodulatory signals (dopamine, curiosity, etc.) that
    // influence neural excitability and plasticity.
    // 
    // Args:
    //   signal: Neuromodulation signal to apply
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    // 
    // Applies STDP, Hebbian, and structural plasticity updates based on
    // neural activity and neuromodulatory signals.
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    // 
    // Applies developmental changes to plasticity rates and network structure
    // based on the brain's developmental stage.
    void develop();
    
    // Reset brain state
    // 
    // Resets all neural state variables, memory, and statistics to initial values.
    // Useful for starting a new episode or experiment.
    void reset();
    
    // Save brain state to file (checkpointing)
    // 
    // Saves the complete brain state (neurons, synapses, memory, etc.) to a file
    // for checkpointing and resumption.
    // 
    // Args:
    //   filepath: Path to the output file
    // 
    // Returns:
    //   true if save succeeds, false otherwise
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // 
    // Loads brain state from a file for checkpointing and resumption.
    // 
    // Args:
    //   filepath: Path to the input file
    // 
    // Returns:
    //   true if load succeeds, false otherwise
    bool load(const std::string& filepath);
    
    // Region management
    // 
    // Manages neural regions (functional areas of the brain).
    // 
    // Args:
    //   name: Optional name for the new region
    // 
    // Returns:
    //   ID of the newly created region
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    // 
    // Returns:
    //   Constant reference to the vector of neural regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    // 
    // Manages long-range connections between neural regions.
    // 
    // Args:
    //   source: Source region ID
    //   target: Target region ID
    //   weight: Synaptic weight (default: 0.0f)
    //   delay: Transmission delay in steps (default: 1)
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    // 
    // Returns various global statistics about brain state
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    // 
    // Stores currently active neural patterns for short-term processing.
    // Models working memory in biological systems.
    // 
    // Returns:
    //   Pointer to the working memory system (may be null if not available)
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    // 
    // Stores episodes of experience for later recall and replay.
    // Supports memory consolidation and pattern completion.
    // 
    // Returns:
    //   Pointer to the episodic memory system (may be null if not available)
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    // 
    // Stores associations between patterns and concepts for
    // rapid pattern completion and prediction.
    // 
    // Returns:
    //   Pointer to the associative memory system (may be null if not available)
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    // 
    // Predicts expected sensory input and computes prediction errors
    // for learning and novelty detection.
    // 
    // Returns:
    //   Pointer to the prediction system (may be null if not available)
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    // 
    // Plans sequences of actions based on current state and goals.
    // Implements hierarchical decision making.
    // 
    // Returns:
    //   Pointer to the neural planner (may be null if not available)
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    // 
    // Discovers and forms abstract concepts from neural activity patterns.
    // Enables generalization and abstraction.
    // 
    // Returns:
    //   Pointer to the concept formation system (may be null if not available)
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    // 
    // Selects which neural representations to process based on
    // relevance, novelty, and current goals.
    // 
    // Returns:
    //   Pointer to the attentional selection system (may be null if not available)
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    // 
    // Computes and applies reward prediction errors for reinforcement learning.
    // Dopamine level influences neural excitability and plasticity.
    // 
    // Returns:
    //   Pointer to the dopamine system (may be null if not available)
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    // 
    // Computes curiosity based on prediction errors and novelty to
    // promote exploration and learning.
    // 
    // Returns:
    //   Pointer to the curiosity system (may be null if not available)
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    // 
    // Detects novel events and patterns to guide learning and attention.
    // 
    // Returns:
    //   Pointer to the novelty system (may be null if not available)
    Novelty* getNovelty();
    
    // Prediction error signal
    // 
    // Computes prediction errors for learning and adaptation.
    // 
    // Returns:
    //   Pointer to the prediction error system (may be null if not available)
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    // 
    // Returns:
    //   Shared pointer to the current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    // 
    // Returns:
    //   Pointer to the random number generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    // 
    // Logs brain status and statistics for monitoring and debugging.
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
