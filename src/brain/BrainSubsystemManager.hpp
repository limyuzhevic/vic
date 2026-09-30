#pragma once

#include "../core/Types/Types.hpp"
#include <memory>
#include <unordered_map>
#include <vector>

namespace nlm {

class Config;
class RandomGenerator;
class SimulationClock;
class Logger;

// Forward declarations for all integrated systems
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

// Brain subsystem manager for Phase 6 integrated brain
// Replaces monolithic Brain class with modular architecture
class BrainSubsystemManager {
public:
    BrainSubsystemManager(std::shared_ptr<Config> config);
    ~BrainSubsystemManager();
    
    // Disable copying, enable moving
    BrainSubsystemManager(const BrainSubsystemManager&) = delete;
    BrainSubsystemManager& operator=(const BrainSubsystemManager&) = delete;
    BrainSubsystemManager(BrainSubsystemManager&&) noexcept;
    BrainSubsystemManager& operator=(BrainSubsystemManager&&) noexcept;
    
    // Initialize all subsystems
    bool initialize();
    
    // Main simulation step
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // Receive sensory input
    void receiveSensoryInput(const class SensoryInput& input);
    
    // Access to subsystems
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory.get(); }
    
    PredictionSystem* getPredictionSystem() { return predictionSystem.get(); }
    NeuralPlanner* getPlanner() { return planner.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation.get(); }
    AttentionalSelection* getAttention() { return attention.get(); }
    
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    
    Dopamine* getDopamine() { return dopamine.get(); }
    Curiosity* getCuriosity() { return curiosity.get(); }
    Novelty* getNovelty() { return novelty.get(); }
    PredictionError* getPredictionError() { return predictionError.get(); }
    
    // Plasticity systems
    SpikeSystem* getSpikeSystem() { return spikeSystem.get(); }
    STDP* getSTDP() { return stdp.get(); }
    Hebbian* getHebbian() { return hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity.get(); }
    
    // Configuration
    std::shared_ptr<Config> getConfig() const { return config; }
    RandomGenerator* getRandomGenerator() { return rng.get(); }
    
    // Statistics and management
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getRegionCount() const;
    
    // Checkpoint management
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Reset and status
    void reset();
    void logStatus() const;
    
    // Neuron and action management
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    std::unique_ptr<class Action> produceAction();
    void applyNeuromodulation(const class Neuromodulator& signal);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
