// Brain Plasticity System - Manages all plasticity mechanisms
#ifndef BRAIN_PLASTICITY_SYSTEM_HPP
#define BRAIN_PLASTICITY_SYSTEM_HPP

#include "Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

class BrainPlasticitySystem {
public:
    explicit BrainPlasticitySystem(Brain* brain);
    
    // Apply all plasticity rules for current step
    void applyPlasticity(float plasticityMod, const std::vector<Neuron*>& neurons);
    
    // Update structural plasticity (periodic)
    void updateStructuralPlasticity(SimulationStep currentStep, std::unique_ptr<RandomGenerator>& rng);
    
    // Get plasticity statistics
    float getLearningRate() const { return learningRate; }
    size_t getStronglyConnectedSynapses() const { return stronglyConnectedSynapses; }
    
    // Development system integration
    void applyDevelopmentEffects(DevelopmentalStage stage);
    
private:
    Brain* brain;
    
    // Plasticity component systems
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
    
    // Development system
    std::unique_ptr<DevelopmentSystem> developmentSystem;
    
    // Statistics
    float learningRate;
    size_t stronglyConnectedSynapses;
    
    // Helper methods
    void applySTDP(float plasticityMod, const std::vector<Neuron*>& neurons);
    void applyHebbian(const std::vector<Neuron*>& neurons);
    void updateDevelopment(SimulationStep currentStep);
    void adjustPlasticityForDevelopment(DevelopmentalStage stage);
};

} // namespace nlm

#endif // BRAIN_PLASTICITY_SYSTEM_HPP