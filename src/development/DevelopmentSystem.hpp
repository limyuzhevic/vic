#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include <functional>
#include <unordered_map>
#include <vector>

namespace nlm {

// Forward declarations
class RandomGenerator;
class AttentionalSelection;
class ConceptFormation;
class PredictionSystem;
class NeuralWorkingMemory;
class NeuralEpisodicMemory;
class Neuromodulator;

// Enhanced developmental stages with biological realism
enum class EnhancedDevelopmentalStage : uint8_t {
    // Early stages (0-30% of lifespan)
    Embryonic,      // 0-60 simulation seconds
    Neonatal,       // 60-180 simulation seconds
    Infancy,        // 180-360 simulation seconds
    
    // Childhood (30-60% of lifespan)
    EarlyChildhood, // 360-720 simulation seconds
    MiddleChildhood, // 720-960 simulation seconds
    
    // Adolescence (60-80% of lifespan)
    Puberty,        // 960-1440 simulation seconds
    LateAdolescence, // 1440-2160 simulation seconds
    
    // Adulthood (80-95% of lifespan)
    YoungAdult,     // 2160-2880 simulation seconds
    
    // Late life (95-100% of lifespan)
    MiddleAge,     // 2880-3600 simulation seconds
    Senior,        // 3600-4320 simulation seconds
    
    // Traditional developmental stages for compatibility
    Initial = 99,
    CriticalPeriod = 98,
    Maturation = 97,
    Adult = 96,
    Aging = 95
};

// Development metrics for all brain systems
struct DevelopmentMetrics {
    double age;
    EnhancedDevelopmentalStage stage;
    
    // Attention development
    float attentionalCapacity;      // Number of concurrent attentional foci
    float attentionalStability;     // Consistency of attention maintenance
    float attentionalPrecision;      // Accuracy of selective attention
    
    // Memory development
    size_t workingMemoryCapacity;    // Number of active memory traces
    float episodicMemoryConsolidation; // Rate of memory consolidation
    float memoryRetrievalSpeed;      // Speed of memory access
    
    // Prediction development
    float predictionAccuracy;        // Accuracy of predictive models
    float predictionConfidence;      // Confidence in predictions
    float predictionLatency;         // Time to generate predictions
    
    // Concept formation development
    float conceptStabilityThreshold;  // Threshold for concept stability
    float conceptGeneralizationAbility; // Ability to apply concepts broadly
    size_t conceptFormationRate;      // Rate of new concept formation
    
    // Learning rate development
    float learningRate;               // Overall learning speed
    float exploratoryDrive;           // Motivation to explore novel inputs
    float habituationRate;            // Rate of decreased response to repeated stimuli
    
    // Cognitive skill development
    float planningHorizon;            // How far ahead planning extends
    float problemSolvingAbility;      // Ability to solve complex problems
    float workingMemoryEfficiency;    // Efficiency of working memory operations
    
    // Social development
    float socialLearningCapacity;     // Ability to learn from others
    float theoryOfMind;               // Understanding others' mental states
    float empathyLevel;                // Emotional connection to others
    
    // Motivational development
    float curiosityLevel;             // Drive to explore novel information
    float noveltySeeking;              // Response to new stimuli
    float intrinsicMotivation;        // Internal drive for learning
    
    // Sensory development
    float sensoryReceptorCount;       // Number of sensory receptors
    float sensoryProcessingSpeed;     // Speed of sensory processing
    float sensoryDiscrimination;      // Ability to distinguish sensory inputs
    
    // Neuromodulation development
    float neuromodulationMaturity;     // Maturity of neuromodulatory systems
    float stressResilience;            // Ability to handle stress
    float rewardSensitivity;          // Responsiveness to rewards
};

// Development system: orchestrates comprehensive neural development
// Implements enhanced developmental stages with biological realism and effects on all cognitive systems

class DevelopmentSystem {
public:
    DevelopmentSystem();
    ~DevelopmentSystem();
    
    // Get current developmental stage
    DevelopmentalStage getStage() const;
    EnhancedDevelopmentalStage getEnhancedStage() const;
    void setStage(DevelopmentalStage stage);
    
    // Advance developmental stage
    void advanceStage();
    
    // Get stage name
    const char* getStageName() const;
    const char* getEnhancedStageName() const;
    
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
    
    // Development metrics for all brain systems
    const DevelopmentMetrics& getMetrics() const { return metrics_; }
    
    // Apply developmental effects to all brain systems
    void applyDevelopmentToAttention(AttentionalSelection* attention) const;
    void applyDevelopmentToConceptFormation(ConceptFormation* conceptFormation) const;
    void applyDevelopmentToWorkingMemory(NeuralWorkingMemory* workingMemory) const;
    void applyDevelopmentToEpisodicMemory(NeuralEpisodicMemory* episodicMemory) const;
    void applyDevelopmentToPredictionSystem(PredictionSystem* predictionSystem) const;
    void applyDevelopmentToNeuromodulation(Neuromodulator* neuromodulator) const;
    
    // Get developmental stage based on age
    static EnhancedDevelopmentalStage getStageFromAge(double age);
    static float getAgeMultiplier(EnhancedDevelopmentalStage stage);
    
    // Get memory capacity based on developmental stage
    static size_t getMemoryCapacityForStage(EnhancedDevelopmentalStage stage);
    
    // Get learning rate modifier based on developmental stage
    static float getLearningRateModifier(EnhancedDevelopmentalStage stage);
    
    // Get attentional capacity based on developmental stage
    static float getAttentionalCapacityForStage(EnhancedDevelopmentalStage stage);
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
    EnhancedDevelopmentalStage enhancedStage_;
    DevelopmentMetrics metrics_;
    
    // Update developmental metrics based on current stage
    void updateMetrics();
};

} // namespace nlm
