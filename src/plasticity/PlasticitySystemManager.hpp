#pragma once

#include "../plasticity/STDP.hpp"
#include "../plasticity/Hebbian.hpp"
#include "../plasticity/StructuralPlasticity.hpp"

namespace nlm {

class Brain;

class PlasticitySystemManager {
public:
    PlasticitySystemManager();
    ~PlasticitySystemManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update plasticity systems
    void update();
    
    // Get plasticity systems
    STDP* getSTDP() { return stdp_.get(); }
    Hebbian* getHebbian() { return hebbian_.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity_.get(); }
    
    // Configure from config
    void configureFromConfig(const Config& config);
    
    // Reset
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    std::unique_ptr<STDP> stdp_;
    std::unique_ptr<Hebbian> hebbian_;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity_;
};

} // namespace nlm