//=============================================================
// BRAIN - Brain.cpp Refactoring (Phase 7)
// Main brain class integrating core functionality with advanced systems
//=============================================================

#pragma once

#include "BrainCore.hpp"
#include "BrainManager.hpp"
#include "SystemOrchestrator.hpp"
#include "../core/Config/Config.hpp"
#include "../core/Random/Random.hpp"
#include "../core/Logger/Logger.hpp"

namespace nlm {

// Forward declarations for integrated systems
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

class Brain : public BrainCore {
public:
    // Create brain with configuration
    explicit Brain(std::shared_ptr<Config> config);
    
    ~Brain() override;
    
    // Disable copying, enable moving
    Brain(const Brain&) = delete;
    Brain& operator=(const Brain&) = delete;
    Brain(Brain&&) noexcept;
    Brain& operator=(Brain&&) noexcept;
    
    // Initialize brain with configuration
    bool initialize() override;
    
    // Main simulation step
    void step(SimulationStep currentStep) override;
    
    // Simulation time step
    void step(SimulationStep currentStep, Timestamp currentTime) override;
    
    // Receive sensory input from environment
    void receiveSensoryInput(const class SensoryInput& input) override;
    
    // Inject current directly into a specific neuron
    void injectCurrent(NeuronId neuron, MembranePotential current) override;
    
    // Inject current into all neurons of a specific type
    void injectCurrentToNeurons(NeuronType type, MembranePotential current) override;
    
    // Statistics and queries
    size_t getTotalNeuronCount() const override;
    size_t getTotalSynapseCount() const override;
    size_t getActiveNeuronCount() const override;
    size_t getFiringNeuronCount() const override;
    float getAverageFiringRate() const override;
    float getExcitationInhibitionRatio() const override;
    size_t getTotalSpikeCount() const override;
    size_t getPendingSpikeEventCount() const override;
    
    // Action production
    std::unique_ptr<class Action> produceAction() override;
    
    // Plasticity and development
    void applyNeuromodulation(const class Neuromodulator& signal) override;
    void updatePlasticity() override;
    void develop() override;
    
    // State management
    void reset() override;
    bool save(const std::string& filepath) const override;
    bool load(const std::string& filepath) override;
    
    // Region management
    RegionId addRegion(const std::string& name = "") override;
    NeuralRegion* getRegion(RegionId id) override;
    const NeuralRegion* getRegion(RegionId id) const override;
    size_t getRegionCount() const override;
    std::vector<RegionId> getRegionIds() const override;
    const std::vector<std::unique_ptr<NeuralRegion>>& getRegions() const override;
    
    // Inter-region connections
    void addInterRegionConnection(RegionId source, RegionId target, float weight = 0.0f, Delay delay = 1) override;
    void removeInterRegionConnection(RegionId source, RegionId target) override;
    
    // System access
    SpikeSystem* getSpikeSystem() override;
    const SpikeSystem* getSpikeSystem() const override;
    STDP* getSTDP() override;
    Hebbian* getHebbian() override;
    StructuralPlasticity* getStructuralPlasticity() override;
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const override;
    RandomGenerator* getRandomGenerator() override;
    
    // Logging
    void logStatus() const override;
    
    // State tracking
    SimulationStep getCurrentStep() const override;
    Timestamp getCurrentTime() const override;
    void setCurrentStep(SimulationStep step) override;
    void setCurrentTime(Timestamp time) override;
    
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
    DevelopmentStage getDevelopmentalStage() const;
    void setDevelopmentalStage(DevelopmentStage stage);
    
    // Neuromodulation system access
    Dopamine* getDopamine();
    Curiosity* getCuriosity();
    Novelty* getNovelty();
    PredictionError* getPredictionErrorSignal();
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
