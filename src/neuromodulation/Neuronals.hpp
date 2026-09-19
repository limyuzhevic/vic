#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Neural dynamics system
class Neuronals {
public:
    Neuronals();
    ~Neuronals();
    
    void initialize(Brain* brain);
    void update(double timestep);
    void setExternalInput(float input);
    
private:
    struct Impl;
    Impl* pImpl;
    
    void updateExcitabilityInhibition(Brain& brain, float dt);
    void applyExternalInputs(Brain& brain);
    void updateConnectivityPatterns(Brain& brain);
    void applyHomeostaticMechanisms(Brain& brain);
};

} // namespace nlm