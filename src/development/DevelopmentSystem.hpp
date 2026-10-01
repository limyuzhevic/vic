#pragma once

#include "../core/Types/Types.hpp"
#include "../brain/Brain.hpp"
#include "../memory/NeuralWorkingMemory.hpp"
#include "../memory/NeuralEpisodicMemory.hpp"
#include "../prediction/PredictionSystem.hpp"
#include "../cognition/ConceptFormation.hpp"
#include "../cognition/AttentionalSelection.hpp"
#include "../neuromodulation/Dopamine.hpp"
#include "../neuromodulation/Curiosity.hpp"
#include "../neuromodulation/PredictionError.hpp"
#include "../neuromodulation/novelty.hpp"
#include <functional>

namespace nlm {

// Forward declarations
class RandomGenerator;

// Development system: orchestrates neural development
// Implements developmental stages, plasticity modulation, and structural changes
// Enhanced to affect multiple brain systems beyond just structural plasticity

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
    
    // === ENHANCED DEVELOPMENT EFFECTS ===
    
    // Affect neural excitability
    void setNeuralExcitability(Brain* brain, float excitabilityMod);
    
    // Affect plasticity rules
    void setPlasticityRules(Brain* brain, float stdpMod, float hebbianMod);
    
    // Affect attention system
    void setAttention(Brain* brain, float attentionGain);
    
    // Affect memory consolidation
    void setMemoryConsolidation(Brain* brain, float consolidationBoost);
    
    // Affect neuromodulation levels
    void setNeuromodulation(Brain* brain, float dopamineMod, float curiosityMod);
    
    // Affect concept formation
    void setConceptFormation(Brain* brain, float conceptThreshold);
    
    // Affect prediction system
    void setPredictionSystem(Brain* brain, float predictionConfidence);
    
    // Get developmental effect strength for a specific system
    float getDevelopmentalEffect(DevelopmentalEffectType type) const;
    
    // Set developmental effect strength
    void setDevelopmentalEffect(DevelopmentalEffectType type, float strength);
    
    // Get current developmental stage characteristics
    DevelopmentalStage getCurrentStage() const { return getStage(); }
    
    // Calculate developmental influence on neural dynamics
    void calculateNeuralDevelopment(Brain* brain, float& leakConductance, float& threshold);
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
    double age_;
    
    // Developmental effects on different systems
    float neuralExcitabilityMod_;
    float plasticityRulesMod_;
    float attentionGain_;
    float memoryConsolidation_;
    float neuromodulationDopamine_;
    float neuromodulationCuriosity_;
    float conceptFormationThreshold_;
    float predictionConfidence_;
};

} // namespace nlm
