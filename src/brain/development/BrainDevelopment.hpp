#pragma once

// brain/development/BrainDevelopment.hpp
// Development system management for the brain
// Extracts development system from monolithic Brain.cpp

#include "../core/Types/Types.hpp"
#include "../core/Config/Config.hpp"
#include "../development/DevelopmentSystem.hpp"
#include "../plasticity/StructuralPlasticity.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include <memory>
#include <vector>
#include <string>
#include <chrono>

namespace nlm {

// Forward declarations
class Config;
class DevelopmentSystem;
class StructuralPlasticity;
class Dopamine;
class RandomGenerator;

// Brain development system manager
class BrainDevelopment {
public:
    // Constructor
    explicit BrainDevelopment(std::shared_ptr<Config> config);
    
    // Destructor
    ~BrainDevelopment();
    
    // Disable copying
    BrainDevelopment(const BrainDevelopment&) = delete;
    BrainDevelopment& operator=(const BrainDevelopment&) = delete;
    
    // Configuration and initialization
    bool initialize();
    void reset();
    
    // Development system access
    DevelopmentSystem* getDevelopmentSystem() { return developmentSystem_.get(); }
    StructuralPlasticity* getStructuralPlasticity() { return structuralPlasticity_.get(); }
    
    // Configuration
    std::shared_ptr<const Config> getConfig() const { return config_; }
    
    // Development operations
    void updateDevelopment(double timestep);
    void applyDevelopmentEffects(double timestep);
    void updateStructuralPlasticity(double timestep);
    
    // Developmental stage management
    DevelopmentalStage getDevelopmentalStage() const { return currentStage_; }
    void setDevelopmentalStage(DevelopmentalStage stage) { currentStage_ = stage; }
    
    // Development tracking
    double getDevelopmentalAge() const { return developmentalAge_; }
    double getCriticalPeriodEnd() const { return criticalPeriodEnd_; }
    
    // Plasticity rate management
    void updatePlasticityRates(double timestep);
    void adjustSynaptogenesisRate(float rate) { synaptogenesisRate_ = rate; }
    void adjustPruningRate(float rate) { pruningRate_ = rate; }
    
    // Statistics
    size_t getDevelopmentEvents() const { return developmentEvents_; }
    float getPlasticityMaturation() const { return plasticityMaturation_; }
    
    // Configuration utilities
    static bool validateDevelopmentConfig(const std::shared_ptr<const Config>& config);
    static void applyDefaultDevelopmentSettings(std::shared_ptr<Config> config);
    
    // Critical period detection
    bool isInCriticalPeriod() const { return developmentalAge_ < criticalPeriodEnd_; }
    bool isInMaturationPhase() const { return developmentalAge_ >= criticalPeriodEnd_ && 
                                               developmentalAge_ < maturationEnd_; }
    
    // Developmental milestones
    struct DevelopmentalMilestone {
        DevelopmentalStage stage;
        double age;
        std::string description;
        bool achieved;
    };
    
    // Get developmental milestones
    std::vector<DevelopmentalMilestone> getDevelopmentalMilestones() const { return developmentalMilestones_; }
    
    // Trigger developmental event
    void triggerDevelopmentEvent(const std::string& eventType, double timestep);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    
    // Core components
    std::shared_ptr<Config> config_;
    std::unique_ptr<RandomGenerator> rng_;
    
    // Development systems
    std::unique_ptr<DevelopmentSystem> developmentSystem_;
    std::unique_ptr<StructuralPlasticity> structuralPlasticity_;
    
    // Development state
    DevelopmentalStage currentStage_;
    double developmentalAge_;
    double criticalPeriodEnd_;
    double maturationEnd_;
    
    // Plasticity rates
    float synaptogenesisRate_;
    float pruningRate_;
    
    // Statistics
    size_t developmentEvents_;
    float plasticityMaturation_;
    
    // Developmental milestones
    std::vector<DevelopmentalMilestone> developmentalMilestones_;
    
    // Development timing
    std::chrono::steady_clock::time_point developmentStartTime_;
};

} // namespace nlm
