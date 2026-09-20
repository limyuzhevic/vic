#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <memory>

namespace nlm {

// Development system: orchestrates neural development
// Implements developmental stages, plasticity modulation, and structural changes

class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Disable copying, enable moving
    DevelopmentSystem(const DevelopmentSystem&) = delete;
    DevelopmentSystem& operator=(const DevelopmentSystem&) = delete;
    DevelopmentSystem(DevelopmentSystem&&) noexcept;
    DevelopmentSystem& operator=(DevelopmentSystem&&) noexcept;
    
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
    
    // Reset system state
    void reset();
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    double age_;
};

} // namespace nlm
