#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>
#include <unordered_map>
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
    
    // ========== ADVANCED DEVELOPMENT ==========
    
    // Custom developmental trajectories
    class DevelopmentalTrajectory {
    public:
        enum class TrajectoryType {
            Linear,      // Linear development
            Exponential, // Exponential growth
            Sigmoid,     // Sigmoid growth (critical period)
            Gaussian,    // Gaussian growth
            Custom      // User-defined trajectory
        };
        
        DevelopmentalTrajectory();
        ~DevelopmentalTrajectory();
        
        // Set trajectory type
        void setTrajectoryType(TrajectoryType type);
        
        // Configure trajectory parameters
        void setGrowthRate(float rate);
        void setPeakTime(float time);
        void setSaturationLevel(float level);
        
        // Calculate developmental state at given time
        float calculateDevelopmentalState(double age) const;
        
        // Set custom trajectory function
        void setCustomTrajectory(const std::function<float(double)>& trajectoryFunc);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Phase transition control
    class PhaseTransitionController {
    public:
        enum class TransitionType {
            Gradual,  // Smooth transition
            Abrupt,   // Sudden transition
            Oscillating,  // Periodic transitions
            Critical  // Critical period transition
        };
        
        PhaseTransitionController();
        ~PhaseTransitionController();
        
        // Initiate phase transition
        void initiateTransition(DevelopmentalStage targetStage,
                               TransitionType transitionType,
                               float transitionDuration = 10.0f);
        
        // Get current transition progress
        float getTransitionProgress() const;
        
        // Is transition in progress?
        bool isTransitionActive() const;
        
        // Configure transition parameters
        void setTransitionThreshold(float threshold);
        void setTransitionSensitivity(float sensitivity);
        void setTransitionMemory(float memory);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Epigenetic factors
    class EpigeneticSystem {
    public:
        struct EpigeneticFactor {
            std::string name;
            float baselineValue;
            float currentValue;
            float mutationRate;
            float heritability;
            std::string inheritancePattern;
            std::unordered_map<std::string, float> environmentalDependencies;
        };
        
        EpigeneticSystem();
        ~EpigeneticSystem();
        
        // Add epigenetic factor
        void addFactor(const EpigeneticFactor& factor);
        
        // Update factor based on environmental input
        void updateFactors(const std::unordered_map<std::string, float>& environment);
        
        // Get factor value
        float getFactorValue(const std::string& factorName) const;
        
        // Get all factor values
        std::unordered_map<std::string, float> getAllFactorValues() const;
        
        // Configure environmental influences
        void setEnvironmentalInfluence(const std::string& factor,
                                      const std::string& environment,
                                      float influenceStrength);
        
    private:
        struct Impl;
        Impl* pImpl;
    };
    
    // Configure advanced development system
    void configureDevelopmentalTrajectory(DevelopmentalTrajectory::TrajectoryType type,
                                         const std::unordered_map<std::string, float>& parameters = {});
    void configurePhaseTransitions(PhaseTransitionController::TransitionType type,
                                  float transitionSpeed = 1.0f);
    void configureEpigeneticFactors(const std::vector<EpigeneticSystem::EpigeneticFactor>& factors);
    
    // Get development statistics
    struct DevelopmentStats {
        DevelopmentalStage currentStage;
        double developmentalAge;
        float plasticityModifier;
        bool isInCriticalPeriod;
        float criticalPeriodProgress;
        std::unordered_map<std::string, float> epigeneticValues;
        float trajectoryProgress;
        float transitionProgress;
        std::unordered_map<std::string, float> customDevelopmentMetrics;
    };
    
    DevelopmentStats getDevelopmentStats() const;
    
    // Environmental influences
    void applyEnvironmentalInfluence(const std::unordered_map<std::string, float>& environment);
    void setEnvironmentalContext(const std::string& context);
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
};

} // namespace nlm

#endif // NLM_DEVELOPMENTSYSTEM_HPP