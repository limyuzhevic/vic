#pragma once

#include "../../plasticity/PlasticityRule.hpp"
#include "../../brain/MemorySystem.hpp"
#include "../../prediction/PredictionSystem.hpp"
#include "../../development/DevelopmentSystem.hpp"
#include <cmath>
#include <algorithm>

namespace nlm {

// New plasticity mechanisms that need to be implemented:
// 1. Reward-modulated plasticity (already done)
// 2. Curiosity-driven exploration plasticity
// 3. Novelty-based plasticity  
// 4. Prediction error-based plasticity
// 5. Developmental stage-dependent plasticity
// 6. Unified plasticity management system

// For now, let's focus on creating the developmental structural changes
// and then integrating everything.

// Add developmental structural changes to the development system
class DevelopmentalStructuralChanges {
public:
    DevelopmentalStructuralChanges();
    ~DevelopmentalStructuralChanges();
    
    // Update structural changes based on developmental stage
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get developmental stage
    DevelopmentalStage getStage() const;
    void setStage(DevelopmentalStage stage);
    
    // Get current developmental age
    double getDevelopmentalAge() const;
    void setDevelopmentalAge(double age);
    
    // Check if critical period is active
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Get developmental plasticity rate
    float getPlasticityRate() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

// Forward declaration for memory system
class MemorySystem {
public:
    virtual ~MemorySystem() = default;
    // Basic memory system interface
    virtual void store(NeuronId neuron, float value) = 0;
    virtual float retrieve(NeuronId neuron) const = 0;
};

// Forward declaration for context
class Context {
public:
    virtual ~Context() = default;
    // Basic context interface
    virtual float getContextValue(const std::string& key) const = 0;
    virtual void setContextValue(const std::string& key, float value) = 0;
};

} // namespace nlm
