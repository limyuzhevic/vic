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
// Phase 6: Integrated Artificial Brain - All systems connected and operational
// 
// Key Features:
// - Event-driven neural computation with spike-based signaling
// - Modular architecture with separate components for different brain functions
// - Integrated memory systems (working, episodic, associative)
// - Prediction and cognitive systems (planning, concept formation, attention)
// - Neuromodulation (dopamine, curiosity, novelty detection)
// - Plasticity mechanisms (STDP, Hebbian, structural plasticity)
// - Developmental system with changing plasticity rates
// - Real-world sensory and motor interfaces
// - Checkpoint/save/load functionality
// - Comprehensive error handling and logging

class Brain {
public:
    // Create brain with configuration
    // 
    // Args:
    //   config: Brain configuration (neuron count, regions, connectivity, etc.)
    // 
    // Returns:
    //   Brain instance ready for initialization
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    // 
    // Sets up brain regions, neurons, populations, connectivity, and initializes
    // all integrated systems (memory, cognition, neuromodulation, plasticity)
    // 
    // Returns:
    //   true if initialization successful, false otherwise
    bool initialize();
    
    // Main simulation step
    // 
    // Args:
    //   currentStep: Current simulation step number
    void step(SimulationStep currentStep);
    
    // Simulation time step with explicit time
    // 
    // Args:
    //   currentStep: Current simulation step number
    //   currentTime: Current simulation time (seconds)
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    // 
    // Args:
    //   input: Sensory input data from environment
    // 
    // The input is distributed across sensory neurons and stored in working memory
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Inject current directly into a specific neuron
    // 
    // Args:
    //   neuron: Target neuron ID
    //   current: Current to inject (microsiemens)
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    // 
    // Args:
    //   type: Neuron type (Sensory, Internal, Motor)
    //   current: Current to inject (microsiemens)
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Spike system access
    // 
    // Returns:
    //   Pointer to spike system for external access
    SpikeSystem* getSpikeSystem();
    const SpikeSystem* getSpikeSystem() const;
    
    // Plasticity system access
    // 
    // Returns:
    //   Pointers to plasticity systems (STDP, Hebbian, structural)
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Statistics
    // 
    // Returns:
    //   Excitatory to inhibitory synaptic weight ratio
    float getExcitationInhibitionRatio() const;
    
    // Total spike count since brain creation
    // 
    // Returns:
    //   Total number of spikes across all neurons
    size_t getTotalSpikeCount() const;
    
    // Count of pending spike events
    // 
    // Returns:
    //   Number of scheduled spike events
    size_t getPendingSpikeEventCount() const;
    
    // Produce motor/action output based on motor neuron activity
    // 
    // Returns:
    //   Unique pointer to action object (MoveForward, Wait, etc.)
    std::unique_ptr<class Action> produceAction();
    
    // Apply neuromodulatory signals
    // 
    // Args:
    //   signal: Neuromodulator signal (dopamine, curiosity, etc.)
    // 
    // Modulates neural excitability and plasticity based on neuromodulation level
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Update plasticity rules (called automatically in step)
    // 
    // This method is kept for API compatibility; plasticity is now
    // applied automatically during each simulation step
    void updatePlasticity();
    
    // Apply developmental changes (called automatically in step)
    // 
    // Development updates structural plasticity rates based on
    // current developmental stage
    void develop();
    
    // Reset brain state
    // 
    // Resets all neural states, memory systems, and statistics to initial conditions
    void reset();
    
    // Save brain state to file (checkpointing)
    // 
    // Args:
    //   filepath: Path to save brain state
    // 
    // Returns:
    //   true if save successful, false otherwise
    bool save(const std::string& filepath) const;
    
    // Load brain state from file
    // 
    // Args:
    //   filepath: Path to brain state file
    // 
    // Returns:
    //   true if load successful, false otherwise
    bool load(const std::string& filepath);
    
    // Region management
    // 
    // Args:
    //   name: Region name (empty for default naming)
    // 
    // Returns:
    //   Region ID for the newly created region
    RegionId addRegion(const std::string& name = "");
    
    // Get region by ID
    // 
    // Args:
    //   id: Region ID
    // 
    // Returns:
    //   Pointer to region, or nullptr if not found
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    
    // Get number of regions
    // 
    // Returns:
    //   Number of brain regions
    size_t getRegionCount() const;
    
    // Get all region IDs
    // 
    // Returns:
    //   Vector of all region IDs
    std::vector<RegionId> getRegionIds() const;
    
    // Get all regions
    // 
    // Returns:
    //   Reference to vector of all regions (unique_ptrs)
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connection management
    // 
    // Args:
    //   source: Source region ID
    //   target: Target region ID
    //   weight: Connection weight
    //   delay: Transmission delay (time steps)
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    
    // Remove inter-region connection
    // 
    // Args:
    //   source: Source region ID
    //   target: Target region ID
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    // 
    // Returns:
    //   Total number of neurons in brain
    size_t getTotalNeuronCount() const;
    
    // Total number of synapses in brain
    // 
    // Returns:
    //   Number of synapses across all regions
    size_t getTotalSynapseCount() const;
    
    // Number of active neurons (above threshold)
    // 
    // Returns:
    //   Count of neurons currently active
    size_t getActiveNeuronCount() const;
    
    // Number of firing neurons in current step
    // 
    // Returns:
    //   Count of neurons that spiked this step
    size_t getFiringNeuronCount() const;
    
    // Average firing rate across all neurons
    // 
    // Returns:
    //   Average firing rate (spikes per second)
    float getAverageFiringRate() const;
    
    // ========== MEMORY SYSTEMS ==========
    
    // Working memory - transient active information
    // 
    // Returns:
    //   Pointer to working memory system
    NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    // 
    // Returns:
    //   Pointer to episodic memory system
    NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    // 
    // Returns:
    //   Pointer to associative memory system
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    // 
    // Returns:
    //   Pointer to prediction system
    PredictionSystem* getPredictionSystem();
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    // 
    // Returns:
    //   Pointer to neural planner
    NeuralPlanner* getPlanner();
    
    // Concept formation for pattern discovery
    // 
    // Returns:
    //   Pointer to concept formation system
    ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    // 
    // Returns:
    //   Pointer to attentional selection system
    AttentionalSelection* getAttention();
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    // Development system
    // 
    // Returns:
    //   Pointer to development system
    DevelopmentSystem* getDevelopmentSystem();
    
    // Current developmental stage
    // 
    // Returns:
    //   Current developmental stage (Initial, CriticalPeriod, etc.)
    DevelopmentalStage getDevelopmentalStage() const;
    
    // Set developmental stage
    // 
    // Args:
    //   stage: New developmental stage
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    // 
    // Returns:
    //   Pointer to dopamine neuromodulator
    Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    // 
    // Returns:
    //   Pointer to curiosity neuromodulator
    Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    // 
    // Returns:
    //   Pointer to novelty detector
    Novelty* getNovelty();
    
    // Prediction error signal
    // 
    // Returns:
    //   Pointer to prediction error signal
    PredictionError* getPredictionErrorSignal();
    
    // Get current configuration
    // 
    // Returns:
    //   Shared pointer to configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    // 
    // Returns:
    //   Pointer to random number generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    // 
    // Logs current brain state, statistics, and system health
    void logStatus() const;

private:
    // Private helper methods for initialization and processing
    
    // Initialize brain regions and neurons
    bool initializeRegions(size_t neuronCount, size_t regionCount, float connectionProbability);
    
    // Initialize neural connectivity
    void initializeConnectivity(size_t regionCount, float connectionProbability);
    
    // Initialize memory systems
    void initializeMemorySystems(size_t neuronCount);
    
    // Initialize cognition systems
    void initializeCognitionSystems();
    
    // Initialize neuromodulation systems
    void initializeNeuromodulation();
    
    // Initialize plasticity systems
    void initializePlasticitySystems();
    
    // Setup spike event handlers
    void setupSpikeHandlers();
    
    // Setup checkpoint system
    void setupCheckpointSystem();
    
    // Process step operations in modular fashion
    void processStepPhase1(SimulationStep currentStep, Timestamp currentTime);
    void processStepPhase2(SimulationStep currentStep, Timestamp currentTime);
    void processStepPhase3(SimulationStep currentStep, Timestamp currentTime);
    void processStepPhase4(SimulationStep currentStep, Timestamp currentTime);
    void processStepPhase5(SimulationStep currentStep, Timestamp currentTime);
    void processStepPhase6(SimulationStep currentStep, Timestamp currentTime);
    void processStepPhase7(SimulationStep currentStep, Timestamp currentTime);
    void processStepPhase8(SimulationStep currentStep, Timestamp currentTime);
    void processStepPhase9(SimulationStep currentStep, Timestamp currentTime);
    void processStepPhase10(SimulationStep currentStep, Timestamp currentTime);

    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
