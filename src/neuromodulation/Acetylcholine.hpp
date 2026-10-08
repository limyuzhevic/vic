#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Acetylcholine {
public:
    Acetylcholine();
    ~Acetylcholine();
    
    const char* getName() const;
    float getLevel() const;
    void setLevel(float level);
    float getConcentration() const;
    float getBaselineConcentration() const;
    float getHomeostaticSetPoint() const;
    bool isHomeostaticallyRegulated() const;
    void resetToBaseline();
    float getPlasticityFactor() const;
    void update(TimestepDuration dt);
    
    // Cholinergic dynamics
    void updateAChDynamics(float corticalActivity, float attentionalDemand);
    void modulateSensoryGain(float baselineGain, float cholinergicInfluence);
    
    // Memory consolidation mechanisms
    void triggerMemoryConsolidation(float noveltySignal, float consolidationStrength);
    void updateWorkingMemoryTrace(float memoryTrace, float cholinergicModulation);
    
    // Ventral tegmental area projections
    void updateVTAChProjections(float dopamineLevel, float rewardPredictionError);
    void modulatePrefrontalCortex(float pfActivity, float attentionalFocus);
    
    // Cross-modulation
    void modulateWithDopamine(float daLevel);
    void modulateWithNorepinephrine(float neLevel);
    void modulateWithSerotonin(float fiveHtLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm