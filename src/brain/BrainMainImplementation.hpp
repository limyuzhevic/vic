#pragma once

// BrainMainImplementation.hpp - Main brain::step() implementation and utilities
// Contains the main brain implementation that orchestrates all integrated systems

#include "BrainCore.hpp"
#include "BrainMemoryIntegration.hpp"
#include "BrainPredictionIntegration.hpp"
#include "BrainCognitionIntegration.hpp"
#include "BrainNeuromodulationIntegration.hpp"
#include "BrainPlasticityIntegration.hpp"
#include "BrainDevelopmentIntegration.hpp"
#include <memory>
#include <vector>

namespace nlm {

// Forward declarations for checkpoint and memory types
class CheckpointManager;
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
class SpikeSystem;
class STDP;
class Hebbian;
class StructuralPlasticity;

// Main Brain Implementation
class BrainMainImplementation {
public:
    struct Impl;
    Impl* pImpl;
    
    // Create brain main implementation
    explicit BrainMainImplementation(std::shared_ptr<Config> config);
    
    ~BrainMainImplementation();
    
    // Disable copying, enable moving
    BrainMainImplementation(const BrainMainImplementation&) = delete;
    BrainMainImplementation& operator=(const BrainMainImplementation&) = delete;
    BrainMainImplementation(BrainMainImplementation&&) noexcept;
    BrainMainImplementation& operator=(BrainMainImplementation&&) noexcept;
    
    // Initialize all integrated systems
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    
    // Simulation time step with time parameter
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
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
    RegionId addRegion(const std::string& name);
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
    
    // ========== MEMORY SYSTEM ACCESSORS ==========
    
    // Working memory - transient active information
    NeuralWorkingMemory* getWorkingMemory() { return pImpl->memoryIntegration.getWorkingMemory(); }
    
    // Episodic memory - experience storage
    NeuralEpisodicMemory* getEpisodicMemory() { return pImpl->memoryIntegration.getEpisodicMemory(); }
    
    // Associative memory - pattern associations
    NeuralAssociativeMemory* getAssociativeMemory() { return pImpl->memoryIntegration.getAssociativeMemory(); }
    
    // ========== PREDICTION SYSTEM ==========
    
    // Prediction system for sensory prediction and error computation
    PredictionSystem* getPredictionSystem() { return pImpl->predictionIntegration.getPredictionSystem(); }
    
    // ========== COGNITION SYSTEMS ==========
    
    // Neural planner for action planning
    NeuralPlanner* getPlanner() { return pImpl->cognitionIntegration.getPlanner(); }
    
    // Concept formation for pattern discovery
    ConceptFormation* getConceptFormation() { return pImpl->cognitionIntegration.getConceptFormation(); }
    
    // Attentional selection for focus
    AttentionalSelection* getAttention() { return pImpl->cognitionIntegration.getAttention(); }
    
    // ========== DEVELOPMENT SYSTEM ==========
    
    DevelopmentSystem* getDevelopmentSystem() { return pImpl->developmentIntegration.getDevelopmentSystem(); }
    DevelopmentalStage getDevelopmentalStage() const { return pImpl->developmentIntegration.getDevelopmentalStage(); }
    void setDevelopmentalStage(DevelopmentalStage stage) { pImpl->developmentIntegration.setDevelopmentalStage(stage); }
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Dopamine - reward and reinforcement
    Dopamine* getDopamine() { return pImpl->neuromodulationIntegration.getDopamine(); }
    
    // Curiosity - exploration motivation
    Curiosity* getCuriosity() { return pImpl->neuromodulationIntegration.getCuriosity(); }
    
    // Novelty - novelty detection
    Novelty* getNovelty() { return pImpl->neuromodulationIntegration.getNovelty(); }
    
    // Prediction error signal
    PredictionError* getPredictionErrorSignal() { return pImpl->neuromodulationIntegration.getPredictionErrorSignal(); }
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const { return pImpl->config; }
    
    // Get random generator
    RandomGenerator* getRandomGenerator() { return pImpl->rng.get(); }
    
    // Logging
    void logStatus() const;
    
    // Internal integration methods
    void createIntegratedSystems();
    void configureIntegratedSystems();
    void registerIntegratedHandlers();
    
    // Integration step methods
    void stepIntegratedMemory(TimestepDuration dt, SimulationStep currentStep, Timestamp currentTime);
    void stepIntegratedPrediction(SimulationStep currentStep, Timestamp currentTime);
    void stepIntegratedCognition(TimestepDuration dt);
    void stepIntegratedNeuromodulation(TimestepDuration dt, SimulationStep currentStep, Timestamp currentTime);
    void stepIntegratedPlasticity(SimulationStep currentStep, Timestamp currentTime);
    void stepIntegratedDevelopment(SimulationStep currentStep);
    void stepMemoryReplay(SimulationStep currentStep);
    void stepMemoryConsolidation(SimulationStep currentStep);
    void stepCheckpointManager(SimulationStep currentStep, Timestamp currentTime);
    
    // Utility methods
    void collectIntegratedStatistics() const;
    void logIntegratedStepDetails() const;
    
private:
    // Individual integration subsystems
    BrainMemoryIntegration memoryIntegration;
    BrainPredictionIntegration predictionIntegration;
    BrainCognitionIntegration cognitionIntegration;
    BrainNeuromodulationIntegration neuromodulationIntegration;
    BrainPlasticityIntegration plasticityIntegration;
    BrainDevelopmentIntegration developmentIntegration;
    
    // Helper classes
    class Checkpointer;
    
    // Internal implementation details
    struct Impl {
        std::shared_ptr<Config> config;
        std::unique_ptr<RandomGenerator> rng;
        std::vector<std::unique_ptr<NeuralRegion>> regions;
        std::vector<InterRegionConnection> interRegionConnections;
        
        // Integration subsystems
        std::unique_ptr<BrainMemoryIntegration> memoryIntegration;
        std::unique_ptr<BrainPredictionIntegration> predictionIntegration;
        std::unique_ptr<BrainCognitionIntegration> cognitionIntegration;
        std::unique_ptr<BrainNeuromodulationIntegration> neuromodulationIntegration;
        std::unique_ptr<BrainPlasticityIntegration> plasticityIntegration;
        std::unique_ptr<BrainDevelopmentIntegration> developmentIntegration;
        
        // Simulation parameters
        TimestepDuration timestep;
        SimulationStep currentStep;
        Timestamp currentTime;
        
        // Statistics
        size_t totalSpikesThisStep;
        size_t totalSpikesTotal;
        
        // Sensory neurons for input injection
        std::vector<Neuron*> sensoryNeurons;
        std::vector<Neuron*> motorNeurons;
        
        // Integration state
        bool isResting;  // For sleep/rest cycle
        size_t stepsSinceLastEpisode;
        size_t replayInterval;
        size_t consolidationInterval;
        
        // Checkpoint system
        std::unique_ptr<CheckpointManager> checkpointManager;
        
        // Storage for previous sensory activity
        std::vector<float> previousSensoryActivity;
        
        Impl(std::shared_ptr<Config> cfg);
        
        DevelopmentalStage developmentalStage;
        RegionId nextRegionId;
    };
};

} // namespace nlm

