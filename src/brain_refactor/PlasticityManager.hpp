#pragma once

#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "ISubsystem.hpp"
#include <memory>
#include <vector>

namespace nlm {

class Synapse;

class PlasticityManager : public ISubsystem {
public:
    PlasticityManager();
    ~PlasticityManager() override;
    
    // ISubsystem interface
    bool initialize(Brain* brain) override;
    void update(float dt) override;
    void reset() override;
    void logStatus() const override;
    
    // Plasticity rule access
    STDP* getSTDP() const;
    Hebbian* getHebbian() const;
    StructuralPlasticity* getStructuralPlasticity() const;
    
    // STDP-specific operations
    void configureSTDP(float ltpWeight, float ltdWeight, float tau);
    float getSTDPTimeConstant() const;
    
    // Hebbian learning
    void configureHebbian(float learningRate);
    float getHebbianLearningRate() const;
    
    // Structural plasticity operations
    void updateStructuralPlasticity(Brain* brain, RandomGenerator& rng);
    void setSynaptogenesisRate(float rate);
    void setPruningRate(float rate);
    float getSynaptogenesisRate() const;
    float getPruningRate() const;
    
    // Neuromodulation effects on plasticity
    void applyNeuromodulationFactor(float factor);
    void setPlasticityFactor(float factor);
    float getPlasticityFactor() const;
    
    // Development effects on plasticity
    void applyDevelopmentModulation(DevelopmentalStage stage);
    float getDevelopmentModulation(DevelopmentalStage stage) const;
    
    // Synapse plasticity control
    void enablePlasticity(Synapse* synapse, bool hebbian, bool stdp, bool rewardModulated);
    PlasticityFlags getSynapsePlasticity(Synapse* synapse) const;
    
    // Plasticity statistics
    size_t getSTDPPlasticityEvents() const;
    size_t getHebbianPlasticityEvents() const;
    size_t getStructuralPlasticityEvents() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
