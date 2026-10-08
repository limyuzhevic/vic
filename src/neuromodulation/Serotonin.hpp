#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

class Serotonin {
public:
    Serotonin();
    ~Serotonin();
    
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
    
    // Raphe nucleus serotonin dynamics
    void updateRapheDynamics(float moodState, float behavioralDemand);
    void modulateMoodRegulation(float baselineMood, float fiveHtInfluence);
    
    // Behavioral inhibition and punishment learning
    void processPunishmentSignal(float punishmentIntensity, float aversiveLearning);
    void updateAvoidanceLearning(float threatLevel, float avoidanceStrength);
    
    // Exploration vs exploitation strategy switching
    void updateStrategySwitching(float explorationDrive, float exploitationValue);
    void modulateDecisionMaking(float choiceUncertainty, float fiveHtModulation);
    
    // 5-HT receptor subtypes
    float get5HT1AReceptorActivation() const;
    float get5HT2AReceptorActivation() const;
    void set5HT1AReceptorActivation(float activation);
    void set5HT2AReceptorActivation(float activation);
    
    // Cross-modulation
    void modulateWithDopamine(float daLevel);
    void modulateWithAcetylcholine(float achLevel);
    void modulateWithNorepinephrine(float neLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm