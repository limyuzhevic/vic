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
    
    // NEW: Neural excitability effects
    float getNeuralExcitability() const;
    float getAttentionBalance() const;
    float getMemoryConsolidationStrength() const;
    float getNeuromodulationSensitivity() const;
    
    // NEW: Developmental effects on specific systems
    void applyEffectsToNeurons();
    void applyEffectsToAttention();
    void applyEffectsToMemory();
    void applyEffectsToNeuromodulation();
    
    // NEW: Developmental parameter tuning
    void tuneForDevelopmentStage();
    bool isOptimalStageForLearning() const;
    float getLearningPotential() const;
    
    // Initialize with brain reference
    void initialize(Brain* brain);
    
    // Accessors for developmental parameters
    float getBaseExcitability() const { return baseExcitability_; }
    void setBaseExcitability(float value) { baseExcitability_ = value; }
    
    float getAttentionBroadness() const { return attentionBroadness_; }
    void setAttentionBroadness(float value) { attentionBroadness_ = value; }
    
    float getConsolidationRate() const { return consolidationRate_; }
    void setConsolidationRate(float value) { consolidationRate_ = value; }
    
    float getDopamineLevel() const { return dopamineLevel_; }
    void setDopamineLevel(float value) { dopamineLevel_ = value; }
    
    float getAcetylcholineLevel() const { return acetylcholineLevel_; }
    void setAcetylcholineLevel(float value) { acetylcholineLevel_ = value; }
    
    float getNorepinephrineLevel() const { return norepinephrineLevel_; }
    void setNorepinephrineLevel(float value) { norepinephrineLevel_ = value; }
    
    float getSerotoninLevel() const { return serotoninLevel_; }
    void setSerotoninLevel(float value) { serotoninLevel_ = value; }
    
private:
    struct Impl;
    Impl* pImpl;
    double age_;
    
    // Development effects state
    float baseExcitability_;
    float attentionInhibition_;
    float attentionExcitation_;
    float attentionBroadness_;
    float consolidationRate_;
    float workingMemoryCapacity_;
    float episodicStorageCapacity_;
    float dopamineLevel_;
    float acetylcholineLevel_;
    float norepinephrineLevel_;
    float serotoninLevel_;
    
    Brain* brain_;
};

} // namespace nlm
