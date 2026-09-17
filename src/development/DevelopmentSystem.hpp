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
    
    // Use enhanced development system
    void useEnhancedSystem(bool useEnhanced) { useEnhanced_ = useEnhanced; }
    
    bool isUsingEnhancedSystem() const { return useEnhanced_; }
    
    // Get enhanced development system interface
    class EnhancedDevelopmentSystem* getEnhancedSystem();
    
    // Initialize enhanced system
    void initializeEnhanced(Brain* brain);
    
    // Get comprehensive developmental metrics
    const class DevelopmentalMetrics& getMetrics() const;
    
    // Get cognitive capabilities
    const class CognitiveCapabilities& getCognitiveCapabilities() const;
    
    // Get behavioral profile
    const class BehavioralProfile& getBehavioralProfile() const;
    
    // Get neural region specializations
    const std::vector<class NeuralRegionSpecialization>& getSpecializationProfile() const;
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
    bool useEnhanced_;
    class DevelopmentalMetrics metrics;
    class CognitiveCapabilities capabilities;
    class BehavioralProfile behaviorProfile;
    std::vector<class NeuralRegionSpecialization> specializationProfile;
};

} // namespace nlm
