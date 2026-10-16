#pragma once

#include "../Brain.hpp"

namespace nlm {

class BrainUtils : public Brain {
public:
    // Spike system access
    SpikeSystem* getSpikeSystem() override;
    const SpikeSystem* getSpikeSystem() const override;
    
    // Plasticity system access
    STDP* getSTDP() override;
    Hebbian* getHebbian() override;
    StructuralPlasticity* getStructuralPlasticity() override;
    
    // Statistics
    float getExcitationInhibitionRatio() const override;
    size_t getTotalSpikeCount() const override;
    size_t getPendingSpikeEventCount() const override;
    
    // Action production
    std::unique_ptr<class Action> produceAction() override;
    
    // Plasticity
    void updatePlasticity() override;
    void develop() override;
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory() override;
    NeuralEpisodicMemory* getEpisodicMemory() override;
    NeuralAssociativeMemory* getAssociativeMemory() override;
    
    // Prediction system access
    PredictionSystem* getPredictionSystem() override;
    
    // Cognition system access
    NeuralPlanner* getPlanner() override;
    ConceptFormation* getConceptFormation() override;
    AttentionalSelection* getAttention() override;
    
    // Random generator access
    RandomGenerator* getRandomGenerator() override;
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const override;
    
    // Logging
    void logStatus() const override;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm