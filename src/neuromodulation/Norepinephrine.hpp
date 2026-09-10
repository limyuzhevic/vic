#pragma once

#include "../core/Types/Types.hpp"

namespace nlm {

// Norepinephrine: Arousal and vigilance
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override;
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    void initialize(class Brain* brain) override;
    
    // Arousal modulation
    void modulateArousal(float novelty, float predictionError);
    
    // Alertness and focus
    void modulateAlertness(float sustainedAttention);
    
    // Stress response
    void handleStress(float threatLevel);
    
private:
    struct Impl;
    Impl* pImpl;
};

} // namespace nlm
