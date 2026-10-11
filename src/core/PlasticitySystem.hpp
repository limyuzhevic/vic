// PlasticitySystem - Handles STDP, Hebbian, and structural plasticity
// Extracted from Brain::step() method
#pragma once

#include "../brain/Brain.hpp"
#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include <memory>

namespace nlm {

class PlasticitySystem {
public:
    PlasticitySystem(std::shared_ptr<Brain> brain);
    ~PlasticitySystem();
    
    // Initialize the plasticity system
    bool initialize();
    
    // Process one timestep of plasticity operations
    void step(SimulationStep currentStep, Timestamp currentTime, TimestepDuration timestep);
    
    // Get plasticity system pointers
    STDP* getSTDP() const { return stdp_.get(); }
    Hebbian* getHebbian() const { return hebbian_.get(); }
    StructuralPlasticity* getStructuralPlasticity() const { return structuralPlasticity_.get(); }
    
    // Configuration methods
    void setSynaptogenesisRate(float rate) { structuralPlasticity_->setSynaptogenesisRate(rate); }
    void setPruningRate(float rate) { structuralPlasticity_->setPruningRate(rate); }
    void setSTDPParameters(float ltp, float ltd, float tau) {
        if (stdp_) {
            stdp_->configure(ltp, ltd, tau);
        }
    }
    
private:
    std::shared_ptr<Brain> brain_;
    
    // Plasticity systems
    std::unique_ptr<STDP> stdp_;
    std::unique_ptr<Hebbian> hebbian_;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity_;
    
    // State tracking
    float plasticityMod_;
    
    // Helper methods
    void applySTDP(SimulationStep currentStep, Timestamp currentTime);
    void applyHebbianLearning(SimulationStep currentStep, Timestamp currentTime);
    void applyStructuralPlasticity(SimulationStep currentStep);
    void applyNeuromodulationEffects();
};

} // namespace nlm
