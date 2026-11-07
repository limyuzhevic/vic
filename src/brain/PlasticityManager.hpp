#pragma once

#include "Brain.hpp"
#include <memory>

namespace nlm {

class PlasticityManager {
public:
    PlasticityManager(Brain* brain);
    ~PlasticityManager();
    
    bool initialize();
    void step(SimulationStep currentStep, Timestamp currentTime, double timestep);
    void reset();
    
    // Plasticity rule access
    STDP* getSTDP() { return stdp.get(); }
    Hebbian* getHebbian() { return hebbian.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity.get(); }
    
    // Plasticity application
    void applyPlasticityRules(SimulationStep currentStep, Timestamp currentTime, double timestep);
    void applySTDP(SimulationStep currentStep, Timestamp currentTime);
    void applyHebbian(SimulationStep currentStep, Timestamp currentTime);
    void applyStructuralPlasticity(SimulationStep currentStep);
    
    // Homeostatic scaling
    void applyHomeostaticScaling(SimulationStep currentStep);
    
private:
    Brain* brain_;
    std::unique_ptr<STDP> stdp;
    std::unique_ptr<Hebbian> hebbian;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity;
};

} // namespace nlm
