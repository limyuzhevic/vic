#pragma once

/**
 * Enhanced Development System - Advanced developmental control for NLM
 * 
 * Provides sophisticated developmental control with adaptive stages,
 * custom development sequences, and external influence mechanisms.
 * 
 * Features:
 * - Custom developmental stages with flexible transitions
 * - Adaptive development based on learning progress
 * - External developmental triggers and influences
 * - Development profiling and metrics
 * - Optimized developmental parameter selection
 * - Developmental event system
 * - Epigenetic influences on development
 * - Experience-dependent development
 * - Critical period management
 * - Developmental stage optimization
 */

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../core/Random/Random.hpp"
#include <functional>
#include <map>
#include <vector>
#include <string>
#include <memory>
#include <algorithm>
#include <chrono>
#include <unordered_map>

namespace nlm {

// Enhanced developmental stage with more flexibility
enum class EnhancedDevelopmentalStage {
    Initial,
    CriticalPeriod,
    Maturation,
    Adult,
    Aging,
    
    // Specialized stages for advanced control
    SensoryIntegration,      // Early sensory processing development
    MotorCoordination,       // Motor system development
    CognitiveEmergence,       // Basic cognitive abilities
    SocialLearning,          // Social interaction capabilities
    AbstractReasoning,        // Higher-order thinking
    SelfAware,               // Meta-cognition and self-awareness
    Transcendent           // Advanced developmental stage
};

// Developmental trigger types
enum class DevelopmentalTrigger {
    TimeBased,
    ExperienceBased,
    MemoryBased,
    SocialBased,
    EnvironmentalBased,
    GeneticBased,
    EpigeneticBased,
    ExternalBased
};

// Developmental influence types
enum class DevelopmentalInfluence {
    Positive,      // Promotes development
    Negative,      // Inhibits development
    Neutral,       // No effect
    Adaptive,      // Adaptive response
    Compensatory   // Compensatory mechanism
};

// Custom developmental stage definition
struct CustomDevelopmentalStage {
    EnhancedDevelopmentalStage stage;
    double startAge;                    // Age when stage begins
    double endAge;                      // Age when stage ends
    float plasticityModifier;          // Plasticity during this stage
    float learningRate;                 // Learning rate during this stage
    float explorationRate;              // Exploration rate during this stage
    bool hasCriticalPeriod;             // Whether this stage has critical period
    std::vector<std::string> requirements; // Requirements to enter stage
    std::vector<std::string> outcomes;   // Expected outcomes when stage completes
    std::map<std::string, float> environmentalFactors; // Environmental influences
    
    CustomDevelopmentalStage(EnhancedDevelopmentalStage s, double start, double end,
                            float plasticity = 1.0f, float learning = 1.0f,
                            float exploration = 0.1f)
        : stage(s), startAge(start), endAge(end), plasticityModifier(plasticity),
          learningRate(learning), explorationRate(exploration), hasCriticalPeriod(false) {}
};

// Developmental event
struct DevelopmentalEvent {
    double triggerAge;                  // Age when event triggers
    DevelopmentalTrigger triggerType;   // Type of trigger
    EnhancedDevelopmentalStage targetStage; // Stage to transition to
    std::map<std::string, float> conditions; // Conditions required for trigger
    std::function<void(Brain* brain)> callback; // Callback function
    std::string description;            // Human-readable description
    bool enabled;                       // Whether event is enabled
    double priority;                    // Event priority (higher = earlier)
    
    DevelopmentalEvent(double age, DevelopmentalTrigger type,
                      EnhancedDevelopmentalStage target, const std::string& desc)
        : triggerAge(age), triggerType(type), targetStage(target), 
          description(desc), enabled(true), priority(1.0) {}
};

// Epigenetic factor
struct EpigeneticFactor {
    std::string name;                   // Factor name
    float baselineValue;               // Baseline value (0-1)
    float currentValue;                 // Current value (0-1)
    float adaptationRate;              // Rate of adaptation (0-1)
    double decayRate;                  // Rate of natural decay
    bool isStable;                      // Whether factor is stable
    std::function<float(float, float)> adaptationFunction; // Adaptation function
    
    EpigeneticFactor(const std::string& n, float baseline, float adaptation = 0.01f,
                   double decay = 0.001, bool stable = false)
        : name(n), baselineValue(baseline), currentValue(baseline),
          adaptationRate(adaptation), decayRate(decay), isStable(stable) {
        adaptationFunction = [this](float experience, float environment) {
            return currentValue + adaptationRate * (environment - currentValue) * experience;
        };
    }
};

// Development profile for tracking
struct DevelopmentProfile {
    double startTime;                   // Profile start time
    double currentAge;                  // Current developmental age
    EnhancedDevelopmentalStage currentStage; // Current stage
    std::map<std::string, float> metrics; // Development metrics
    std::vector<std::pair<double, EnhancedDevelopmentalStage>> stageHistory; // Stage changes over time
    double totalLearningExperience;    // Total learning experience accumulated
    double environmentalStability;      // Environmental stability (0-1)
    double socialComplexity;            // Social complexity level (0-1)
    float geneticPotential;             // Genetic developmental potential (0-1)
    float epigeneticState;              // Current epigenetic state (0-1)
    
    DevelopmentProfile() : startTime(0.0), currentAge(0.0), currentStage(EnhancedDevelopmentalStage::Initial),
                          totalLearningExperience(0.0), environmentalStability(0.5f), 
                          socialComplexity(0.5f), geneticPotential(1.0f), epigeneticState(1.0f) {
        startTime = std::chrono::duration<double>(std::chrono::system_clock::now().time_since_epoch()).count();
    }
    
    void update(double timeDelta) {
        currentAge += timeDelta;
        totalLearningExperience += timeDelta * environmentalStability * socialComplexity;
    }
    
    float getDevelopmentScore() const {
        float stageScore = static_cast<float>(static_cast<int>(currentStage)) / 6.0f;
        float experienceScore = std::min(totalLearningExperience / 1000.0f, 1.0f);
        float epigeneticScore = epigeneticState;
        return (stageScore + experienceScore + epigeneticScore) / 3.0f;
    }
};

// External influence on development
struct ExternalInfluence {
    std::string source;                 // Source of influence (e.g., "environment", "social", "technology")
    DevelopmentalInfluence type;        // Type of influence
    float strength;                     // Strength of influence (0-1)
    std::map<std::string, float> factors; // Specific factors affected
    double duration;                    // Duration in seconds
    double startTime;                   // Start time
    std::function<void(float)> effectFunction; // Effect function
    
    ExternalInfluence(const std::string& src, DevelopmentalInfluence inf, float strength,
                    const std::map<std::string, float>& factors, double dur)
        : source(src), type(inf), strength(strength), factors(factors), duration(dur), startTime(0.0) {}
    
    float getCurrentEffect(double currentTime) const {
        if (currentTime < startTime || currentTime > startTime + duration) {
            return 0.0f;
        }
        return strength * effectFunction((currentTime - startTime) / duration);
    }
};

// Advanced Development System
class EnhancedDevelopmentSystem {
public:
    EnhancedDevelopmentSystem();
    ~EnhancedDevelopmentSystem();
    
    // Stage management
    void addCustomStage(const CustomDevelopmentalStage& stage);
    void removeStage(EnhancedDevelopmentalStage stage);
    void setCurrentStage(EnhancedDevelopmentalStage stage);
    EnhancedDevelopmentalStage getCurrentStage() const { return profile_.currentStage; }
    
    // Stage transitions
    bool attemptStageTransition(Brain* brain, EnhancedDevelopmentalStage newStage);
    bool shouldTransitionToStage(EnhancedDevelopmentalStage stage) const;
    EnhancedDevelopmentalStage getNextStage() const;
    
    // Development control
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    void setDevelopmentSpeed(double speed) { developmentSpeed_ = speed; }
    double getDevelopmentSpeed() const { return developmentSpeed_; }
    
    // Epigenetic control
    void addEpigeneticFactor(const EpigeneticFactor& factor);
    void updateEpigeneticFactors(float environmentalStability, float socialComplexity);
    float getEpigeneticFactor(const std::string& name) const;
    void setEpigeneticFactor(const std::string& name, float value);
    
    // Developmental events
    void addDevelopmentalEvent(const DevelopmentalEvent& event);
    void removeDevelopmentalEvent(const std::string& description);
    void processDevelopmentalEvents(Brain* brain, double currentAge);
    void enableEvent(const std::string& description, bool enable);
    
    // External influences
    void addExternalInfluence(const ExternalInfluence& influence);
    void removeExternalInfluence(const std::string& source);
    void updateExternalInfluences(double currentTime);
    void applyExternalInfluences(Brain* brain);
    
    // Development profiling
    DevelopmentProfile getProfile() const { return profile_; }
    void updateProfile(double timeDelta);
    std::string getDevelopmentReport() const;
    
    // Adaptive development
    void enableAdaptiveDevelopment(bool enable) { adaptiveDevelopmentEnabled_ = enable; }
    bool isAdaptiveDevelopmentEnabled() const { return adaptiveDevelopmentEnabled_; }
    void setDevelopmentTarget(EnhancedDevelopmentalStage target) { developmentTarget_ = target; }
    EnhancedDevelopmentalStage getDevelopmentTarget() const { return developmentTarget_; }
    
    // Optimization
    void optimizeDevelopmentParameters();
    std::map<std::string, float> getOptimalDevelopmentParameters() const;
    float getDevelopmentalHealth() const;
    
    // Event callbacks
    using StageTransitionCallback = std::function<void(EnhancedDevelopmentalStage oldStage, EnhancedDevelopmentalStage newStage, Brain* brain)>;
    using EpigeneticChangeCallback = std::function<void(const std::string& factorName, float oldValue, float newValue)>;
    using ExternalInfluenceCallback = std::function<void(const ExternalInfluence& influence, Brain* brain)>;
    
    void addStageTransitionCallback(StageTransitionCallback callback);
    void addEpigeneticChangeCallback(EpipenicChangeCallback callback);
    void addExternalInfluenceCallback(ExternalInfluenceCallback callback);
    
    // Configuration
    void setCriticalPeriodMultiplier(float multiplier) { criticalPeriodMultiplier_ = multiplier; }
    void setMaturationFactor(float factor) { maturationFactor_ = factor; }
    void setPlasticityDecayRate(float rate) { plasticityDecayRate_ = rate; }
    
    // Debug and analysis
    std::string getDevelopmentAnalysis() const;
    std::vector<std::pair<double, EnhancedDevelopmentalStage>> getStageHistory() const { return profile_.stageHistory; }
    std::map<std::string, float> getCurrentEpigeneticState() const;
    
private:
    EnhancedDevelopmentSystem(const EnhancedDevelopmentSystem&) = delete;
    EnhancedDevelopmentSystem& operator=(const EnhancedDevelopmentSystem&) = delete;
    
    void updateDevelopmentalStage(Brain* brain, double currentAge);
    void updateEpigeneticState(float environmentalStability, float socialComplexity);
    void checkDevelopmentalTriggers(Brain* brain, double currentAge);
    bool evaluateStageRequirements(const CustomDevelopmentalStage& stage, Brain* brain) const;
    float calculateStagePlacticity(float stagePlasticity, float environmentalFactors) const;
    void triggerExternalInfluences(Brain* brain, double currentTime);
    void updateProfileMetrics(Brain* brain);
    double calculateDevelopmentalAge(double simulationTime) const;
    
    DevelopmentProfile profile_;
    double developmentSpeed_;
    bool adaptiveDevelopmentEnabled_;
    EnhancedDevelopmentalStage developmentTarget_;
    float criticalPeriodMultiplier_;
    float maturationFactor_;
    float plasticityDecayRate_;
    
    std::vector<CustomDevelopmentalStage> customStages_;
    std::vector<DevelopmentalEvent> developmentalEvents_;
    std::vector<EpigeneticFactor> epigeneticFactors_;
    std::vector<ExternalInfluence> externalInfluences_;
    
    // Callbacks
    std::vector<StageTransitionCallback> stageTransitionCallbacks_;
    std::vector<EpigeneticChangeCallback> epigeneticChangeCallbacks_;
    std::vector<ExternalInfluenceCallback> externalInfluenceCallbacks_;
    
    // Internal state
    std::map<std::string, float> currentEpigeneticState_;
    EnhancedDevelopmentalStage previousStage_;
    double lastStageTransitionTime_;
    bool stageTransitioning_;
};

} // namespace nlm