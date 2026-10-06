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

// Enhanced error handling includes
#include "../core/ErrorHandling/NLMExceptionHierarchy.hpp"
#include "EnhancedErrorHandling.hpp"
#include "EnhancedErrorHandlingIntegration.hpp"

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

// =============================================================================
// Brain: The central coordinator of the neural system
// =============================================================================
// 
// This class implements the core artificial brain architecture for NLM.
// Key features:
// - Real spiking neural computation with event-driven dynamics
// - Integrated memory, prediction, cognition, and neuromodulation systems
// - Leaky Integrate-and-Fire (LIF) neuron dynamics
// - Multiple plasticity rules (STDP, Hebbian, structural)
// - Developmental stages and progression
// - Checkpoint/save and load capabilities
// 
// The brain operates through a continuous loop:
// 1. Receive sensory input from environment
// 2. Process neural dynamics and spikes
// 3. Apply plasticity rules and learning
// 4. Generate motor actions
// 5. Receive reward/novelty feedback
// 6. Develop and mature over time
// 
// The architecture supports modular design with pluggable components
// for different neural dynamics, plasticity rules, and cognitive functions.
// =============================================================================

class Brain {
public:
    // =======================================================================
    // Constructor and Destructor
    // =======================================================================
    
    // Create brain with configuration
    // 
    // Parameters:
    //   config: Shared pointer to configuration object containing
    //           brain parameters (neuron count, connection probability,
    //           plasticity settings, etc.)
    // 
    // Returns:
    //   Brain object initialized with the given configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    // Destructor
    ~Brain();
    
    // =======================================================================
    // Copy and Move Semantics
    // =======================================================================
    
    // Disable copying to prevent multiple ownership of shared resources
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    
    // Enable move operations for efficient resource management
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // =======================================================================
    // Brain Initialization
    // =======================================================================
    
    // Initialize brain with configuration
    // 
    // This method sets up the internal data structures and neural networks
    // based on the configuration. Must be called before any simulation steps.
    // 
    // Parameters:
    //   None
    // 
    // Returns:
    //   true if initialization succeeded, false otherwise
    bool initialize();
    
    // =======================================================================
    // Main Simulation Loop
    // =======================================================================
    
    // Perform one simulation step
    // 
    // Parameters:
    //   currentStep: Current simulation step number (for logging and timing)
    // 
    // Notes:
    //   - Advances the internal simulation clock
    //   - Processes neural events and spike propagation
    //   - Applies plasticity rules
    //   - Updates developmental state if development is enabled
    void step(SimulationStep currentStep);
    
    // Perform one simulation step with explicit timing
    // 
    // Parameters:
    //   currentStep: Current simulation step number
    //   currentTime: Current simulation timestamp (seconds)
    // 
    // Notes:
    //   - Same as step() but with explicit time parameter
    //   - Useful for time-synchronized simulations
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // =======================================================================
    // Sensory Processing
    // =======================================================================
    
    // Receive sensory input from environment
    // 
    // This method injects current into sensory neurons based on the provided
    // sensory input. This allows the brain to process environmental information.
    // 
    // Parameters:
    //   input: Sensory input containing data from environment (vision, audio,
    //         internal signals, etc.)
    // 
    // Notes:
    //   - Converts environmental data to neural current injection
    //   - For vision: converts pixel intensity to visual neuron current
    //   - For internal signals: converts energy/health to internal neuron current
    //   - Updates novelty and prediction error signals
    void receiveSensoryInput(const class SensoryInput& input);
    
    // =======================================================================
    // Neural Control
    // =======================================================================
    
    // Inject current directly into a specific neuron
    // 
    // Parameters:
    //   neuron: Target neuron ID
    //   current: Current to inject (in pA)
    // 
    // Notes:
    //   - Useful for direct neural manipulation in experiments
    //   - Current affects membrane potential immediately
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    // 
    // Parameters:
    //   type: Neuron type (excitatory, inhibitory, sensory, motor, etc.)
    //   current: Current to inject (in pA)
    // 
    // Notes:
    //   - Batch operation for stimulating neural populations
    //   - Commonly used for sensory input stimulation
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // =======================================================================
    // System Access
    // =======================================================================
    
    // Access spike processing system
    // 
    // Returns:
    //   Pointer to spike system (event-driven spike processing)
    SpikeSystem* getSpikeSystem();
    
    // Const version for read-only access
    const SpikeSystem* getSpikeSystem() const;
    
    // Access spike processing system (const version)
    // 
    // Returns:
    //   Const pointer to spike system
    const SpikeSystem* getSpikeSystem() const;
    
    // =======================================================================
    // Plasticity System Access
    // =======================================================================
    
    // Access Spike-Timing-Dependent Plasticity (STDP) system
    // 
    // Returns:
    //   Pointer to STDP plasticity rule implementation
    STDP* getSTDP();
    
    // Access Hebbian plasticity system
    // 
    // Returns:
    //   Pointer to Hebbian plasticity rule implementation
    Hebbian* getHebbian();
    
    // Access structural plasticity system
    // 
    // Returns:
    //   Pointer to structural plasticity rule implementation
    StructuralPlasticity* getStructuralPlasticity();
    
    // =======================================================================
    // Statistics and Metrics
    // =======================================================================
    
    // Get excitation/inhibition balance ratio
    // 
    // Returns:
    //   Ratio of excitatory to inhibitory activity (0.0 to 1.0+)
    float getExcitationInhibitionRatio() const;
    
    // Get total spike count across all neurons
    // 
    // Returns:
    //   Total number of spikes generated during simulation
    size_t getTotalSpikeCount() const;
    
    // Get count of pending spike events
    // 
    // Returns:
    //   Number of spikes scheduled but not yet processed
    size_t getPendingSpikeEventCount() const;
    
    // =======================================================================
    // Action Production
    // =======================================================================
    
    // Produce motor/action output based on motor neuron activity
    // 
    // Returns:
    //   Unique pointer to Action object containing motor command
    //   and associated parameters
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    // 
    // This method applies neuromodulator signals (dopamine, serotonin, etc.)
    // that affect neural excitability, plasticity, and learning.
    // 
    // Parameters:
    //   signal: Neuromodulator signal containing type and intensity
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    // 
    // This method applies all enabled plasticity rules to synapses
    // based on recent neural activity patterns.
    // 
    // Notes:
    //   - Called automatically during each simulation step
    //   - Can be called manually for synchronous plasticity updates
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    // 
    // This method updates the brain's developmental stage based on
    // accumulated simulation time and applies age-related changes.
    // 
    // Notes:
    //   - Called automatically during each simulation step
    //   - Modifies plasticity rates, neural properties, and connectivity
    void develop();
    
    // =======================================================================
    // State Management
    // =======================================================================
    
    // Reset brain state
    // 
    // This method resets all neural state variables to initial conditions,
    // preserving configuration but clearing synaptic weights, membrane
    // potentials, and developmental state.
    void reset();
    
    // Save brain state to file (checkpointing)
    // 
    // Serializes the complete brain state (neurons, synapses, weights,
    // development, memory contents) to a file for later restoration.
    // 
    // Parameters:
    //   filepath: Path to save checkpoint file
    // 
    // Returns:
    //   true if save succeeded, false otherwise
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // 
    // Restores brain state from a checkpoint file, overwriting current
    // state with saved neural configuration, weights, and history.
    // 
    // Parameters:
    //   filepath: Path to checkpoint file to load
    // 
    // Returns:
    //   true if load succeeded, false otherwise
    bool load(const std::string& filepath);
    
    // =======================================================================
    // Region Management
    // =======================================================================
    
    // Add a new neural region to the brain
    // 
    // Parameters:
    //   name: Optional name for the region (for identification)
    // 
    // Returns:
    //   RegionId of the newly created region
    RegionId addRegion(const std::string& name = "");
    
    // Get neural region by ID
    // 
    // Parameters:
    //   id: Region identifier
    // 
    // Returns:
    //   Pointer to NeuralRegion if found, nullptr otherwise
    NeuralRegion* getRegion(RegionId id);
    
    // Const version for read-only access
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Get number of regions in brain
    // 
    // Returns:
    //   Number of neural regions
    size_t getRegionCount() const;
    
    // Get all region IDs
    // 
    // Returns:
    //   Vector of all region identifiers
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    // 
    // Returns:
    //   Const reference to vector of all neural regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // =======================================================================
    // Inter-Regional Connections
    // =======================================================================
    
    // Add connection between regions (long-range connectivity)
    // 
    // Parameters:
    //   source: Source region ID
    //   target: Target region ID  
    //   weight: Initial synaptic weight (default: 0.0f)
    //   delay: Transmission delay in simulation steps (default: 1)
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    // Remove connection between regions
    // 
    // Parameters:
    //   source: Source region ID
    //   target: Target region ID
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // =======================================================================
    // Global Statistics
    // =======================================================================
    
    // Get total neuron count across all regions
    // 
    // Returns:
    //   Total number of neurons in brain
    size_t getTotalNeuronCount() const;
    
    // Get total synapse count across all regions
    // 
    // Returns:
    //   Total number of synapses in brain
    size_t getTotalSynapseCount() const;
    
    // Get count of neurons currently active
    // 
    // Returns:
    //   Number of neurons with non-zero membrane potential
    size_t getActiveNeuronCount() const;
    
    // Get count of neurons currently firing (spiking)
    // 
    // Returns:
    //   Number of neurons in active firing state
    size_t getFiringNeuronCount() const;
    
    // Get average firing rate across all neurons
    // 
    // Returns:
    //   Average firing rate in Hz (spikes per second)
    float getAverageFiringRate() const;
    
    // =======================================================================
    // MEMORY SYSTEMS
    // =======================================================================
    
    // Get working memory system
    // 
    // Returns:
    //   Pointer to working memory system for transient storage
    NeuralWorkingMemory* getWorkingMemory();
    
    // Get episodic memory system
    // 
    // Returns:
    //   Pointer to episodic memory system for experience storage
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Get associative memory system
    // 
    // Returns:
    //   Pointer to associative memory system for pattern associations
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Get prediction system
    // 
    // Returns:
    //   Pointer to prediction system for temporal sequence learning
    PredictionSystem* getPredictionSystem();
    
    // Get cognitive planner system
    // 
    // Returns:
    //   Pointer to neural planner for action selection
    NeuralPlanner* getPlanner();
    
    // Get concept formation system
    // 
    // Returns:
    //   Pointer to concept formation for pattern discovery
    ConceptFormation* getConceptFormation();
    
    // Get attentional selection system
    // 
    // Returns:
    //   Pointer to attentional selection for competitive focus
    AttentionalSelection* getAttention();
    
    // Get development system
    // 
    // Returns:
    //   Pointer to development system for maturation
    DevelopmentSystem* getDevelopmentSystem();
    
    // Get dopamine neuromodulation system
    // 
    // Returns:
    //   Pointer to dopamine system for reward processing
    Dopamine* getDopamine();
    
    // Get curiosity neuromodulation system
    // 
    // Returns:
    //   Pointer to curiosity system for exploration motivation
    Curiosity* getCuriosity();
    
    // Get novelty detection system
    // 
    // Returns:
    //   Pointer to novelty system for change detection
    Novelty* getNovelty();
    
    // Get prediction error system
    // 
    // Returns:
    //   Pointer to prediction error system for learning signals
    PredictionError* getPredictionError();
    
    // =======================================================================
    // Developmental State
    // =======================================================================
    
    // Get current developmental stage
    // 
    // Returns:
    //   Current developmental stage (Initial, CriticalPeriod, etc.)
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Set developmental stage
    // 
    // Parameters:
    //   stage: Target developmental stage
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // =======================================================================
    // Configuration
    // =======================================================================
    
    // Get brain configuration
    // 
    // Returns:
    //   Const reference to configuration object
    const Config& getConfig() const;
    
    // =======================================================================
    // Enhanced Error Handling
    // =======================================================================
    
    // Initialize enhanced error handling for this brain instance
    void initializeEnhancedErrorHandling();
    
    // Get enhanced error handling logger
    EnhancedLogger& getEnhancedLogger() const;
    
    // Get enhanced exception handler
    EnhancedExceptionHandler& getEnhancedExceptionHandler() const;
    
    // =======================================================================
    // Comprehensive Parameter Validation
    // =======================================================================
    
    // Validate brain initialization parameters
    bool validateInitializationParameters() const;
    
    // Validate simulation step parameters
    bool validateStepParameters(SimulationStep currentStep, Timestamp currentTime) const;
    
    // Validate brain state consistency
    bool validateBrainStateConsistency() const;
    
    // Validate configuration
    bool validateConfiguration() const;
    
    // =======================================================================
    // Enhanced Brain Management
    // =======================================================================
    
    // Initialize brain with enhanced error handling
    bool initialize();
    
    // Perform one simulation step with enhanced error handling
    void step(SimulationStep currentStep);
    
    // Perform one simulation step with explicit timing
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // =======================================================================
    // Error Handling Helper Methods
    // =======================================================================
    
    // Handle brain initialization errors
    void handleBrainInitializationError(const EnhancedException& e);
    
    // Handle simulation step errors
    void handleSimulationStepError(const EnhancedException& e);
    
    // Handle memory management errors
    void handleMemoryManagementError(const EnhancedException& e);
    
    // Handle serialization errors
    void handleSerializationError(const EnhancedException& e);
    
    // Handle configuration errors
    void handleConfigurationError(const EnhancedException& e);
    
protected:
    // Internal implementation details (pImpl pattern)
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Internal constructor for implementation
    explicit Brain(Impl* impl);
    
    // Helper methods for internal use
    static std::string trim(const std::string& str);
    static std::string toLower(const std::string& str);
    
private:
    // Additional private implementation details
    class RandomGenerator* getRandomGenerator() const;
    class SimulationClock* getClock() const;
    class Logger* getLogger() const;
};

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
