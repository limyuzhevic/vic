#pragma once

#include "../core/Types/Types.hpp"
#include <string>

namespace nlm {

// Norepinephrine: Arousal and vigilance
// Implements NE signaling for arousal, stress response, and enhanced processing
class Norepinephrine : public Neuromodulator {
public:
    Norepinephrine();
    ~Norepinephrine() override;
    
    const char* getName() const override { return "NE"; }
    float getLevel() const override;
    void setLevel(float level) override;
    float getPlasticityFactor() const override;
    void update(TimestepDuration dt) override;
    
    // Signal arousal and stress effects
    void signalArousal(float arousal);
    void signalStress(float stress);
    void signalWarning(float warning);
    
    // Get neuromodulation effects
    float getArousalLevel() const;
    float getStressLevel() const;
    float getProcessingSpeed() const;
    
private:
    struct Impl;
    std::unique_ptr<Impl> pImpl;
};

} // namespace nlm
