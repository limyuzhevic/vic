#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class RandomGenerator;
class Maturation;
class Pruning;
class Synaptogenesis;

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
    
    // Get development system components
    Maturation* getMaturation() const { return maturation_.get(); }
    Pruning* getPruning() const { return pruning_.get(); }
    Synaptogenesis* getSynaptogenesis() const { return synaptogenesis_.get(); }
    
    // Set development system components
    void setMaturation(std::unique_ptr<Maturation> maturation) { maturation_ = std::move(maturation); }
    void setPruning(std::unique_ptr<Pruning> pruning) { pruning_ = std::move(pruning); }
    void setSynaptogenesis(std::unique_ptr<Synaptogenesis> synaptogenesis) { synaptogenesis_ = std::move(synaptogenesis); }
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
    
    std::unique_ptr<Maturation> maturation_;
    std::unique_ptr<Pruning> pruning_;
    std::unique_ptr<Synaptogenesis> synaptogenesis_;
};

} // namespace nlm

