#pragma once

#include "Neuromodulator.hpp"
#include "../../brain/Types/Types.hpp"

namespace nlm {

// Serotonin: Mood, impulsivity, and social behavior neuromodulator
// Biological basis:
// - Raphe nuclei are the primary source of serotonin (5-HT) in the brain
// - Serotonin regulates mood, emotion, and social behavior
// - Low serotonin linked to impulsivity, aggression, depression
// - Involved in reward processing and decision-making
// - Modulates dopamine system via 5-HT2A receptors
// 
// Functions:
// 1. Mood regulation: influences emotional state and well-being
// 2. Impulsivity control: reduces premature responses
// 3. Social behavior: modulates social bonding and cohesion
// 4. Reward processing: interacts with dopamine system
// 5. Stress response: buffers against stress effects

class Serotonin : public Neuromodulator {
public:
    Serotonin();
    ~Serotonin() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Reward signaling (social and non-social)
    void signalReward(float reward, float socialContext);
    
    // Social behavior signaling
    void signalSocialEvent(float socialValue, float groupCohesion);
    
    // Stress response
    void signalStress(float stressLevel);
    
    // Getters for specialized functions
    float getMoodScore() const;
    float getSatietyLevel() const;
    float getImpulsivity() const;
    float getSocialCohesion() const;
    float getAggression() const;
    
    const std::vector<float>& getMoodHistory() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm