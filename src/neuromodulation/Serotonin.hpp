#pragma once

#include "Neuromodulator.hpp"

namespace nlm {

// Serotonin: Mood, behavioral flexibility, decision making (Phase 6)
class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Behavioral flexibility functions
    void modulateBehavioralFlexibility(float flexibility);
    void influenceActionSelection(float actionStrength);
    void affectDecisionMaking(float decisionBias);
    
    // Memory consolidation functions
    void influenceMemoryConsolidation(float consolidationFactor);
    void prioritizeNegativeMemoryConsolidation(float negativePriority);
    
    // Mood and social behavior functions
    void modulateMood(float moodLevel);
    void influenceSocialBehavior(float socialFactor);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm