// Brain.hpp - Main Brain interface header
// Clean, focused interface for the refactored Brain class
// Uses BrainSetup and BrainIntegration components for modularity

#pragma once

#include "../core/Config/Config.hpp"
#include "../core/Types/Types.hpp"
#include <memory>
#include <string>

namespace nlm {

// Forward declarations for system components
class BrainSetup;
class BrainIntegration;
class NeuralRegion;

// Main Brain class - refactored into modular components
// Provides unified interface while maintaining backward compatibility
class Brain {
public:
    // Construction and destruction
    explicit Brain(std::shared_ptr<Config> config);
    ~Brain();
    
    // Disable copying, enable moving for performance
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Core brain lifecycle
    bool initialize();
    void reset();
    
    // Simulation management
    void step(SimulationStep currentStep);
    void step(SimulationStep currentStep, Timestamp currentTime);
    
    // I/O operations
    void receiveSensoryInput(const class SensoryInput& input);
    void injectCurrent(NeuronId neuron, MembranePotential current);
    void injectCurrentToNeurons(NeuronType type, MembranePotential current);
    
    // Statistics and monitoring
    float getExcitationInhibitionRatio() const;
    size_t getTotalSpikeCount() const;
    size_t getPendingSpikeEventCount() const;
    size_t getTotalNeuronCount() const;
    size_t getTotalSynapseCount() const;
    size_t getActiveNeuronCount() const;
    size_t getFiringNeuronCount() const;
    float getAverageFiringRate() const;
    
    // Output and control
    std::unique_ptr<class Action> produceAction();
    void applyNeuromodulation(const class Neuromodulator& signal);
    
    // Development and plasticity
    void updatePlasticity();
    void develop();
    
    // Persistence and checkpointing
    bool save(const std::string& filepath) const;
    bool load(const std::string& filepath);
    
    // Region management
    RegionId addRegion(const std::string& name = "");
    NeuralRegion* getRegion(RegionId id);
    const NeuralRegion* getRegion(RegionId id) const;
    size_t getRegionCount() const;
    std::vector<RegionId> getRegionIds() const;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const;
    
    // Inter-region connectivity
    void addInterRegionConnection(RegionId source, RegionId target, 
                                  float weight = 0.0f, Delay delay = 1);
    void removeInterRegionConnection(RegionId source, RegionId target);
    
    // System access methods
    // Plasticity systems
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
    // Memory systems
    NeuralWorkingMemory* getWorkingMemory();
    NeuralEpisodicMemory* getEpisodicMemory();
    NeuralAssociativeMemory* getAssociativeMemory();
    
    // Other integrated systems
    PredictionSystem* getPredictionSystem();
    NeuralPlanner* getPlanner();
    ConceptFormation* getConceptFormation();
    AttentionalSelection* getAttention();
    DevelopmentSystem* getDevelopmentSystem();
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
    // Internal systems
    std::shared_ptr<const Config> getConfig() const;
    RandomGenerator* getRandomGenerator();
    BrainIntegration* getIntegration();
    const BrainIntegration* getIntegration() const;
    
    // Development control
    DevelopmentalStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentalStage stage);
    
    // Diagnostics
    void logStatus() const;
    
    // Legacy compatibility methods
    void integrateDevelopment();
    void integrateNeuromodulation();
    void integrateMemorySystems();

private:
    // Component composition for modularity
    std::unique_ptr<BrainSetup> setupComponent;
    std::unique_ptr<BrainIntegration> integrationComponent;
};

} // namespace nlm