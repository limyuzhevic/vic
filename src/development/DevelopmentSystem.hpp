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
    
    // ========== DEVELOPMENTAL EFFECTS ON MEMORY SYSTEMS ==========
    
    // Apply developmental effects to memory systems
    void applyMemoryEffects(Brain* brain);
    
    // Get age-dependent memory capacity modifier
    float getMemoryCapacityModifier() const;
    
    // Get age-dependent memory stability modifier
    float getMemoryStabilityModifier() const;
    
    // Get experience-dependent memory enhancement
    float getExperienceMemoryEnhancement(const std::vector<float>& experiencePattern) const;
    
    // Get developmental effects on episodic memory consolidation
    float getEpisodicConsolidationStrength() const;
    
    // Get developmental effects on working memory
    float getWorkingMemoryModifier() const;
    
    // Get developmental effects on memory replay
    float getReplayStrengthModifier() const;
    
    // Get developmental effects on schema formation
    float getSchemaFormationModifier() const;
    
    // ========== DEVELOPMENTAL EFFECTS ON NEUROMODULATION ==========
    
    // Apply developmental effects to neuromodulation systems
    void applyNeuromodulationEffects(Brain* brain);
    
    // Get age-dependent dopamine modulation
    float getDopamineModulation() const;
    
    // Get age-dependent curiosity modulation
    float getCuriosityModulation() const;
    
    // Get developmental effects on prediction error signaling
    float getPredictionErrorModulation() const;
    
    // Get developmental effects on reward prediction
    float getRewardPredictionModulation() const;
    
    // ========== DEVELOPMENTAL EFFECTS ON COGNITION ==========
    
    // Apply developmental effects to cognitive systems
    void applyCognitiveEffects(Brain* brain);
    
    // Get developmental effects on planning
    float getPlanningModifier() const;
    
    // Get developmental effects on attention
    float getAttentionModifier() const;
    
    // Get developmental effects on concept formation
    float getConceptFormationModifier() const;
    
    // Get developmental effects on prediction
    float getPredictionModifier() const;
    
    // ========== AGE-DEPENDENT DEVELOPMENT ==========
    
    // Apply age-dependent developmental effects
    void applyAgeDependentEffects(Brain* brain, double age);
    
    // Get age-dependent synaptic strength modulation
    float getAgeSynapticStrength(double age) const;
    
    // Get age-dependent critical period window
    bool isAgeAppropriateCriticalPeriod(double age) const;
    
    // Get age-dependent neural efficiency
    float getNeuralEfficiency(double age) const;
    
    // Get age-dependent learning rate
    float getLearningRate(double age) const;
    
    // ========== EXPERIENCE-DEPENDENT DEVELOPMENT ==========
    
    // Apply experience-dependent developmental effects
    void applyExperienceDependentEffects(Brain* brain, const std::vector<float>& experiencePattern);
    
    // Record experience for developmental learning
    void recordExperience(const std::vector<float>& experiencePattern);
    
    // Get experience-dependent plasticity enhancement
    float getExperiencePlasticityEnhancement(const std::vector<float>& experiencePattern) const;
    
    // Get developmental stage for given experience
    DevelopmentalStage getExperienceStage(const std::vector<float>& experiencePattern) const;
    
    // Get developmental adaptation based on experience diversity
    float getExperienceAdaptation(const std::vector<float>& experiencePattern) const;
    
    // ========== MEMORY CONSOLIDATION AND SCHEMA FORMATION ==========
    
    // Get developmental effects on memory consolidation during sleep
    float getSleepConsolidationStrength() const;
    
    // Get developmental effects on memory replay timing
    SimulationStep getReplayInterval() const;
    
    // Get developmental effects on schema formation
    bool shouldFormSchemas() const;
    
    // Get developmental effects on memory prioritization
    float getMemoryPrioritization(const EpisodicMemoryItem* episode) const;
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
    
    // Experience-dependent development tracking
    std::vector<std::vector<float>> recordedExperiences_;
    double lastExperienceTime_;
};

} // namespace nlm
