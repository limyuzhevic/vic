// System manager for coordinating brain components
#pragma once

#include "IBrainInterface.hpp"
#include "../core/EventSystem.hpp"
#include <memory>

namespace nlm {

class ISystemManager {
public:
    virtual ~ISystemManager() = default;
    virtual void initialize(IBrainInterface* brain) = 0;
    virtual void update(double timestep) = 0;
    virtual void reset() = 0;
    virtual bool isActive() const = 0;
};

class MemorySystemManager : public ISystemManager {
public:
    MemorySystemManager();
    ~MemorySystemManager() override;
    
    void initialize(IBrainInterface* brain) override;
    void update(double timestep) override;
    void reset() override;
    bool isActive() const override;
    
    // Memory system access
    NeuralWorkingMemory* getWorkingMemory() { return workingMemory_.get(); }
    NeuralEpisodicMemory* getEpisodicMemory() { return episodicMemory_.get(); }
    NeuralAssociativeMemory* getAssociativeMemory() { return associativeMemory_.get(); }
    
private:
    std::unique_ptr<NeuralWorkingMemory> workingMemory_;
    std::unique_ptr<NeuralEpisodicMemory> episodicMemory_;
    std::unique_ptr<NeuralAssociativeMemory> associativeMemory_;
    bool active_;
};

class NeuromodulationSystemManager : public ISystemManager {
public:
    NeuromodulationSystemManager();
    ~NeuromodulationSystemManager() override;
    
    void initialize(IBrainInterface* brain) override;
    void update(double timestep) override;
    void reset() override;
    bool isActive() const override;
    
    // Neuromodulation system access
    Dopamine* getDopamine() { return dopamine_.get(); }
    Curiosity* getCuriosity() { return curiosity_.get(); }
    Novelty* getNovelty() { return novelty_.get(); }
    PredictionError* getPredictionError() { return predictionError_.get(); }
    
private:
    std::unique_ptr<Dopamine> dopamine_;
    std::unique_ptr<Curiosity> curiosity_;
    std::unique_ptr<Novelty> novelty_;
    std::unique_ptr<PredictionError> predictionError_;
    bool active_;
};

class CognitionSystemManager : public ISystemManager {
public:
    CognitionSystemManager();
    ~CognitionSystemManager() override;
    
    void initialize(IBrainInterface* brain) override;
    void update(double timestep) override;
    void reset() override;
    bool isActive() const override;
    
    // Cognition system access
    NeuralPlanner* getPlanner() { return planner_.get(); }
    ConceptFormation* getConceptFormation() { return conceptFormation_.get(); }
    AttentionalSelection* getAttention() { return attention_.get(); }
    
private:
    std::unique_ptr<NeuralPlanner> planner_;
    std::unique_ptr<ConceptFormation> conceptFormation_;
    std::unique_ptr<AttentionalSelection> attention_;
    bool active_;
};

class PlasticitySystemManager : public ISystemManager {
public:
    PlasticitySystemManager();
    ~PlasticitySystemManager() override;
    
    void initialize(IBrainInterface* brain) override;
    void update(double timestep) override;
    void reset() override;
    bool isActive() const override;
    
    // Plasticity system access
    STDP* getSTDP() { return stdp_.get(); }
    Hebbian* getHebbian() { return hebbian_.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity_.get(); }
    
private:
    std::unique_ptr<STDP> stdp_;
    std::unique_ptr<Hebbian> hebbian_;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity_;
    bool active_;
};

class DevelopmentSystemManager : public ISystemManager {
public:
    DevelopmentSystemManager();
    ~DevelopmentSystemManager() override;
    
    void initialize(IBrainInterface* brain) override;
    void update(double timestep) override;
    void reset() override;
    bool isActive() const override;
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem_.get(); }
    
private:
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    bool active_;
};

} // namespace nlm
