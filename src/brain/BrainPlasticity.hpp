// BrainPlasticity.hpp - Plasticity system integration interface
#pragma once

#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../neuromodulation/Neuromodulator.hpp"

namespace nlm {

class Brain;

class BrainPlasticity {
public:
    BrainPlasticity();
    ~BrainPlasticity();
    
    // Set reference to parent brain
    void setBrain(Brain* brain) { brain_ = brain; }
    
    // Apply neuromodulation effects on plasticity
    void applyNeuromodulation(const Neuromodulator& signal);
    
    // Get plasticity statistics
    float getExcitationInhibitionRatio() const;
    size_t getTotalSynapseCount() const;
    size_t getFiringNeuronCount() const;
    
    // Access to underlying plasticity systems
    STDP* getSTDP();
    Hebbian* getHebbian();
    StructuralPlasticity* getStructuralPlasticity();
    
private:
    Brain* brain_;
    
    // Reference to plasticity systems
    STDP* stdp_;
    Hebbian* hebbian_;
    StructuralPlasticity* structuralPlasticity_;
};

} // namespace nlm