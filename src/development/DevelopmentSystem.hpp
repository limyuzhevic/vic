#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>
#include <map>
#include <vector>
#include <string>

namespace nlm {

// Forward declarations
class RandomGenerator;

// Enhanced developmental stages with more granularity
enum class DevelopmentalStage {
    Embryonic,           // Early development, rapid cell division
    Larval,             // Early postnatal growth
    Juvenile,           // Childhood growth and learning
    Adolescent,         // Puberty and puberty
    YoungAdult,         // Early adulthood
    PrimeAdult,         // Peak performance
    MatureAdult,        // Late adulthood
    Elderly,            // Senior years
    
    // Specialized stages for research
    CriticalPeriod,     // Sensitive period for learning
    SensitivePeriod,    // Other sensitive periods
    Adaptation,         // Adaptation phase
    Recovery,           // Recovery phase
    
    // Research model stages
    ExperimentalStage1, // For experimental models
    ExperimentalStage2, // For experimental models
    ExperimentalStage3, // For experimental models
    
    Unknown            // Default/unknown stage
};

// Enhanced developmental system with granular control
class DevelopmentSystem {
public:
    // Development system configuration
    struct Configuration {
        // Stage duration configuration
        std::map<DevelopmentalStage, double> stageDurations;  // Duration in simulation time
        std::map<DevelopmentalStage, float> plasticityRates;  // Plasticity rate for each stage
        std::map<DevelopmentalStage, float> learningRates;   // Learning rate for each stage
        
        // Environmental influences
        bool enableEnvironmentalInfluence;  // Allow environment to affect development
        bool enableNutritionalInfluence;    // Allow nutrition to affect development
        bool enableSocialInfluence;        // Allow social factors to affect development
        bool enableStressInfluence;         // Allow stress to affect development
        bool enableTemperatureInfluence;   // Allow temperature to affect development
        
        // Individual variation
        float developmentalVariability;     // Variability in development timing (0.0-1.0)
        bool enableGeneticInfluence;        // Allow genetic factors to affect development
        float geneticDeterminism;          // How much development is genetically determined (0.0-1.0)
        
        // Adaptation and plasticity
        bool enableAdaptivePlasticity;      // Enable plasticity that adapts to environment
        float plasticityLearningRate;       // Rate at which plasticity adapts
        bool enableExperienceDependence;   // Development depends on experiences
        
        // Disorders and impairments
        bool enableDevelopmentalDisorders; // Can model developmental disorders
        std::string disorderType;          // Type of disorder (if enabled)
        float disorderSeverity;            // Severity of developmental disorder
        
        // Research features
        bool enableResearchMode;            // Enable special research modes
        std::map<std::string, double> researchParameters; // Research-specific parameters
        
        Configuration() 
            : enableEnvironmentalInfluence(true), enableNutritionalInfluence(true),
              enableSocialInfluence(true), enableStressInfluence(true), enableTemperatureInfluence(true),
              developmentalVariability(0.1f), enableGeneticInfluence(true), geneticDeterminism(0.3f),
              enableAdaptivePlasticity(true), plasticityLearningRate(0.01f),
              enableExperienceDependence(true), enableDevelopmentalDisorders(false),
              disorderSeverity(0.0f), enableResearchMode(false) {
            
            // Default stage durations (in simulation seconds)
            stageDurations = {
                {DevelopmentalStage::Embryonic, 30.0},
                {DevelopmentalStage::Larval, 120.0},
                {DevelopmentalStage::Juvenile, 600.0},
                {DevelopmentalStage::Adolescent, 900.0},
                {DevelopmentalStage::YoungAdult, 300.0},
                {DevelopmentalStage::PrimeAdult, 400.0},
                {DevelopmentalStage::MatureAdult, 500.0},
                {DevelopmentalStage::Elderly, 600.0},
                {DevelopmentalStage::CriticalPeriod, 240.0},
                {DevelopmentalStage::SensitivePeriod, 180.0},
                {DevelopmentalStage::Adaptation, 200.0},
                {DevelopmentalStage::Recovery, 150.0}
            };
            
            // Default plasticity rates
            plasticityRates = {
                {DevelopmentalStage::Embryonic, 1.0f},
                {DevelopmentalStage::Larval, 0.9f},
                {DevelopmentalStage::Juvenile, 0.7f},
                {DevelopmentalStage::Adolescent, 0.5f},
                {DevelopmentalStage::YoungAdult, 0.3f},
                {DevelopmentalStage::PrimeAdult, 0.2f},
                {DevelopmentalStage::MatureAdult, 0.15f},
                {DevelopmentalStage::Elderly, 0.1f},
                {DevelopmentalStage::CriticalPeriod, 0.8f},
                {DevelopmentalStage::SensitivePeriod, 0.6f},
                {DevelopmentalStage::Adaptation, 0.5f},
                {DevelopmentalStage::Recovery, 0.4f}
            };
            
            // Default learning rates
            learningRates = {
                {DevelopmentalStage::Embryonic, 0.0f},
                {DevelopmentalStage::Larval, 0.1f},
                {DevelopmentalStage::Juvenile, 0.3f},
                {DevelopmentalStage::Adolescent, 0.4f},
                {DevelopmentalStage::YoungAdult, 0.2f},
                {DevelopmentalStage::PrimeAdult, 0.1f},
                {DevelopmentalStage::MatureAdult, 0.05f},
                {DevelopmentalStage::Elderly, 0.02f},
                {DevelopmentalStage::CriticalPeriod, 0.6f},
                {DevelopmentalStage::SensitivePeriod, 0.5f},
                {DevelopmentalStage::Adaptation, 0.3f},
                {DevelopmentalStage::Recovery, 0.2f}
            };
        }
    };

    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Configuration management
    void setConfiguration(const Configuration& config) { config_ = config; }
    const Configuration& getConfiguration() const { return config_; }
    
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
    
    // Enhanced development methods
    void applyEnvironmentalInfluence(double timestep);
    void applyNutritionalInfluence(double timestep);
    void applySocialInfluence(double timestep);
    void applyStressInfluence(double timestep);
    void applyTemperatureInfluence(double timestep);
    
    // Individual variation and genetics
    void applyGeneticInfluence(double timestep);
    void applyIndividualVariation(double timestep);
    
    // Experience-dependent development
    void applyExperienceDependence(double timestep);
    void recordExperience(const std::string& experienceType, double value);
    float getExperienceLevel(const std::string& experienceType) const;
    
    // Adaptive plasticity
    void updateAdaptivePlasticity(double timestep);
    void adjustPlasticityBasedOnEnvironment();
    
    // Developmental disorders and impairments
    void applyDevelopmentalDisorders(double timestep);
    void setDisorder(const std::string& disorderType, float severity);
    bool hasDisorder() const;
    std::string getDisorderType() const;
    float getDisorderSeverity() const;
    
    // Research mode
    void enableResearchMode(bool enable, const std::map<std::string, double>& parameters);
    bool isResearchModeEnabled() const;
    double getResearchParameter(const std::string& name) const;
    
    // Advanced stage management
    void setStageDuration(DevelopmentalStage stage, double duration);
    double getStageDuration(DevelopmentalStage stage) const;
    void addStage(DevelopmentalStage stage, double duration, float plasticityRate);
    void removeStage(DevelopmentalStage stage);
    std::vector<DevelopmentalStage> getAvailableStages() const;
    
    // Development statistics
    double getStageProgress() const;
    double getOverallProgress() const;
    std::map<DevelopmentalStage, double> getStageProgressMap() const;
    
    // Developmental milestones and checkpoints
    bool hasReachedMilestone(const std::string& milestoneName) const;
    void setMilestoneCompleted(const std::string& milestoneName, bool completed);
    std::vector<std::string> getCompletedMilestones() const;
    
    // Development analysis
    void analyzeDevelopmentTrends();
    std::string getDevelopmentSummary() const;
    std::map<std::string, double> getDevelopmentMetrics() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    double age_;
    Configuration config_;
    
    // Internal helper methods
    void validateConfiguration() const;
    void updatePlasticityModifiers();
    void advanceToNextAppropriateStage();
    void applyStageSpecificEffects(double timestep, Brain* brain, RandomGenerator& rng);
    void handleStageTransitions();
};
