#pragma once

#include "../core/Types/Types.hpp"
#include <memory>

namespace nlm {

// Forward declarations
class Brain;
class Config;
class RandomGenerator;

// PlasticitySystem: Synaptic modification coordination
// Manages STDP, Hebbian, and structural plasticity
class PlasticitySystem {
public:
    PlasticitySystem(std::shared_ptr<Config> config);
    ~PlasticitySystem();
    
    // Initialize plasticity systems
    bool initialize(Brain* brain);
    
    // Plasticity system access
    class STDP* getSTDP() { return stdp_.get(); }
    class Hebbian* getHebbian() { return hebbian_.get(); }
    class StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity_.get(); }
    
    // Plasticity update
    void update(Brain* brain, RandomGenerator& rng, double timestep);
    
    // Plasticity factor management
    void setPlasticityFactor(float factor);
    float getPlasticityFactor() const { return plasticityFactor_; }
    
    // Configuration access
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
    // Synaptogenesis and pruning rates
    void setSynaptogenesisRate(float rate);
    void setPruningRate(float rate);
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Plasticity factors
    float plasticityFactor_;
    float synaptogenesisRate_;
    float pruningRate_;
};

} // namespace nlm
