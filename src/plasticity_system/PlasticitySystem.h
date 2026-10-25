#ifndef PLASTICITY_SYSTEM_H
#define PLASTICITY_SYSTEM_H

#include "plasticity/PlasticityRule.hpp"
#include "plasticity/STDP.hpp"
#include "plasticity/Hebbian.hpp"
#include "plasticity/StructuralPlasticity.hpp"
#include "brain/Brain.hpp"
#include "dynamics/SpikeSystem.hpp"

namespace nlm {

class PlasticitySystem {
public:
    virtual ~PlasticitySystem() = default;
    
    virtual void initialize(Brain* brain) = 0;
    virtual void update(const std::vector<std::unique_ptr<NeuralRegion>>& regions,
                       SimulationStep currentStep, Timestamp currentTime) = 0;
    
    virtual STDP* getSTDP() = 0;
    virtual Hebbian* getHebbian() = 0;
    virtual StructuralPlasticity* getStructuralPlasticity() = 0;
    
    virtual void applyNeuromodulation(float neuromodulationLevel) = 0;
    virtual void applyDevelopmentEffects(DevelopmentalStage stage) = 0;
};

} // namespace nlm

#endif // PLASTICITY_SYSTEM_H