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
// This class uses the Pimpl idiom to hide implementation details and provide
// a clean API while maintaining high performance through direct memory access
// patterns and efficient data structures.
// 
// The brain supports multiple interconnected neural regions, each containing
// populations of neurons organized by type (Sensory, Internal, Motor). This
// modular architecture enables complex cognitive behaviors through distributed
// processing and specialized neural circuits.
// 
// Key features:
// - Real-time event-driven spiking neural network simulation
// - Integrated memory systems (working, episodic, associative)
// - Prediction and planning systems for goal-directed behavior
// - Neuromodulation (dopamine, curiosity, novelty) for adaptive learning
// - Developmental plasticity for lifelong learning capabilities
// - Checkpointing and state persistence for reproducible experiments

class Brain {
public:
    // Create brain with configuration
    // 
    // Args:
    //   config: Shared pointer to configuration object containing simulation parameters
    // 
    // Returns:
    //   Brain instance with initialized internal structures
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving for performance and memory safety
    // 
    // Copy operations are disabled because the Pimpl pattern requires
    // deep copy semantics. Move operations enable efficient transfer of ownership.
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    // 
    // Performs complete initialization of all neural components, systems, and
    // computational resources. This must be called before any simulation steps
    // or neural processing can occur.
    // 
    // Returns:
    //   true if initialization succeeds, false if configuration is invalid or
    //   initialization fails for any reason
    bool initialize();
    
    // Main simulation step
    // 
    // Advances the neural simulation by one discrete time step, processing
    // all neural dynamics, plasticity rules, memory updates, and cognitive
    // processes. This is the core computational engine of the brain.
    // 
    // Args:
    //   currentStep: Current simulation step number for time tracking
    void step(SimulationStep currentStep);
    
    // Simulation time step (overload with explicit timestamp)
    // 
    // Provides the same functionality as step(SimulationStep) but includes
    // the actual time value for systems that need precise timing information.
    // 
    // Args:
    //   currentStep: Current simulation step number
    //   currentTime: Absolute simulation time in seconds
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // 
    // Injects current into sensory neurons based on input pattern, enabling
    // the brain to respond to environmental stimuli. This is the primary
    // interface for perception and environmental interaction.
    // 
    // Args:
    //   input: Sensory input data containing environmental observations
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    // 
    // Provides direct control over individual neurons for experimental
    // manipulation and targeted stimulation of specific neural pathways.
    // 
    // Args:
    //   neuron: ID of the target neuron
    //   current: Amount of current to inject (in nanoAmperes)
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    // 
    // Provides efficient batch control over neuron populations, useful for
    // global modulation and system-wide state changes.
    // 
    // Args:
    //   type: Type of neurons to target (Sensory, Internal, Motor)
    //   current: Amount of current to inject (in nanoAmperes)
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    // 
    // Provides direct access to the spike system for event-driven spike
    // management and timing control. This is essential for maintaining
    // accurate neural timing and synchronous processing.
    // 
    // Returns:
    //   Pointer to the internal spike system (must be managed carefully)
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    // 
    // Provides access to synaptic plasticity systems that implement learning
    // rules including STDP (Spike-Timing-Dependent Plasticity) and Hebbian
    // learning. These are fundamental for adaptive behavior and memory formation.
    // 
    // Returns:
    //   Pointers to individual plasticity system components
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    // 
    // Global performance and state statistics for monitoring and analysis.
    // These provide insights into brain activity, network dynamics, and
    // learning progress.
    // 
    // Returns:
    //   Various statistical measures about brain state and activity
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    // 
    // Implements simple action selection based on motor neuron population
    // activity, enabling goal-directed behavior and environmental interaction.
    // 
    // Returns:
    //   Unique pointer to action that can be executed by the agent
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    // 
    // Integrates neuromodulatory signals (dopamine, acetylcholine, etc.) that
    // modulate neural excitability, plasticity, and learning rates. This is
    // essential for adaptive behavior and reward-based learning.
    // 
    // Args:
    //   signal: Neuromodulator signal containing level and type information
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    // 
    // This method is kept for API compatibility but plasticity is now applied
    // continuously during each simulation step. It provides an opportunity
    // for custom plasticity updates or specialized learning algorithms.
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    // 
    // Implements developmental stages that affect plasticity rates and
    // neural connectivity over the lifetime of the system. This enables
    // lifelong learning and adaptive development.
    void develop();
    
    // Reset brain state
    // 
    // Returns the brain to its initial state, clearing all neural activity,
    // memories, and developmental progress. Useful for starting fresh
    // simulations or experimental trials.
    void reset();
    
    // Save brain state to file (checkpointing)
    // 
    // Persists the complete brain state to disk for checkpointing, resuming,
    // or long-term storage. This enables reproducible experiments and
    // recovery from failures.
    // 
    // Args:
    //   filepath: Path to the checkpoint file
    // 
    // Returns:
    //   true if save succeeds, false otherwise
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // 
    // Restores a previously saved brain state from disk, enabling recovery
    // from checkpoints or loading experimental conditions.
    // 
    // Args:
    //   filepath: Path to the checkpoint file
    // 
    // Returns:
    //   true if load succeeds, false otherwise
    bool load(const std::string& filepath);
    
    // Region management
    // 
    // Manages the collection of neural regions that constitute the brain.
    // Regions can contain specialized neural circuits for different functions.
    // 
    // Args:
    //   name: Optional name for the region (defaults to auto-generated)
    // 
    // Returns:
    //   Unique ID for the created region
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    // 
    // Provides access to all neural regions in the brain, enabling analysis
    // and manipulation of the complete neural architecture.
    // 
    // Returns:
    //   Constant reference to the regions vector
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    // 
    // Manages long-range connections between neural regions, enabling complex
    // cognitive functions that require coordination across distributed neural
    // circuits.
    // 
    // Args:
    //   source: Source region ID
    //   target: Target region ID
    //   weight: Synaptic weight for the connection
    //   delay: Transmission delay for the connection (in simulation steps)
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    // 
    // Comprehensive statistics about brain state and activity, providing
    // insights into neural dynamics, connectivity, and computational performance.
    // 
    // Returns:
    //   Various measures of brain state and activity levels
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    // 
    // Provides access to working memory that maintains transient neural
    // representations for immediate cognitive processing and decision-making.
    // 
    // Returns:
    //   Pointer to working memory system
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    // 
    // Provides access to episodic memory that stores actual experiences
    // as episodic memory items for later recall and integration.
    // 
    // Returns:
    //   Pointer to episodic memory system
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    // 
    // Provides access to associative memory that forms associations between
    // patterns and concepts through experience and learning.
    // 
    // Returns:
    //   Pointer to associative memory system
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    // 
    // Provides access to prediction system that implements predictive coding
    // and error-based learning for anticipatory behavior and motor control.
    // 
    // Returns:
    //   Pointer to prediction system
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    // 
    // Provides access to neural planner that implements goal-directed planning
    // and action selection based on current state and learned expectations.
    // 
    // Returns:
    //   Pointer to neural planner
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    // 
    // Provides access to concept formation system that abstracts and generalizes
    // from sensory input to form stable concepts and categories.
    // 
    // Returns:
    //   Pointer to concept formation system
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    // 
    // Provides access to attentional selection system that implements competition
    // and focus mechanisms for selective information processing.
    // 
    // Returns:
    //   Pointer to attentional selection system
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem();
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    // 
    // Provides access to dopamine system that implements reward prediction
    // error and reinforcement learning for adaptive behavior.
    // 
    // Returns:
    //   Pointer to dopamine system
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    // 
    // Provides access to curiosity system that implements exploration motivation
    // and intrinsic reward for discovery and novelty seeking.
    // 
    // Returns:
    //   Pointer to curiosity system
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    // 
    // Provides access to novelty detection system that identifies novel
    // information and stimuli for learning and adaptation.
    // 
    // Returns:
    //   Pointer to novelty detection system
    Novelty* getNovelty();
    
    // Prediction error signal
    // 
    // Provides access to prediction error system that computes the difference
    // between predicted and actual sensory input for learning.
    // 
    // Returns:
    //   Pointer to prediction error signal system
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    // 
    // Provides access to the configuration object for inspection and modification.
    // 
    // Returns:
    //   Constant shared pointer to configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    // 
    // Provides access to the random number generator used for initialization
    // and stochastic processes. This enables reproducible experiments and
    // stochastic neural dynamics.
    // 
    // Returns:
    //   Pointer to random number generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    // 
    // Provides diagnostic information about brain status, performance, and
    // internal state. Useful for debugging, monitoring, and analysis.
    void logStatus() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
