#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "DevelopmentSystem.hpp"
#include <vector>
#include <unordered_map>

namespace nlm {
public:
    Development();
    ~Development();
    
    // Update developmental processes and effects
    // Applies age-dependent changes to neural excitability, attention capacity,
    // memory consolidation, neuromodulator release, and plasticity rules
    void update(Brain* brain, RandomGenerator& rng, TimestepDuration dt);
    
    // Get current developmental stage
    DevelopmentalStage getStage() const { return currentStage; }
    
    // Get developmental age (in simulation time)
    double getAge() const { return age; }
    
    // Set developmental age
    void setAge(double age) { this->age = age; }
    
    // Get neural excitability modifier for current age
    // Age-dependent changes in neuronal threshold and responsiveness
    float getNeuralExcitabilityModifier() const;
    
    // Get attention capacity modifier for current stage
    // Developmental changes in attentional focus and capacity
    float getAttentionCapacityModifier() const;
    
    // Get memory consolidation strength for current stage
    // Age-dependent memory consolidation and replay effectiveness
    float getMemoryConsolidationStrength() const;
    
    // Get neuromodulator release modifier for current stage
    // Developmental modulation of neuromodulator synthesis and release
    float getNeuromodulatorReleaseModifier(NeuromodulatorType type) const;
    
    // Get plasticity rule modifier for current stage
    // Age-dependent changes in STDP, Hebbian, and structural plasticity
    float getPlasticityRuleModifier(PlasticityType type) const;
    
    // Get critical period progress (0-1)
    float getCriticalPeriodProgress() const;
    
    // Check if currently in critical period
    bool isCriticalPeriod() const;
    
    // Check if currently in sensitive period
    bool isSensitivePeriod() const;
    
    // Get age-appropriate learning rate
    float getLearningRate() const;
    
    // Get age-appropriate memory retention factor
    float getMemoryRetentionFactor() const;
    
    // Get age-appropriate exploration vs exploitation bias
    float getExplorationBias() const;
    
    // Get developmental stage transition progress (0-1)
    float getStageTransitionProgress() const;
    
    // Force transition to next developmental stage
    void advanceStage(Brain* brain);
    
    // Set developmental stage
    void setStage(DevelopmentalStage stage);
    
    // Apply developmental effects to neurons
    void applyExcitabilityEffects(Brain* brain);
    
    // Apply developmental effects to attention system
    void applyAttentionEffects(Brain* brain);
    
    // Apply developmental effects to memory consolidation
    void applyMemoryConsolidationEffects(Brain* brain);
    
    // Apply developmental effects to neuromodulation
    void applyNeuromodulationEffects(Brain* brain);
    
    // Apply developmental effects to plasticity rules
    void applyPlasticityRuleEffects(Brain* brain);
    
    // Get developmental metrics for testing
    struct DevelopmentalMetrics {
        double age;
        DevelopmentalStage stage;
        float neuralExcitability;
        float attentionCapacity;
        float memoryConsolidation;
        float dopamineRelease;
        float acetylcholineRelease;
        float norepinephrineRelease;
        float serotoninRelease;
        float plasticityStrength;
        float learningRate;
        float explorationBias;
        float memoryRetention;
    };
    
    DevelopmentalMetrics getMetrics() const;
    
    // Initialize development with brain reference
    void initialize(Brain* brain) { this->brain = brain; }
    
private:
    struct Impl;
    Impl* pImpl;
    
    // Current developmental state
    double age;
    DevelopmentalStage currentStage;
    double stageStartAge;
    double stageTransitionTimer;
    
    // Brain reference for integration
    Brain* brain;
    
    // Age-dependent parameters
    std::unordered_map<NeuromodulatorType, float> baseReleaseLevels;
    std::unordered_map<PlasticityType, float> basePlasticityRules;
    
    // Helper methods
    void updateDevelopmentalStage(Brain* brain, RandomGenerator& rng);
    void initializeStageParameters();
    double calculateStageDuration(DevelopmentalStage stage) const;
    float interpolate(float start, float end, double t) const;
};

} // namespace nlm
