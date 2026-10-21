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
// This class represents the core artificial brain that integrates multiple neural subsystems
// to produce intelligent behavior through learning, memory, and adaptive control.

class Brain {
public:
    // Constructor with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    // Destructor
    ~Brain();
    
    // Disable copying, enable moving semantics
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // ============================================================================
    // Core Simulation Methods
    // ============================================================================
    
    // Initialize brain with configuration and set up neural components
    // This method must be called before the brain can be used
    [[nodiscard]] bool initialize();
    
    // Main simulation step with step counter only
    // Advances the brain by one simulation step using default timestep
    void step(SimulationStep currentStep);
    
    // Main simulation step with step counter and timestamp
    // Advances the brain by one simulation step with precise timing
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // ============================================================================
    // Sensory Processing
    // ============================================================================
    
    // Receive sensory input from environment
    // Injects current into sensory neurons based on input pattern
    void receiveSensoryInput(const class SensoryInput& input);
    
    // ============================================================================
    // Motor Control
    // ============================================================================
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current);
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Produce motor/action output based on motor neuron activity
    [[nodiscard]] std::unique_ptr<class Action> produceAction();
    
    // ============================================================================
    // Plasticity Systems
    // ============================================================================
    
    // Spike system access
    // Get spike system for event-driven spike processing
    [[nodiscard]] SpikeSystem* getSpikeSystem() const;
    SpikeSystem* getSpikeSystem();
    
    // Plasticity system access
    // Get STDP (Spike-Timing-Dependent Plasticity) system
    [[nodiscard]] STDP* getSTDP();
    [[nodiscard]] Hebbian* getHebbian();
    [[nodiscard]] StructuralPlasticity* getStructuralPlasticity();
    
    // ============================================================================
    // Statistics and Metrics
    // ============================================================================
    
    // Network statistics
    // Get excitation/inhibition ratio
    [[nodiscard]] float getExcitationInhibitionRatio() const;
    
    // Get spike statistics
    [[nodiscard]] size_t getTotalSpikeCount() const;
    [[nodiscard]] size_t getPendingSpikeEventCount() const;
    
    // ============================================================================
    // Neuromodulation Systems
    // ============================================================================
    
    // Apply neuromodulatory signals
    void applyRewardModulation(float reward, float predictedReward);
    void applyNoveltySignal(float novelty);
    void applyCuriositySignal(float curiosity);
    void applyPredictionError(float error);
    
    // ============================================================================
    // Development System
    // ============================================================================
    
    // Get development system
    DevelopmentSystem* getDevelopmentSystem();
    
    // Get developmental stage
    [[nodiscard]] DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // ============================================================================
    // Neuromodulator Access
    // ============================================================================
    
    // Dopamine - reward and reinforcement
    [[nodiscard]] Dopamine* getDopamine();
    
    // Curiosity - exploration motivation
    [[nodiscard]] Curiosity* getCuriosity();
    
    // Novelty - novelty detection
    [[nodiscard]] Novelty* getNovelty();
    
    // Prediction error signal
    [[nodiscard]] PredictionError* getPredictionErrorSignal();
    
    // ============================================================================
    // Memory Systems
    // ============================================================================
    
    // Working memory - transient active information
    [[nodiscard]] NeuralWorkingMemory* getWorkingMemory();
    
    // Episodic memory - experience storage
    [[nodiscard]] NeuralEpisodicMemory* getEpisodicMemory();
    
    // Associative memory - pattern associations
    [[nodiscard]] NeuralAssociativeMemory* getAssociativeMemory();
    
    // ============================================================================
    // Prediction System
    // ============================================================================
    
    // Prediction system for sensory prediction and error computation
    [[nodiscard]] PredictionSystem* getPredictionSystem();
    
    // ============================================================================
    // Cognitive Systems
    // ============================================================================
    
    // Neural planner for action planning
    [[nodiscard]] NeuralPlanner* getNeuralPlanner();
    
    // Concept formation for pattern discovery
    [[nodiscard]] ConceptFormation* getConceptFormation();
    
    // Attentional selection for focus
    [[nodiscard]] AttentionalSelection* getAttentionalSelection();
    
    // ============================================================================
    // Region Management
    // ============================================================================
    
    // Add a new neural region
    [[nodiscard]] RegionId addRegion(const std::string& name = "");
    
    // Get region by ID
    [[nodiscard]] NeuralRegion* getRegion(RegionId id);
    [[nodiscard]] const NeuralRegion* getRegion(RegionId id) const;
    
    // Get all regions
    [[nodiscard]] const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Region statistics
    [[nodiscard]] size_t getRegionCount() const;
    [[nodiscard]] std::vector<RegionId> getRegionIds() const;
    
    // ============================================================================
    // Global Statistics
    // ============================================================================
    
    // Total neuron and synapse statistics
    [[nodiscard]] size_t getTotalNeuronCount() const;
    [[nodiscard]] size_t getTotalSynapseCount() const;
    
    // Network activity statistics
    [[nodiscard]] size_t getActiveNeuronCount() const;
    [[nodiscard]] size_t getFiringNeuronCount() const;
    [[nodiscard]] float getAverageFiringRate() const;
    [[nodiscard]] float getExcitatoryFiringRate() const;
    [[nodiscard]] float getInhibitoryFiringRate() const;
    
    // ============================================================================
    // State Management
    // ============================================================================
    
    // Get current configuration
    [[nodiscard]] std::shared_ptr<const Config> getConfig() const;
    
    // Reset brain state
    void reset();
    
    // Save brain state to file (checkpointing)
    [[nodiscard]] bool save(const std::string& filepath) const;
    
    // Load brain state from file
    [[nodiscard]] bool load(const std::string& filepath);
    
    // ============================================================================
    // Logging and Debugging
    // ============================================================================
    
    // Log brain status and statistics
    void logStatus() const;
    
    // ============================================================================
    // Health and Performance
    // ============================================================================
    
    // Check if brain is healthy
    [[nodiscard]] bool isHealthy() const;
    
    // Get detailed health report
    [[nodiscard]] std::string getHealthReport() const;
    
    // Get average step time
    [[nodiscard]] double getAverageStepTime() const;
    
    // Get number of active neurons
    [[nodiscard]] size_t getActiveNeurons() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
