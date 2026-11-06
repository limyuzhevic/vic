#pragma once

#include "../core/Types/Types.hpp"
#include <string>
#include <vector>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Phase 2: Implemented - arousal modulation and stress response

class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Arousal modulation mechanisms
    void enhanceAlertness(float stress, float rewardPrediction);
    void updateVigilance();
    
    // Initialize with brain reference
    void initialize(class Brain* brain);
    
    // Getters for derived states
    float getVigilance() const;
    float getArousal() const;
    float getStressLevel() const;
    float getRewardSensitivity() const;
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm