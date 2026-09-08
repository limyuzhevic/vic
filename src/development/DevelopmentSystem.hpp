#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>

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
    
    // Record a consolidation event for memory development
    void recordConsolidationEvent(int episodeCount, float dopamineLevel, SimulationStep step);
    
    // Get number of consolidation events recorded
    size_t getConsolidationCount() const { return consolidationCount_; }
    
    // Get average consolidation strength (0-1)
    float getAverageConsolidationStrength() const { return avgConsolidationStrength_; }
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
    size_t consolidationCount_;
    float avgConsolidationStrength_;
};

} // namespace nlm
