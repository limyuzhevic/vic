#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <vector>
#include <memory>

namespace nlm {

// Forward declarations
class RandomGenerator;

// Development system: orchestrates neural development
// Implements developmental stages, plasticity modulation, and structural changes

class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Get current developmental stage
    DevelopmentalStage getStage() const;
    void setStage(DevelopmentalStage stage);
    
    // Advance developmental stage
    void advanceStage();
    
    // Get stage name
    const char* getStageName() const;
    
    // Update development (simple version)
    void update(Brain* brain, SimulationStep currentStep);
    
    // Update development with full parameters
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get plasticity modifier for current stage
    float getPlasticityModifier() const;
    
    // Get critical period info
    bool isCriticalPeriod() const;
    float getCriticalPeriodProgress() const;
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const { return age_; }
    
    // Set developmental age
    void setDevelopmentalAge(double age) { age_ = age; }
    
    // Development control methods
    void enableDevelopment(bool enable) { developmentEnabled_ = enable; }
    bool isDevelopmentEnabled() const { return developmentEnabled_; }
    
    // Structural plasticity control
    void enableStructuralPlasticity(bool enable) { structuralPlasticityEnabled_ = enable; }
    bool isStructuralPlasticityEnabled() const { return structuralPlasticityEnabled_; }
    
    // Synaptogenesis control
    void enableSynaptogenesis(bool enable) { synaptogenesisEnabled_ = enable; }
    bool isSynaptogenesisEnabled() const { return synaptogenesisEnabled_; }
    
    // Pruning control
    void enablePruning(bool enable) { pruningEnabled_ = enable; }
    bool isPruningEnabled() const { return pruningEnabled_; }
    
    // Maturation control
    void enableMaturation(bool enable) { maturationEnabled_ = enable; }
    bool isMaturationEnabled() const { return maturationEnabled_; }
    
    // Development statistics
    size_t getTotalNeuronsAdded() const { return totalNeuronsAdded_; }
    size_t getTotalSynapsesAdded() const { return totalSynapsesAdded_; }
    size_t getTotalSynapsesRemoved() const { return totalSynapsesRemoved_; }
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
    bool developmentEnabled_;
    bool structuralPlasticityEnabled_;
    bool synaptogenesisEnabled_;
    bool pruningEnabled_;
    bool maturationEnabled_;
    size_t totalNeuronsAdded_;
    size_t totalSynapsesAdded_;
    size_t totalSynapsesRemoved_;
};

} // namespace nlm
