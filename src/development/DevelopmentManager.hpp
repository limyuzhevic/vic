#pragma once

#include "../core/Types/Types.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <memory>

namespace nlm {

// Development Manager
// Coordinates developmental stages and plasticity rate modifications
// Manages neural development throughout the simulation lifecycle

class DevelopmentManager {
public:
    DevelopmentManager();
    ~DevelopmentManager();
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Update development system
    void update(float timestep);
    
    // Get development system instance
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem.get(); }
    
    // Configure development parameters
    void configure(float criticalPeriodDuration, float maturationRate,
                   float adultStability, float plasticityDecayRate);
    
    // Advance developmental stage
    void advanceStage();
    
    // Get current developmental stage
    DevelopmentalStage getStage() const;
    
    // Get stage name and progress
    const char* getStageName() const;
    float getCriticalPeriodProgress() const;
    float getMaturationProgress() const;
    float getAdultStabilityLevel() const;
    
    // Get developmental age (in simulation time)
    double getDevelopmentalAge() const;
    void setDevelopmentalAge(double age);
    
    // Apply developmental plasticity modifiers
    void applyPlasticityModifiers(STDP* stdp, Hebbian* hebbian,
                                  StructuralPlasticity* structuralPlasticity);
    
    // Check if in critical period
    bool isCriticalPeriod() const;
    
    // Get developmental statistics
    double getTotalDevelopmentTime() const;
    uint32_t getDevelopmentSteps() const;
    float getPlasticityModifier() const;
    
    // Reset development state
    void reset();
    
    // Trigger development milestone (e.g., puberty, adulthood)
    void triggerMilestone(const char* milestoneName);
    
    // Set development speed (for testing or simulation control)
    void setDevelopmentSpeed(float speed);
    
    // Check if development should advance
    bool shouldAdvanceDevelopment(double currentStep) const;
    
private:
    // Private implementation for PIMPL pattern
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
