// Create a minimal working Brain class for testing purposes
// This is a simplified version focusing on core functionality

#pragma once

#include "Types/Types.hpp"
#include <memory>
#include <vector>
#include <string>

namespace nlm {

// Forward declarations
class Config;
class RandomGenerator;
class SimulationClock;
class Logger;

// Neural region and neuron interfaces
class NeuralRegion;
class Neuron;
class Synapse;

// Memory system interfaces
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class NeuralAssociativeMemory;

// Prediction system interface
class PredictionSystem;

// Cognition system interfaces
class NeuralPlanner;
class ConceptFormation;
class AttentionalSelection;

// Development system interface
class DevelopmentSystem;

// Neuromodulation system interfaces
class Dopamine;
class Curiosity;
class Novelty;

// Sensory and motor interfaces
class SensoryInput;
class Action;

// Spike system interface
class SpikeSystem;

// Plasticity system interfaces
class STDP;
class Hebbian;
class StructuralPlasticity;

// Checkpoint system interfaces
class CheckpointWriter;
class CheckpointReader;

// For now, create a minimal Brain class with the essential methods
class Brain {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain();
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input from environment
    void receiveSensoryInput(const SensoryInput& input);
    
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
    std::unique_ptr<Action> produceAction();
    
    // Apply neuromodulatory signals
    void applyNeuromodulation(const Neuromodulator& signal);
    
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
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Prediction system access
    PredictionSystem* getPredictionSystem();
    
    // Cognition system access
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem();
    
    // Neuromodulation system access
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    
    // Get current configuration
    std::shared_ptr<const Config> getConfig() const;
    
    // Get random generator
    RandomGenerator* getRandomGenerator();
    
    // Logging
    void logStatus() const;
    
    // ========== SLEEP/REST CYCLE SUPPORT ==========
    
    // Enter rest state (reduced neural activity)
    void enterRestState();
    
    // Exit rest state (return to normal activity)
    void exitRestState();
    
    // Is the brain currently resting?
    bool isResting() const { return pImpl->isResting; }
    
    // Get time spent resting in current rest cycle
    float getRestTime() const { return pImpl->restTime; }
    
    // ========== MEMORY CONSOLIDATION ==========
    
    // Consolidate memories (enhance strong memories, weaken weak ones)
    void consolidateMemories(float threshold = 0.5f);
    
    // Get consolidation status
    bool isConsolidationActive() const { return pImpl->consolidationActive; }
    
    // ========== REPLAY MECHANISMS ==========
    
    // Trigger memory replay
    void triggerReplay(size_t episodeCount = 5);
    
    // Get replay status
    bool isReplayActive() const { return pImpl->replayActive; }
    
    // Get number of memories replayed
    size_t getReplayCount() const { return pImpl->replayCount; }
    
    // ========== REGION MANAGEMENT ==========
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    
    // ========== GLOBAL STATISTICS ==========
    
    // Get all regions
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Get all neurons
    size_t getTotalNeuronCount() const;
    
    // Get all synapses
    size_t getTotalSynapseCount() const;
    
    // Get active neuron count
    size_t getActiveNeuronCount() const;
    
    // Get firing neuron count
    size_t getFiringNeuronCount() const;
    
    // Get average firing rate
    float getAverageFiringRate() const;
    
    // Get inter-region connections
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // Global statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    
    // ========== NEUROMODULATION SYSTEMS ==========
    
    // Get neuromodulation systems
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm